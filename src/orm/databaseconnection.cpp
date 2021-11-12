#include "orm/databaseconnection.hpp"

#include <QDateTime>
#if defined(TINYORM_DEBUG_SQL) || defined(TINYORM_MYSQL_PING)
#include <QDebug>
#endif

#include "orm/configuration.hpp"
#include "orm/exceptions/sqltransactionerror.hpp"
#include "orm/macros/likely.hpp"
#include "orm/query/querybuilder.hpp"
#ifdef TINYORM_DEBUG_SQL
#include "orm/utils/query.hpp"
#endif
#include "orm/utils/type.hpp"

#ifdef TINYORM_DEBUG_SQL
using QueryUtils = Orm::Utils::Query;
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

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
        std::function<Connectors::ConnectionName()> &&connection,
        // NOLINTNEXTLINE(modernize-pass-by-value)
        const QString &database, const QString &tablePrefix, const QVariantHash &config
)
    : m_qtConnectionResolver(std::move(connection))
    , m_database(database)
    , m_tablePrefix(tablePrefix)
    , m_config(config)
    , m_savepointNamespace(Orm::Support::DatabaseConfiguration::defaultSavepointNamespace)
    , m_connectionName(getConfig(NAME).value<QString>())
    , m_hostName(getConfig(host_).value<QString>())
{}

QSharedPointer<QueryBuilder>
DatabaseConnection::table(const QString &table, const QString &as)
{
    auto builder = QSharedPointer<QueryBuilder>::create(*this, *m_queryGrammar);

    builder->from(table, as);

    return builder;
}

DatabaseConnection &DatabaseConnection::setTablePrefix(const QString &prefix)
{
    m_tablePrefix = prefix;

    getQueryGrammar().setTablePrefix(prefix);

    return *this;
}

BaseGrammar &DatabaseConnection::withTablePrefix(BaseGrammar &grammar) const
{
    grammar.setTablePrefix(m_tablePrefix);

    return grammar;
}

QSharedPointer<QueryBuilder> DatabaseConnection::query()
{
    return QSharedPointer<QueryBuilder>::create(*this, *m_queryGrammar);
}

bool DatabaseConnection::beginTransaction()
{
    Q_ASSERT(m_inTransaction == false);

    static const auto query = QStringLiteral("START TRANSACTION");

    // Elapsed timer needed
    const auto countElapsed = !m_pretending && (m_debugSql || m_countingElapsed);

    QElapsedTimer timer;
    if (countElapsed)
        timer.start();

    if (!m_pretending && !getQtConnection().transaction())
        throw Exceptions::SqlTransactionError(
                QStringLiteral("Statement in %1() failed : %2").arg(__tiny_func__,
                                                                    query),
                getRawQtConnection().lastError());

    m_inTransaction = true;

    // Queries execution time counter / Query statements counter
    auto elapsed = hitTransactionalCounters(timer, countElapsed);

    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    if (m_pretending)
        logTransactionQueryForPretend(query);
    else
        logTransactionQuery(query, std::move(elapsed));

    return true;
}

bool DatabaseConnection::commit()
{
    Q_ASSERT(m_inTransaction);

    static const auto query = QStringLiteral("COMMIT");

    // Elapsed timer needed
    const auto countElapsed = !m_pretending && (m_debugSql || m_countingElapsed);

    QElapsedTimer timer;
    if (countElapsed)
        timer.start();

    // TODO rewrite transactions to DatabaseConnection::statement, so I have access to QSqlQuery for logQuery() silverqx
    if (!m_pretending && !getQtConnection().commit())
        throw Exceptions::SqlTransactionError(
                QStringLiteral("Statement in %1() failed : %2").arg(__tiny_func__,
                                                                    query),
                getRawQtConnection().lastError());

    m_inTransaction = false;

    // Queries execution time counter / Query statements counter
    auto elapsed = hitTransactionalCounters(timer, countElapsed);

    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    if (m_pretending)
        logTransactionQueryForPretend(query);
    else
        logTransactionQuery(query, std::move(elapsed));

    return true;
}

