#include "orm/drivers/mysql/mysqlresult.hpp"

#include <QDateTime>

#include <orm/macros/likely.hpp>

#include "orm/drivers/mysql/mysqldriver_p.hpp"
#include "orm/drivers/mysql/mysqlresult_p.hpp"
#include "orm/drivers/mysql/mysqlutils_p.hpp"
#include "orm/drivers/sqlrecord.hpp"

// this is a copy of the old MYSQL_TIME before an additional integer was added in
// 8.0.27.0. This kills the sanity check during retrieving this struct from mysql
// when another libmysql version is used during runtime than during compile time
struct QT_MYSQL_TIME
{
    unsigned int year, month, day, hour, minute, second = 0;
    /*! The fractional part of the second in microseconds. */
    unsigned long second_part = 0;
    my_bool neg = false;
    enum enum_mysql_timestamp_type time_type = MYSQL_TIMESTAMP_NONE;
};

TINYORM_BEGIN_COMMON_NAMESPACE

using MySqlUtils = Orm::Drivers::MySql::MySqlUtilsPrivate;

using namespace Qt::StringLiterals;

namespace Orm::Drivers::MySql
{

/* public */

MySqlResult::MySqlResult(const MySqlDriver *const db)
    : SqlResult(std::make_unique<MySqlResultPrivate>(db))
{}

MySqlResult::~MySqlResult()
{
    cleanup();
}

QVariant MySqlResult::handle() const
{
    Q_D(const MySqlResult);

    if (d->preparedQuery)
        // CUR check this later; I don't understand it now silverqx
        return d->meta ? QVariant::fromValue(d->meta) : QVariant::fromValue(d->stmt);
    else
        return QVariant::fromValue(d->result);
}

/* protected */

/* Normal queries */

bool MySqlResult::exec(const QString &query)
{
    Q_D(MySqlResult);

    cleanup();

    const auto *const sqldriver = d->drv_d_func();
    auto *const mysql = sqldriver->mysql;

    // Execute query
    if (const auto queryArray = query.toUtf8();
        mysql_real_query(mysql, queryArray.constData(), queryArray.size()) != 0
    )
        return setLastError(MySqlUtils::makeError(
                                "Unable to execute query",
                                SqlDriverError::StatementError, mysql));
    // Obtain result set
    d->result = mysql_store_result(mysql);

    if (const auto errNo = mysql_errno(mysql);
        d->result == nullptr && errNo != 0
    )
        return setLastError(MySqlUtils::makeError(
                                "Unable to store result",
                                SqlDriverError::StatementError, mysql, errNo));

    const auto hasFields = d->populateFields(mysql);

    // Executed query has result set
    setSelect(hasFields);
    // Everything is ready 👌
    setActive(true);

    return true;
}

/* Prepared queries */

bool MySqlResult::prepare(const QString &query)
{
    Q_D(MySqlResult);

    // Nothing to do
    if (query.isEmpty())
        return false;

    cleanup();

    // Create the MYSQL_STMT handler
    if (auto *const mysql = d->drv_d_func()->mysql;
        (d->stmt = mysql_stmt_init(mysql)) == nullptr
    )
        return setLastError(MySqlUtils::makeError(
                                "Unable to prepare statement",
                                SqlDriverError::StatementError, mysql));

    // Not the same as the errno error code
    int status = 1;

    // Prepare the SQL statement
    const auto queryArray = query.toUtf8();
    status = mysql_stmt_prepare(d->stmt, queryArray.constData(), queryArray.size());
    if (status != 0) {
        mysqlStmtClose();
        return setLastError(MySqlResultPrivate::makeStmtError(
                                "Unable to prepare statement",
                                SqlDriverError::StatementError, d->stmt));
    }

    setSelect(d->bindInValues());

    d->query = query;
    d->preparedQuery = true;

    return true;
}

bool MySqlResult::exec()
{
    Q_D(MySqlResult);

    if (!d->preparedQuery)
        throw std::exception(
                "The prepared query is empty, to call normal queries use "
                "the SqlQuery1::exec(QString) overload.");

    if (!d->stmt)
        return false;

    /* The int type is ok because some mysql_xyz() return bool true if failed. 🤔🙃
       I leave it this way as it's the best solution. */
    int status = 1;
    // These vectors keep values alive long enough until mysql_stmt_execute() is invoked
    QList<my_bool> nullVector;
    QList<QByteArray> stringVector;
    QList<QT_MYSQL_TIME> timeVector;

    /* Reset a prepared statement on client and server to state after prepare,
       unbuffered result sets and current errors. */
    status = mysql_stmt_reset(d->stmt);
    if (status != 0)
        return setLastError(MySqlResultPrivate::makeStmtError(
                                "Unable to reset statement",
                                SqlDriverError::StatementError, d->stmt));

    // Bind prepared bindings if the correct number of prepared bindings was bound
    if (d->hasPreparedBindings(
            // Number of parameter placeholders in the prepared statement
            mysql_stmt_param_count(d->stmt))
    ) {
        d->bindPreparedBindings(nullVector, stringVector, timeVector);

        // Bind data for parameter placeholders 🕺
        status = mysql_stmt_bind_param(d->stmt, d->outBinds);
        if (status != 0) {
            setLastError(MySqlResultPrivate::makeStmtError(
                             "Unable to bind data for parameter placeholders",
                             SqlDriverError::StatementError, d->stmt));
            return false;
        }
    }

    // Execute query
    status = mysql_stmt_execute(d->stmt);

    if (status != 0)
        return setLastError(MySqlResultPrivate::makeStmtError(
                                "Unable to execute prepared statement",
                                SqlDriverError::StatementError, d->stmt));

    // Executed query has result set, if there are metadata there are also result sets
    setSelect(d->meta != nullptr);

    if (isSelect()) {
        my_bool update_max_length = true;

        // Bind output columns in the result set to data buffers and length buffers
        status = mysql_stmt_bind_result(d->stmt, d->inBinds);
        if (status != 0)
            return setLastError(MySqlResultPrivate::makeStmtError(
                                    "Unable to bind result set data buffers",
                                    SqlDriverError::StatementError, d->stmt));

        // Update the metadata MYSQL_FIELD->max_length value
        if (d->hasBlobs)
            mysql_stmt_attr_set(d->stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &update_max_length);

        // Buffer the complete result set on the client (will be prepared for fetching)
        status = mysql_stmt_store_result(d->stmt);
        if (status != 0)
            return setLastError(MySqlResultPrivate::makeStmtError(
                                    "Unable to store prepared statement result sets",
                                    SqlDriverError::StatementError, d->stmt));

        // CUR drivers revisit and drop silverqx
//        if (d->hasBlobs) {
//            // mysql_stmt_store_result() with STMT_ATTR_UPDATE_MAX_LENGTH set to true crashes
//            // when called without a preceding call to mysql_stmt_bind_result()
//            // in versions < 4.1.8
//            d->bindBlobs();
//            status = mysql_stmt_bind_result(d->stmt, d->inBinds);
//            if (status != 0)
//                return setLastError(MySqlResultPrivate::makeStmtError(
//                                        "Unable to bind outvalues",
//                                        SqlDriverError::StatementError, d->stmt));
//        }
        setAt(QSql::BeforeFirstRow);
    }

    setActive(true);

    return true;
}

/* Result sets */

SqlRecord MySqlResult::record() const
{
    Q_D(const MySqlResult);
    // CUR drivers check if is a good idea to fetch fields again because all fields are already cached in the MyField:::myField silverqx

    MYSQL_RES *const res = d->preparedQuery ? d->meta : d->result;
    // Backup the current cursor position
    const auto currentCursor = mysql_field_tell(res);
    // Seek to the beginning
    mysql_field_seek(res, 0);

    SqlRecord result;
    const MYSQL_FIELD *fieldInfo = nullptr;

    while ((fieldInfo = mysql_fetch_field(res)))
        result.append(MySqlUtils::convertToSqlField(fieldInfo));

    // Restore the cursor position
    mysql_field_seek(res, currentCursor);

    return result;
}

QVariant MySqlResult::lastInsertId() const
{
    Q_D(const MySqlResult);

    if (d->preparedQuery) {
        if (const auto id = mysql_stmt_insert_id(d->stmt);
            id != 0
        )
            return id;
    }
    else {
        if (const auto id = mysql_insert_id(d->drv_d_func()->mysql);
            id != 0
        )
            return id;
    }

    return {};
}

bool MySqlResult::fetch(const int index)
{
    Q_D(MySqlResult);

    // Nothing to fetch, an empty result set
    if (size() == 0)
        return false;

    // Cursor is already on the requested row/result
    if (at() == index)
        return true;

    if (d->preparedQuery) {
        mysql_stmt_data_seek(d->stmt, index);

        /* Here are fetched real data and lengths into the buffers that were bound by
           the mysql_stmt_bind_result(). 🥳
           Also, the MyField::nullIndicator or inBinds.is_null is populated
           (see MySqlResultPrivate::bindInValues()). */
        const auto status = mysql_stmt_fetch(d->stmt);

        // Success, no more data exists; returning false here is correct
        if (status == MYSQL_NO_DATA)
            return false;

        QString errorMessage;
        if (status == 1)
            errorMessage = "Unable to fetch data"_L1;
        else if (status == MYSQL_DATA_TRUNCATED)
            errorMessage = "Data truncated during fetching data"_L1;

        // This means there was an error or data were truncated
        if (!errorMessage.isEmpty())
            return setLastError(MySqlResultPrivate::makeStmtError(
                                    errorMessage, SqlDriverError::StatementError,
                                    d->stmt));
    } else {
        mysql_data_seek(d->result, index);

        d->row = mysql_fetch_row(d->result);
        if (d->row == nullptr)
            return false;
    }

    setAt(index);
    return true;
}

bool MySqlResult::fetchFirst()
{
    // Nothing to fetch, an empty result set
    if (size() == 0)
        return false;

    // Cursor is already on the first row/result
    if (at() == 0)
        return true;

    return fetch(0);
}

bool MySqlResult::fetchLast()
{
    const auto rowsCount = size();

    // Nothing to fetch, an empty result set
    if (rowsCount == 0)
        return false;

    // Cursor is already on the last row/result
    if (at() == rowsCount)
        return true;

    return fetch(rowsCount - 1);
}

bool MySqlResult::fetchNext()
{
    Q_D(MySqlResult);

    // Nothing to fetch, an empty result set
    if (size() == 0)
        return false;

    // CUR drivers duplicate code in fetch(int) silverqx
    if (d->preparedQuery) {
        const auto status = mysql_stmt_fetch(d->stmt);

        // Success, no more data exists; returning false here is correct
        if (status == MYSQL_NO_DATA)
            return false;

        QString errorMessage;
        if (status == 1)
            errorMessage = "Unable to fetch data"_L1;
        else if (status == MYSQL_DATA_TRUNCATED)
            errorMessage = "Data truncated during fetching data"_L1;

        // This means there was an error or data were truncated
        if (!errorMessage.isEmpty())
            return setLastError(MySqlResultPrivate::makeStmtError(
                                    errorMessage, SqlDriverError::StatementError,
                                    d->stmt));
    } else {
        d->row = mysql_fetch_row(d->result);
        // CUR drivers missing error check silverqx
        if (d->row == nullptr)
            return false;
    }

    setAt(at() + 1);
    return true;
}

QVariant MySqlResult::data(const int index)
{
    Q_D(MySqlResult);

    // Nothing to do
    if (const auto fieldsCount = d->fields.size(); index >= fieldsCount) {
        qWarning().noquote()
                << QStringLiteral(
                       "MySqlResult::data: column index '%1' is out of range, "
                       "the current number of fields is '%2'")
                   .arg(index).arg(fieldsCount);
        return {};
    }

    if (d->preparedQuery)
        return d->getValueForPrepared(index);

    return d->getValueForNormal(index);
}

bool MySqlResult::isNull(const int index)
{
   Q_D(const MySqlResult);

    // CUR drivers test isNull() out of bounds if no metadata silverqx
   if (const auto fieldsCount = d->fields.size(); index < 0 || index >= fieldsCount)
       throw std::exception(
               QStringLiteral(
                   "Field index '%1' is out of bounds, the index must be between "
                   "0-%2")
               .arg(index).arg(fieldsCount - 1).toUtf8().constData());

   /* MyField::nullIndicator is populated for prepared statements only.
      The row/result set/data must be fetched first to obtain the correct result. ❗ */

   if (d->preparedQuery)
       /* The same value is in the inBinds.is_null
          (see MySqlResultPrivate::bindInValues()). */
       return d->fields.at(index).nullIndicator;

   else
       /* NULL values in the row are indicated by NULL pointers.
          Empty fields and fields containing NULL both have length 0;
          you can distinguish these by checking the pointer for the field value.
          If the pointer is NULL, the field is NULL; otherwise, the field is empty. */
       return d->row[index] == nullptr;
}

int MySqlResult::size()
{
    Q_D(const MySqlResult);

    // Don't cache the result as it's already cached in the MYSQL_RES/MYSQL_STMT
    if (d->preparedQuery)
        // CUR drivers fix all these int-s they must be quint64 silverqx
        return static_cast<int>(mysql_stmt_num_rows(d->stmt));
    else
        return static_cast<int>(mysql_num_rows(d->result));
}

int MySqlResult::numRowsAffected()
{
    Q_D(const MySqlResult);

    // Don't cache the result as it's already cached in the MYSQL_RES/MYSQL_STMT
    if (d->preparedQuery)
        return static_cast<int>(mysql_stmt_affected_rows(d->stmt));
    else
        return static_cast<int>(mysql_affected_rows(d->drv_d_func()->mysql));
}

void MySqlResult::detachFromResultSet()
{
    Q_D(MySqlResult);

    if (d->preparedQuery)
        mysql_stmt_free_result(d->stmt);
}

/* Others */

void MySqlResult::cleanup()
{
    Q_D(MySqlResult);

    mysqlFreeResults();
    mysqlStmtClose();

    if (d->meta != nullptr) {
        mysql_free_result(d->meta);
        d->meta = nullptr;
    }

    if (d->outBinds != nullptr) {
        delete[] d->outBinds;
        d->outBinds = nullptr;
    }

    if (d->inBinds != nullptr) {
        delete[] d->inBinds;
        d->inBinds = nullptr;
    }

    for (const auto &field : std::as_const(d->fields))
        delete[] field.outField;

    d->fields.clear();

    d->hasBlobs = false;
    d->preparedQuery = false;

    d->result = nullptr;
    d->row = nullptr;

    setAt(QSql::BeforeFirstRow);
    setActive(false);
}

/* private */

void MySqlResult::mysqlFreeResults() const
{
    Q_D(const MySqlResult);

    if (d->result != nullptr)
        mysql_free_result(d->result);

    /* Must iterate through leftover result sets from multi-selects or stored procedures
       if this isn't done then subsequent queries will fail with Commands out of sync. */
    const auto *const sqldriver = d->drv_d_func();

    // The d->sqldriver must always be set
    Q_ASSERT(d->sqldriver != nullptr);
    auto *const mysql = sqldriver->mysql;

    // Nothing to do, no more result set/s
    if (mysql == nullptr || !mysql_more_results(mysql))
        return;

    // More results? -1 = no, >0 = error, 0 = yes (keep looping)
    int status = -1;
    // This condition is weird but I don't want test if <= 0
    while (!((status = mysql_next_result(mysql)) > 0))

        if (auto *const res = mysql_store_result(mysql);
            res != nullptr
        ) T_LIKELY
            mysql_free_result(res);

        /* Check the error code because mysql_store_result() may return the nullptr if
           it succeeded or failed. */
        else if (const auto errNo = mysql_errno(mysql); errNo != 0) T_UNLIKELY
            qWarning().noquote()
                << QStringLiteral(
                       "MySqlResult::mysqlFreeResult: unable to store a result set "
                       "using the mysql_store_result(); %1: %2")
                   .arg(errNo).arg(mysql_error(mysql));

    // The mysql_next_result() returned an error
    if (status > 0)
        qWarning().noquote()
            << QStringLiteral(
                   "MySqlResult::mysqlFreeResult: unable to obtain the next "
                   "result set using the mysql_next_result(); %1: %2")
               .arg(mysql_errno(mysql))
               .arg(mysql_error(mysql));
}

void MySqlResult::mysqlStmtClose()
{
    Q_D(MySqlResult);

    // Nothing to do
    if (d->stmt == nullptr)
        return;

    if (mysql_stmt_close(d->stmt) != 0)
        qWarning("MySqlResult::mysqlStmtClose: unable to free statement handle");

    d->stmt = nullptr;
}

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE
