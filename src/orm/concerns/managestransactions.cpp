#include "orm/concerns/managestransactions.hpp"

#ifdef TINYORM_USING_QTSQLDRIVERS
#  include <QtSql/QSqlQuery>
#endif

#include "orm/concerns/countsqueries.hpp"
#include "orm/databaseconnection.hpp"
#include "orm/support/databaseconfiguration.hpp"

#include TINY_INCLUDE_TSqlTransactionError

#ifndef sl
/*! Alias for the QStringLiteral(). */
#  define sl(str) QStringLiteral(str)
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

using TSqlTransactionError;

namespace Orm::Concerns
{

/* Used the QSqlError and QSqlQuery directly to make it more clear that TSqlXyz mappings
   are not needed. */

/* private */

/*! QString constant for the "BEGIN WORK" query (Qt internally uses this query string). */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, StartTransaction, ("BEGIN WORK")) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
/*! QString constant for the "START TRANSACTION" query. */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, Commit,           ("COMMIT")) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
/*! QString constant for the "START TRANSACTION" query. */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, RollBack,         ("ROLLBACK")) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
#ifdef TINYORM_USING_QTSQLDRIVERS
/*! QString constant for the "ManagesTransactions::beginTransaction" function name. */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, BeginTransactionFunction, // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
                          ("ManagesTransactions::beginTransaction"))
/*! QString constant for the "ManagesTransactions::commit" function name. */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, CommitFunction, // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
                          ("ManagesTransactions::commit"))
/*! QString constant for the "ManagesTransactions::rollBack" function name. */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, RollBackFunction, // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
                          ("ManagesTransactions::rollBack"))
/*! QString constant for the "ManagesTransactions::savepoint" function name. */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, SavepointFunction, // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
                          ("ManagesTransactions::savepoint"))
/*! QString constant for the "ManagesTransactions::rollbackToSavepoint" function name. */
Q_GLOBAL_STATIC_WITH_ARGS(const QString, RollbackToSavepointFunction, // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
                          ("ManagesTransactions::rollbackToSavepoint"))
#endif

/* public */

ManagesTransactions::ManagesTransactions()
    : m_savepointNamespace(Support::DatabaseConfiguration::defaultSavepointNamespace)
{}

bool ManagesTransactions::beginTransaction()
{
    Q_ASSERT(m_inTransaction == false);
    Q_ASSERT(m_savepoints == 0);

    auto &connection = databaseConnection();

    connection.reconnectIfMissingConnection();

    // Is Elapsed timer needed?
    const auto countElapsed = connection.shouldCountElapsed();

    QElapsedTimer timer;
    if (countElapsed)
        timer.start();

    try {
#ifdef TINYORM_USING_QTSQLDRIVERS
        runBeginTransaction(connection);
#elif defined(TINYORM_USING_TINYDRIVERS)
        if (!connection.pretending())
            connection.getSqlConnection().transaction();
#endif
    } catch (const SqlTransactionError &e) {
        tryAgainIfCausedByLostConnectionStart(connection, std::current_exception(),
                                              e.databaseText());
    }

    m_inTransaction = true;

    // Queries execution time counter / Query statements counter
    const auto elapsed = countsQueries().hitTransactionalCounters(timer, countElapsed);

    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    if (connection.pretending())
        connection.logTransactionQueryForPretend(*StartTransaction);
    else
        connection.logTransactionQuery(*StartTransaction, elapsed);

    return true;
}

bool ManagesTransactions::commit()
{
    Q_ASSERT(m_inTransaction);

    auto &connection = databaseConnection();

    // Is Elapsed timer needed?
    const auto countElapsed = connection.shouldCountElapsed();

    QElapsedTimer timer;
    if (countElapsed)
        timer.start();

    try {
#ifdef TINYORM_USING_QTSQLDRIVERS
        runCommit(connection);
#elif defined(TINYORM_USING_TINYDRIVERS)
        if (!connection.pretending())
            connection.getRawSqlConnection().commit();
#endif
    } catch (const SqlTransactionError &e) {
        tryAgainIfCausedByLostConnectionCommon(std::current_exception(),
                                               e.databaseText());
    }

    resetTransactions();

    // Queries execution time counter / Query statements counter
    const auto elapsed = countsQueries().hitTransactionalCounters(timer, countElapsed);

    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    if (connection.pretending())
        connection.logTransactionQueryForPretend(*Commit);
    else
        connection.logTransactionQuery(*Commit, elapsed);

    return true;
}

bool ManagesTransactions::rollBack()
{
    Q_ASSERT(m_inTransaction);

    auto &connection = databaseConnection();

    // Is Elapsed timer needed?
    const auto countElapsed = connection.shouldCountElapsed();

    QElapsedTimer timer;
    if (countElapsed)
        timer.start();

    try {
#ifdef TINYORM_USING_QTSQLDRIVERS
        runRollBack(connection);
#elif defined(TINYORM_USING_TINYDRIVERS)
        if (!connection.pretending())
            connection.getRawSqlConnection().rollback();
#endif
    } catch (const SqlTransactionError &e) {
        tryAgainIfCausedByLostConnectionCommon(std::current_exception(),
                                               e.databaseText());
    }

    resetTransactions();

    // Queries execution time counter / Query statements counter
    const auto elapsed = countsQueries().hitTransactionalCounters(timer, countElapsed);

    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    if (connection.pretending())
        connection.logTransactionQueryForPretend(*RollBack);
    else
        connection.logTransactionQuery(*RollBack, elapsed);

    return true;
}

bool ManagesTransactions::savepoint(const QString &id)
{
    Q_ASSERT(m_inTransaction);

    auto &connection = databaseConnection();

    static const auto savepointQueryTmpl = sl("SAVEPOINT %1_%2");
    const auto queryString = savepointQueryTmpl.arg(m_savepointNamespace, id);

    // Is Elapsed timer needed?
    const auto countElapsed = connection.shouldCountElapsed();

    QElapsedTimer timer;
    if (countElapsed)
        timer.start();

    // Execute a savepoint query
    try {
#ifdef TINYORM_USING_QTSQLDRIVERS
        runCommonSavepointQuery(connection, queryString, *SavepointFunction);
#elif defined(TINYORM_USING_TINYDRIVERS)
        if (!connection.pretending())
            connection.getSqlQuery().exec(queryString);
#endif
    } catch (const SqlTransactionError &e) {
        tryAgainIfCausedByLostConnectionCommon(std::current_exception(),
                                               e.databaseText());
    }

    ++m_savepoints;

    // Queries execution time counter / Query statements counter
    const auto elapsed = countsQueries().hitTransactionalCounters(timer, countElapsed);

    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    if (connection.pretending())
        connection.logTransactionQueryForPretend(queryString);
    else
        connection.logTransactionQuery(queryString, elapsed);

    return true;
}

bool ManagesTransactions::rollbackToSavepoint(const QString &id)
{
    Q_ASSERT(m_inTransaction);
    Q_ASSERT(m_savepoints > 0);

    auto &connection = databaseConnection();

    static const auto rollbackToSavepointQueryTmpl = sl("ROLLBACK TO SAVEPOINT %1_%2");
    const auto queryString = rollbackToSavepointQueryTmpl.arg(m_savepointNamespace, id);

    // Is Elapsed timer needed?
    const auto countElapsed = connection.shouldCountElapsed();

    QElapsedTimer timer;
    if (countElapsed)
        timer.start();

    // Execute a rollback to savepoint query
    try {
#ifdef TINYORM_USING_QTSQLDRIVERS
        runCommonSavepointQuery(connection, queryString, *RollbackToSavepointFunction);
#elif defined(TINYORM_USING_TINYDRIVERS)
        if (!connection.pretending())
            connection.getSqlQuery().exec(queryString);
#endif
    } catch (const SqlTransactionError &e) {
        tryAgainIfCausedByLostConnectionCommon(std::current_exception(),
                                               e.databaseText());
    }

    m_savepoints = std::max<decltype (m_savepoints)>(0, m_savepoints - 1);

    // Queries execution time counter / Query statements counter
    const auto elapsed = countsQueries().hitTransactionalCounters(timer, countElapsed);

    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    if (connection.pretending())
        connection.logTransactionQueryForPretend(queryString);
    else
        connection.logTransactionQuery(queryString, elapsed);

    return true;
}

DatabaseConnection &
ManagesTransactions::setSavepointNamespace(const QString &savepointNamespace)
{
    m_savepointNamespace = savepointNamespace;

    return databaseConnection();
}

/* private */

#ifdef TINYORM_USING_QTSQLDRIVERS
void ManagesTransactions::runBeginTransaction(DatabaseConnection &connection)
{
    auto qtConnection = connection.getSqlConnection();

    // Nothing to do
    if (connection.pretending() || qtConnection.transaction())
        return;

    throwSqlTransactionError(*BeginTransactionFunction, *StartTransaction,
                             qtConnection.lastError());
}

void ManagesTransactions::runCommit(DatabaseConnection &connection)
{
    auto qtConnection = connection.getRawSqlConnection();

    // Nothing to do
    if (connection.pretending() || qtConnection.commit())
        return;

    throwSqlTransactionError(*CommitFunction, *Commit, qtConnection.lastError());
}

void ManagesTransactions::runRollBack(DatabaseConnection &connection)
{
    auto qtConnection = connection.getRawSqlConnection();

    // Nothing to do
    if (connection.pretending() || qtConnection.rollback())
        return;

    throwSqlTransactionError(*RollBackFunction, *RollBack, qtConnection.lastError());
}

void ManagesTransactions::runCommonSavepointQuery(
        DatabaseConnection &connection, const QString &queryString,
        const QString &functionName)
{
    auto sqlQuery = connection.getSqlQuery();

    // Nothing to do
    if (connection.pretending() || sqlQuery.exec(queryString))
        return;

    throwSqlTransactionError(functionName, sqlQuery);
}

void ManagesTransactions::throwSqlTransactionError(
        const QString &functionName, const QString &queryString, QSqlError &&error)
{
    throw Exceptions::SqlTransactionError(
                sl("Transaction statement in %1() failed : %2")
                .arg(functionName, queryString),
                std::move(error));
}

void ManagesTransactions::throwSqlTransactionError(const QString &functionName,
                                                   const QSqlQuery &sqlQuery)
{
    auto executedQuery = sqlQuery.executedQuery();
    if (executedQuery.isEmpty())
        executedQuery = sqlQuery.lastQuery();

    throwSqlTransactionError(functionName, executedQuery, sqlQuery.lastError());
}
#endif

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

void ManagesTransactions::tryAgainIfCausedByLostConnectionStart(
        DatabaseConnection &connection, const std::exception_ptr &ePtr,
        const QString &errorMessage)
{
    if (!DetectsLostConnections::causedByLostConnection(errorMessage))
        std::rethrow_exception(ePtr);

    connection.reconnect();

    connection.getSqlConnection().transaction();
}

void ManagesTransactions::tryAgainIfCausedByLostConnectionCommon(
        const std::exception_ptr &ePtr, const QString &errorMessage)
{
    /* Don't call reconnection logic here because if the current session is
       in the transaction and eg. a connection is lost then the transaction will be
       rolled back which means reconnecting and re-executing the SAVEPOINT statement
       doesn't make sense as the START TRANSACTION must be called before SAVEPOINT
       statement and also, there could be many queries between. */
    if (DetectsLostConnections::causedByLostConnection(errorMessage))
        resetTransactions();

    std::rethrow_exception(ePtr);
}

} // namespace Orm::Concerns

TINYORM_END_COMMON_NAMESPACE
