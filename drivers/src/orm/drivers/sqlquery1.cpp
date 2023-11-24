#include "orm/drivers/sqlquery1.hpp"

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

//SqlQuery1::SqlQuery1(SqlQuery1 &&) noexcept = default;
//SqlQuery1 &SqlQuery1::operator=(SqlQuery1 &&) noexcept = default;

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

QSql::NumericalPrecisionPolicy SqlQuery1::numericalPrecisionPolicy() const
{
    return m_sqlResult->numericalPrecisionPolicy();
}

void
SqlQuery1::setNumericalPrecisionPolicy(const QSql::NumericalPrecisionPolicy precision)
{
    m_sqlResult->setNumericalPrecisionPolicy(precision);
}

const SqlDriver *SqlQuery1::driver() const noexcept
{
    return m_sqlResult->driver();
}

/* Normal queries */

bool SqlQuery1::exec(const QString &query)
{
#ifdef QT_DEBUG_SQL
    QElapsedTimer timer;
    timer.start();
#endif

    m_sqlResult->clearValues();
    m_sqlResult->setActive(false);
    m_sqlResult->setLastError({});
    m_sqlResult->setAt(QSql::BeforeFirstRow);

    m_sqlResult->setQuery(query.trimmed());

    if (!driver()->isOpen() || driver()->isOpenError()) {
        qWarning("SqlQuery1::exec: database not open");
        return false;
    }
    if (query.isEmpty())
        throw std::exception("SqlQuery1::exec: empty query");

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

    if (!driver()->isOpen() || driver()->isOpenError()) {
        qWarning("SqlQuery1::prepare: database not open");
        return false;
    }

    m_sqlResult->clearValues();
    m_sqlResult->setActive(false);
    m_sqlResult->setLastError({});
    m_sqlResult->setAt(QSql::BeforeFirstRow);

    return m_sqlResult->prepare(query);
}

bool SqlQuery1::exec()
{
#ifdef QT_DEBUG_SQL
    QElapsedTimer t;
    t.start();
#endif

    if (m_sqlResult->query().isEmpty())
        throw std::exception(
                "The prepared query is empty, call the SqlQuery1::prepare() first "
                "for prepared statements or pass the query string directly "
                "to the SqlQuery1::exec(QString) for normal statements.");

    if (m_sqlResult->lastError().isValid())
        m_sqlResult->setLastError({});

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
                          const QSql::ParamType /*unused*/)
{
    /* Need to pass the QSql::In to preserve the same API because I can't remove this
       parameter so I need to pass something but it has no effect. */
    m_sqlResult->bindValue(index, value, QSql::In);
}

void SqlQuery1::addBindValue(const QVariant &value, const QSql::ParamType /*unused*/)
{
    // Append, index after the last value
    const auto index = m_sqlResult->boundValuesCount();

    /* Need to pass the QSql::In to preserve the same API because I can't remove this
       parameter so I need to pass something but it has no effect. */
    m_sqlResult->bindValue(index, value, QSql::In);
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
    case QSql::BeforeFirstRow:
        return m_sqlResult->fetchFirst();

    case QSql::AfterLastRow:
        return false;

    default:
        if (m_sqlResult->fetchNext())
            return true;

        m_sqlResult->setAt(QSql::AfterLastRow);
        return false;
    }
}

bool SqlQuery1::previous()
{
    // Nothing to do
    if (!isSelect() || !isActive())
        return false;

    switch (at()) {
    case QSql::BeforeFirstRow:
        return false;

    case QSql::AfterLastRow:
        return m_sqlResult->fetchLast();

    default:
        if (m_sqlResult->fetchPrevious())
            return true;

        m_sqlResult->setAt(QSql::BeforeFirstRow);
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

    auto actualIdx = static_cast<int>(QSql::BeforeFirstRow);

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

    qWarning("SqlQuery1::value: unknown field name '%s'", qPrintable(name));

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

    qWarning("SqlQuery1::isNull: unknown field name '%s'", qPrintable(name));

    return true;
}

int SqlQuery1::size() const
{
    // Nothing to do
    if (!driver()->hasFeature(SqlDriver::QuerySize) || !isSelect() || !isActive())
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
    // CUR drivers revisit, maybe clear everything manually? What happens with current values, is below correct? silverqx
    *this = SqlQuery1(driver()->createResult());
}

void SqlQuery1::finish()
{
    // Nothing to do
    if (!isActive())
        return;

    m_sqlResult->setLastError({});
    m_sqlResult->setAt(QSql::BeforeFirstRow);
    m_sqlResult->detachFromResultSet();
    m_sqlResult->setActive(false);
}

/* private */

bool SqlQuery1::seekArbitrary(const int index, int &actualIdx)
{
    // Nothing to do
    if (index < 0) {
        m_sqlResult->setAt(QSql::BeforeFirstRow);
        return false;
    }

    actualIdx = index;
    return true;
}

bool SqlQuery1::seekRelative(const int index, int &actualIdx)
{
    // CUR drivers finish this if I will have higher IQ silverqx
    switch (at()) {
    case QSql::BeforeFirstRow:
        if (index > 0)
            actualIdx = index - 1;
        else
            return false;
        break;

    case QSql::AfterLastRow:
        if (index < 0) {
            m_sqlResult->fetchLast();
            actualIdx = at() + index + 1;
        } else
            return false;
        break;

    default:
        if (at() + index < 0) {
            m_sqlResult->setAt(QSql::BeforeFirstRow);
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
    if (actualIdx == at() + 1 && at() != QSql::BeforeFirstRow) {
        if (m_sqlResult->fetchNext())
            return true;

        m_sqlResult->setAt(QSql::AfterLastRow);
        return false;
    }

    // fetchPrevious()
    if (actualIdx == at() - 1) {
        if (m_sqlResult->fetchPrevious())
            return true;

        m_sqlResult->setAt(QSql::BeforeFirstRow);
        return false;
    }

    // fetch()
    if (m_sqlResult->fetch(actualIdx))
        return true;

    m_sqlResult->setAt(QSql::AfterLastRow);

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

    // Get the SqlResult instance
    return connection.driver()->createResult();

}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE
