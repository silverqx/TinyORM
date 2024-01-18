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
    unsigned int year = 0;
    unsigned int month = 0;
    unsigned int day = 0;
    unsigned int hour = 0;
    unsigned int minute = 0;
    unsigned int second = 0;

    /*! The fractional part of the second in microseconds. */
    unsigned long second_part = 0; // NOLINT(google-runtime-int)
    my_bool neg = false;
    enum enum_mysql_timestamp_type time_type = MYSQL_TIMESTAMP_NONE;
};

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

using MySqlUtils = Orm::Drivers::MySql::MySqlUtilsPrivate;

namespace Orm::Drivers::MySql
{

/* public */

MySqlResult::MySqlResult(const std::weak_ptr<MySqlDriver> &driver)
    : SqlResult(std::make_unique<MySqlResultPrivate>(driver))
{}

MySqlResult::~MySqlResult() noexcept
{
    cleanupForDtor();
}

QVariant MySqlResult::handle() const
{
    Q_D(const MySqlResult);

    if (d->preparedQuery)
        // CUR check this later; I don't understand it now silverqx
        return d->meta == nullptr ? QVariant::fromValue(d->stmt)
                                  : QVariant::fromValue(d->meta);

    return QVariant::fromValue(d->result);
}

/* protected */

/* Normal queries */

bool MySqlResult::exec(const QString &query)
{
    Q_D(MySqlResult);

    cleanup();

    d->query = query.trimmed();

    auto *const mysql = d->drv_d_func()->mysql;

    // Execute query
    if (const auto queryArray = query.toUtf8();
        mysql_real_query(mysql, queryArray.constData(), queryArray.size()) != 0
    )
        return setLastError(MySqlUtils::createError(
                                u"Unable to execute query"_s,
                                SqlError::StatementError, mysql));
    // Obtain result set
    d->result = mysql_store_result(mysql);

    if (const auto errNo = mysql_errno(mysql);
        d->result == nullptr && errNo != 0
    )
        return setLastError(MySqlUtils::createError(
                                u"Unable to store result"_s,
                                SqlError::StatementError, mysql, errNo));

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

    cleanup();

    d->query = query.trimmed();
    d->preparedQuery = true;

    // Create the MYSQL_STMT handler
    {
        auto *const mysql = d->drv_d_func()->mysql;

        if (d->stmt = mysql_stmt_init(mysql); d->stmt == nullptr)
            return setLastError(MySqlUtils::createError(
                                    u"Unable to prepare statement"_s,
                                    SqlError::StatementError, mysql));
    }

    // Prepare the SQL statement
    const auto queryArray = query.toUtf8();

    if (mysql_stmt_prepare(d->stmt, queryArray.constData(), queryArray.size()) != 0)
        return setLastError(MySqlResultPrivate::createStmtError(
                                u"Unable to prepare statement"_s,
                                SqlError::StatementError, d->stmt));

    setSelect(d->bindResultValues());

    return true;
}

bool MySqlResult::exec()
{
    Q_D(MySqlResult);

    if (!d->preparedQuery)
        throw std::exception(
                "The prepared query is empty, to call normal queries use "
                "the SqlQuery::exec(QString) overload.");

    if (d->stmt == nullptr)
        return false;

    /* These mysql_stmt_xyz()-s functions are weird, if they return a bool, then
       they return zero for success and non-zero if an error occurred, but the type is
       bool, which means they return false if succeed and true if they failed! 🤔🙃😲 */

    // These vectors keep values alive long enough until mysql_stmt_execute() is invoked
    QList<my_bool> nullVector;
    QList<QByteArray> stringVector;
    QList<QT_MYSQL_TIME> timeVector;

    /* Reset a prepared statement on client and server to state after prepare,
       unbuffered result sets and current errors. */
    if (mysql_stmt_reset(d->stmt))
        return setLastError(MySqlResultPrivate::createStmtError(
                                u"Unable to reset statement"_s,
                                SqlError::StatementError, d->stmt));

    // Bind prepared bindings if the correct number of prepared bindings was bound
    if (d->shouldPrepareBindings(
            // Number of parameter placeholders in the prepared statement
            mysql_stmt_param_count(d->stmt))
    ) {
        d->bindPreparedBindings(nullVector, stringVector, timeVector);

        // Bind data for parameter placeholders 🕺
        if (mysql_stmt_bind_param(d->stmt, d->preparedBinds.get()))
            return setLastError(MySqlResultPrivate::createStmtError(
                                    u"Unable to bind data for parameter placeholders"_s,
                                    SqlError::StatementError, d->stmt));
    }

    // Execute prepared query 🥳
    if (mysql_stmt_execute(d->stmt) != 0)
        return setLastError(MySqlResultPrivate::createStmtError(
                                u"Unable to execute prepared statement"_s,
                                SqlError::StatementError, d->stmt));

    // Executed query has result set, if there are metadata there are also result sets
    setSelect(d->meta != nullptr);

    if (isSelect()) {
        my_bool update_max_length = true;

        // Bind output columns in the result set to data buffers and length buffers
        if (mysql_stmt_bind_result(d->stmt, d->resultBinds.get()))
            return setLastError(MySqlResultPrivate::createStmtError(
                                    u"Unable to bind result set data buffers"_s,
                                    SqlError::StatementError, d->stmt));

        // Update the metadata MYSQL_FIELD->max_length value
        if (d->hasBlobs)
            mysql_stmt_attr_set(d->stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &update_max_length);

        // Buffer the complete result set on the client (will be prepared for fetching)
        if (mysql_stmt_store_result(d->stmt) != 0)
            return setLastError(MySqlResultPrivate::createStmtError(
                                    u"Unable to store prepared statement result sets"_s,
                                    SqlError::StatementError, d->stmt));

        // CUR drivers revisit try to achieve or avoid calling the mysql_stmt_bind_result() twice silverqx
        if (d->hasBlobs) {
            d->bindResultBlobs();

            // Re-bind output columns in the result set to data buffers and length buffers
            if (mysql_stmt_bind_result(d->stmt, d->resultBinds.get()))
                return setLastError(MySqlResultPrivate::createStmtError(
                                        u"Unable to re-bind result set data buffers "
                                        "for BLOB-s"_s,
                                        SqlError::StatementError, d->stmt));
        }

        setAt(BeforeFirstRow);
    }

    setActive(true);

    return true;
}

/* Result sets */

SqlRecord MySqlResult::record() const
{
    Q_D(const MySqlResult);
    // CUR drivers check if is a good idea to fetch fields again because all fields are already cached in the MyField:::myField silverqx

    auto *const mysqlRes = d->preparedQuery ? d->meta : d->result;
    // Backup the current cursor position
    const auto currentCursor = mysql_field_tell(mysqlRes);
    // Seek to the beginning
    mysql_field_seek(mysqlRes, 0);

    SqlRecord result;
    const MYSQL_FIELD *fieldInfo = nullptr;

    while ((fieldInfo = mysql_fetch_field(mysqlRes)) != nullptr)
        result.append(MySqlUtils::convertToSqlField(fieldInfo));

    // Restore the cursor position
    mysql_field_seek(mysqlRes, currentCursor);

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
           Also, the MyField::isNull and resultBinds.is_null is populated
           (see MySqlResultPrivate::bindResultValues()). */
        const auto status = mysql_stmt_fetch(d->stmt);

        // Success, no more data exists; returning false here is correct
        if (status == MYSQL_NO_DATA)
            return false;

        // This means there was an error or data were truncated
        if (const auto errorMessage = MySqlResultPrivate::fetchErrorMessage(status);
            errorMessage
        )
            return setLastError(MySqlResultPrivate::createStmtError(
                                    *errorMessage, SqlError::StatementError, d->stmt));
    } else {
        mysql_data_seek(d->result, index);

        if (d->row = mysql_fetch_row(d->result);
            d->row == nullptr
        )
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

        // This means there was an error or data were truncated
        if (const auto errorMessage = MySqlResultPrivate::fetchErrorMessage(status);
            errorMessage
        )
            return setLastError(MySqlResultPrivate::createStmtError(
                                    *errorMessage, SqlError::StatementError, d->stmt));
    } else {
        // CUR drivers missing error check silverqx
        if (d->row = mysql_fetch_row(d->result);
            d->row == nullptr
        )
            return false;
    }

