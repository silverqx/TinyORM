#include "orm/drivers/mysql/mysqlresult.hpp"

#include <QDateTime>

#include "orm/drivers/exceptions/queryerror.hpp"
#include "orm/drivers/mysql/mysqldriver_p.hpp"
#include "orm/drivers/mysql/mysqlresult_p.hpp"
#include "orm/drivers/mysql/mysqlutils_p.hpp"
#include "orm/drivers/sqlrecord.hpp"

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

QVariant MySqlResult::handle() const noexcept
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
        mysql_real_query(mysql, queryArray.constData(),
                         static_cast<ulong>(queryArray.size())) != 0
    )
        throw Exceptions::QueryError(
                d->connectionName,
                u"Unable to execute the normal query in %1()."_s.arg(__tiny_func__),
                MySqlUtils::prepareMySqlError(mysql), d->query);

    // Obtain the Result Set
    d->result = mysql_store_result(mysql);

    if (const auto errNo = mysql_errno(mysql);
        d->result == nullptr && errNo != 0
    )
        throw Exceptions::QueryError(
                d->connectionName,
                u"Unable to store the result for the normal query in %1()."_s
                .arg(__tiny_func__),
                MySqlUtils::prepareMySqlError(mysql, errNo), d->query);

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
            throw Exceptions::SqlError(
                        u"Unable to prepare the MYSQL_STMT handler in %1()."_s
                        .arg(__tiny_func__),
                        MySqlUtils::prepareMySqlError(mysql));
    }

    // Prepare the SQL statement
    const auto queryArray = query.toUtf8();

    if (mysql_stmt_prepare(d->stmt, queryArray.constData(),
                           static_cast<ulong>(queryArray.size())) != 0
    )
        throw Exceptions::QueryError(
                d->connectionName,
                u"Unable to prepare the prepared statement in %1()."_s.arg(__tiny_func__),
                MySqlUtils::prepareStmtError(d->stmt), d->query);

    setSelect(d->bindResultValues());

    return true;
}

bool MySqlResult::exec()
{
    Q_D(MySqlResult);

    if (!d->preparedQuery)
        throw Exceptions::LogicError(
                u"The prepared query is empty, call the SqlQuery::prepare() first "
                 "for prepared statements or pass the query string directly "
                 "to the SqlQuery::exec(QString) for normal statements in %1()."_s
                .arg(__tiny_func__));

    if (d->stmt == nullptr)
        return false;

    /* These mysql_stmt_xyz()-s functions are weird, if they return a bool, then
       they return zero for success and non-zero if an error occurred, but the type is
       bool, which means they return false if succeed and true if they failed! 🤔🙃😲 */

    // These vectors keep values alive long enough until mysql_stmt_execute() is invoked
    QList<my_bool> nullVector;
    QList<QByteArray> stringVector;
    QList<MYSQL_TIME> timeVector;

    /* Reset a prepared statement on the client and server side to the state after
       the prepare (resets the statement on the server, unbuffered result sets, and
       clear current errors). */
    if (mysql_stmt_reset(d->stmt))
        throw Exceptions::QueryError(
                d->connectionName,
                u"Unable to reset statement in %1()."_s.arg(__tiny_func__),
                MySqlUtils::prepareStmtError(d->stmt), d->query);

    // Bind prepared bindings if the correct number of prepared bindings was bound
        // Number of parameter placeholders in the prepared statement
    if (const auto paramCount = static_cast<uint>(mysql_stmt_param_count(d->stmt));
        d->shouldPrepareBindings(paramCount)
    ) {
        d->bindPreparedBindings(nullVector, stringVector, timeVector);

        // Bind data for parameter placeholders 🕺
#if defined(MARIADB_VERSION_ID) || MYSQL_VERSION_ID < 80300
        if (mysql_stmt_bind_param(d->stmt, d->preparedBinds.get()))
#else
        if (mysql_stmt_bind_named_param(d->stmt, d->preparedBinds.get(), paramCount,
                                        nullptr)
        )
#endif
            throw Exceptions::QueryError(
                    d->connectionName,
                    u"Unable to bind data for parameter placeholders in %1()."_s
                    .arg(__tiny_func__),
                    MySqlUtils::prepareStmtError(d->stmt), d->query);
    }

    // Execute prepared query 🥳
    if (mysql_stmt_execute(d->stmt) != 0)
        throw Exceptions::QueryError(
                d->connectionName,
                u"Unable to execute the prepared statement in %1()."_s.arg(__tiny_func__),
                MySqlUtils::prepareStmtError(d->stmt), d->query, d->boundValues);

    // Executed query has result set, if there are metadata there are also result sets
    setSelect(d->meta != nullptr);

    if (isSelect()) {
        my_bool update_max_length = true;

        // Bind output columns in the result set to data buffers and length buffers
        if (mysql_stmt_bind_result(d->stmt, d->resultBinds.get()))
            throw Exceptions::QueryError(
                    d->connectionName,
                    u"Unable to bind result set data buffers in %1()"_s
                    .arg(__tiny_func__),
                    MySqlUtils::prepareStmtError(d->stmt), d->query, d->boundValues);

        // Update the metadata MYSQL_FIELD->max_length value
        if (d->hasBlobs)
            mysql_stmt_attr_set(d->stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &update_max_length);

        // Buffer the complete result set on the client (will be prepared for fetching)
        if (mysql_stmt_store_result(d->stmt) != 0)
            throw Exceptions::QueryError(
                    d->connectionName,
                    u"Unable to store result sets for the prepared statement in %1()."_s
                    .arg(__tiny_func__),
                    MySqlUtils::prepareStmtError(d->stmt), d->query, d->boundValues);

        // CUR drivers revisit try to achieve or avoid calling the mysql_stmt_bind_result() twice silverqx
        if (d->hasBlobs) {
            d->bindResultBlobs();

            // Re-bind output columns in the result set to data buffers and length buffers
            if (mysql_stmt_bind_result(d->stmt, d->resultBinds.get()))
                throw Exceptions::QueryError(
                        d->connectionName,
                        u"Unable to re-bind result set data buffers for BLOB-s in %1()."_s
                        .arg(__tiny_func__),
                        MySqlUtils::prepareStmtError(d->stmt), d->query);
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
#if defined(__LP64__) || defined(_LP64)
            // To avoid ambiguous overload
            return QVariant::fromValue(id);
#else
            return id;
#endif
    }
    else {
        if (const auto id = mysql_insert_id(d->drv_d_func()->mysql);
            id != 0
        )
#if defined(__LP64__) || defined(_LP64)
            // To avoid ambiguous overload
            return QVariant::fromValue(id);
#else
            return id;
#endif
    }

    return {};
}

bool MySqlResult::fetch(const size_type index)
{
    Q_D(MySqlResult);

    // Nothing to fetch, an empty result set
    if (size() == 0)
        return false;

    // Cursor is already on the requested row/result
    if (at() == index)
        return true;

    // Fetch the next row in the result set
    if (d->preparedQuery) {
        mysql_stmt_data_seek(d->stmt, static_cast<quint64>(index));

        if (!mysqlStmtFetch())
            return false;
    }
    else {
        mysql_data_seek(d->result, static_cast<quint64>(index));

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

    // Fetch the next row in the result set
    if (d->preparedQuery) {
        if (!mysqlStmtFetch())
            return false;
    }
    else {
        // CUR drivers missing error check silverqx
        if (d->row = mysql_fetch_row(d->result);
            d->row == nullptr
        )
            return false;
    }

    setAt(at() + 1);
    return true;
}

QVariant MySqlResult::data(const size_type index)
{
    Q_D(MySqlResult);

    const auto idx = static_cast<MySqlResultPrivate::ResultFieldsSizeType>(index);

    // Throw an exception if an index for result fields vector is out of bounds
    d->throwIfBadResultFieldsIndex(idx);

    if (d->preparedQuery)
        return d->getValueForPrepared(idx);

    return d->getValueForNormal(idx);
}

bool MySqlResult::isNull(const size_type index)
{
    Q_D(const MySqlResult);

    const auto idx = static_cast<MySqlResultPrivate::ResultFieldsSizeType>(index);

    // CUR drivers test isNull() out of bounds if no metadata silverqx
    // Throw an exception if an index for result fields vector is out of bounds
    d->throwIfBadResultFieldsIndex(idx);

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

MySqlResult::size_type MySqlResult::size() noexcept
{
    Q_D(const MySqlResult);

    // BUG drivers also verify all removed isXyz() conditions if the is some possible nullptr behavior; omg how I will fix this silverqx
    // BUG drivers test this, also test affected for select queries, guarantee the same behavior as QtSql module silverqx
    // Nothing to do
    if (!isSelect())
        return -1;

    // Don't cache the result as it's already cached in the MYSQL_RES/MYSQL_STMT
    if (d->preparedQuery)
        return static_cast<size_type>(mysql_stmt_num_rows(d->stmt));

    return static_cast<size_type>(mysql_num_rows(d->result));
}

MySqlResult::size_type MySqlResult::numRowsAffected()
{
    Q_D(const MySqlResult);

    // Don't cache the result as it's already cached in the MYSQL_RES/MYSQL_STMT
    if (d->preparedQuery)
        return static_cast<size_type>(mysql_stmt_affected_rows(d->stmt));

    // CUR drivers try to pass d->result like above and make it noexcept and also SqlQuery::numRowsAffected() silverqx
    return static_cast<size_type>(mysql_affected_rows(d->drv_d_func()->mysql));
}

void MySqlResult::detachFromResultSet() const noexcept
{
    Q_D(const MySqlResult);

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

    // d->meta != nullptr check is inside as the first thing
    mysql_free_result(d->meta);
    d->meta = nullptr;

    d->preparedBinds.reset();
    d->resultBinds.reset();

    d->hasBlobs = false;
    d->preparedQuery = false;

    // Common code for both
    /* The MyField.fieldValue buffer will be auto-freed as it's a smart pointer and
       the MyField::myField is freed during the mysql_free_result() call. */
    d->resultFields.clear();
    d->boundValues.clear();

    setActive(false);
    setAt(BeforeFirstRow);
}

/* private */

/* Result sets */

bool MySqlResult::mysqlStmtFetch()
{
    Q_D(const MySqlResult);

    /* Here are fetched real data and lengths into the buffers that were bound by
       the mysql_stmt_bind_result(). 🥳
       Also, the MyField::isNull and resultBinds.is_null is populated
       (see MySqlResultPrivate::bindResultValues()). */
    const auto status = mysql_stmt_fetch(d->stmt);

    // Success, no more data exists; returning false here is correct
    if (status == MYSQL_NO_DATA)
        return false;

    // This means there was an error or data were truncated
    if (const auto errorMessage = MySqlResultPrivate::errorMessageForStmtFetch(status);
        errorMessage
    )
        throw Exceptions::QueryError(d->connectionName, errorMessage->arg(__tiny_func__),
                                     MySqlUtils::prepareStmtError(d->stmt), d->query);

    return true;
}

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

    // d->meta != nullptr check is inside as the first thing
    mysql_free_result(d->meta);
    d->meta = nullptr;

    /* The d->preparedBinds and d->resultBinds will be auto-destroyed if called
       from the destructor as they are smart pointers. */
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
    // Free the memory allocated for multi-result sets
    mysqlFreeMultiResultsForDtor();

    d->result = nullptr;
    d->row = nullptr;
}

void MySqlResult::mysqlFreeMultiResultsForDtor() noexcept
{
    Q_D(MySqlResult);

    // BUG drivers? find out if this free-ing multi-results logic is really needed, TinyDrivers currectly doesn't support the nextResult() so next results will never be read and stored on the client; and if it will support this then before next result will be stored we will free the current result so it looks like this is useless silverqx
    /* Must iterate through leftover result sets from multi-selects or stored procedures
       if this isn't done then subsequent queries will fail with Commands out of sync.
       Also, the drv_d_func_noexcept() is needed to have the noexcept destructor. */
    const auto *const driver = d->drv_d_func_noexcept();

    // Nothing to do, the MySQL driver was invalidated early using the removeConnection()
    if (driver == nullptr)
        return;

    auto *const mysql = driver->mysql;

    // Nothing to do, no more result set/s
    if (mysql == nullptr || !mysql_more_results(mysql))
        return;

    // More results? -1 = no, >0 = error, 0 = yes (keep looping)
    while (mysql_next_result(mysql) == 0)
        if (auto *const mysqlRes = mysql_store_result(mysql); mysqlRes != nullptr)
            mysql_free_result(mysqlRes);
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

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE
