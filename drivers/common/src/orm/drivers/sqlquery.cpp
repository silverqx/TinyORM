#include "orm/drivers/sqlquery.hpp"

/* I'll keep the QT_DEBUG_SQL C macro to be compatible with the QtSql module, so
   the QtSql module and the TinyDrivers are interchangeable. */

/*! Log an executed query, elapsed time, and query /affected size to the stderr. */
//#define QT_DEBUG_SQL
/*! Log an executed query, elapsed time, and query /affected size to the stderr. */
//#define TINYDRIVERS_DEBUG_SQL

#if defined(QT_DEBUG_SQL) || defined(TINYDRIVERS_DEBUG_SQL)
#  include <QDebug>
#  include <QElapsedTimer>
#endif

#include "orm/drivers/dummysqlerror.hpp"
#include "orm/drivers/exceptions/invalidargumenterror.hpp"
#include "orm/drivers/sqldatabase.hpp"
#include "orm/drivers/sqldriver.hpp"
#include "orm/drivers/sqlrecord.hpp"
#include "orm/drivers/sqlresult.hpp"
#include "orm/drivers/utils/type_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

namespace Orm::Drivers
{

/* public */

SqlQuery::SqlQuery()
    : m_sqlResult(initSqlResult())
{}

SqlQuery::SqlQuery(const SqlDatabase &connection)
    : m_sqlResult(initSqlResult(connection))
{}

SqlQuery::SqlQuery(std::unique_ptr<SqlResult> &&result) noexcept
    : m_sqlResult(std::move(result))
{}

/* The destructor, move/assign constructors, and the swap() method must be defined
   in the cpp file because the m_sqlResult is unique_ptr. If they are defined as inline
   then the compilation fails because a unique_ptr can't destroy an incomplete type. */

SqlQuery::~SqlQuery() = default;

SqlQuery::SqlQuery(SqlQuery &&) noexcept = default;

SqlQuery &SqlQuery::operator=(SqlQuery &&) noexcept = default;

void SqlQuery::swap(SqlQuery &other) noexcept
{
    std::swap(m_sqlResult, other.m_sqlResult);
}

/* Getters / Setters */

bool SqlQuery::isValid() const noexcept
{
    return m_sqlResult->isValid();
}

QString SqlQuery::executedQuery() const noexcept
{
    return m_sqlResult->query();
}

QString SqlQuery::lastQuery() const noexcept
{
    return m_sqlResult->query();
}

DummySqlError SqlQuery::lastError() const noexcept // NOLINT(readability-convert-member-functions-to-static)
{
    return {};
}

SqlQuery::size_type SqlQuery::at() const noexcept
{
    return m_sqlResult->at();
}

bool SqlQuery::isActive() const noexcept
{
    return m_sqlResult->isActive();
}

bool SqlQuery::isSelect() const noexcept
{
    /* isSelect() practically means that the executed query has a result set (data that
       can be looped over), it could also be named hasResultSet(). */
    return m_sqlResult->isSelect();
}

NumericalPrecisionPolicy SqlQuery::numericalPrecisionPolicy() const noexcept
{
    return m_sqlResult->numericalPrecisionPolicy();
}

void
SqlQuery::setNumericalPrecisionPolicy(const NumericalPrecisionPolicy precision) noexcept
{
    m_sqlResult->setNumericalPrecisionPolicy(precision);
}

/* The reason why the non-const methods are not provided is because they are not needed,
   everything can be accessed through getter-s, setter-s, or other methods and if
   something is not accessible then it has to be exposed through some method. */

const SqlDriver *SqlQuery::driver() const noexcept
{
    /* I must provide this raw pointer method to have compatible API with QtSql and that
       users can avoid #ifdef-s in their code. There is no way to remove this method. */
    return m_sqlResult->driver().lock().get();
}

std::weak_ptr<const SqlDriver> SqlQuery::driverWeak() const noexcept
{
    /* This must be the std::weak_ptr() because when the connection is removed from
       the SqlDatabaseManager using the SqlDatabase::removeDatabase() then the SqlDriver
       will be invalidated immediately (using the std::shared_ptr::reset()).
       This means we don't want to keep the SqlDriver alive after removeDatabase(). */
    return m_sqlResult->driver();
}

const SqlResult &SqlQuery::result() const noexcept // noexcept is correct here
{
    // This method exists only to access the SqlResult::handle() method
    return *m_sqlResult;
}

/* Normal queries */

bool SqlQuery::exec(const QString &query)
{
    throwIfNoDatabaseConnection();
    throwIfEmptyQueryString(query);

#if defined(QT_DEBUG_SQL) || defined(TINYDRIVERS_DEBUG_SQL)
    QElapsedTimer timer;
    timer.start();
#endif

    const auto result = m_sqlResult->exec(query);

#if defined(QT_DEBUG_SQL) || defined(TINYDRIVERS_DEBUG_SQL)
    qDebug().nospace() << "Executed normal query (" << timer.elapsed() << "ms, "
                       << m_sqlResult->size() << " results, "
                       << m_sqlResult->numRowsAffected() << " affected): "
                       << m_sqlResult->query();
#endif

    return result;
}

/* Prepared queries */

bool SqlQuery::prepare(const QString &query)
{
    throwIfNoDatabaseConnection();
    throwIfEmptyQueryString(query);

    return m_sqlResult->prepare(query);
}

bool SqlQuery::exec()
{
    // Nothing to do
    if (m_sqlResult->query().isEmpty())
        throw Exceptions::LogicError(
                u"The prepared query is empty, call the SqlQuery::prepare() first "
                 "for prepared statements or pass the query string directly "
                 "to the SqlQuery::exec(QString) for normal statements, "
                 "for '%1' database connection in %2()."_s
                .arg(connectionName(), __tiny_func__));

#if defined(QT_DEBUG_SQL) || defined(TINYDRIVERS_DEBUG_SQL)
    QElapsedTimer t;
    t.start();
#endif

    const auto result = m_sqlResult->exec();

#if defined(QT_DEBUG_SQL) || defined(TINYDRIVERS_DEBUG_SQL)
    qDebug().nospace() << "Executed prepared query (" << t.elapsed() << "ms, "
                       << m_sqlResult->size() << " results, "
                       << m_sqlResult->numRowsAffected() << " affected): "
                       << m_sqlResult->query();
#endif

    return result;
}

void SqlQuery::bindValue(const size_type index, const QVariant &value,
                         const ParamType /*unused*/)
{
    /* Need to pass the ParamType::In to preserve the same API because I can't remove this
       parameter so I need to pass something but it has no effect. */
    m_sqlResult->bindValue(index, value, ParamType::In);
}

void SqlQuery::bindValue(const size_type index, QVariant &&value,
                         const ParamType /*unused*/)
{
    /* Need to pass the ParamType::In to preserve the same API because I can't remove this
       parameter so I need to pass something but it has no effect. */
    m_sqlResult->bindValue(index, std::move(value), ParamType::In);
}

void SqlQuery::addBindValue(const QVariant &value, const ParamType /*unused*/)
{
    // Append, index after the last value
    const auto index = m_sqlResult->boundValuesCount();

    /* Need to pass the ParamType::In to preserve the same API because I can't remove this
       parameter so I need to pass something but it has no effect. */
    m_sqlResult->bindValue(index, value, ParamType::In);
}

void SqlQuery::addBindValue(QVariant &&value, const ParamType /*unused*/)
{
    // Append, index after the last value
    const auto index = m_sqlResult->boundValuesCount();

    /* Need to pass the ParamType::In to preserve the same API because I can't remove this
       parameter so I need to pass something but it has no effect. */
    m_sqlResult->bindValue(index, std::move(value), ParamType::In);
}

QVariant SqlQuery::boundValue(const size_type index) const
{
    return m_sqlResult->boundValue(index);
}

QVariantList SqlQuery::boundValues() const
{
    return m_sqlResult->boundValues();
}

/* Result sets */

SqlRecord SqlQuery::record() const
{
    throwIfNoResultSet();

    /* Will provide information about all fields such as length, precision,
       SQL column type, auto-incrementing, ..., and also the field value. */
    return m_sqlResult->record();
}

const SqlRecord &SqlQuery::recordCached() const
{
    throwIfNoResultSet();

    /* The record will be cached for better performance, it avoids materialization
       of the record again and again. Cache is invalidated during seek(), fetchXyz()
       operations or if executing a new query on the same instance or re-executing
       query. */
    return m_sqlResult->recordCached();
}

bool SqlQuery::next()
{
    // Nothing to do
    if (!isActive() || !isSelect())
        return false;

    switch (at()) {
    case BeforeFirstRow:
        return m_sqlResult->fetchFirst();

    case AfterLastRow:
        return false;

    default:
        if (m_sqlResult->fetchNext())
            return true;

        m_sqlResult->setAt(AfterLastRow);
        return false;
    }
}

bool SqlQuery::previous()
{
    // Nothing to do
    if (!isActive() || !isSelect())
        return false;

    switch (at()) {
    case BeforeFirstRow:
        return false;

    case AfterLastRow:
        return m_sqlResult->fetchLast();

    default:
        if (m_sqlResult->fetchPrevious())
            return true;

        m_sqlResult->setAt(BeforeFirstRow);
        return false;
    }
}

bool SqlQuery::first()
{
    // Nothing to do
    if (!isActive() || !isSelect())
        return false;

    return m_sqlResult->fetchFirst();
}

bool SqlQuery::last()
{
    if (!isActive() || !isSelect())
        return false;

    return m_sqlResult->fetchLast();
}

bool SqlQuery::seek(const size_type index, const bool relative)
{
    // Nothing to do
    if (!isActive() || !isSelect())
        return false;

    // Nothing to fetch, an empty result set
    if (size() == 0)
        return false;

    auto actualIdx = static_cast<size_type>(BeforeFirstRow);

    // Arbitrary seek
    if (!relative && !seekArbitrary(index, actualIdx))
        return false;

    // Relative seek
    if (relative && !seekRelative(index, actualIdx))
        return false;

    return mapSeekToFetch(actualIdx);
}

QVariant SqlQuery::value(const size_type index) const
{
    throwIfNoValidResultSet();

    return m_sqlResult->data(index);
}

QVariant SqlQuery::value(const QString &name) const
{
    throwIfNoValidResultSet();

    if (const auto index = m_sqlResult->recordCached().indexOf(name); index > -1)
        return m_sqlResult->data(index);

    throwNoFieldName(name);
}

bool SqlQuery::isNull(const size_type index) const
{
    throwIfNoValidResultSet();

    return m_sqlResult->isNull(index);
}

bool SqlQuery::isNull(const QString &name) const
{
    throwIfNoValidResultSet();

    if (const auto index = m_sqlResult->recordCached().indexOf(name); index > -1)
        return m_sqlResult->isNull(index);

    throwNoFieldName(name);
}

SqlQuery::size_type SqlQuery::size() const
{
    // Nothing to do
    if (const auto driver = driverWeak().lock();
        !driver->hasFeature(SqlDriver::QuerySize)
    )
        throw Exceptions::LogicError(
                u"The '%1' database driver doesn't support query size reporting, "
                 "for '%2' database connection in %3()."_s
                .arg(driver->driverName(), connectionName(), __tiny_func__));

    throwIfNoResultSet();

    return m_sqlResult->size();
}

SqlQuery::size_type SqlQuery::numRowsAffected() const
{
    /* Nothing to do, query was not executed, also don't check the isSelect() here
       to have the same API as QtSql. */
    throwIfNoActiveQuery();

    return m_sqlResult->numRowsAffected();
}

QVariant SqlQuery::lastInsertId() const
{
    // Nothing to do, query was not executed
    throwIfNoActiveQuery();

    return m_sqlResult->lastInsertId();
}

/* Others */

/* clear() vs finish()

   clear() resets the SqlQuery instance to the initial state, it creates a new SqlResult
   (eg. MySqlResult) and also a new SqlQuery and moves this new instance to the *this.

   finish() instructs the database driver that no more data will be fetched
   from this query until it is re-executed. Sets a query to an inactive state and resets
   the cursor to initial state (BeforeFirstRow).

   Also, eg. for MySQL prepared statements it releases memory associated with
   the result set produced by execution of the prepared statement. If there is a cursor
   open for the statement, mysql_stmt_free_result() closes it. Free the client side memory
   buffers, reset long data state on client if necessary, and reset the server side
   statement if this has been requested.

   Bound values retain their values. */

void SqlQuery::clear()
{
    // Ownership of a weak_ptr()
    const auto driver = driverWeakInternal();

                     // Get a new empty SqlResult instance
    *this = SqlQuery(driver.lock()->createResult(driver));
}

void SqlQuery::finish() noexcept
{
    // Nothing to do
    if (!isActive())
        return;

    m_sqlResult->setActive(false);
    m_sqlResult->setAt(BeforeFirstRow);
    m_sqlResult->detachFromResultSet();
}

/* private */

/* Getters / Setters */

/* Leave the non-const driverWeak() private, it's correct, it's not needed anywhere and
   is for special cases only. */

std::weak_ptr<SqlDriver> SqlQuery::driverWeakInternal() noexcept
{
    /* This must be the std::weak_ptr() because when the connection is removed from
       the SqlDatabaseManager using the SqlDatabase::removeDatabase() then the SqlDriver
       is invalidated (using the std::shared_ptr::reset()).
       This means we don't want to keep the SqlDriver alive after removeDatabase(). */
    return m_sqlResult->driver();
}

QString SqlQuery::connectionName() const noexcept
{
    return m_sqlResult->connectionName();
}

/* Common for both */

void SqlQuery::throwIfNoDatabaseConnection()
{
    // Nothing to do
    if (this->driverWeakInternal().lock()->isOpen())
        return;

    throw Exceptions::LogicError(
                u"The '%1' database connection isn't open in %2()."_s
                .arg(connectionName(), __tiny_func__));
}

void SqlQuery::throwIfEmptyQueryString(const QString &query)
{
    // Nothing to do
    if (!query.isEmpty())
        return;

    throw Exceptions::InvalidArgumentError(
                u"The query argument can't be empty for '%1' database connection "
                 "in %2()."_s.arg(connectionName(), __tiny_func__));
}

/* Result sets */

bool SqlQuery::seekArbitrary(const size_type index, size_type &actualIdx) noexcept
{
    // Nothing to do
    if (index < 0) {
        m_sqlResult->setAt(BeforeFirstRow);
        return false;
    }

    actualIdx = index;
    return true;
}

bool SqlQuery::seekRelative(const size_type index, size_type &actualIdx)
{
    switch (at()) {
    case BeforeFirstRow:
        // Nothing to do
        if (index <= 0)
            return false;

        actualIdx = index - 1;
        break;

    case AfterLastRow:
        // Nothing to do
        if (index >= 0)
            return false;

        m_sqlResult->fetchLast();
        actualIdx = at() + index + 1;
        break;

    default:
        // Nothing to do
        if (at() + index < 0) {
            m_sqlResult->setAt(BeforeFirstRow);
            return false;
        }
        actualIdx = at() + index;
        break;
    }

    return true;
}

bool SqlQuery::mapSeekToFetch(const size_type actualIdx)
{
    // fetchNext()
    if (actualIdx == at() + 1 && at() != static_cast<size_type>(BeforeFirstRow)) {
        if (m_sqlResult->fetchNext())
            return true;

        m_sqlResult->setAt(AfterLastRow);
        return false;
    }

    // fetchPrevious()
    if (actualIdx == at() - 1) {
        if (m_sqlResult->fetchPrevious())
            return true;

        /* This code branch is unreachable but don't use the Q_UNREACHABLE() here as
           the seeking logic has a lot of branches and there is a small chance that
           it can be reached. But I didn't succeed to reach it and I tried hard. */
        m_sqlResult->setAt(BeforeFirstRow);
        return false;
    }

    // fetch()
    if (m_sqlResult->fetch(actualIdx))
        return true;

    m_sqlResult->setAt(AfterLastRow);
    return false;
}

/* I must drop the functionName parameter, it was passed using the __tiny_func__ macro
   inside the origin method to print where the exception occurred, but the __tiny_func__
   internally instantiates QRegularExpression and that is really slow.
   Partial solution that I also tried was to save the __tiny_func__ as a static local
   variable but it also slower ~5-10ms for tst_sqlquery_normal test case.
   Solution would be to rewrite the TypePrivate::prettyFunction() without
   QRegularExpression and use static local variable for __tiny_func__ and even better
   would be to use the std::source_location or std::stacktrace.
   Removing the __tiny_func__ and make exception messages less informative is a good deal
   because of the gained performance ~40ms and that is a lot. */

void SqlQuery::throwIfNoActiveQuery() const
{
    // Nothing to do
    if (isActive())
        return;

    throw Exceptions::LogicError(
            u"No active query, you need to execute a query for '%1' database connection "
             "in %2()."_s.arg(connectionName(), __tiny_func__));
}

void SqlQuery::throwIfNoResultSet() const
{
    // Nothing to do
    if (isActive() && isSelect())
        return;

    throw Exceptions::LogicError(
            u"No active result set, you need to execute a query to obtain the record "
             "for '%1' database connection in %2()."_s
            .arg(connectionName(), __tiny_func__));
}

void SqlQuery::throwIfNoValidResultSet() const
{
    // Nothing to do
    if (isActive() && isSelect() && isValid())
        return;

    // NOTE api different, QtSql also returns true for not active and not valid fields :/ silverqx
    throw Exceptions::LogicError(
            u"No active and valid result set, first, you need to execute the query "
             "and place the cursor on an existing row to obtain the column/field value, "
             "for '%1' database connection in %2()."_s
            .arg(connectionName(), __tiny_func__));
}

void SqlQuery::throwNoFieldName(const QString &name) const
{
    // NOTE api different, QtSql also returns true for field names that don't exist :/ silverqx
    throw Exceptions::InvalidArgumentError(
                u"The field name '%1' doesn't exist or was not fetched for the current "
                 "result set for '%2' database connection in %3()."_s
                .arg(name, connectionName(), __tiny_func__));
}

/* Constructors */

std::unique_ptr<SqlResult> SqlQuery::initSqlResult()
{
    /* Instantiate the default connection if a connection was not passed
       to the constructor (needed to obtain the SqlResult instance). */
    return initSqlResult(
                SqlDatabase::database(SqlDatabase::defaultConnection, false));
}

std::unique_ptr<SqlResult> SqlQuery::initSqlResult(const SqlDatabase &connection)
{
    // Nothing to do
    if (!connection.isValid())
        throw Exceptions::InvalidArgumentError(
                u"The default database connection isn't available or the given '%1' "
                 "database connection isn't valid in %2()."_s
                .arg(connection.connectionName(), __tiny_func__));

    // This const_cast<> is needed because of the SqlQuery constructor (to have same API)
    const auto driver = const_cast<SqlDatabase &>(connection).driverWeak(); // NOLINT(cppcoreguidelines-pro-type-const-cast)

    // Ownership of a unique_ptr()
    return driver.lock()->createResult(driver);
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE
