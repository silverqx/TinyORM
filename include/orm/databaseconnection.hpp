#pragma once
#ifndef ORM_DATABASECONNECTION_HPP
#define ORM_DATABASECONNECTION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QElapsedTimer>
#include <QtSql/QSqlDatabase>

#include <optional>

#include "orm/concerns/detectslostconnections.hpp"
#include "orm/connectioninterface.hpp"
#include "orm/connectors/connectorinterface.hpp"
#include "orm/exceptions/queryerror.hpp"
#include "orm/query/grammars/grammar.hpp"
#include "orm/query/processors/processor.hpp"
#include "orm/schema/grammars/schemagrammar.hpp"
#include "orm/schema/schemabuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

    // CUR1 return DatabaseConnection vs ConnectionInterface silverqx
    /*! Database connection base class. */
    class SHAREDLIB_EXPORT DatabaseConnection :
            public ConnectionInterface,
            public Concerns::DetectsLostConnections
    {
        Q_DISABLE_COPY(DatabaseConnection)

    public:
        /*! Default connection name. */
        static const char *defaultConnectionName;
        /*! Namespace prefix for MySQL savepoints. */
        static const char *savepointNamespace;

        /*! Constructor. */
        explicit DatabaseConnection(
                std::function<Connectors::ConnectionName()> &&connection,
                const QString &database = "", const QString &tablePrefix = "",
                const QVariantHash &config = {});
        /*! Pure virtual destructor. */
        inline ~DatabaseConnection() override = 0;

        /*! Begin a fluent query against a database table. */
        QSharedPointer<QueryBuilder>
        table(const QString &table, const QString &as = "") override;

        /*! Get the table prefix for the connection. */
        QString getTablePrefix() const override;
        /*! Set the table prefix in use by the connection. */
        DatabaseConnection &setTablePrefix(const QString &prefix) override;
        /*! Set the table prefix and return the query grammar. */
        BaseGrammar &withTablePrefix(BaseGrammar &grammar) const override;

        /*! Get a new query builder instance. */
        QSharedPointer<QueryBuilder> query() override;

        /*! Get a new raw query expression. */
        Query::Expression raw(const QVariant &value) const override;

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
        bool savepoint(std::size_t id) override;
        /*! Rollback to a named transaction savepoint. */
        bool rollbackToSavepoint(const QString &id) override;
        /*! Rollback to a named transaction savepoint. */
        bool rollbackToSavepoint(std::size_t id) override;
        /*! Get the number of active transactions. */
        std::size_t transactionLevel() const override;

        /*! Run a select statement against the database. */
        QSqlQuery
        select(const QString &queryString,
               const QVector<QVariant> &bindings = {}) override;
        /*! Run a select statement against the database. */
        QSqlQuery
        selectFromWriteConnection(const QString &queryString,
                                  const QVector<QVariant> &bindings = {}) override;
        /*! Run a select statement and return a single result. */
        QSqlQuery
        selectOne(const QString &queryString,
                  const QVector<QVariant> &bindings = {}) override;
        /*! Run an insert statement against the database. */
        QSqlQuery
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

        /*! Execute an SQL statement, should be used for DDL queries, internally calls
            DatabaseConnection::recordsHaveBeenModified(). */
        QSqlQuery statement(const QString &queryString,
                            const QVector<QVariant> &bindings = {}) override;
        /*! Run an SQL statement and get the number of rows affected. */
        std::tuple<int, QSqlQuery>
        affectingStatement(const QString &queryString,
                           const QVector<QVariant> &bindings = {}) override;

        /*! Run a raw, unprepared query against the database. */
        QSqlQuery unprepared(const QString &queryString) override;

        /*! Get underlying database connection (QSqlDatabase). */
        QSqlDatabase getQtConnection();
        /*! Get underlying database connection without executing any reconnect logic. */
        QSqlDatabase getRawQtConnection() const;
        /*! Get the connection resolver for an underlying database connection. */
        const std::function<Connectors::ConnectionName()> &
        getQtConnectionResolver() const;
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

        /*! Check database connection and show warnings when the state changed. */
        bool pingDatabase() override;

        /*! Reconnect to the database. */
        void reconnect() const;
        /*! Disconnect from the underlying Qt's connection. */
        void disconnect();

        /*! Get the database connection name. */
        const QString &getName() const override;
        /*! Get the name of the connected database. */
        const QString &getDatabaseName() const override;
        /*! Get the host name of the connected database. */
        const QString &getHostName() const override;

        /*! Set the query grammar to the default implementation. */
        void useDefaultQueryGrammar() override;
        /*! Get the query grammar used by the connection. */
        const QueryGrammar &getQueryGrammar() const override;

        /*! Set the schema grammar to the default implementation. */
        void useDefaultSchemaGrammar() override;
        /*! Get the schema grammar used by the connection. */
        const SchemaGrammar &getSchemaGrammar() const override;

        /*! Get a schema builder instance for the connection. */
        std::unique_ptr<SchemaBuilder> getSchemaBuilder() override;

        /*! Set the query post processor to the default implementation. */
        void useDefaultPostProcessor() override;
        /*! Get the query post processor used by the connection. */
        const QueryProcessor &getPostProcessor() const override;

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

        /* Logging */
        /*! Log a query into the connection's query log. */
        void logQuery(const QSqlQuery &query, std::optional<qint64> elapsed) const;
        /*! Log a query into the connection's query log. */
        void logQuery(const std::tuple<int, QSqlQuery> &queryResult,
                      std::optional<qint64> elapsed) const;
        /*! Log a query into the connection's query log in the pretending mode. */
        void logQueryForPretend(const QString &query,
                                const QVector<QVariant> &bindings) const;
        /*! Log a transaction query into the connection's query log. */
        void logTransactionQuery(const QString &query,
                                 std::optional<qint64> elapsed) const;
        /*! Log a transaction query into the connection's query log
            in the pretending mode. */
        void logTransactionQueryForPretend(const QString &query) const;

        /*! Get the connection query log. */
        std::shared_ptr<QVector<Log>> getQueryLog() const override;
        /*! Clear the query log. */
        void flushQueryLog() override;
        /*! Enable the query log on the connection. */
        void enableQueryLog() override;
        /*! Disable the query log on the connection. */
        void disableQueryLog() override;
        /*! Determine whether we're logging queries. */
        bool logging() const override;
        /*! The current order value for a query log record. */
        static std::size_t getQueryLogOrder();

        /* Others */
        /*! Return the connection's driver name. */
        QString driverName() override;
        /*! Return the connection's driver name in printable format eg. QMYSQL -> MySQL. */
        const QString &driverNamePrintable() override;

        /*! Execute the given callback in "dry run" mode. */
        QVector<Log>
        pretend(const std::function<void()> &callback) override;
        /*! Execute the given callback in "dry run" mode. */
        QVector<Log>
        pretend(const std::function<void(ConnectionInterface &)> &callback) override;
        /*! Determine if the connection is in a "dry run". */
        bool pretending() const override;

        /*! Check if any records have been modified. */
        bool getRecordsHaveBeenModified() const override;
        /*! Indicate if any records have been modified. */
        void recordsHaveBeenModified(bool value = true) override;
        /*! Reset the record modification state. */
        void forgetRecordModificationState() override;

    protected:
        // NOTE api different, getDefaultQueryGrammar() can not be non-pure because it contains pure virtual member function silverqx
        /*! Get the default query grammar instance. */
        virtual std::unique_ptr<QueryGrammar> getDefaultQueryGrammar() const = 0;
        /*! Get the default schema grammar instance. */
        virtual std::unique_ptr<SchemaGrammar> getDefaultSchemaGrammar() const = 0;
        /*! Get the default post processor instance. */
        virtual std::unique_ptr<QueryProcessor> getDefaultPostProcessor() const;

        /*! Callback type used in the run() method. */
        template<typename Return>
        using RunCallback =
                std::function<Return(const QString &, const QVector<QVariant> &)>;

        /*! Run a SQL statement and log its execution context. */
        template<typename Return>
        Return run(
                const QString &queryString, const QVector<QVariant> &bindings,
                const RunCallback<Return> &callback);
        /*! Run a SQL statement. */
        template<typename Return>
        Return runQueryCallback(
                const QString &queryString, const QVector<QVariant> &bindings,
                const RunCallback<Return> &callback) const;

        /*! Reconnect to the database if a PDO connection is missing. */
        void reconnectIfMissingConnection() const;

        /*! Reset in transaction state and savepoints. */
        DatabaseConnection &resetTransactions();

        /*! Log database disconnected, invoked during MySQL ping. */
        void logDisconnected();
        /*! Log database connected, invoked during MySQL ping. */
        void logConnected();

        /*! Execute the given callback in "dry run" mode. */
        QVector<Log>
        withFreshQueryLog(const std::function<QVector<Log>()> &callback);

        /*! The active QSqlDatabase connection name. */
        std::optional<Connectors::ConnectionName> m_qtConnection = std::nullopt;
        /*! The QSqlDatabase connection resolver. */
        std::function<Connectors::ConnectionName()> m_qtConnectionResolver;
        /*! The name of the connected database. */
        const QString m_database;
        /*! The table prefix for the connection. */
        QString m_tablePrefix;
        /*! The database connection configuration options. */
        const QVariantHash m_config;
        /*! The reconnector instance for the connection. */
        ReconnectorType m_reconnector = nullptr;

        /*! The query grammar implementation. */
        std::unique_ptr<QueryGrammar> m_queryGrammar = nullptr;
        /*! The schema grammar implementation. */
        std::unique_ptr<SchemaGrammar> m_schemaGrammar = nullptr;
        /*! The query post processor implementation. */
        std::unique_ptr<QueryProcessor> m_postProcessor = nullptr;

        /* Queries execution time counter */
        /*! Indicates whether queries elapsed time are being counted. */
        bool m_countingElapsed = false;
        /*! Queries elpased time counter. */
        qint64 m_elapsedCounter = -1;

        /* Queries executed counter */
        /*! Indicates whether executed queries are being counted. */
        bool m_countingStatements = false;
        /*! Counts executed statements on current connection. */
        StatementsCounter m_statementsCounter {};

        /* Logging */
        /*! Indicates if changes have been made to the database. */
        bool m_recordsModified = false;
        /*! All of the queries run against the connection. */
        std::shared_ptr<QVector<Log>> m_queryLog = nullptr;
        // CUR atomic inc/dec? investigate how m_queryLogId works silverqx
        /*! ID of the query log record. */
        inline static std::size_t m_queryLogId = 0;

        /* Others */
        /*! Indicates if the connection is in a "dry run". */
        bool m_pretending = false;

    private:
        /*! Prepare an SQL statement and return the query object. */
        QSqlQuery prepareQuery(const QString &queryString);

        /*! Handle a query exception. */
        template<typename Return>
        Return handleQueryException(
                const std::exception_ptr &ePtr, const Exceptions::QueryError &e,
                const QString &queryString, const QVector<QVariant> &bindings,
                const RunCallback<Return> &callback) const;
        /*! Handle a query exception that occurred during query execution. */
        template<typename Return>
        Return tryAgainIfCausedByLostConnection(
                const std::exception_ptr &ePtr, const Exceptions::QueryError &e,
                const QString &queryString, const QVector<QVariant> &bindings,
                const RunCallback<Return> &callback) const;

        /*! Count transactional queries execution time and statements counter. */
        std::optional<qint64>
        hitTransactionalCounters(QElapsedTimer timer, bool countElapsed);
        /*! Convert a named bindings map to the positional bindings vector. */
        QVector<QVariant>
        convertNamedToPositionalBindings(QVariantMap &&bindings) const;

        /*! Get the query grammar used by the connection. */
        QueryGrammar &getQueryGrammar();

        /*! The flag for the database was disconnected, used during MySQL ping. */
        bool m_disconnectedLogged = false;
        /*! The flag for the database was connected, used during MySQL ping. */
        bool m_connectedLogged = false;
        /*! The connection is in the transaction state. */
        bool m_inTransaction = false;
        /*! Active savepoints counter. */
        std::size_t m_savepoints = 0;

        /*! Connection name, obtained from the connection configuration. */
        QString m_connectionName;
        /*! Host name, obtained from the connection configuration. */
        QString m_hostName;

        /* Logging */
        /*! Indicates whether queries are being logged (private intentionally). */
        bool m_loggingQueries = false;
        /*! All of the queries run against the connection. */
        std::shared_ptr<QVector<Log>> m_queryLogForPretend = nullptr;

#ifdef TINYORM_DEBUG_SQL
        /*! Indicates whether logging of sql queries is enabled. */
        const bool m_debugSql = true;
#else
        /*! Indicates whether logging of sql queries is enabled. */
        const bool m_debugSql = false;
#endif

        /*! Connection's driver name in printable format eg. QMYSQL -> MySQL. */
        std::optional<std::reference_wrapper<
                const QString>> m_driverNamePrintable = std::nullopt;
    };

    inline QString DatabaseConnection::getTablePrefix() const
    {
        return m_tablePrefix;
    }

    inline Query::Expression
    DatabaseConnection::raw(const QVariant &value) const
    {
        return Query::Expression(value);
    }

    inline std::size_t DatabaseConnection::transactionLevel() const
    {
        return m_savepoints;
    }

    inline const std::function<Connectors::ConnectionName()> &
    DatabaseConnection::getQtConnectionResolver() const
    {
        return m_qtConnectionResolver;
    }

    inline const QString &DatabaseConnection::getName() const
    {
        return m_connectionName;
    }

    inline const QString &DatabaseConnection::getDatabaseName() const
    {
        return m_database;
    }

    inline const QString &DatabaseConnection::getHostName() const
    {
        return m_hostName;
    }

    inline std::shared_ptr<QVector<Log>>
    DatabaseConnection::getQueryLog() const
    {
        return m_queryLog;
    }

    inline void DatabaseConnection::disableQueryLog()
    {
        m_loggingQueries = false;
    }

    inline bool DatabaseConnection::logging() const
    {
        return m_loggingQueries;
    }

    inline bool DatabaseConnection::pretending() const
    {
        return m_pretending;
    }

    inline bool DatabaseConnection::getRecordsHaveBeenModified() const
    {
        return m_recordsModified;
    }

    inline void DatabaseConnection::recordsHaveBeenModified(const bool value)
    {
        m_recordsModified = value;
    }

    inline void DatabaseConnection::forgetRecordModificationState()
    {
        m_recordsModified = false;
    }

    template<typename Return>
    Return
    DatabaseConnection::run(
            const QString &queryString, const QVector<QVariant> &bindings,
            const RunCallback<Return> &callback)
    {
        reconnectIfMissingConnection();

        // Elapsed timer needed
        const auto countElapsed = !m_pretending && (m_debugSql || m_countingElapsed);

        QElapsedTimer timer;
        if (countElapsed)
            timer.start();

        Return result;

        /* Here we will run this query. If an exception occurs we'll determine if it was
           caused by a connection that has been lost. If that is the cause, we'll try
           to re-establish connection and re-run the query with a fresh connection. */
        try {
            result = runQueryCallback(queryString, bindings, callback);

        }  catch (const Exceptions::QueryError &e) {
            result = handleQueryException(std::current_exception(), e,
                                          queryString, bindings, callback);
        }

        std::optional<qint64> elapsed;
        if (countElapsed) {
            // Hit elapsed timer
            elapsed = timer.elapsed();

            // Queries execution time counter
            m_elapsedCounter += *elapsed;
        }

        /* Once we have run the query we will calculate the time that it took
           to run and then log the query, bindings, and execution time. We'll
           log time in milliseconds. */
        if (m_pretending)
            logQueryForPretend(queryString, bindings);
        else
            logQuery(result, elapsed);

        return result;
    }

    template<typename Return>
    Return
    DatabaseConnection::runQueryCallback(
            const QString &queryString, const QVector<QVariant> &bindings,
            const RunCallback<Return> &callback) const
    {
        /* To execute the statement, we'll simply call the callback, which will actually
           run the SQL against the QSqlDatabase connection. Then we can calculate the time
           it took to execute and log the query SQL, bindings and time in our memory. */
        return std::invoke(callback, queryString, bindings);
    }

    template<typename Return>
    Return
    DatabaseConnection::handleQueryException(
            const std::exception_ptr &ePtr, const Exceptions::QueryError &e,
            const QString &queryString, const QVector<QVariant> &bindings,
            const RunCallback<Return> &callback) const
    {
        // FUTURE add info about in transaction into the exception that it was a reason why connection was not reconnected/recovered silverqx
        if (m_inTransaction)
            std::rethrow_exception(ePtr);

        return tryAgainIfCausedByLostConnection(ePtr, e, queryString, bindings,
                                                callback);
    }

    template<typename Return>
    Return
    DatabaseConnection::tryAgainIfCausedByLostConnection(
            const std::exception_ptr &ePtr, const Exceptions::QueryError &e,
            const QString &queryString, const QVector<QVariant> &bindings,
            const RunCallback<Return> &callback) const
    {
        if (causedByLostConnection(e)) {
            reconnect();

            return runQueryCallback(queryString, bindings, callback);
        }

        std::rethrow_exception(ePtr);
    }

    DatabaseConnection::~DatabaseConnection() = default;

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DATABASECONNECTION_HPP
