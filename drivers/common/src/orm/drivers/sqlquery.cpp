#include "orm/drivers/sqlquery.hpp"

/* I leave the QT_DEBUG_SQL defined to be compatible with the QtSql module, so
   the QtSql module and the TinyDrivers are interchangeable. */

/*! Log an executed query, elapsed time, and query /affected size to the stderr. */
//#define QT_DEBUG_SQL
/*! Log an executed query, elapsed time, and query /affected size to the stderr. */
//#define TINYDRIVERS_DEBUG_SQL

#if defined(QT_DEBUG_SQL) || defined(TINYDRIVERS_DEBUG_SQL)
#  include <QDebug>
#  include <QElapsedTimer>
#endif

#include "orm/drivers/sqldatabase.hpp"
#include "orm/drivers/sqlerror.hpp"
#include "orm/drivers/sqlrecord.hpp"
#include "orm/drivers/sqlresult.hpp"
#include "orm/drivers/sqldriver.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals;

namespace Orm::Drivers
{

/* public */

SqlQuery::SqlQuery()
    : m_sqlResult(initSqlResult())
{}

SqlQuery::SqlQuery(const SqlDatabase &connection)
    : m_sqlResult(initSqlResult(connection))
{}

SqlQuery::SqlQuery(std::unique_ptr<SqlResult> &&result)
    : m_sqlResult(std::move(result))
{}

/* The destructor must be in the cpp file because the m_sqlResult is unique_ptr.
   If the destructor is inline then the compilation fails because a unique_ptr can't
   destroy an incomplete type. */
SqlQuery::~SqlQuery() = default;

/* Getters / Setters */

bool SqlQuery::isValid() const
{
    return m_sqlResult->isValid();
}

QString SqlQuery::executedQuery() const
{
    return m_sqlResult->query();
}

QString SqlQuery::lastQuery() const
{
    return m_sqlResult->query();
}

SqlError SqlQuery::lastError() const
{
    return m_sqlResult->lastError();
}

int SqlQuery::at() const
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

NumericalPrecisionPolicy SqlQuery::numericalPrecisionPolicy() const
{
    return m_sqlResult->numericalPrecisionPolicy();
}

void SqlQuery::setNumericalPrecisionPolicy(const NumericalPrecisionPolicy precision)
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

/* Normal queries */

bool SqlQuery::exec(const QString &query)
{
    // Nothing to do
    if (query.isEmpty())
        throw std::exception("SqlQuery::exec: empty query");

    if (const auto driver = this->driverWeak().lock();
        !driver->isOpen() || driver->isOpenError()
    ) {
        qWarning("SqlQuery::exec: database not open");
        return false;
    }

    m_sqlResult->clearBoundValues();
    m_sqlResult->setActive(false);
    m_sqlResult->setLastError({});
    m_sqlResult->setAt(BeforeFirstRow);

    m_sqlResult->setQuery(query.trimmed());

#if defined(QT_DEBUG_SQL) || defined(TINYDRIVERS_DEBUG_SQL)
    QElapsedTimer timer;
    timer.start();
#endif

    // CUR drivers this is bad, the mysql_real_query() inside the reset() silverqx
    const auto result = m_sqlResult->exec(query);

#if defined(QT_DEBUG_SQL) || defined(TINYDRIVERS_DEBUG_SQL)
    qDebug().nospace() << "Executed query (" << timer.elapsed() << "ms, "
                       << m_sqlResult->size() << " results, "
                       << m_sqlResult->numRowsAffected() << " affected): "
                       << m_sqlResult->query();
#endif

    return result;
}

/* Prepared queries */

bool SqlQuery::prepare(const QString &query)
{
    // Nothing to do
    if (query.isEmpty())
        throw std::exception("SqlQuery::exec: empty query");

    if (const auto driver = this->driverWeak().lock();
        !driver->isOpen() || driver->isOpenError()
    ) {
        qWarning("SqlQuery::prepare: database not open");
        return false;
    }

    m_sqlResult->clearBoundValues();
    m_sqlResult->setActive(false);
    m_sqlResult->setLastError({});
    m_sqlResult->setAt(BeforeFirstRow);

    return m_sqlResult->prepare(query);
}

bool SqlQuery::exec()
{
    // Nothing to do
    if (m_sqlResult->query().isEmpty())
        throw std::exception(
                "The prepared query is empty, call the SqlQuery::prepare() first "
                "for prepared statements or pass the query string directly "
                "to the SqlQuery::exec(QString) for normal statements.");

    // CUR drivers check if this is needed as it's reset in the prepare() and if some error occurs in the m_sqlResult->prepare(query) then I don't know if make sense to continue silverqx
    if (m_sqlResult->lastError().isValid())
        m_sqlResult->setLastError({});

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

void SqlQuery::bindValue(const int index, const QVariant &value,
                          const ParamType /*unused*/)
{
    /* Need to pass the ParamType::In to preserve the same API because I can't remove this
       parameter so I need to pass something but it has no effect. */
    m_sqlResult->bindValue(index, value, ParamType::In);
}

void SqlQuery::addBindValue(const QVariant &value, const ParamType /*unused*/)
{
    // Append, index after the last value
    const auto index = m_sqlResult->boundValuesCount();

    /* Need to pass the ParamType::In to preserve the same API because I can't remove this
       parameter so I need to pass something but it has no effect. */
    m_sqlResult->bindValue(index, value, ParamType::In);
}

QVariant SqlQuery::boundValue(const int index) const
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
    // Nothing to do
    if (!isActive() || !isSelect())
        return {};

    auto record = m_sqlResult->record();

    // Populate also the field values if the cursor is positioned on a valid record/row
    if (isValid())
        for (qsizetype index = 0; index < record.count(); ++index)
            record.setValue(index, value(index));

    return record;
}

QVariant SqlQuery::lastInsertId() const
{
    // Nothing to do, query was not executed
    if (!isActive())
        return {};

    return m_sqlResult->lastInsertId();
}

bool SqlQuery::next()
{
    // Nothing to do
    if (!isSelect() || !isActive())
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
    if (!isSelect() || !isActive())
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
    if (!isSelect() || !isActive())
        return false;

    return m_sqlResult->fetchFirst();
}

bool SqlQuery::last()
{
    if (!isSelect() || !isActive())
        return false;

    return m_sqlResult->fetchLast();
}

bool SqlQuery::seek(const int index, const bool relative)
{
    // Nothing to do
    if (!isSelect() || !isActive())
        return false;

    auto actualIdx = static_cast<int>(BeforeFirstRow);

    // Arbitrary seek
    if (!relative && !seekArbitrary(index, actualIdx))
        return false;

    // Relative seek
    if (!seekRelative(index, actualIdx))
        return false;

    return mapSeekToFetch(actualIdx);
}

QVariant SqlQuery::value(const int index) const
{
    if (isActive() && isValid() && isSelect() && index > -1)
        return m_sqlResult->data(index);

    qWarning("SqlQuery::value: not positioned on a valid record");

    return {};
}

QVariant SqlQuery::value(const QString &name) const
{
    if (const auto index = m_sqlResult->record().indexOf(name);
        index > -1
    )
        return value(index);

    qWarning().noquote() << u"SqlQuery::value: unknown field name '%1'"_s.arg(name);

    return {};
}

bool SqlQuery::isNull(const int index) const
{
    if (isActive() && isValid())
        return m_sqlResult->isNull(index);

    throw std::exception(
                "First, you need to execute the query and place the cursor "
                "on the existing row to see if a field is NULL.");
}

bool SqlQuery::isNull(const QString &name) const
{
    if (const auto index = m_sqlResult->record().indexOf(name);
        index > -1
    )
        return isNull(index);

    qWarning().noquote() << u"SqlQuery::isNull: unknown field name '%1'"_s.arg(name);

    return true;
}

int SqlQuery::size() const
{
    // Nothing to do
    if (!driverWeak().lock()->hasFeature(SqlDriver::QuerySize) ||
        !isSelect() || !isActive()
    )
        return -1;

    return m_sqlResult->size();
}

int SqlQuery::numRowsAffected() const
{
    // Nothing to do
    if (!isActive())
        return -1;

    return m_sqlResult->numRowsAffected();
}

/* Others */

void SqlQuery::clear()
{
    const auto driver = driverWeak();

    // CUR drivers revisit, maybe clear everything manually? What happens with current values, is below correct? silverqx
    // Get the SqlResult instance
    *this = SqlQuery(driver.lock()->createResult(driver));
}

void SqlQuery::finish()
{
    // CUR drivers finish this finish() method, also look hasFearures(FinishQuery), update description silverqx
    // Nothing to do
    if (!isActive())
        return;

    // CUR drivers I saw 3 duplicates of this code block, extract silverqx
    m_sqlResult->setLastError({});
    m_sqlResult->setAt(BeforeFirstRow);
    m_sqlResult->detachFromResultSet();
    m_sqlResult->setActive(false);
}

/* private */

/* Getters / Setters */

/* Leave the non-const driverWeak() private, it's correct, it's not needed anywhere and
   is for special cases only. */

std::weak_ptr<SqlDriver> SqlQuery::driverWeak() noexcept
{
    /* This must be the std::weak_ptr() because when the connection is removed from
       the SqlDatabaseManager using the SqlDatabase::removeDatabase() then the SqlDriver
       is invalidated (using the std::shared_ptr::reset()).
       This means we don't want to keep the SqlDriver alive after removeDatabase(). */
    return m_sqlResult->driver();
}

/* Result sets */

bool SqlQuery::seekArbitrary(const int index, int &actualIdx)
{
    // Nothing to do
    if (index < 0) {
        m_sqlResult->setAt(BeforeFirstRow);
        return false;
    }

    actualIdx = index;
    return true;
}

bool SqlQuery::seekRelative(const int index, int &actualIdx)
{
    // CUR drivers finish this if I will have higher IQ silverqx
    switch (at()) {
    case BeforeFirstRow:
        if (index > 0)
            actualIdx = index - 1;
        else
            return false;
        break;

    case AfterLastRow:
        if (index < 0) {
            m_sqlResult->fetchLast();
            actualIdx = at() + index + 1;
        } else
            return false;
        break;

    default:
        if (at() + index < 0) {
            m_sqlResult->setAt(BeforeFirstRow);
            return false;
        }
        actualIdx = at() + index;
        break;
    }

    return true;
}

bool SqlQuery::mapSeekToFetch(const int actualIdx)
{
    // fetchNext()
    if (actualIdx == at() + 1 && at() != static_cast<int>(BeforeFirstRow)) {
        if (m_sqlResult->fetchNext())
            return true;

        m_sqlResult->setAt(AfterLastRow);
        return false;
    }

    // fetchPrevious()
    if (actualIdx == at() - 1) {
        if (m_sqlResult->fetchPrevious())
            return true;

        m_sqlResult->setAt(BeforeFirstRow);
        return false;
    }

    // fetch()
    if (m_sqlResult->fetch(actualIdx))
        return true;

    m_sqlResult->setAt(AfterLastRow);

    return false;
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
        // CUR drivers throw if the default connection is not valid silverqx
        throw std::exception("No database connection available (isn't valid).");

    const auto driver = const_cast<SqlDatabase &>(connection).driverWeak();

    // Get the SqlResult instance
    return driver.lock()->createResult(driver);
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE
