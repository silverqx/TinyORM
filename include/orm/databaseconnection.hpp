#pragma once
#ifndef ORM_DATABASECONNECTION_HPP
#define ORM_DATABASECONNECTION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/sqldrivermappings.hpp"
#include TINY_INCLUDE_TQueryError // NOLINT(llvm-include-order)
#include TINY_INCLUDE_TSqlDatabase

#include "orm/concerns/countsqueries.hpp"
#include "orm/concerns/detectslostconnections.hpp"
#include "orm/concerns/logsqueries.hpp"
#include "orm/concerns/managestransactions.hpp"
#include "orm/connectors/connectorinterface.hpp"
#include "orm/query/grammars/grammar.hpp"
#include "orm/query/processors/processor.hpp"
#include "orm/schema/grammars/schemagrammar.hpp"
#include "orm/schema/schemabuilder.hpp"
#include "orm/types/sqlquery.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

    /*! Alias for the QueryGrammar. */
    using QueryGrammar   = Query::Grammars::Grammar;
    /*! Alias for the QueryProcessor. */
    using QueryProcessor = Query::Processors::Processor;
    /*! Alias for the SchemaBuilder. */
    using SchemaBuilder  = SchemaNs::SchemaBuilder;
    /*! Alias for the SchemaGrammar. */
    using SchemaGrammar  = SchemaNs::Grammars::SchemaGrammar;

