#include "orm/drivers/sqlquery1.hpp"

/*! Log an executed query, elapsed time, and query /affected size to the stderr. */
//#define QT_DEBUG_SQL

#ifdef QT_DEBUG_SQL
#  include <QDebug>
#  include <QElapsedTimer>
#endif

#include "orm/drivers/sqldatabase.hpp"
#include "orm/drivers/sqldrivererror.hpp"
#include "orm/drivers/sqlrecord.hpp"
#include "orm/drivers/sqlresult.hpp"
#include "orm/drivers/sqldriver.hpp"

#define sl(str) QStringLiteral(str)

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

/* public */

SqlQuery1::SqlQuery1()
    : m_sqlResult(initSqlResult())
{}

SqlQuery1::SqlQuery1(const SqlDatabase &connection)
    : m_sqlResult(initSqlResult(connection))
{}

SqlQuery1::SqlQuery1(std::unique_ptr<SqlResult> &&result)
    : m_sqlResult(std::move(result))
{}

/* The destructor must be in the cpp file because the m_sqlResult is unique_ptr.
   If the destructor is inline then the compilation fails because a unique_ptr can't
   destroy an incomplete type. */
SqlQuery1::~SqlQuery1() = default;

/* Getters / Setters */

bool SqlQuery1::isValid() const
{
    return m_sqlResult->isValid();
}

QString SqlQuery1::executedQuery() const
{
    return m_sqlResult->query();
}

QString SqlQuery1::lastQuery() const
{
    return m_sqlResult->query();
}

SqlDriverError SqlQuery1::lastError() const
{
    return m_sqlResult->lastError();
}

int SqlQuery1::at() const
{
    return m_sqlResult->at();
}

bool SqlQuery1::isActive() const noexcept
{
    return m_sqlResult->isActive();
}

bool SqlQuery1::isSelect() const noexcept
{
    /* isSelect() practically means that the executed query has a result set (data that
       can be looped over), it could also be named hasResultSet(). */
    return m_sqlResult->isSelect();
}

NumericalPrecisionPolicy SqlQuery1::numericalPrecisionPolicy() const
{
    return m_sqlResult->numericalPrecisionPolicy();
}

void
SqlQuery1::setNumericalPrecisionPolicy(const NumericalPrecisionPolicy precision)
{
    m_sqlResult->setNumericalPrecisionPolicy(precision);
}

std::weak_ptr<const SqlDriver> SqlQuery1::driver() const noexcept
{
    /* This must be the std::weak_ptr() because when the connection is removed from
       the SqlDatabaseManager using the SqlDatabase::removeDatabase() then the SqlDriver
       is invalidated (using the std::shared_ptr::reset()).
       This means we don't want to keep the SqlDriver alive after removeDatabase(). */
    return m_sqlResult->driver();
}

/* Normal queries */

bool SqlQuery1::exec(const QString &query)
{
    // Nothing to do
    if (query.isEmpty())
        throw std::exception("SqlQuery1::exec: empty query");

    if (const auto driver = this->driver().lock();
        !driver->isOpen() || driver->isOpenError()
    ) {
        qWarning("SqlQuery1::exec: database not open");
        return false;
    }

    m_sqlResult->clearBoundValues();
    m_sqlResult->setActive(false);
    m_sqlResult->setLastError({});
    m_sqlResult->setAt(BeforeFirstRow);

    m_sqlResult->setQuery(query.trimmed());

#ifdef QT_DEBUG_SQL
    QElapsedTimer timer;
    timer.start();
#endif

    // CUR drivers this is bad, the mysql_real_query() inside the reset() silverqx
    const auto result = m_sqlResult->exec(query);

#ifdef QT_DEBUG_SQL
    qDebug().nospace() << "Executed query (" << timer.elapsed() << "ms, "
                       << m_sqlResult->size() << " results, "
                       << m_sqlResult->numRowsAffected() << " affected): "
                       << m_sqlResult->query();
#endif

    return result;
}

/* Prepared queries */

bool SqlQuery1::prepare(const QString &query)
{
    // Nothing to do
    if (query.isEmpty())
        throw std::exception("SqlQuery1::exec: empty query");

    if (const auto driver = this->driver().lock();
        !driver->isOpen() || driver->isOpenError()
    ) {
        qWarning("SqlQuery1::prepare: database not open");
        return false;
    }

    m_sqlResult->clearBoundValues();
    m_sqlResult->setActive(false);
    m_sqlResult->setLastError({});
    m_sqlResult->setAt(BeforeFirstRow);

    return m_sqlResult->prepare(query);
}

bool SqlQuery1::exec()
{
    // Nothing to do
    if (m_sqlResult->query().isEmpty())
        throw std::exception(
                "The prepared query is empty, call the SqlQuery1::prepare() first "
                "for prepared statements or pass the query string directly "
                "to the SqlQuery1::exec(QString) for normal statements.");

    // CUR drivers check if this is needed as it's reset in the prepare() and if some error occurs in the m_sqlResult->prepare(query) then I don't know if make sense to continue silverqx
    if (m_sqlResult->lastError().isValid())
        m_sqlResult->setLastError({});

#ifdef QT_DEBUG_SQL
    QElapsedTimer t;
    t.start();
#endif

    const auto result = m_sqlResult->exec();

#ifdef QT_DEBUG_SQL
    qDebug().nospace() << "Executed prepared query (" << t.elapsed() << "ms, "
                       << m_sqlResult->size() << " results, "
                       << m_sqlResult->numRowsAffected() << " affected): "
                       << m_sqlResult->query();
#endif

    return result;
}

