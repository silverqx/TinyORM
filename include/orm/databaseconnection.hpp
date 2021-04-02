#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include <QDebug>
#include <QElapsedTimer>
#include <QtSql/QSqlDatabase>

#include <optional>

#include "orm/concerns/detectslostconnections.hpp"
#include "orm/connectioninterface.hpp"
#include "orm/connectors/connectorinterface.hpp"
#include "orm/query/grammars/grammar.hpp"
#include "orm/queryerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    class SHAREDLIB_EXPORT DatabaseConnection :
            public ConnectionInterface,
            public Concerns::DetectsLostConnections
    {
    public:
        /*! Default connection name. */
        static const char *defaultConnectionName;

        explicit DatabaseConnection(
                const std::function<Connectors::ConnectionName()> &connection,
                const QString &database = "", const QString tablePrefix = "",
                const QVariantHash &config = {});
        inline virtual ~DatabaseConnection() = default;

        /*! Begin a fluent query against a database table. */
        QSharedPointer<QueryBuilder>
        table(const QString &table, const QString &as = "") override;

        /*! Get a new query builder instance. */
        QSharedPointer<QueryBuilder> query() override;

        // FEATURE dilemma Raw Expressions fuckup🤔 silverqx
        /*! Get a new raw query expression. */
        inline Query::Expression raw(const QVariant &value) const override
        { return value; }

        // TODO next transaction method with callback silverqx
        /*! Start a new database transaction. */
        bool beginTransaction() override;
        /*! Commit the active database transaction. */
        bool commit() override;
        /*! Rollback the active database transaction. */
        bool rollBack() override;
        /*! Start a new named transaction savepoint. */
        bool savepoint(const QString &id) override;
        /*! Start a new named transaction savepoint. */
        bool savepoint(size_t id) override;
        /*! Rollback to a named transaction savepoint. */
        bool rollbackToSavepoint(const QString &id) override;
        /*! Rollback to a named transaction savepoint. */
        bool rollbackToSavepoint(size_t id) override;
        /*! Get the number of active transactions. */
        inline uint transactionLevel() const override
        { return m_savepoints; }

        /*! Run a select statement against the database. */
        std::tuple<bool, QSqlQuery>
        select(const QString &queryString,
               const QVector<QVariant> &bindings = {}) override;
        /*! Run a select statement and return a single result. */
        std::tuple<bool, QSqlQuery>
        selectOne(const QString &queryString,
                  const QVector<QVariant> &bindings = {}) override;
        /*! Run an insert statement against the database. */
        std::tuple<bool, QSqlQuery>
        insert(const QString &queryString,
               const QVector<QVariant> &bindings = {}) override;
        /*! Run an update statement against the database. */
        std::tuple<int, QSqlQuery>
        update(const QString &queryString,
               const QVector<QVariant> &bindings = {}) override;
        /*! Run a delete statement against the database. */
        std::tuple<int, QSqlQuery>
        remove(const QString &queryString,
               const QVector<QVariant> &bindings = {}) override;

        /*! Execute an SQL statement and return the boolean result and QSqlQuery. */
        std::tuple<bool, QSqlQuery>
        statement(const QString &queryString,
                  const QVector<QVariant> &bindings = {}) override;
        /*! Run an SQL statement and get the number of rows affected. */
        std::tuple<int, QSqlQuery>
        affectingStatement(const QString &queryString,
                           const QVector<QVariant> &bindings = {}) override;

        /*! Get underlying database connection (QSqlDatabase). */
        QSqlDatabase getQtConnection();
        /*! Get underlying database connection without executing any reconnect logic. */
        QSqlDatabase getRawQtConnection() const;
        /*! Get the connection resolver for an underlying database connection. */
        inline const std::function<Connectors::ConnectionName()> &
        getQtConnectionResolver() const
        { return m_qtConnectionResolver; }
        /*! Set the connection resolver for an underlying database connection. */
        DatabaseConnection &setQtConnectionResolver(
                const std::function<Connectors::ConnectionName()> &resolver);

        /*! Get a new QSqlQuery instance for the current connection. */
        QSqlQuery getQtQuery() override;

        /*! Prepare the query bindings for execution. */
        QVector<QVariant>
        prepareBindings(QVector<QVariant> bindings) const override;
        /*! Bind values to their parameters in the given statement. */
        void bindValues(QSqlQuery &query,
                        const QVector<QVariant> &bindings) const;
        /*! Log a query into the connection's query log. */
        void logQuery(const QSqlQuery &query,
                      const std::optional<qint64> &elapsed);

        /*! Check database connection and show warnings when the state changed. */
        bool pingDatabase() override;

        /*! Reconnect to the database. */
        void reconnect() const;
        /*! Disconnect from the underlying Qt's connection. */
        void disconnect();

        /*! Get the default query grammar instance. */
        virtual std::unique_ptr<QueryGrammar> getDefaultQueryGrammar() const = 0;

        /*! Get the database connection name. */
        inline const QString getName() const override
        { return getConfig("name").value<QString>(); }
        /*! Get the name of the connected database. */
        inline const QString &getDatabaseName() const override
        { return m_database; }
        /*! Set the query grammar to the default implementation. */
        inline void useDefaultQueryGrammar() override
        { m_queryGrammar = getDefaultQueryGrammar(); }
        /*! Get the query grammar used by the connection. */
        inline const QueryGrammar &getQueryGrammar() const override
        { return *m_queryGrammar; }

        // TODO duplicate, extract to some internal types silverqx
        /*! Reconnector lambda type. */
        using ReconnectorType = std::function<void(const DatabaseConnection &)>;
        /*! Set the reconnect instance on the connection. */
        DatabaseConnection &setReconnector(const ReconnectorType &reconnector);

        /*! Get an option from the configuration options. */
        QVariant getConfig(const QString &option) const;
        /*! Get the configuration for the current connection. */
        const QVariantHash &getConfig() const;

        /* Queries execution time counter */
        /*! Determine whether we're counting queries execution time. */
        bool countingElapsed() const override;
        /*! Enable counting queries execution time on the current connection. */
        DatabaseConnection &enableElapsedCounter() override;
        /*! Disable counting queries execution time on the current connection. */
        DatabaseConnection &disableElapsedCounter() override;
        /*! Obtain queries execution time. */
        qint64 getElapsedCounter() const override;
        /*! Obtain and reset queries execution time. */
        qint64 takeElapsedCounter() override;
        /*! Reset queries execution time. */
        DatabaseConnection &resetElapsedCounter() override;

        /* Queries executed counter */
        /*! Determine whether we're counting the number of executed queries. */
        bool countingStatements() const override;
        /*! Enable counting the number of executed queries on the current connection. */
        DatabaseConnection &enableStatementsCounter() override;
        /*! Disable counting the number of executed queries on the current connection. */
        DatabaseConnection &disableStatementsCounter() override;
        /*! Obtain the number of executed queries. */
        const StatementsCounter &getStatementsCounter() const override;
        /*! Obtain and reset the number of executed queries. */
        StatementsCounter takeStatementsCounter() override;
        /*! Reset the number of executed queries. */
        DatabaseConnection &resetStatementsCounter() override;

        /*! Return the connection's driver name. */
        QString driverName() override;

    protected:
        /*! Callback type used in the run() methods. */
        template<typename Result>
        using RunCallback = std::function<std::tuple<Result, QSqlQuery>
                                          (const QString &, const QVector<QVariant> &)>;

        /*! Run a SQL statement and log its execution context. */
        template<typename Result>
        std::tuple<Result, QSqlQuery>
        run(const QString &queryString, const QVector<QVariant> &bindings,
            const RunCallback<Result> &callback);
        /*! Run a SQL statement. */
        template<typename Result>
        std::tuple<Result, QSqlQuery>
        runQueryCallback(
                const QString &queryString, const QVector<QVariant> &bindings,
                const RunCallback<Result> &callback) const;

        /*! Reconnect to the database if a PDO connection is missing. */
        void reconnectIfMissingConnection() const;

        /*! Reset in transaction state and savepoints. */
        DatabaseConnection &resetTransactions();

        /*! Log database disconnected, examined during MySQL ping. */
        void logDisconnected();
        /*! Log database connected, examined during MySQL ping. */
        void logConnected();

        /*! The active QSqlDatabase connection name. */
        std::optional<Connectors::ConnectionName> m_qtConnection;
        /*! The QSqlDatabase connection resolver. */
        std::function<Connectors::ConnectionName()> m_qtConnectionResolver;
        /*! The name of the connected database. */
        const QString m_database;
        // FEATURE table prefix silverqx
        /*! The table prefix for the connection. */
        const QString m_tablePrefix {""};
        /*! The database connection configuration options. */
        const QVariantHash m_config;
        /*! The reconnector instance for the connection. */
        ReconnectorType m_reconnector;

        /* Queries execution time counter */
        /*! Indicates whether queries elapsed time are being counted. */
        bool m_countingElapsed = false;
        /*! Queries elpased time counter. */
        qint64 m_elapsedCounter = -1;

        /* Queries executed counter */
        /*! Indicates whether executed queries are being counted. */
        bool m_countingStatements = false;
        /*! Counts executed statements on current connection. */
        StatementsCounter m_statementsCounter;

    private:
        /*! Namespace prefix for MySQL savepoints. */
        static const char *SAVEPOINT_NAMESPACE;

        /*! Prepare an SQL statement and return the query object. */
        QSqlQuery prepareQuery(const QString &queryString);
        /*! Handle a query exception. */
        template<typename Result>
        std::tuple<Result, QSqlQuery>
        handleQueryException(
                const QueryError &e, const QString &queryString,
                const QVector<QVariant> &bindings,
                const RunCallback<Result> &callback) const;
        /*! Handle a query exception that occurred during query execution. */
        template<typename Result>
        std::tuple<Result, QSqlQuery>
        tryAgainIfCausedByLostConnection(
                const QueryError &e, const QString &queryString,
                const QVector<QVariant> &bindings,
                const RunCallback<Result> &callback) const;

        /*! Log a transaction query into the connection's query log. */
        void logTransactionQuery(const QString &query,
                                 const std::optional<qint64> &elapsed);
        /*! Count transactional queries execution time and statements counter. */
        std::optional<qint64>
        hitTransactionalCounters(const QElapsedTimer &timer);

        /*! The flag for the database was disconnected. */
        bool m_disconnectedLogged = false;
        /*! The flag for the database was connected. */
        bool m_connectedLogged = false;
        /*! The connection is in the transaction state. */
        bool m_inTransaction = false;
        /*! Active savepoints counter. */
        uint m_savepoints = 0;
        /*! The query grammar implementation. */
        std::unique_ptr<QueryGrammar> m_queryGrammar;

#ifdef TINYORM_DEBUG_SQL
        /*! Indicates whether logging of sql queries is enabled. */
        const bool m_debugSql = true;
#else
        /*! Indicates whether logging of sql queries is enabled. */
        const bool m_debugSql = false;
#endif
    };

    template<typename Result>
    std::tuple<Result, QSqlQuery>
    DatabaseConnection::run(
            const QString &queryString, const QVector<QVariant> &bindings,
            const RunCallback<Result> &callback)
    {
        reconnectIfMissingConnection();

        Result result;
        QSqlQuery query;

        QElapsedTimer timer;
        if (m_debugSql || m_countingElapsed)
            timer.start();

        /* Here we will run this query. If an exception occurs we'll determine if it was
           caused by a connection that has been lost. If that is the cause, we'll try
           to re-establish connection and re-run the query with a fresh connection. */
        try {
            std::tie(result, query) =
                    runQueryCallback<Result>(queryString, bindings, callback);

        }  catch (const QueryError &e) {
            std::tie(result, query) =
                    handleQueryException(e, queryString, bindings, callback);
        }

        std::optional<qint64> elapsed;
        if (m_debugSql || m_countingElapsed)
            elapsed = timer.elapsed();

        // Queries execution time counter
        if (m_countingElapsed)
            m_elapsedCounter += *elapsed;

#ifdef TINYORM_DEBUG_SQL
        /* Once we have run the query we will calculate the time that it took
           to run and then log the query, bindings, and execution time. We'll
           log time in milliseconds. */
        logQuery(query, elapsed);
#endif

        return {result, query};
    }

    template<typename Result>
    std::tuple<Result, QSqlQuery>
    DatabaseConnection::runQueryCallback(
            const QString &queryString, const QVector<QVariant> &bindings,
            const RunCallback<Result> &callback) const
    {
        Result result;
        QSqlQuery query;

        /* To execute the statement, we'll simply call the callback, which will actually
           run the SQL against the QSqlDatabase connection. Then we can calculate the time
           it took to execute and log the query SQL, bindings and time in our memory. */
        std::tie(result, query) = std::invoke(callback, queryString, bindings);

        return {result, query};
    }

    template<typename Result>
    std::tuple<Result, QSqlQuery>
    DatabaseConnection::handleQueryException(
            const QueryError &e, const QString &queryString,
            const QVector<QVariant> &bindings,
            const RunCallback<Result> &callback) const
    {
        // TODO next debug Eloquent $this->transactions >= 1 silverqx
        if (m_inTransaction)
            throw e;

        return tryAgainIfCausedByLostConnection(e, queryString, bindings, callback);
    }

    template<typename Result>
    std::tuple<Result, QSqlQuery>
    DatabaseConnection::tryAgainIfCausedByLostConnection(
            const QueryError &e, const QString &queryString,
            const QVector<QVariant> &bindings,
            const RunCallback<Result> &callback) const
    {
        if (causedByLostConnection(e)) {
            reconnect();

            return runQueryCallback(queryString, bindings, callback);
        }

        throw e;
    }

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // DATABASECONNECTION_H