#if defined(__GNUG__) && !defined(__clang__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif
    /*! Database connection base class.
        TinyORM's DatabaseConnection never physically connects to the database after
        create()/reconnect() method calls, it only creates a connection resolver,
        a physical connection to the database is made lazily during a first query call.
        It is designed in such a way that you don't have to worry about connecting or
        reconnecting  to the database, this is handled by the TinyORM library
        internally. The reconnection is handled correctly if a connection loss is
        detected. */
    class TINYORM_EXPORT DatabaseConnection :
            public Concerns::DetectsLostConnections,
            public Concerns::ManagesTransactions,
            public Concerns::LogsQueries,
            public Concerns::CountsQueries,
            // Needed to suppress the -Wnon-virtual-dtor diagnostic
            public std::enable_shared_from_this<DatabaseConnection>
    {
        Q_DISABLE_COPY_MOVE(DatabaseConnection)

        // To access shouldCountElapsed() method
        friend Concerns::ManagesTransactions;
        /* The friend declaration doesn't affect an ABI or binary compatibility so
           wrapping it in the #ifdef is safe:
           https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C++ */
#ifdef TINYORM_MYSQL_PING
        // To access logConnected()/logDisconnected() methods
        friend MySqlConnection;
#endif

        /*! Alias for the QueryError exception. */
        using QueryError = TQueryError;

    protected:
        /*! Protected constructor. */
        explicit DatabaseConnection(
                std::function<Connectors::ConnectionName()> &&connection,
                QString &&database = "", QString &&tablePrefix = "",
                QtTimeZoneConfig &&qtTimeZone = QtTimeZoneConfig::utc(),
                QVariantHash &&config = {});
        /*! Protected constructor for SQLite connection. */
        explicit DatabaseConnection(
                std::function<Connectors::ConnectionName()> &&connection,
                QString &&database = "", QString &&tablePrefix = "",
                QtTimeZoneConfig &&qtTimeZone = QtTimeZoneConfig::utc(),
                std::optional<bool> returnQDateTime = true,
                QVariantHash &&config = {});

    public:
        /*! Pure virtual destructor. */
        inline ~DatabaseConnection() override = 0;

        /*! Begin a fluent query against a database table. */
        std::shared_ptr<QueryBuilder>
        table(const QString &table, const QString &as = "");

        /*! Get the table prefix for the connection. */
        inline QString getTablePrefix() const;
        /*! Set the table prefix in use by the connection. */
        DatabaseConnection &setTablePrefix(const QString &prefix);
        /*! Set the table prefix and return the query grammar. */
        BaseGrammar &withTablePrefix(BaseGrammar &grammar) const;

        /*! Get a new query builder instance. */
        std::shared_ptr<QueryBuilder> query();

        /*! Get a new raw query expression. */
        inline Query::Expression raw(const QVariant &value) const;
        /*! Get a new raw query expression. */
        inline Query::Expression raw(QVariant &&value) const noexcept;

        /* Running SQL Queries */
        /*! Run a select statement against the database. */
        SqlQuery
        select(const QString &queryString, QList<QVariant> bindings = {});
        /*! Run a select statement against the database. */
        inline SqlQuery
        selectFromWriteConnection(const QString &queryString,
                                  QList<QVariant> bindings = {});

        /*! Run a select statement and return a single result. */
        SqlQuery
        selectOne(const QString &queryString, QList<QVariant> bindings = {});
        /*! Run a select statement and return the first column of the first row. */
        QVariant
        scalar(const QString &queryString, QList<QVariant> bindings = {});

        /*! Run an insert statement against the database. */
        inline SqlQuery
        insert(const QString &queryString, QList<QVariant> bindings = {});
        /*! Run an update statement against the database. */
        inline std::tuple<int, TSqlQuery>
        update(const QString &queryString, QList<QVariant> bindings = {});
        /*! Run a delete statement against the database. */
        inline std::tuple<int, TSqlQuery>
        remove(const QString &queryString, QList<QVariant> bindings = {});

        /*! Execute an SQL statement, should be used for DDL/DML queries, internally
            calls DatabaseConnection::recordsHaveBeenModified(). */
        SqlQuery statement(const QString &queryString, QList<QVariant> bindings = {});
        /*! Run an SQL statement and get the number of rows affected (for DML queries). */
        std::tuple<int, TSqlQuery>
        affectingStatement(const QString &queryString, QList<QVariant> bindings = {});

        /*! Run a raw, unprepared query against the database (good for DDL queries). */
        SqlQuery unprepared(const QString &queryString);

        /* Obtain connection instance */
        /*! Get underlying database connection (Q/SqlDatabase). */
        TSqlDatabase getSqlConnection();
        /*! Get underlying database connection without executing any reconnect logic. */
        TSqlDatabase getRawSqlConnection() const;
        /*! Get the connection resolver for an underlying database connection. */
        inline const std::function<Connectors::ConnectionName()> &
        getSqlConnectionResolver() const noexcept;
        /*! Set the connection resolver for an underlying database connection. */
        DatabaseConnection &setSqlConnectionResolver(
                const std::function<Connectors::ConnectionName()> &resolver);

        /*! Get a new Q/SqlQuery instance for the current connection. */
        TSqlQuery getSqlQuery();

        /*! Prepare the query bindings for execution. */
        QList<QVariant> &prepareBindings(QList<QVariant> &bindings) const;
        /*! Bind values to their parameters in the given statement. */
        static void bindValues(TSqlQuery &query, const QList<QVariant> &bindings);

        /*! Determine whether the database connection is currently open. */
        inline bool isOpen();
        /*! Check database connection and show warnings when the state changed. */
        virtual bool pingDatabase();

        /*! Get the SQL database driver used to access the database connection (const). */
        const TSqlDriver *driver();
#ifdef TINYORM_USING_TINYDRIVERS
        /*! Get the SQL database driver used to access the database connection (const). */
        std::weak_ptr<const TSqlDriver> driverWeak();
#endif

        /*! Force connection to the database (creates physical connection), doesn't have
            to be called before querying a database. */
        inline void connectEagerly();
        /*! Reconnect to the database if a Qt connection is missing (doesn't create
            a physical connection, only refreshs connection resolver). */
        void reconnectIfMissingConnection() const;
        /*! Reconnect to the database (doesn't create physical connection, only refreshes
            a connection resolver). */
        void reconnect() const;
        /*! Disconnect from the underlying Qt's connection. */
        void disconnect();

        /*! Get the query grammar used by the connection. */
        inline const QueryGrammar &getQueryGrammar() const noexcept;
        /*! Get the query grammar used by the connection. */
        inline QueryGrammar &getQueryGrammar() noexcept;
        /*! Get the schema grammar used by the connection. */
        inline const SchemaGrammar &getSchemaGrammar();
        /*! Get the schema builder used by the connection. */
        SchemaBuilder &getSchemaBuilder();
        /*! Get the query post processor used by the connection. */
        inline const QueryProcessor &getPostProcessor() const noexcept;

        /*! Get the query grammar used by the connection as a std::shared_ptr. */
        inline std::shared_ptr<QueryGrammar> getQueryGrammarShared() const noexcept;
        /*! Get the schema grammar used by the connection as a std::shared_ptr. */
        std::shared_ptr<SchemaGrammar> getSchemaGrammarShared();

        /*! Set the reconnect instance on the connection. */
        DatabaseConnection &setReconnector(const ReconnectorType &reconnector);

        /* Connection configuration */
        /*! Get an option value from the configuration options. */
        QVariant getConfig(const QString &option) const;
        /*! Get the configuration for the current connection. */
        inline const QVariantHash &getConfig() const noexcept;
        /*! Check whether the configuration contains the given option. */
        bool hasConfig(const QString &option) const;

        /* Getters */
        /*! Return the connection's driver name. */
        QString driverName();
        /*! Return connection's driver name in printable format eg. QMYSQL -> MySQL. */
        const QString &driverNamePrintable();
        /*! Get the database connection name. */
        inline const QString &getName() const noexcept;
        /*! Get the name of the connected database. */
        inline const QString &getDatabaseName() const noexcept;
        /*! Get the hostname of the connected database. */
        inline const QString &getHostName() const noexcept;

        /*! Get the QtTimeZoneConfig for the current connection. */
        inline const QtTimeZoneConfig &getQtTimeZone() const noexcept;
        /*! Set the QtTimeZoneConfig for the current connection (override qt_timezone). */
        DatabaseConnection &setQtTimeZone(const QVariant &timezone);
        /*! Set the QtTimeZoneConfig for the current connection (override qt_timezone). */
        DatabaseConnection &setQtTimeZone(QtTimeZoneConfig &&timezone) noexcept;
        /*! Determine whether the QDateTime time zone should be converted. */
        inline bool isConvertingTimeZone() const noexcept;

        /* Others */
        /*! Execute the given callback in "dry run" mode. */
        QList<Log> pretend(const std::function<void()> &callback);
        /*! Execute the given callback in "dry run" mode. */
        QList<Log> pretend(const std::function<void(DatabaseConnection &)> &callback);
        /*! Determine if the connection is in a "dry run". */
        inline bool pretending() const;

        /*! Check if any records have been modified. */
        inline bool getRecordsHaveBeenModified() const;
        /*! Indicates if any records have been modified. */
        inline void recordsHaveBeenModified(bool value = true);
        /*! Reset the record modification state. */
        inline void forgetRecordModificationState();

    protected:
        /*! Set the query grammar to the default implementation. */
        void useDefaultQueryGrammar();
        /*! Set the schema grammar to the default implementation. */
        void useDefaultSchemaGrammar();
        /*! Set the schema builder to the default implementation. */
        void useDefaultSchemaBuilder();
        /*! Set the query post processor to the default implementation. */
        void useDefaultPostProcessor();

        /*! Get the default query grammar instance. */
        virtual std::unique_ptr<QueryGrammar> getDefaultQueryGrammar() const = 0;
        /*! Get the default schema grammar instance. */
        virtual std::unique_ptr<SchemaGrammar> getDefaultSchemaGrammar() = 0;
        /*! Get the default schema builder instance. */
        virtual std::unique_ptr<SchemaBuilder> getDefaultSchemaBuilder() = 0;
        /*! Get the default post processor instance. */
        virtual std::unique_ptr<QueryProcessor> getDefaultPostProcessor() const = 0;

        /*! Callback type used in the run() method. */
        template<typename Return>
        using RunCallback = std::function<Return(const QString &,
                                                 const QList<QVariant> &)>;

        /*! Run a SQL statement and log its execution context. */
        template<typename Return>
        Return run(
                const QString &queryString, QList<QVariant> &&bindings,
                const QString &type, const RunCallback<Return> &callback);
        /*! Run a SQL statement. */
        template<typename Return>
        Return runQueryCallback(
                const QString &queryString, const QList<QVariant> &preparedBindings,
                const RunCallback<Return> &callback) const;

        /*! The active QSqlDatabase connection name. */
        std::optional<Connectors::ConnectionName> m_qtConnection = std::nullopt;
        /*! The QSqlDatabase connection resolver. */
        std::function<Connectors::ConnectionName()> m_qtConnectionResolver;
        /*! The name of the connected database. */
        /*const*/ QString m_database;
        /*! The table prefix for the connection. */
        QString m_tablePrefix;
        /*! Determine how the QDateTime time zone will be converted. */
        QtTimeZoneConfig m_qtTimeZone;
        /*! Determine whether the QDateTime time zone should be converted. */
        bool m_isConvertingTimeZone;
        /*! Determine whether to return the QDateTime/QDate or QString (SQLite only). */
        std::optional<bool> m_returnQDateTime = std::nullopt;
        /*! The database connection configuration options. */
        /*const*/ QVariantHash m_config;
        /*! The reconnector instance for the connection. */
        ReconnectorType m_reconnector = nullptr;

        /*! The query grammar implementation. */
        std::shared_ptr<QueryGrammar> m_queryGrammar = nullptr;
        /*! The schema grammar implementation. */
        std::shared_ptr<SchemaGrammar> m_schemaGrammar = nullptr;
        /*! The schema builder implementation. */
        std::unique_ptr<SchemaBuilder> m_schemaBuilder = nullptr;
        /*! The query post processor implementation. */
        std::unique_ptr<QueryProcessor> m_postProcessor = nullptr;

        /* Others */
        /*! Indicates if the connection is in a "dry run". */
        bool m_pretending = false;

    private:
        /*! Prepare an SQL statement and return the query object. */
        TSqlQuery prepareQuery(const QString &queryString);
        /*! Get a new Q/SqlQuery instance for the pretend for the current connection. */
        inline TSqlQuery getSqlQueryForPretend();

        /*! Prepare the QDateTime query binding for execution. */
        QDateTime prepareBinding(const QDateTime &binding) const;

        /*! Handle a query exception. */
        template<typename Return>
        Return handleQueryException(
                const std::exception_ptr &ePtr, const QString &errorMessage,
                const QString &queryString, const QList<QVariant> &preparedBindings,
                const RunCallback<Return> &callback) const;
        /*! Handle a query exception that occurred during query execution. */
        template<typename Return>
        Return tryAgainIfCausedByLostConnection(
                const std::exception_ptr &ePtr, const QString &errorMessage,
                const QString &queryString, const QList<QVariant> &preparedBindings,
                const RunCallback<Return> &callback) const;

        /*! Determine if the elapsed time for queries should be counted. */
        inline bool shouldCountElapsed() const;

        /*! Log database connected, invoked during MySQL ping. */
        void logConnected();
        /*! Log database disconnected, invoked during MySQL ping. */
        void logDisconnected();

        /*! The flag for the database was disconnected, used during MySQL ping. */
        bool m_disconnectedLogged = false;
        /*! The flag for the database was connected, used during MySQL ping. */
        bool m_connectedLogged = false;

        /*! Connection name, obtained from the connection configuration. */
        QString m_connectionName;
        /*! Host name, obtained from the connection configuration. */
        QString m_hostName;

        /*! Connection's driver name in printable format eg. QMYSQL -> MySQL. */
        std::optional<std::reference_wrapper<const QString>>
        m_driverNamePrintable = std::nullopt;
    };