bool DatabaseConnection::rollBack()
{
    Q_ASSERT(m_inTransaction);

    static const auto query = QStringLiteral("ROLLBACK");

    // Elapsed timer needed
    const auto countElapsed = !m_pretending && (m_debugSql || m_countingElapsed);

    QElapsedTimer timer;
    if (countElapsed)
        timer.start();

    if (!m_pretending && !getQtConnection().rollback())
        throw Exceptions::SqlTransactionError(
                QStringLiteral("Statement in %1() failed : %2").arg(__tiny_func__,
                                                                    query),
                getRawQtConnection().lastError());

    m_inTransaction = false;

    // Queries execution time counter / Query statements counter
    auto elapsed = hitTransactionalCounters(timer, countElapsed);

    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    if (m_pretending)
        logTransactionQueryForPretend(query);
    else
        logTransactionQuery(query, std::move(elapsed));

    return true;
}

bool DatabaseConnection::savepoint(const QString &id)
{
    // TODO rewrite savepoint() and rollBack() with a new m_connection.statement() API silverqx
    Q_ASSERT(m_inTransaction);

    auto savePoint = getQtQuery();
    const auto query = QStringLiteral("SAVEPOINT %1_%2").arg(m_savepointNamespace, id);

    // Elapsed timer needed
    const auto countElapsed = !m_pretending && (m_debugSql || m_countingElapsed);

    QElapsedTimer timer;
    if (countElapsed)
        timer.start();

    // Execute a savepoint query
    if (!m_pretending && !savePoint.exec(query))
        throw Exceptions::SqlTransactionError(
                QStringLiteral("Statement in %1() failed : %2")
                    .arg(__tiny_func__, query),
                savePoint.lastError());

    ++m_savepoints;

    // Queries execution time counter / Query statements counter
    auto elapsed = hitTransactionalCounters(timer, countElapsed);

    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    if (m_pretending)
        logTransactionQueryForPretend(query);
    else
        logTransactionQuery(query, std::move(elapsed));

    return true;
}

bool DatabaseConnection::savepoint(const std::size_t id)
{
    return savepoint(QString::number(id));
}

bool DatabaseConnection::rollbackToSavepoint(const QString &id)
{
    Q_ASSERT(m_inTransaction);
    Q_ASSERT(m_savepoints > 0);

    auto rollbackToSavepoint = getQtQuery();
    const auto query = QStringLiteral("ROLLBACK TO SAVEPOINT %1_%2")
                       .arg(m_savepointNamespace, id);

    // Elapsed timer needed
    const auto countElapsed = !m_pretending && (m_debugSql || m_countingElapsed);

    QElapsedTimer timer;
    if (countElapsed)
        timer.start();

    // Execute a rollback to savepoint query
    if (!m_pretending && !rollbackToSavepoint.exec(query))
        throw Exceptions::SqlTransactionError(
                QStringLiteral("Statement in %1() failed : %2")
                    .arg(__tiny_func__, query),
                rollbackToSavepoint.lastError());

    m_savepoints = std::max<std::size_t>(0, m_savepoints - 1);

    // Queries execution time counter / Query statements counter
    auto elapsed = hitTransactionalCounters(timer, countElapsed);

    /* Once we have run the transaction query we will calculate the time
       that it took to run and then log the query and execution time.
       We'll log time in milliseconds. */
    if (m_pretending)
        logTransactionQueryForPretend(query);
    else
        logTransactionQuery(query, std::move(elapsed));

    return true;
}

bool DatabaseConnection::rollbackToSavepoint(const std::size_t id)
{
    return rollbackToSavepoint(QString::number(id));
}

QSqlQuery
DatabaseConnection::select(const QString &queryString,
                           const QVector<QVariant> &bindings)
{
    return run<QSqlQuery>(queryString, bindings,
               [this](const QString &queryString_, const QVector<QVariant> &bindings_)
               -> QSqlQuery
    {
        if (m_pretending)
            return getQtQuery();

        // Prepare QSqlQuery
        auto query = prepareQuery(queryString_);

        bindValues(query, prepareBindings(bindings_));

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
        throw Exceptions::QueryError(
                    QStringLiteral("Select statement in %1() failed.")
                        .arg(__tiny_func__),
                    query, bindings_);
    });
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
    auto query = select(queryString, bindings);

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
               [this](const QString &queryString_, const QVector<QVariant> &bindings_)
               -> QSqlQuery
    {
        if (m_pretending)
            return getQtQuery();

        // Prepare QSqlQuery
        auto query = prepareQuery(queryString_);

        bindValues(query, prepareBindings(bindings_));

        if (query.exec()) {
            // Query statements counter
            if (m_countingStatements)
                ++m_statementsCounter.normal;

            recordsHaveBeenModified();

            return query;
        }

        /* If an error occurs when attempting to run a query, we'll transform it
           to the exception QueryError(), which formats the error message to
           include the bindings with SQL, which will make this exception a lot
           more helpful to the developer instead of just the database's errors. */
        throw Exceptions::QueryError(
                    // TODO next use __tiny_func__ in similar statements/exceptions silverqx
                    QStringLiteral("Statement in %1() failed.").arg(__tiny_func__),
                    query, bindings_);
    });
}

std::tuple<int, QSqlQuery>
DatabaseConnection::affectingStatement(const QString &queryString,
                                       const QVector<QVariant> &bindings)
{
    return run<std::tuple<int, QSqlQuery>>(queryString, bindings,
            [this](const QString &queryString_, const QVector<QVariant> &bindings_)
            -> std::tuple<int, QSqlQuery>
    {
        if (m_pretending)
            return {0, getQtQuery()};

        // Prepare QSqlQuery
        auto query = prepareQuery(queryString_);

        bindValues(query, prepareBindings(bindings_));

        if (query.exec()) {
            // Affecting statements counter
            if (m_countingStatements)
                ++m_statementsCounter.affecting;

            auto numRowsAffected = query.numRowsAffected();

            recordsHaveBeenModified(numRowsAffected > 0);

            return {numRowsAffected, query};
        }

        /* If an error occurs when attempting to run a query, we'll transform it
           to the exception QueryError(), which formats the error message to
           include the bindings with SQL, which will make this exception a lot
           more helpful to the developer instead of just the database's errors. */
        throw Exceptions::QueryError(
                    QStringLiteral("Affecting statement in %1() failed.")
                        .arg(__tiny_func__),
                    query, bindings_);
    });
}

