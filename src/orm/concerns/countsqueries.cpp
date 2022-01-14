#include "orm/concerns/countsqueries.hpp"

#include "orm/databaseconnection.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Concerns
{

/* public */

bool CountsQueries::countingElapsed() const
{
    return m_countingElapsed;
}

DatabaseConnection &CountsQueries::enableElapsedCounter()
{
    m_countingElapsed = true;
    m_elapsedCounter = 0;

    return databaseConnection();
}

DatabaseConnection &CountsQueries::disableElapsedCounter()
{
    m_countingElapsed = false;
    m_elapsedCounter = -1;

    return databaseConnection();
}

qint64 CountsQueries::getElapsedCounter() const
{
    return m_elapsedCounter;
}

qint64 CountsQueries::takeElapsedCounter()
{
    if (!m_countingElapsed)
        return -1;

    const auto elapsed = m_elapsedCounter;

    m_elapsedCounter = 0;

    return elapsed;
}

DatabaseConnection &CountsQueries::resetElapsedCounter()
{
    m_elapsedCounter = 0;

    return databaseConnection();
}

bool CountsQueries::countingStatements() const
{
    return m_countingStatements;
}

DatabaseConnection &CountsQueries::enableStatementsCounter()
{
    m_countingStatements = true;

    m_statementsCounter.normal        = 0;
    m_statementsCounter.affecting     = 0;
    m_statementsCounter.transactional = 0;

    return databaseConnection();
}

DatabaseConnection &CountsQueries::disableStatementsCounter()
{
    m_countingStatements = false;

    m_statementsCounter.normal        = -1;
    m_statementsCounter.affecting     = -1;
    m_statementsCounter.transactional = -1;

    return databaseConnection();
}

const StatementsCounter &CountsQueries::getStatementsCounter() const
{
    return m_statementsCounter;
}

StatementsCounter CountsQueries::takeStatementsCounter()
{
    if (!m_countingStatements)
        return {};

    const auto counter = m_statementsCounter;

    m_statementsCounter.normal        = 0;
    m_statementsCounter.affecting     = 0;
    m_statementsCounter.transactional = 0;

    return counter;
}

DatabaseConnection &CountsQueries::resetStatementsCounter()
{
    m_statementsCounter.normal        = 0;
    m_statementsCounter.affecting     = 0;
    m_statementsCounter.transactional = 0;

    return databaseConnection();
}

/* private */

std::optional<qint64>
CountsQueries::hitTransactionalCounters(const QElapsedTimer timer,
                                        const bool countElapsed)
{
    std::optional<qint64> elapsed;

    if (countElapsed) {
        // Hit elapsed timer
        elapsed = timer.elapsed();

        // Queries execution time counter
        m_elapsedCounter += *elapsed;
    }

    // Query statements counter
    if (m_countingStatements)
        ++m_statementsCounter.transactional;

    return elapsed;
}

DatabaseConnection &CountsQueries::databaseConnection()
{
    return dynamic_cast<DatabaseConnection &>(*this);
}

} // namespace Orm::Concerns

TINYORM_END_COMMON_NAMESPACE
