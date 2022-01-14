#pragma once
#ifndef ORM_DATABASECONNECTION_HPP
#define ORM_DATABASECONNECTION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QElapsedTimer>
#include <QtSql/QSqlDatabase>

#include "orm/concerns/countsqueries.hpp"
#include "orm/concerns/detectslostconnections.hpp"
#include "orm/concerns/logsqueries.hpp"
#include "orm/connectors/connectorinterface.hpp"
#include "orm/exceptions/queryerror.hpp"
#include "orm/query/grammars/grammar.hpp"
#include "orm/query/processors/processor.hpp"
#include "orm/schema/grammars/schemagrammar.hpp"
#include "orm/schema/schemabuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

namespace Query
{
namespace Grammars
{
    class Grammar;
}
namespace Processors
{
    class Processor;
}
} // namespace Query

namespace Schema
{
    class SchemaBuilder;
}

    /*! QueryGrammar alias. */
    using QueryGrammar   = Query::Grammars::Grammar;
    /*! QueryProcessor alias. */
    using QueryProcessor = Query::Processors::Processor;
    /*! SchemaBuilder alias. */
    using SchemaBuilder  = Schema::SchemaBuilder;
    /*! SchemaGrammar alias. */
    using SchemaGrammar  = Schema::Grammars::SchemaGrammar;

    /*! Database connection base class. */
    class SHAREDLIB_EXPORT DatabaseConnection :
            public Concerns::DetectsLostConnections,
            public Concerns::LogsQueries,
            public Concerns::CountsQueries
    {
        Q_DISABLE_COPY(DatabaseConnection)

    public:
        /*! Constructor. */
        explicit DatabaseConnection(
                std::function<Connectors::ConnectionName()> &&connection,
                const QString &database = "", const QString &tablePrefix = "",
                const QVariantHash &config = {});
        /*! Pure virtual destructor. */
        inline ~DatabaseConnection() override = 0;

        /*! Begin a fluent query against a database table. */
        QSharedPointer<QueryBuilder>
        table(const QString &table, const QString &as = "");

        /*! Get the table prefix for the connection. */
        inline QString getTablePrefix() const;
        /*! Set the table prefix in use by the connection. */
        DatabaseConnection &setTablePrefix(const QString &prefix);
        /*! Set the table prefix and return the query grammar. */
        BaseGrammar &withTablePrefix(BaseGrammar &grammar) const;

        /*! Get a new query builder instance. */
        QSharedPointer<QueryBuilder> query();

        /*! Get a new raw query expression. */
        inline Query::Expression raw(const QVariant &value) const;

        // TODO next transaction method with callback silverqx
        /*! Start a new database transaction. */
        bool beginTransaction();
        /*! Commit the active database transaction. */
        bool commit();
        /*! Rollback the active database transaction. */
        bool rollBack();
        /*! Start a new named transaction savepoint. */
        bool savepoint(const QString &id);
        /*! Start a new named transaction savepoint. */
        bool savepoint(std::size_t id);
        /*! Rollback to a named transaction savepoint. */
        bool rollbackToSavepoint(const QString &id);
        /*! Rollback to a named transaction savepoint. */
        bool rollbackToSavepoint(std::size_t id);
        /*! Get the number of active transactions. */
        inline std::size_t transactionLevel() const;

        /* Running SQL Queries */
        /*! Run a select statement against the database. */
        QSqlQuery
        select(const QString &queryString,
               const QVector<QVariant> &bindings = {});
        /*! Run a select statement against the database. */
        QSqlQuery
        selectFromWriteConnection(const QString &queryString,
                                  const QVector<QVariant> &bindings = {});
        /*! Run a select statement and return a single result. */
        QSqlQuery
        selectOne(const QString &queryString,
                  const QVector<QVariant> &bindings = {});
        /*! Run an insert statement against the database. */
        QSqlQuery
        insert(const QString &queryString,
               const QVector<QVariant> &bindings = {});
        /*! Run an update statement against the database. */
        std::tuple<int, QSqlQuery>
        update(const QString &queryString,
               const QVector<QVariant> &bindings = {});
        /*! Run a delete statement against the database. */
        std::tuple<int, QSqlQuery>
        remove(const QString &queryString,
               const QVector<QVariant> &bindings = {});

        /*! Execute an SQL statement, should be used for DDL queries, internally calls
            DatabaseConnection::recordsHaveBeenModified(). */
        QSqlQuery statement(const QString &queryString,
                            const QVector<QVariant> &bindings = {});
        /*! Run an SQL statement and get the number of rows affected. */
        std::tuple<int, QSqlQuery>
        affectingStatement(const QString &queryString,
                           const QVector<QVariant> &bindings = {});

        /*! Run a raw, unprepared query against the database. */
        QSqlQuery unprepared(const QString &queryString);

        /*! Get underlying database connection (QSqlDatabase). */
        QSqlDatabase getQtConnection();
        /*! Get underlying database connection without executing any reconnect logic. */
        QSqlDatabase getRawQtConnection() const;
        /*! Get the connection resolver for an underlying database connection. */
        inline const std::function<Connectors::ConnectionName()> &
        getQtConnectionResolver() const;
        /*! Set the connection resolver for an underlying database connection. */
        DatabaseConnection &setQtConnectionResolver(
                const std::function<Connectors::ConnectionName()> &resolver);

        /*! Get a new QSqlQuery instance for the current connection. */
        QSqlQuery getQtQuery();

        /*! Prepare the query bindings for execution. */
        QVector<QVariant>
        prepareBindings(QVector<QVariant> bindings) const;
        /*! Bind values to their parameters in the given statement. */
        void bindValues(QSqlQuery &query,
                        const QVector<QVariant> &bindings) const;

        /*! Check database connection and show warnings when the state changed. */
        virtual bool pingDatabase();

        /*! Reconnect to the database. */
        void reconnect() const;
        /*! Disconnect from the underlying Qt's connection. */
        void disconnect();

        /*! Get the query grammar used by the connection. */
        const QueryGrammar &getQueryGrammar() const;
        /*! Get the query grammar used by the connection. */
        QueryGrammar &getQueryGrammar();
        /*! Get the schema grammar used by the connection. */
        const SchemaGrammar &getSchemaGrammar() const;
        /*! Get a schema builder instance for the connection. */
        virtual std::unique_ptr<SchemaBuilder> getSchemaBuilder();
        /*! Get the query post processor used by the connection. */
        const QueryProcessor &getPostProcessor() const;

        /*! Set the reconnect instance on the connection. */
        DatabaseConnection &setReconnector(const ReconnectorType &reconnector);

        /*! Get an option from the configuration options. */
        QVariant getConfig(const QString &option) const;
        /*! Get the configuration for the current connection. */
        const QVariantHash &getConfig() const;

        /* Getters */
        /*! Return the connection's driver name. */
        QString driverName();
        /*! Return connection's driver name in printable format eg. QMYSQL -> MySQL. */
        const QString &driverNamePrintable();
        /*! Get the database connection name. */
        inline const QString &getName() const;
        /*! Get the name of the connected database. */
        inline const QString &getDatabaseName() const;
        /*! Get the host name of the connected database. */
        inline const QString &getHostName() const;

        /* Others */
        /*! Execute the given callback in "dry run" mode. */
        QVector<Log>
        pretend(const std::function<void()> &callback);
        /*! Execute the given callback in "dry run" mode. */
        QVector<Log>
        pretend(const std::function<void(DatabaseConnection &)> &callback);
        /*! Determine if the connection is in a "dry run". */
        inline bool pretending() const;

        /*! Check if any records have been modified. */
        inline bool getRecordsHaveBeenModified() const;
        /*! Indicate if any records have been modified. */
        inline void recordsHaveBeenModified(bool value = true);
        /*! Reset the record modification state. */
        inline void forgetRecordModificationState();

        /*! Get namespace prefix for MySQL savepoints. */
        inline const QString &getSavepointNamespace() const;
        /*! Set namespace prefix for MySQL savepoints. */
        inline DatabaseConnection &
        setSavepointNamespace(const QString &savepointNamespace);

    protected:
        /*! Set the query grammar to the default implementation. */
        void useDefaultQueryGrammar();
        /*! Set the schema grammar to the default implementation. */
        void useDefaultSchemaGrammar();
        /*! Set the query post processor to the default implementation. */
        void useDefaultPostProcessor();

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

        /* Others */
        /*! Indicates if the connection is in a "dry run". */
        bool m_pretending = false;
        /*! Namespace prefix for MySQL savepoints. */
        QString m_savepointNamespace;

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

    /* public */

    DatabaseConnection::~DatabaseConnection() = default;

    QString DatabaseConnection::getTablePrefix() const
    {
        return m_tablePrefix;
    }

    Query::Expression
    DatabaseConnection::raw(const QVariant &value) const
    {
        return Query::Expression(value);
    }

    std::size_t DatabaseConnection::transactionLevel() const
    {
        return m_savepoints;
    }

    const std::function<Connectors::ConnectionName()> &
    DatabaseConnection::getQtConnectionResolver() const
    {
        return m_qtConnectionResolver;
    }

    const QString &DatabaseConnection::getName() const
    {
        return m_connectionName;
    }

    const QString &DatabaseConnection::getDatabaseName() const
    {
        return m_database;
    }

    const QString &DatabaseConnection::getHostName() const
    {
        return m_hostName;
    }

    bool DatabaseConnection::pretending() const
    {
        return m_pretending;
    }

    bool DatabaseConnection::getRecordsHaveBeenModified() const
    {
        return m_recordsModified;
    }

    void DatabaseConnection::recordsHaveBeenModified(const bool value)
    {
        m_recordsModified = value;
    }

    void DatabaseConnection::forgetRecordModificationState()
    {
        m_recordsModified = false;
    }

    const QString &DatabaseConnection::getSavepointNamespace() const
    {
        return m_savepointNamespace;
    }

    DatabaseConnection &
    DatabaseConnection::setSavepointNamespace(const QString &savepointNamespace)
    {
        m_savepointNamespace = savepointNamespace;

        return *this;
    }

    /* protected */

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

    /* private */

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

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DATABASECONNECTION_HPP