    setAt(at() + 1);
    return true;
}

QVariant MySqlResult::data(const int index)
{
    Q_D(MySqlResult);

    const auto idx = static_cast<MySqlResultPrivate::ResultFieldsSizeType>(index);

    // Throw an exception if an index for result fields vector is out of bounds
    throwIfBadResultFieldsIndex(idx);

    if (d->preparedQuery)
        return d->getValueForPrepared(idx);

    return d->getValueForNormal(idx);
}

bool MySqlResult::isNull(const int index)
{
    Q_D(const MySqlResult);

    const auto idx = static_cast<MySqlResultPrivate::ResultFieldsSizeType>(index);

    // CUR drivers test isNull() out of bounds if no metadata silverqx
    // Throw an exception if an index for result fields vector is out of bounds
    throwIfBadResultFieldsIndex(idx);

   /* MyField::isNull is populated for prepared statements only.
      The row/result set/data must be fetched first to obtain the correct result. ❗ */

   if (d->preparedQuery)
       /* The same value is in the resultBinds.is_null
          (see MySqlResultPrivate::bindResultValues()). */
       return d->resultFields[idx].isNull;

   /* NULL values in the row are indicated by NULL pointers.
      Empty fields and fields containing NULL both have length 0;
      you can distinguish these by checking the pointer for the field value.
      If the pointer is NULL, the field is NULL; otherwise, the field is empty. */
   return d->row[idx] == nullptr;
}