QSqlQuery DatabaseConnection::unprepared(const QString &queryString)
{
    return run<QSqlQuery>(queryString, {},
               [this](const QString &queryString_, const QVector<QVariant> &/*unused*/)
               -> QSqlQuery
    {
        if (m_pretending)
            return getQtQuery();

        // Prepare unprepared QSqlQuery 🙂
        auto query = getQtQuery();

        if (query.exec(queryString_)) {
            // Query statements counter
            if (m_countingStatements)
                ++m_statementsCounter.normal;

            recordsHaveBeenModified();

            return query;
        }

        /* If an error occurs when attempting to run a query, we'll transform it
           to the exception QueryError(), which formats the error message to
           include the bindings with SQL, which will make this exception a lot
           more helpful to the developer instead of just the database's errors. */
        throw Exceptions::QueryError(
                    QStringLiteral("Statement in %1() failed.").arg(__tiny_func__),
                    query);
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
            throw Exceptions::RuntimeError(
                    "Connection '" + *m_qtConnection + "' doesn't exist.");
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

        /* I have decided to not handle the QMetaType::Bool here, little info:
           - Qt's MySql driver handles bool values internally, it doesn't matter if you
             pass true/false or 0/1
           - I have not investigated how Qt's Postgres driver works internally, but
             Postgres is very sensitive about bool columns and bool values, so if you
             have bool column then you have to pass bool type to the driver
           - I don't remember about Qt's Sqlite driver exactly, but I'm pretty sure that
             it doesn't handle bool values as Qt's mysql driver does, because I had some
             problems with bool values when I have added Sqlite support */

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

std::optional<qint64>
DatabaseConnection::hitTransactionalCounters(const QElapsedTimer timer,
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

bool DatabaseConnection::pingDatabase()
{
    throw Exceptions::RuntimeError(
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

void DatabaseConnection::logQuery(
        const QSqlQuery &query,
        const std::optional<qint64> elapsed = std::nullopt) const
{
    if (m_loggingQueries && m_queryLog) {
        auto executedQuery = query.executedQuery();
        if (executedQuery.isEmpty())
            executedQuery = query.lastQuery();

        m_queryLog->append({std::move(executedQuery),
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                            query.boundValues(),
#else
                            convertNamedToPositionalBindings(query.boundValues()),
#endif
                            Log::Type::NORMAL, ++m_queryLogId,
                            elapsed ? *elapsed : -1, query.size(),
                            query.numRowsAffected()});
    }

#ifdef TINYORM_DEBUG_SQL
    const auto &connectionName = getName();

    qDebug("Executed prepared query (%llims, %i results, %i affected%s) : %s",
           elapsed ? *elapsed : -1,
           query.size(),
           query.numRowsAffected(),
           connectionName.isEmpty() ? ""
                                    : QStringLiteral(", %1").arg(connectionName)
                                      .toUtf8().constData(),
           QueryUtils::parseExecutedQuery(query).toUtf8().constData());
#endif
}

void DatabaseConnection::logQuery(
        const std::tuple<int, QSqlQuery> &queryResult,
        const std::optional<qint64> elapsed) const
{
    logQuery(std::get<1>(queryResult), elapsed);
}

void DatabaseConnection::logQueryForPretend(
        const QString &query, const QVector<QVariant> &bindings) const
{
    if (m_loggingQueries && m_queryLog)
        m_queryLog->append({query, bindings, Log::Type::NORMAL, ++m_queryLogId});

#ifdef TINYORM_DEBUG_SQL
    const auto &connectionName = getName();

    qDebug("Pretended prepared query (%s) : %s",
           connectionName.isEmpty() ? "" : connectionName.toUtf8().constData(),
           QueryUtils::parseExecutedQueryForPretend(query,
                                                    bindings).toUtf8().constData());
#endif
}

void DatabaseConnection::logTransactionQuery(
        const QString &query, const std::optional<qint64> elapsed) const
{
    if (m_loggingQueries && m_queryLog)
        m_queryLog->append({query, {}, Log::Type::TRANSACTION, ++m_queryLogId,
                            elapsed ? *elapsed : -1});

#ifdef TINYORM_DEBUG_SQL
    const auto &connectionName = getName();

    qDebug("%s transaction query (%llims%s) : %s",
           QStringLiteral("Executed").toUtf8().constData(),
           elapsed ? *elapsed : -1,
           connectionName.isEmpty() ? ""
                                    : QStringLiteral(", %1").arg(connectionName)
                                      .toUtf8().constData(),
           query.toUtf8().constData());
#endif
}

void DatabaseConnection::logTransactionQueryForPretend(const QString &query) const
{
    if (m_loggingQueries && m_queryLog)
        m_queryLog->append({query, {}, Log::Type::TRANSACTION, ++m_queryLogId});

#ifdef TINYORM_DEBUG_SQL
    const auto &connectionName = getName();

    qDebug("%s transaction query (%s) : %s",
           QStringLiteral("Pretended").toUtf8().constData(),
           connectionName.isEmpty() ? "" : connectionName.toUtf8().constData(),
           query.toUtf8().constData());
#endif
}

void DatabaseConnection::flushQueryLog()
{
    // TODO sync silverqx
    if (m_queryLog)
        m_queryLog->clear();

    m_queryLogId = 0;
}

void DatabaseConnection::enableQueryLog()
{
    /* Instantiate the query log vector lazily, right before it is really needed,
       and do not flush it. */
    if (!m_queryLog)
        m_queryLog = std::make_shared<QVector<Log>>();

    m_loggingQueries = true;
}

std::size_t DatabaseConnection::getQueryLogOrder()
{
    return m_queryLogId;
}

QString DatabaseConnection::driverName()
{
    return getQtConnection().driverName();
}

namespace
{
    using DriverNameMapType = std::unordered_map<QString, const QString &>;

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
    Q_GLOBAL_STATIC_WITH_ARGS(DriverNameMapType, DRIVER_NAME_MAP, ({
                                  {QMYSQL,  MYSQL_},
                                  {QPSQL,   POSTGRESQL},
                                  {QSQLITE, SQLITE}
                              }));
} // namespace

const QString &DatabaseConnection::driverNamePrintable()
{
    if (m_driverNamePrintable)
        return *m_driverNamePrintable;

    // Cache
    m_driverNamePrintable = DRIVER_NAME_MAP->at(driverName());

    return *m_driverNamePrintable;
}

QVector<Log>
DatabaseConnection::pretend(const std::function<void()> &callback)
{
    return withFreshQueryLog([this, &callback]
    {
        m_pretending = true;

        /* Basically to make the database connection "pretend", we will just return
           the default values for all the query methods, then we will return an
           array of queries that were "executed" within the Closure callback. */
        std::invoke(callback);

        m_pretending = false;

        return *m_queryLog;
    });
}

QVector<Log>
DatabaseConnection::pretend(const std::function<void(ConnectionInterface &)> &callback)
{
    return withFreshQueryLog([this, &callback]
    {
        m_pretending = true;

        /* Basically to make the database connection "pretend", we will just return
           the default values for all the query methods, then we will return an
           array of queries that were "executed" within the Closure callback. */
        std::invoke(callback, *this);

        m_pretending = false;

        return *m_queryLog;
    });
}

QVector<Log>
DatabaseConnection::withFreshQueryLog(const std::function<QVector<Log>()> &callback)
{
    /* First we will back up the value of the logging queries data members and then
       we'll enable query logging. The query log will also get cleared so we will
       have a new log of all the queries that will be executed. */
    const auto loggingQueries = m_loggingQueries;
    const auto queryLogId = m_queryLogId.load();
    m_queryLogId.store(0);

    enableQueryLog();

    if (m_queryLogForPretend) T_LIKELY
        m_queryLogForPretend->clear();
    else T_UNLIKELY
        // Create the query log lazily, right before it is really needed
        m_queryLogForPretend = std::make_shared<QVector<Log>>();

    // Swap query logs, so I don't have to manage separate logic for pretend code
    m_queryLog.swap(m_queryLogForPretend);

    /* Now we'll execute this callback and capture the result. Once it has been
       executed we will restore original values and give back the value of the callback
       so the original callers can have the results. */
    const auto result = std::invoke(callback);

    // Restore
    m_queryLog.swap(m_queryLogForPretend);
    m_loggingQueries = loggingQueries;
    m_queryLogId.store(queryLogId);

    // NRVO kicks in
    return result;
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

    qWarning("%s database disconnected (%s, %s@%s)",
             driverNamePrintable().toUtf8().constData(),
             m_connectionName.toUtf8().constData(),
             m_hostName.toUtf8().constData(),
             m_database.toUtf8().constData());
}

void DatabaseConnection::logConnected()
{
    if (m_connectedLogged)
        return;
    m_connectedLogged = true;

    // Reset disconnected flag
    m_disconnectedLogged = false;

    qInfo("%s database connected (%s, %s@%s)",
          driverNamePrintable().toUtf8().constData(),
          m_connectionName.toUtf8().constData(),
          m_hostName.toUtf8().constData(),
          m_database.toUtf8().constData());
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

QVector<QVariant>
DatabaseConnection::convertNamedToPositionalBindings(QVariantMap &&bindings) const
{
    QVector<QVariant> result;
    result.reserve(bindings.size());

    for (auto &&binding : bindings)
        result << std::move(binding);

    return result;
}

QueryGrammar &DatabaseConnection::getQueryGrammar()
{
    return *m_queryGrammar;
}

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE
