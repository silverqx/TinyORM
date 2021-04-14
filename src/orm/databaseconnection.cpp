#include "orm/databaseconnection.hpp"

#include <QDateTime>

#include "orm/logquery.hpp"
#include "orm/query/querybuilder.hpp"
#include "orm/sqltransactionerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

const char *
DatabaseConnection::defaultConnectionName = const_cast<char *>("tinyorm_default");
const char *
DatabaseConnection::SAVEPOINT_NAMESPACE   = const_cast<char *>("tinyorm_savepoint");

/*!
    \class DatabaseConnection
    \brief The DatabaseConnection class handles a connection to the database.

    \ingroup database
    \inmodule Export

    Wrapper around QSqlDatabase class, many methods are only proxies with
    some error handling.
    Savepoints (nested transactions) are not managed automatically like eg
    in Laravel's Eloquent ORM, because I want to be more explicit, so when
    I need to start Savepoint, I will call savepoint() method and not
    transcation(). The same is true for rollBack(), so I will not call
    rollBack() for both, to end transaction and to end savepoint, instead,
    I will call rollBack() for transaction and rollbackToSavepoint("xx_1")
    for savepoint. This makes it clear at a glance what is happening.
*/

// TODO err, may be configurable exceptions by config or compiler directive? (completely disable exceptions by directive) silverqx
DatabaseConnection::DatabaseConnection(
        const std::function<Connectors::ConnectionName()> &connection,
        const QString &database, const QString tablePrefix,
        const QVariantHash &config
)
    : m_qtConnectionResolver(std::move(connection))
    , m_database(database)
    , m_tablePrefix(tablePrefix)
    , m_config(config)
{}

QSharedPointer<QueryBuilder>
DatabaseConnection::table(const QString &table, const QString &as)
{
    auto builder = QSharedPointer<QueryBuilder>::create(*this, *m_queryGrammar);

    builder->from(table, as);

    return builder;
}

QSharedPointer<QueryBuilder> DatabaseConnection::query()
{
    return QSharedPointer<QueryBuilder>::create(*this, *m_queryGrammar);
}

bool DatabaseConnection::beginTransaction()
{
    Q_ASSERT(m_inTransaction == false);

    static const auto query = QStringLiteral("START TRANSACTION");

    QElapsedTimer timer;
    if (m_debugSql || m_countingElapsed)
        timer.start();

    if (!getQtConnection().transaction())
        throw SqlTransactionError(
                QStringLiteral("Statement in %1() failed : %2").arg(__FUNCTION__, query),
                getRawQtConnection().lastError());

    m_inTransaction = true;

    // Queries execution time counter / Query statements counter
    const auto elapsed = hitTransactionalCounters(timer);

#ifdef TINYORM_DEBUG_SQL
    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    logTransactionQuery(query, elapsed);
#endif

    return true;
}

bool DatabaseConnection::commit()
{
    Q_ASSERT(m_inTransaction);

    static const auto query = QStringLiteral("COMMIT");

    QElapsedTimer timer;
    if (m_debugSql || m_countingElapsed)
        timer.start();

    if (!getQtConnection().commit())
        throw SqlTransactionError(
                QStringLiteral("Statement in %1() failed : %2").arg(__FUNCTION__, query),
                getRawQtConnection().lastError());

    m_inTransaction = false;

    // Queries execution time counter / Query statements counter
    const auto elapsed = hitTransactionalCounters(timer);

#ifdef TINYORM_DEBUG_SQL
    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    logTransactionQuery(query, elapsed);
#endif

    return true;
}

bool DatabaseConnection::rollBack()
{
    Q_ASSERT(m_inTransaction);

    static const auto query = QStringLiteral("ROLLBACK");

    QElapsedTimer timer;
    if (m_debugSql || m_countingElapsed)
        timer.start();

    if (!getQtConnection().rollback())
        throw SqlTransactionError(
                QStringLiteral("Statement in %1() failed : %2").arg(__FUNCTION__, query),
                getRawQtConnection().lastError());

    m_inTransaction = false;

    // Queries execution time counter / Query statements counter
    const auto elapsed = hitTransactionalCounters(timer);

#ifdef TINYORM_DEBUG_SQL
    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    logTransactionQuery(query, elapsed);
#endif

    return true;
}

bool DatabaseConnection::savepoint(const QString &id)
{
    // TODO rewrite savepoint() and rollBack() with a new m_connection.statement() API silverqx
    Q_ASSERT(m_inTransaction);

    auto savePoint = getQtQuery();
    const auto query = QStringLiteral("SAVEPOINT %1_%2").arg(SAVEPOINT_NAMESPACE, id);

    QElapsedTimer timer;
    if (m_debugSql || m_countingElapsed)
        timer.start();

    // Execute a savepoint query
    if (!savePoint.exec(query))
        throw SqlTransactionError(
                QStringLiteral("Statement in %1() failed : %2")
                    .arg(__FUNCTION__, query),
                savePoint.lastError());

    ++m_savepoints;

    // Queries execution time counter / Query statements counter
    const auto elapsed = hitTransactionalCounters(timer);

#ifdef TINYORM_DEBUG_SQL
    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    logTransactionQuery(query, elapsed);
#endif

    return true;
}

bool DatabaseConnection::savepoint(const size_t id)
{
    return savepoint(QString::number(id));
}

bool DatabaseConnection::rollbackToSavepoint(const QString &id)
{
    Q_ASSERT(m_inTransaction);
    Q_ASSERT(m_savepoints > 0);

    auto rollbackToSavepoint = getQtQuery();
    const auto query = QStringLiteral("ROLLBACK TO SAVEPOINT %1_%2")
                       .arg(SAVEPOINT_NAMESPACE, id);

    QElapsedTimer timer;
    if (m_debugSql || m_countingElapsed)
        timer.start();

    // Execute a rollback to savepoint query
    if (!rollbackToSavepoint.exec(query))
        throw SqlTransactionError(
                QStringLiteral("Statement in %1() failed : %2")
                    .arg(__FUNCTION__, query),
                rollbackToSavepoint.lastError());

    m_savepoints = std::max<int>(0, m_savepoints - 1);

    // Queries execution time counter / Query statements counter
    const auto elapsed = hitTransactionalCounters(timer);

#ifdef TINYORM_DEBUG_SQL
    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    logTransactionQuery(query, elapsed);
#endif

    return true;
}

bool DatabaseConnection::rollbackToSavepoint(const size_t id)
{
    return rollbackToSavepoint(QString::number(id));
}

QSqlQuery
DatabaseConnection::select(const QString &queryString,
                           const QVector<QVariant> &bindings)
{
    /* select() and statement() have the same implementation for now, but
       they don't in original Eloquent implementation.
       They will have different implementation, when I implement
       ::read ::write connections and sticky connection
       (recordsHaveBeenModified()). */
    return statement(queryString, bindings);
}

QSqlQuery
DatabaseConnection::selectFromWriteConnection(const QString &queryString,
                                              const QVector<QVariant> &bindings)
{
    // This member function is used from the schema builders/post-processors only
    // FEATURE read/write connection silverqx
    return select(queryString, bindings/*, false*/);
}

QSqlQuery
DatabaseConnection::selectOne(const QString &queryString,
                              const QVector<QVariant> &bindings)
{
    auto query = statement(queryString, bindings);

    query.first();

    return query;
}

QSqlQuery
DatabaseConnection::insert(const QString &queryString,
                           const QVector<QVariant> &bindings)
{
    return statement(queryString, bindings);
}

std::tuple<int, QSqlQuery>
DatabaseConnection::update(const QString &queryString,
                           const QVector<QVariant> &bindings)
{
    return affectingStatement(queryString, bindings);
}

std::tuple<int, QSqlQuery>
DatabaseConnection::remove(const QString &queryString,
                           const QVector<QVariant> &bindings)
{
    return affectingStatement(queryString, bindings);
}

QSqlQuery DatabaseConnection::statement(const QString &queryString,
                                        const QVector<QVariant> &bindings)
{
    return run<QSqlQuery>(queryString, bindings,
               [this](const QString &queryString, const QVector<QVariant> &bindings)
               -> QSqlQuery
    {
        // Prepare QSqlQuery
        auto query = prepareQuery(queryString);

        bindValues(query, prepareBindings(bindings));

        // TODO dilemma, return ok 😭 silverqx
        if (query.exec()) {
            // Query statements counter
            if (m_countingStatements)
                ++m_statementsCounter.normal;

            return query;
        }

        /* If an error occurs when attempting to run a query, we'll transform it
           to the exception QueryError(), which formats the error message to
           include the bindings with SQL, which will make this exception a lot
           more helpful to the developer instead of just the database's errors. */
        throw QueryError(
                    // TODO next use __FUNCTION__ in similar statements silverqx
                    QStringLiteral("Statement in %1() failed.").arg(__FUNCTION__),
                    query, bindings);
    });
}

std::tuple<int, QSqlQuery>
DatabaseConnection::affectingStatement(const QString &queryString,
                                       const QVector<QVariant> &bindings)
{
    return run<std::tuple<int, QSqlQuery>>(queryString, bindings,
            [this](const QString &queryString, const QVector<QVariant> &bindings)
            -> std::tuple<int, QSqlQuery>
    {
        // Prepare QSqlQuery
        auto query = prepareQuery(queryString);

        bindValues(query, prepareBindings(bindings));

        if (query.exec()) {
            // Affecting statements counter
            if (m_countingStatements)
                ++m_statementsCounter.affecting;

            return {query.numRowsAffected(), query};
        }

        /* If an error occurs when attempting to run a query, we'll transform it
           to the exception QueryError(), which formats the error message to
           include the bindings with SQL, which will make this exception a lot
           more helpful to the developer instead of just the database's errors. */
        throw QueryError(
                    QStringLiteral("Affecting statement in %1() failed.")
                        .arg(__FUNCTION__),
                    query, bindings);
    });
}

QSqlDatabase DatabaseConnection::getQtConnection()
{
    if (!m_qtConnection) {
        // This should never happen 🤔
        Q_ASSERT(m_qtConnectionResolver);

        // Reconnect if missing
        m_qtConnection = std::invoke(m_qtConnectionResolver);

        /* This should never happen 🤔, do this check only when the QSqlDatabase
           connection was resolved by connection resolver. */
        if (!QSqlDatabase::contains(*m_qtConnection))
            throw RuntimeError("Connection '" + *m_qtConnection + "' doesn't exist.");
    }

    // Return the connection from QSqlDatabase connection manager
    return QSqlDatabase::database(*m_qtConnection);
}

QSqlDatabase DatabaseConnection::getRawQtConnection() const
{
    return QSqlDatabase::database(*m_qtConnection);
}

DatabaseConnection &
DatabaseConnection::setQtConnectionResolver(
        const std::function<Connectors::ConnectionName()> &resolver)
{
    /* Reset transaction and savepoints as the underlying connection will be
       disconnected and reconnected again. The m_qtConnection have to be
       reset because it indicates whether the underlying connection is active. */
    resetTransactions();

    /* m_qtConnection.reset() is called also in DatabaseConnection::disconnect(),
       because both methods are public apis.
       m_qtConnection can also be understood as m_qtConnectionWasResolved,
       because it performs two functions, saves active connection name and
       if it's not nullopt, then it means, that the database connection was
       resolved by m_qtConnectionResolver.
       If it's nullopt, then m_qtConnectionResolver should be called to
       resolve a new database connection.
       This ensures, that a database connection will be resolved lazily, only
       when actually needed. */
    m_qtConnection.reset();
    m_qtConnectionResolver = resolver;

    return *this;
}

QSqlQuery DatabaseConnection::getQtQuery()
{
    return QSqlQuery(getQtConnection());
}