int MySqlResult::size()
{
    Q_D(const MySqlResult);

    // BUG drivers also verify all removed isXyz() conditions if the is some possible nullptr behavior; omg how I will fix this silverqx
    // BUG drivers test this, also test affected for select queries, guarantee the same behavior as QtSql module silverqx
    // Nothing to do
    if (!isSelect())
        return -1;

    // Don't cache the result as it's already cached in the MYSQL_RES/MYSQL_STMT
    if (d->preparedQuery)
        // CUR drivers fix all these int-s they must be quint64 silverqx
        return static_cast<int>(mysql_stmt_num_rows(d->stmt));

    return static_cast<int>(mysql_num_rows(d->result));
}

int MySqlResult::numRowsAffected()
{
    Q_D(const MySqlResult);

    // Don't cache the result as it's already cached in the MYSQL_RES/MYSQL_STMT
    if (d->preparedQuery)
        return static_cast<int>(mysql_stmt_affected_rows(d->stmt));

    return static_cast<int>(mysql_affected_rows(d->drv_d_func()->mysql));
}

void MySqlResult::detachFromResultSet()
{
    Q_D(MySqlResult);

    if (d->preparedQuery)
        mysql_stmt_free_result(d->stmt);

    // CUR drivers what about mysql_free_result()? Also look SqlQuery::finish() silverqx
}

/* Cleanup */

// CUR drivers revisit all 4 free methods detachFromResultSet(), cleanup(), mysqlFreeResults(), and mysqlStmtClose() how they relate and how they are used silverqx
void MySqlResult::cleanup()
{
    Q_D(MySqlResult);

    // Normal queries
    mysqlFreeResults();

    // Prepared queries
    mysqlStmtClose();

    d->preparedBinds.reset();
    d->resultBinds.reset();

    // Common code for both cleanup methods
    cleanupCommon();
}

/* private */

/* Cleanup */

void MySqlResult::cleanupForDtor() noexcept
{
    Q_D(MySqlResult);

    // Normal queries
    mysqlFreeResultsForDtor();

    // Prepared queries
    // d->stmt != nullptr check is NOT inside the mysql_stmt_close()
    if (d->stmt != nullptr)
        mysql_stmt_close(d->stmt);
    d->stmt = nullptr;

    /* The d->preparedBinds and d->resultBinds will be auto-destroyed if called
       from the destructor as they are smart pointers. */

    // Common code for both cleanup methods
    cleanupCommon();
}