#if defined(__GNUG__) && !defined(__clang__)
#  pragma GCC diagnostic pop
#endif

    /* public */

    DatabaseConnection::~DatabaseConnection() = default;

    QString DatabaseConnection::getTablePrefix() const
    {
        return m_tablePrefix;
    }

    Query::Expression
    DatabaseConnection::raw(const QVariant &value) const // NOLINT(readability-convert-member-functions-to-static)
    {
        return Query::Expression(value);
    }

    Query::Expression
    DatabaseConnection::raw(QVariant &&value) const noexcept // NOLINT(readability-convert-member-functions-to-static)
    {
        return Query::Expression(std::move(value));
    }

    /* Running SQL Queries */

    SqlQuery
    DatabaseConnection::selectFromWriteConnection(const QString &queryString,
                                                  QList<QVariant> bindings)
    {
        // This member function is used from the schema builders/post-processors only
        // FEATURE read/write connection silverqx
        return select(queryString, std::move(bindings)/*, false*/);
    }

    SqlQuery
    DatabaseConnection::insert(const QString &queryString, QList<QVariant> bindings)
    {
        return statement(queryString, std::move(bindings));
    }

    std::tuple<int, TSqlQuery>
    DatabaseConnection::update(const QString &queryString, QList<QVariant> bindings)
    {
        return affectingStatement(queryString, std::move(bindings));
    }

    std::tuple<int, TSqlQuery>
    DatabaseConnection::remove(const QString &queryString, QList<QVariant> bindings)
    {
        return affectingStatement(queryString, std::move(bindings));
    }

    /* Obtain connection instance */

    const std::function<Connectors::ConnectionName()> &
    DatabaseConnection::getSqlConnectionResolver() const noexcept
    {
        return m_qtConnectionResolver;
    }

    bool DatabaseConnection::isOpen()
    {
        return m_qtConnection && getSqlConnection().isOpen();
    }

    void DatabaseConnection::connectEagerly()
    {
        reconnectIfMissingConnection();

        // This opens a physical database connection
        std::ignore = getSqlConnection();
    }

    const QueryGrammar &DatabaseConnection::getQueryGrammar() const noexcept
    {
        return *m_queryGrammar;
    }

    QueryGrammar &DatabaseConnection::getQueryGrammar() noexcept
    {
        return *m_queryGrammar;
    }

    const SchemaGrammar &DatabaseConnection::getSchemaGrammar()
    {
        return *getSchemaGrammarShared();
    }

    const QueryProcessor &DatabaseConnection::getPostProcessor() const noexcept
    {
        return *m_postProcessor;
    }

    std::shared_ptr<QueryGrammar>
    DatabaseConnection::getQueryGrammarShared() const noexcept
    {
        return m_queryGrammar;
    }

    const QVariantHash &DatabaseConnection::getConfig() const noexcept
    {
        return m_config;
    }

    /* Getters */

    const QString &DatabaseConnection::getName() const noexcept
    {
        return m_connectionName;
    }

    const QString &DatabaseConnection::getDatabaseName() const noexcept
    {
        return m_database;
    }

    const QString &DatabaseConnection::getHostName() const noexcept
    {
        return m_hostName;
    }

    const QtTimeZoneConfig &DatabaseConnection::getQtTimeZone() const noexcept
    {
        return m_qtTimeZone;
    }

    bool DatabaseConnection::isConvertingTimeZone() const noexcept
    {
        return m_isConvertingTimeZone;
    }

    /* Others */

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

    /* protected */

    // See NOTES.txt[DatabaseConnection::run() and QList<QVariant> &&bindings]
    template<typename Return>
    Return
    DatabaseConnection::run(
            const QString &queryString, QList<QVariant> &&bindings, // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
            const QString &type, const RunCallback<Return> &callback)
    {
        reconnectIfMissingConnection();

        // Is Elapsed timer needed?
        const auto countElapsed = shouldCountElapsed();

        QElapsedTimer timer;
        if (countElapsed)
            timer.start();

        /* Prepare bindings early so they will be prepared only once (for performance
           reasons). The weird preparedBindings() return value is for better variable
           naming. */
        const auto &preparedBindings = prepareBindings(bindings);

        /* Here we will run this query. If an exception occurs we'll determine if it was
           caused by a connection that has been lost. If that is the cause, we'll try
           to re-establish connection and re-run the query with a fresh connection. */
        Return result = std::invoke([this, &queryString, &preparedBindings, &callback]
        {
            try {
                return runQueryCallback(queryString, preparedBindings, callback);

            } catch (const QueryError &e) {
                return handleQueryException(std::current_exception(), e.databaseText(),
                                            queryString, preparedBindings, callback);
            }
        });

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
            logQueryForPretend(queryString, preparedBindings, type);
        else
            logQuery(result, elapsed, type);

        return result;
    }

    template<typename Return>
    Return
    DatabaseConnection::runQueryCallback(
            const QString &queryString, const QList<QVariant> &preparedBindings,
            const RunCallback<Return> &callback) const
    {
        /* To execute the statement, we'll simply call the callback, which will actually
           run the SQL against the QSqlDatabase connection. Then we can calculate the time
           it took to execute and log the query SQL, bindings and time in our memory. */
        return std::invoke(callback, queryString, preparedBindings);
    }

    /* private */

    TSqlQuery DatabaseConnection::getSqlQueryForPretend()
    {
        return getSqlQuery();
    }

    template<typename Return>
    Return
    DatabaseConnection::handleQueryException(
            const std::exception_ptr &ePtr, const QString &errorMessage,
            const QString &queryString, const QList<QVariant> &preparedBindings,
            const RunCallback<Return> &callback) const
    {
        // FUTURE add info about in transaction into the exception that it was a reason why connection was not reconnected/recovered silverqx
        if (inTransaction())
            std::rethrow_exception(ePtr);

        return tryAgainIfCausedByLostConnection(ePtr, errorMessage, queryString,
                                                preparedBindings, callback);
    }

    template<typename Return>
    Return
    DatabaseConnection::tryAgainIfCausedByLostConnection(
            const std::exception_ptr &ePtr, const QString &errorMessage,
            const QString &queryString, const QList<QVariant> &preparedBindings,
            const RunCallback<Return> &callback) const
    {
        // TODO would be good to call KILL on lost connection to free locks, https://dev.mysql.com/doc/c-api/9.0/en/c-api-auto-reconnect.html silverqx
        if (!causedByLostConnection(errorMessage))
            std::rethrow_exception(ePtr);

        reconnect();

        // BUG rethrow e when causedByLostConnection to correctly inform user, causedByLostConnection state lost during second runQueryCallback(), because it internally tries to connect to DB and throws "Unable to connect to database" instead of "Lost connection", probably another try-catch and if catched "Unable to connect to database" then rethrow e (Lost connection)? silverqx
        /* After the second failed attempt will be isOpen() == false because
               the m_qtConnection == std::nullopt. */
        return runQueryCallback(queryString, preparedBindings, callback);
    }

    bool DatabaseConnection::shouldCountElapsed() const
    {
        return !m_pretending && (m_debugSql || m_countingElapsed);
    }

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DATABASECONNECTION_HPP