// TODO perf, modify bindings directly and return reference, debug impact silverqx
QVector<QVariant>
DatabaseConnection::prepareBindings(QVector<QVariant> bindings) const
{
//    const auto &grammar = getQueryGrammar();

    for (auto &binding : bindings) {
        // Nothing to convert
        if (!binding.isValid() || binding.isNull())
            continue;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        switch (binding.typeId()) {
#else
        switch (binding.userType()) {
#endif
        /* We need to transform all instances of DateTimeInterface into the actual
           date string. Each query grammar maintains its own date string format
           so we'll just ask the grammar for the format to get from the date. */
        case QMetaType::QDate:
        case QMetaType::QDateTime:
            binding = binding.value<QDateTime>()
                      .toString(m_queryGrammar->getDateFormat());
            break;

        /* Even if eg. the MySQL driver handles this internally, I will do it this way
           to be consistent across all supported databases. */
        case QMetaType::Bool:
            binding.convert(QMetaType::Int);
            break;

        default:
            break;
        }
    }

    return bindings;
}

void DatabaseConnection::bindValues(QSqlQuery &query,
                                    const QVector<QVariant> &bindings) const
{
    auto itBinding = bindings.constBegin();
    while (itBinding != bindings.constEnd()) {
        query.addBindValue(*itBinding);

        ++itBinding;
    }
}

void DatabaseConnection::logQuery(const QSqlQuery &query,
        const std::optional<qint64> &elapsed = std::nullopt) const
{
    qDebug().nospace().noquote()
        << "Executed prepared query (" << (elapsed ? *elapsed : -1) << "ms, "
        << query.size() << " results, " << query.numRowsAffected()
        << " affected"
        // Connection name
        << (m_qtConnection ? QStringLiteral(", %1").arg(*m_qtConnection) : "")
        << ") : " << parseExecutedQuery(query);;
}

void DatabaseConnection::logQuery(
        const std::tuple<int, QSqlQuery> &queryResult,
        const std::optional<qint64> &elapsed) const
{
    logQuery(std::get<1>(queryResult), elapsed);
}

void DatabaseConnection::logTransactionQuery(
        const QString &query,
        const std::optional<qint64> &elapsed = std::nullopt)
{
    const auto connectionName = getName();

    // This is only internal method and logs the passed string
    qDebug().nospace().noquote()
        << "Executed transaction query (" << (elapsed ? *elapsed : -1) << "ms"
        // Connection name
        << (!connectionName.isEmpty() ? QStringLiteral(", %1").arg(connectionName) : "")
        << ") : " << query;
}

std::optional<qint64>
DatabaseConnection::hitTransactionalCounters(const QElapsedTimer &timer)
{
    /* This function was extracted to prevent duplicit code only. */
    std::optional<qint64> elapsed;

    if (m_debugSql || m_countingElapsed)
        elapsed = timer.elapsed();

    // Queries execution time counter
    if (m_countingElapsed)
        m_elapsedCounter += *elapsed;
    // Query statements counter
    if (m_countingStatements)
        ++m_statementsCounter.transactional;

    return elapsed;
}

bool DatabaseConnection::pingDatabase()
{
    throw RuntimeError(
                QStringLiteral("The '%1' database driver doesn't support ping command.")
                .arg(driverName()));
}

void DatabaseConnection::reconnect() const
{
    if (!m_reconnector)
        throw std::runtime_error("Lost connection and no reconnector available.");

    std::invoke(m_reconnector, *this);
}

void DatabaseConnection::disconnect()
{
    /* Closes the database connection, freeing any resources acquired,
       and invalidating any existing QSqlQuery objects that are used
       with the database.
       Only close the QSqlDatabase database connection and don't remove it
       from QSqlDatabase connection repository, so they can be reused, it's
       better for performance. */
    getRawQtConnection().close();

    m_qtConnection.reset();
    m_qtConnectionResolver = nullptr;
}

void DatabaseConnection::useDefaultQueryGrammar()
{
    m_queryGrammar = getDefaultQueryGrammar();
}

const QueryGrammar &DatabaseConnection::getQueryGrammar() const
{
    return *m_queryGrammar;
}

void DatabaseConnection::useDefaultSchemaGrammar()
{
    m_schemaGrammar = getDefaultSchemaGrammar();
}

const SchemaGrammar &DatabaseConnection::getSchemaGrammar() const
{
    return *m_schemaGrammar;
}

std::unique_ptr<SchemaBuilder> DatabaseConnection::getSchemaBuilder()
{
    if (!m_schemaGrammar)
        useDefaultSchemaGrammar();

    return std::make_unique<SchemaBuilder>(*this);
}

void DatabaseConnection::useDefaultPostProcessor()
{
    m_postProcessor = getDefaultPostProcessor();
}

const QueryProcessor &DatabaseConnection::getPostProcessor() const
{
    return *m_postProcessor;
}

DatabaseConnection &
DatabaseConnection::setReconnector(const ReconnectorType &reconnector)
{
    m_reconnector = reconnector;

    return *this;
}

QVariant DatabaseConnection::getConfig(const QString &option) const
{
    return m_config.value(option);
}

const QVariantHash &
DatabaseConnection::getConfig() const
{
    return m_config;
}

bool DatabaseConnection::countingElapsed() const
{
    return m_countingElapsed;
}

DatabaseConnection &DatabaseConnection::enableElapsedCounter()
{
    m_countingElapsed = true;
    m_elapsedCounter = 0;

    return *this;
}

DatabaseConnection &DatabaseConnection::disableElapsedCounter()
{
    m_countingElapsed = false;
    m_elapsedCounter = -1;

    return *this;
}

qint64 DatabaseConnection::getElapsedCounter() const
{
    return m_elapsedCounter;
}

qint64 DatabaseConnection::takeElapsedCounter()
{
    if (!m_countingElapsed)
        return -1;

    const auto elapsed = m_elapsedCounter;

    m_elapsedCounter = 0;

    return elapsed;
}

DatabaseConnection &DatabaseConnection::resetElapsedCounter()
{
    m_elapsedCounter = 0;

    return *this;
}

bool DatabaseConnection::countingStatements() const
{
    return m_countingStatements;
}

DatabaseConnection &DatabaseConnection::enableStatementsCounter()
{
    m_countingStatements = true;

    m_statementsCounter.normal        = 0;
    m_statementsCounter.affecting     = 0;
    m_statementsCounter.transactional = 0;

    return *this;
}

DatabaseConnection &DatabaseConnection::disableStatementsCounter()
{
    m_countingStatements = false;

    m_statementsCounter.normal        = -1;
    m_statementsCounter.affecting     = -1;
    m_statementsCounter.transactional = -1;

    return *this;
}

const StatementsCounter &DatabaseConnection::getStatementsCounter() const
{
    return m_statementsCounter;
}

StatementsCounter DatabaseConnection::takeStatementsCounter()
{
    if (!m_countingStatements)
        return {};

    const auto counter = m_statementsCounter;

    m_statementsCounter.normal        = 0;
    m_statementsCounter.affecting     = 0;
    m_statementsCounter.transactional = 0;

    return counter;
}

DatabaseConnection &DatabaseConnection::resetStatementsCounter()
{
    m_statementsCounter.normal        = 0;
    m_statementsCounter.affecting     = 0;
    m_statementsCounter.transactional = 0;

    return *this;
}

QString DatabaseConnection::driverName()
{
    return getQtConnection().driverName();
}

std::unique_ptr<QueryProcessor> DatabaseConnection::getDefaultPostProcessor() const
{
    return std::make_unique<QueryProcessor>();
}

void DatabaseConnection::reconnectIfMissingConnection() const
{
    if (!m_qtConnectionResolver) {
        // This should never happen, but when it does, I want to know about that
        Q_ASSERT(m_qtConnection);

        reconnect();
    }
}

DatabaseConnection &DatabaseConnection::resetTransactions()
{
    m_savepoints = 0;
    m_inTransaction = false;

    return *this;
}

void DatabaseConnection::logDisconnected()
{
    if (m_disconnectedLogged)
        return;
    m_disconnectedLogged = true;

    // Reset connected flag
    m_connectedLogged = false;

    qWarning() << "No active database connection, torrent additions / removes will "
                  "not be commited";
}

void DatabaseConnection::logConnected()
{
    if (m_connectedLogged)
        return;
    m_connectedLogged = true;

    // Reset disconnected flag
    m_disconnectedLogged = false;

    qInfo() << "Database connected";
}

QSqlQuery DatabaseConnection::prepareQuery(const QString &queryString)
{
    // Prepare query string
    auto query = getQtQuery();

    // TODO solve setForwardOnly() in DatabaseConnection class, again this problem 🤔 silverqx
//    query.setForwardOnly(m_forwardOnly);

    query.prepare(queryString);

    return query;
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