void MySqlResult::cleanupCommon() noexcept
{
    Q_D(MySqlResult);

    // d->meta != nullptr check is inside as the first thing
    mysql_free_result(d->meta);
    d->meta = nullptr;

    d->hasBlobs = false;
    d->preparedQuery = false;

    // Common for both
    // The MyField.fieldValue buffer will be auto-freed as it's a smart pointer
    d->resultFields.clear();

    setAt(BeforeFirstRow);
    setActive(false);
}

// CUR drivers revisit is something similar needed also for prepared statements? Look at detachFromResultSet() silverqx
void MySqlResult::mysqlFreeResults()
{
    Q_D(MySqlResult);

    // d->result != nullptr check is inside as the first thing
    mysql_free_result(d->result);

    /* Must iterate through leftover result sets from multi-selects or stored procedures
       if this isn't done then subsequent queries will fail with Commands out of sync. */
    auto *const mysql = d->drv_d_func()->mysql;

    // Nothing to do, no more result set/s
    if (mysql == nullptr || !mysql_more_results(mysql))
        return;

    // More results? -1 = no, >0 = error, 0 = yes (keep looping)
    int status = -1;
    while ((status = mysql_next_result(mysql)) == 0)

        if (auto *const mysqlRes = mysql_store_result(mysql);
            mysqlRes != nullptr
        ) T_LIKELY
            mysql_free_result(mysqlRes);

        // CUR driver use createError(); create a new eg. ErrorType::FreeError? silverqx
        /* Check the error code because mysql_store_result() may return the nullptr if
           it succeeded or failed. */
        else if (const auto errNo = mysql_errno(mysql); errNo != 0) T_UNLIKELY
            qWarning().noquote()
                << u"MySqlResult::mysqlFreeResult: unable to store a result set "
                    "using the mysql_store_result(); %1: %2"_s
                   .arg(errNo).arg(QString::fromUtf8(mysql_error(mysql)));

    // The mysql_next_result() returned an error
    if (status > 0)
        qWarning().noquote()
            << u"MySqlResult::mysqlFreeResult: unable to obtain the next "
                "result set using the mysql_next_result(); %1: %2"_s
               .arg(mysql_errno(mysql))
               .arg(QString::fromUtf8(mysql_error(mysql)));

    d->result = nullptr;
    d->row = nullptr;
}

void MySqlResult::mysqlFreeResultsForDtor() noexcept
{
    Q_D(MySqlResult);

    // d->result != nullptr check is inside as the first thing
    mysql_free_result(d->result);

    /* Must iterate through leftover result sets from multi-selects or stored procedures
       if this isn't done then subsequent queries will fail with Commands out of sync.
       Also, the drv_d_func_noexcept() is needed to have the noexcept destructor. */
    auto *const mysql = d->drv_d_func_noexcept()->mysql;

    // Nothing to do, no more result set/s
    if (mysql == nullptr || !mysql_more_results(mysql))
        return;

    // More results? -1 = no, >0 = error, 0 = yes (keep looping)
    while (mysql_next_result(mysql) == 0)
        if (auto *const mysqlRes = mysql_store_result(mysql); mysqlRes != nullptr)
            mysql_free_result(mysqlRes);

    d->result = nullptr;
    d->row = nullptr;
}

void MySqlResult::mysqlStmtClose()
{
    Q_D(MySqlResult);

    // Nothing to do
    if (d->stmt == nullptr)
        return;

    if (mysql_stmt_close(d->stmt))
        qWarning("MySqlResult::mysqlStmtClose: unable to free statement handle");

    d->stmt = nullptr;
}

/* Others */

void MySqlResult::throwIfBadResultFieldsIndex(const std::size_t index) const
{
    Q_D(const MySqlResult);

    const auto fieldsCount = d->resultFields.size();

    // Nothing to do
    if (index >= 0 || index < fieldsCount)
        return;

    throw std::exception(
                u"Field index '%1' is out of bounds, the index must be between 0-%2"_s
                .arg(index).arg(fieldsCount - 1).toUtf8().constData());
}

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE
