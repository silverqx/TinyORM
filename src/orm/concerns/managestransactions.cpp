#include "orm/concerns/managestransactions.hpp"

#include "orm/concerns/countsqueries.hpp"
#include "orm/databaseconnection.hpp"
#include "orm/exceptions/sqltransactionerror.hpp"
#include "orm/support/databaseconfiguration.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Concerns
{

/* public */

ManagesTransactions::ManagesTransactions()
    : m_savepointNamespace(Support::DatabaseConfiguration::defaultSavepointNamespace)
{}

bool ManagesTransactions::beginTransaction()
{
    Q_ASSERT(m_inTransaction == false);

    static const auto query = QStringLiteral("START TRANSACTION");

    // Elapsed timer needed
    const auto countElapsed = databaseConnection().shouldCountElapsed();

    QElapsedTimer timer;
    if (countElapsed)
        timer.start();

    if (!databaseConnection().pretending() &&
        !databaseConnection().getQtConnection().transaction()
    )
        throw Exceptions::SqlTransactionError(
                QStringLiteral("Statement in %1() failed : %2")
                    .arg(__tiny_func__, query),
                databaseConnection().getRawQtConnection().lastError());

    m_inTransaction = true;

    // Queries execution time counter / Query statements counter
    auto elapsed = countsQueries().hitTransactionalCounters(timer, countElapsed);

    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    if (databaseConnection().pretending())
        databaseConnection().logTransactionQueryForPretend(query);
    else
        databaseConnection().logTransactionQuery(query, std::move(elapsed));

    return true;
}

bool ManagesTransactions::commit()
{
    Q_ASSERT(m_inTransaction);

    static const auto query = QStringLiteral("COMMIT");

    // Elapsed timer needed
    const auto countElapsed = databaseConnection().shouldCountElapsed();

    QElapsedTimer timer;
    if (countElapsed)
        timer.start();

    if (!databaseConnection().pretending() &&
        !databaseConnection().getQtConnection().commit()
    )
        throw Exceptions::SqlTransactionError(
                QStringLiteral("Statement in %1() failed : %2")
                    .arg(__tiny_func__, query),
                databaseConnection().getRawQtConnection().lastError());

    m_inTransaction = false;

    // Queries execution time counter / Query statements counter
    auto elapsed = countsQueries().hitTransactionalCounters(timer, countElapsed);

    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    if (databaseConnection().pretending())
        databaseConnection().logTransactionQueryForPretend(query);
    else
        databaseConnection().logTransactionQuery(query, std::move(elapsed));

    return true;
}

bool ManagesTransactions::rollBack()
{
    Q_ASSERT(m_inTransaction);

    static const auto query = QStringLiteral("ROLLBACK");

    // Elapsed timer needed
    const auto countElapsed = databaseConnection().shouldCountElapsed();

    QElapsedTimer timer;
    if (countElapsed)
        timer.start();

    if (!databaseConnection().pretending() &&
        !databaseConnection().getQtConnection().rollback()
    )
        throw Exceptions::SqlTransactionError(
                QStringLiteral("Statement in %1() failed : %2")
                    .arg(__tiny_func__, query),
                databaseConnection().getRawQtConnection().lastError());

    m_inTransaction = false;

    // Queries execution time counter / Query statements counter
    auto elapsed = countsQueries().hitTransactionalCounters(timer, countElapsed);

    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    if (databaseConnection().pretending())
        databaseConnection().logTransactionQueryForPretend(query);
    else
        databaseConnection().logTransactionQuery(query, std::move(elapsed));

    return true;
}

bool ManagesTransactions::savepoint(const QString &id)
{
    // TODO rewrite savepoint() and rollBack() with a new m_connection.statement() API silverqx
    Q_ASSERT(m_inTransaction);

    auto savePoint = databaseConnection().getQtQuery();
    const auto query = QStringLiteral("SAVEPOINT %1_%2").arg(m_savepointNamespace, id);

    // Elapsed timer needed
    const auto countElapsed = databaseConnection().shouldCountElapsed();

    QElapsedTimer timer;
    if (countElapsed)
        timer.start();

    // Execute a savepoint query
    if (!databaseConnection().pretending() && !savePoint.exec(query))
        throw Exceptions::SqlTransactionError(
                QStringLiteral("Statement in %1() failed : %2")
                    .arg(__tiny_func__, query),
                savePoint.lastError());

    ++m_savepoints;

    // Queries execution time counter / Query statements counter
    auto elapsed = countsQueries().hitTransactionalCounters(timer, countElapsed);

    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    if (databaseConnection().pretending())
        databaseConnection().logTransactionQueryForPretend(query);
    else
        databaseConnection().logTransactionQuery(query, std::move(elapsed));

    return true;
}

bool ManagesTransactions::savepoint(const std::size_t id)
{
    return savepoint(QString::number(id));
}

bool ManagesTransactions::rollbackToSavepoint(const QString &id)
{
    Q_ASSERT(m_inTransaction);
    Q_ASSERT(m_savepoints > 0);

    auto rollbackToSavepoint = databaseConnection().getQtQuery();
    const auto query = QStringLiteral("ROLLBACK TO SAVEPOINT %1_%2")
                       .arg(m_savepointNamespace, id);

    // Elapsed timer needed
    const auto countElapsed = databaseConnection().shouldCountElapsed();

    QElapsedTimer timer;
    if (countElapsed)
        timer.start();

    // Execute a rollback to savepoint query
    if (!databaseConnection().pretending() && !rollbackToSavepoint.exec(query))
        throw Exceptions::SqlTransactionError(
                QStringLiteral("Statement in %1() failed : %2")
                    .arg(__tiny_func__, query),
                rollbackToSavepoint.lastError());

    m_savepoints = std::max<std::size_t>(0, m_savepoints - 1);

    // Queries execution time counter / Query statements counter
    auto elapsed = countsQueries().hitTransactionalCounters(timer, countElapsed);

    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    if (databaseConnection().pretending())
        databaseConnection().logTransactionQueryForPretend(query);
    else
        databaseConnection().logTransactionQuery(query, std::move(elapsed));

    return true;
}

bool ManagesTransactions::rollbackToSavepoint(const std::size_t id)
{
    return rollbackToSavepoint(QString::number(id));
}

DatabaseConnection &
ManagesTransactions::setSavepointNamespace(const QString &savepointNamespace)
{
    m_savepointNamespace = savepointNamespace;

    return databaseConnection();
}

/* private */

DatabaseConnection &ManagesTransactions::resetTransactions()
{
    m_savepoints = 0;
    m_inTransaction = false;

    return databaseConnection();
}

DatabaseConnection &ManagesTransactions::databaseConnection()
{
    return dynamic_cast<DatabaseConnection &>(*this);
}

CountsQueries &ManagesTransactions::countsQueries()
{
    return dynamic_cast<CountsQueries &>(*this);
}

} // namespace Orm::Concerns

TINYORM_END_COMMON_NAMESPACE