void SqlQuery1::bindValue(const int index, const QVariant &value,
                          const ParamType /*unused*/)
{
    /* Need to pass the ParamType::In to preserve the same API because I can't remove this
       parameter so I need to pass something but it has no effect. */
    m_sqlResult->bindValue(index, value, ParamType::In);
}

void SqlQuery1::addBindValue(const QVariant &value, const ParamType /*unused*/)
{
    // Append, index after the last value
    const auto index = m_sqlResult->boundValuesCount();

    /* Need to pass the ParamType::In to preserve the same API because I can't remove this
       parameter so I need to pass something but it has no effect. */
    m_sqlResult->bindValue(index, value, ParamType::In);
}

QVariant SqlQuery1::boundValue(const int index) const
{
    return m_sqlResult->boundValue(index);
}

QVariantList SqlQuery1::boundValues() const
{
    return m_sqlResult->boundValues();
}

/* Result sets */

SqlRecord SqlQuery1::record() const
{
    // Nothing to do
    if (!isActive() || !isValid() || !isSelect())
        return {};

    auto record = m_sqlResult->record();

    for (qsizetype index = 0; index < record.count(); ++index)
        record.setValue(index, value(index));

    return record;
}

QVariant SqlQuery1::lastInsertId() const
{
    // Nothing to do, query was not executed
    if (!isActive())
        return {};

    return m_sqlResult->lastInsertId();
}

bool SqlQuery1::next()
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

bool SqlQuery1::previous()
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

bool SqlQuery1::first()
{
    // Nothing to do
    if (!isSelect() || !isActive())
        return false;

    return m_sqlResult->fetchFirst();
}

bool SqlQuery1::last()
{
    if (!isSelect() || !isActive())
        return false;

    return m_sqlResult->fetchLast();
}

bool SqlQuery1::seek(const int index, const bool relative)
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

QVariant SqlQuery1::value(const int index) const
{
    if (isActive() && isValid() && isSelect() && index > -1)
        return m_sqlResult->data(index);

    qWarning("SqlQuery1::value: not positioned on a valid record");

    return {};
}

QVariant SqlQuery1::value(const QString &name) const
{
    if (const auto index = m_sqlResult->record().indexOf(name);
        index > -1
    )
        return value(index);

    qWarning().noquote() << sl("SqlQuery1::value: unknown field name '%1'").arg(name);

    return {};
}

bool SqlQuery1::isNull(const int index) const
{
    if (isActive() && isValid())
        return m_sqlResult->isNull(index);

    throw std::exception(
                "First, you need to execute the query and place the cursor "
                "on the existing row to see if a field is NULL.");
}

bool SqlQuery1::isNull(const QString &name) const
{
    if (const auto index = m_sqlResult->record().indexOf(name);
        index > -1
    )
        return isNull(index);

    qWarning().noquote() << sl("SqlQuery1::isNull: unknown field name '%1'").arg(name);

    return true;
}

int SqlQuery1::size() const
{
    // Nothing to do
    if (!driver().lock()->hasFeature(SqlDriver::QuerySize) || !isSelect() || !isActive())
        return -1;

    return m_sqlResult->size();
}

int SqlQuery1::numRowsAffected() const
{
    // Nothing to do
    if (!isActive())
        return -1;

    return m_sqlResult->numRowsAffected();
}

/* Others */

void SqlQuery1::clear()
{
    const auto driver = this->driver();

    // CUR drivers revisit, maybe clear everything manually? What happens with current values, is below correct? silverqx
    // Get the SqlResult instance
    *this = SqlQuery1(driver.lock()->createResult(driver));
}

void SqlQuery1::finish()
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

/* Leave the non-const driver() private, it's correct, it's not needed anywhere and is
   for special cases only. */

std::weak_ptr<SqlDriver> SqlQuery1::driver() noexcept
{
    /* This must be the std::weak_ptr() because when the connection is removed from
       the SqlDatabaseManager using the SqlDatabase::removeDatabase() then the SqlDriver
       is invalidated (using the std::shared_ptr::reset()).
       This means we don't want to keep the SqlDriver alive after removeDatabase(). */
    return m_sqlResult->driver();
}

/* Result sets */

bool SqlQuery1::seekArbitrary(const int index, int &actualIdx)
{
    // Nothing to do
    if (index < 0) {
        m_sqlResult->setAt(BeforeFirstRow);
        return false;
    }

    actualIdx = index;
    return true;
}

bool SqlQuery1::seekRelative(const int index, int &actualIdx)
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

bool SqlQuery1::mapSeekToFetch(const int actualIdx)
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

std::unique_ptr<SqlResult> SqlQuery1::initSqlResult()
{
    /* Instantiate the default connection if a connection was not passed
       to the constructor (needed to obtain the SqlResult instance). */
    return initSqlResult(
                SqlDatabase::database(SqlDatabase::defaultConnection, false));

}

std::unique_ptr<SqlResult> SqlQuery1::initSqlResult(const SqlDatabase &connection)
{
    // Nothing to do
    if (!connection.isValid())
        // CUR drivers throw is the default connection is not valid silverqx
        throw std::exception("No DB connection available.");

    const auto driver = connection.driver();

    // Get the SqlResult instance
    return driver.lock()->createResult(driver);
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE
