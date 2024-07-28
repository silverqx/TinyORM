#pragma once
#ifndef ORM_DATABASEMANAGER_HPP
#define ORM_DATABASEMANAGER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/connectionresolverinterface.hpp"
#include "orm/query/querybuilder.hpp" // IWYU pragma: export
#include "orm/support/databaseconfiguration.hpp"
#include "orm/support/databaseconnectionsmap.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
namespace Query
{
    class Builder;
}

    /*! Database manager. */
    class TINYORM_EXPORT DatabaseManager final : public ConnectionResolverInterface
    {
        Q_DISABLE_COPY_MOVE(DatabaseManager)

        /*! Type for the Database Configuration. */
        using Configuration = Orm::Support::DatabaseConfiguration;

    public:
        /*! Type used for Database Connections map. */
        using ConfigurationsType = Configuration::ConfigurationsType;

        /*! Virtual destructor. */
        ~DatabaseManager() final;

        /* DatabaseManager factories */
        /*! Factory method to create DatabaseManager instance and set a default connection
            at once. */
        static std::shared_ptr<DatabaseManager>
        create(const QString &defaultConnection = Configuration::defaultConnectionName);
        /*! Factory method to create DatabaseManager instance and register a new
            connection as default connection at once. */
        static std::shared_ptr<DatabaseManager>
        create(const QVariantHash &config,
               const QString &connection = Configuration::defaultConnectionName);
        /*! Factory method to create DatabaseManager instance and set connections
            at once. */
        static std::shared_ptr<DatabaseManager>
        create(const ConfigurationsType &configs,
               const QString &defaultConnection = Configuration::defaultConnectionName);

        /* Expression factories */
        /*! Create a new raw query expression. */
        inline Query::Expression raw(const QVariant &value) const;
        /*! Create a new raw query expression. */
        inline Query::Expression raw(QVariant &&value) const noexcept;

        /* Proxy methods to the DatabaseConnection */
        /*! Begin a fluent query against a database table for the connection. */
        std::shared_ptr<QueryBuilder>
        table(const QString &table, const QString &connection = "");
        /*! Begin a fluent query against a database table for the connection. */
        std::shared_ptr<QueryBuilder>
        tableAs(const QString &table, const QString &as = "",
                const QString &connection = "");

        /*! Get a new query builder instance for the connection. */
        std::shared_ptr<QueryBuilder> query(const QString &connection = "");
        /*! Get a new Q/SqlQuery instance for the connection. */
        TSqlQuery sqlQuery(const QString &connection = "");

        // TODO next add support for named bindings, Using Named Bindings silverqx
        /*! Run a select statement against the database. */
        SqlQuery
        select(const QString &query, QList<QVariant> bindings = {},
               const QString &connection = "");
        /*! Run a select statement against the database. */
        SqlQuery
        selectFromWriteConnection(const QString &query, QList<QVariant> bindings = {},
                                  const QString &connection = "");

        /*! Run a select statement and return a single result. */
        SqlQuery
        selectOne(const QString &query, QList<QVariant> bindings = {},
                  const QString &connection = "");
        /*! Run a select statement and return the first column of the first row. */
        QVariant
        scalar(const QString &query, QList<QVariant> bindings = {},
               const QString &connection = "");

        /*! Run an insert statement against the database. */
        SqlQuery
        insert(const QString &query, QList<QVariant> bindings = {},
               const QString &connection = "");
        /*! Run an update statement against the database. */
        std::tuple<int, TSqlQuery>
        update(const QString &query, QList<QVariant> bindings = {},
               const QString &connection = "");
        /*! Run a delete statement against the database. */
        std::tuple<int, TSqlQuery>
        remove(const QString &query, QList<QVariant> bindings = {},
               const QString &connection = "");

        /*! Execute an SQL statement and return the boolean result and SqlQuery. */
        SqlQuery
        statement(const QString &query, QList<QVariant> bindings = {},
                  const QString &connection = "");
        /*! Run an SQL statement and get the number of rows affected. */
        std::tuple<int, TSqlQuery>
        affectingStatement(const QString &query, QList<QVariant> bindings = {},
                           const QString &connection = "");

        /*! Run a raw, unprepared query against the database. */
        SqlQuery unprepared(const QString &query, const QString &connection = "");

        /*! Start a new database transaction. */
        bool beginTransaction(const QString &connection = "");
        /*! Commit the active database transaction. */
        bool commit(const QString &connection = "");
        /*! Rollback the active database transaction. */
        bool rollBack(const QString &connection = "");
        /*! Start a new named transaction savepoint. */
        bool savepoint(const QString &id, const QString &connection = "");
        /*! Start a new named transaction savepoint. */
        bool savepoint(std::size_t id, const QString &connection = "");
        /*! Rollback to a named transaction savepoint. */
        bool rollbackToSavepoint(const QString &id, const QString &connection = "");
        /*! Rollback to a named transaction savepoint. */
        bool rollbackToSavepoint(std::size_t id, const QString &connection = "");
        /*! Get the number of active transactions. */
        std::size_t transactionLevel(const QString &connection = "");

        /*! Determine whether the database connection is currently open. */
        bool isOpen(const QString &connection = "");
        /*! Check database connection and show warnings when the state changed. */
        bool pingDatabase(const QString &connection = "");

        /*! Get the SQL database driver used to access the database connection (const). */
        const TSqlDriver *driver(const QString &connection = "");
#ifdef TINYORM_USING_TINYDRIVERS
        /*! Get the SQL database driver used to access the database connection (const). */
        std::weak_ptr<const TSqlDriver> driverWeak(const QString &connection = "");
#endif

        /* DatabaseManager */
        /*! Obtain a shared pointer to the DatabaseManager. */
        static std::shared_ptr<DatabaseManager> instance();
        /*! Obtain a reference to the DatabaseManager. */
        static DatabaseManager &reference();

        /*! Releases the ownership of the DatabaseManager managed object. */
        static void free() noexcept;

        /*! Get a database connection instance. */
        DatabaseConnection &connection(const QString &name = "") final; // NOLINT(google-default-arguments)
        /*! Begin a fluent query against the database on a given connection (alias for
            the connection() method). */
        inline DatabaseConnection &on(const QString &connection);
        /*! Register a connection with the manager. */
        DatabaseManager &
        addConnection(const QVariantHash &config,
                      const QString &name = Configuration::defaultConnectionName);
        /*! Register connections with the manager. */
        DatabaseManager &
        addConnections(const ConfigurationsType &configs);
        /*! Register connections with the manager and also set a default connection. */
        DatabaseManager &
        addConnections(const ConfigurationsType &configs,
                       const QString &defaultConnection);
        /*! Remove the given connection from the manager. */
        bool removeConnection(const QString &name = "");
        /*! Determine whether a given connection is already registered. */
        bool containsConnection(const QString &name = "");

        /*! Reconnect to the given database. */
        DatabaseConnection &reconnect(const QString &name = "");
        /*! Disconnect from the given database. */
        void disconnect(const QString &name = "") const;
        /*! Force connection to the database (creates physical connection), doesn't have
            to be called before querying a database. */
        void connectEagerly(const QString &name = "");

        /*! Returns a list containing the names of all connections. */
        QStringList connectionNames() const;
        /*! Returns a list containing the names of opened connections. */
        QStringList openedConnectionNames() const;
        /*! Get the number of opened connections. */
        std::size_t openedConnectionsSize() const noexcept;

        /*! Get all of the support drivers. */
        QStringList supportedDrivers() const;
        /*! Get all of the available drivers (loadable). */
        QStringList drivers() const;
        /*! Is the given driver name available? */
        bool isDriverAvailable(const QString &driverName) const;
        /*! Is a driver for the given connection available? */
        bool isConnectionDriverAvailable(const QString &connection);

        /*! Get the default connection name. */
        const QString &getDefaultConnection() const noexcept final;
        /*! Set the default connection name. */
        void setDefaultConnection(const QString &defaultConnection) final;
        /*! Reset the default connection name. */
        void resetDefaultConnection() final;

        /*! Set the database reconnector callback. */
        DatabaseManager &setReconnector(const ReconnectorType &reconnector);

        /* Getters / Setters */
        /*! Return the connection's driver name. */
        QString driverName(const QString &connection = "");
        /*! Return connection's driver name in printable format eg. QMYSQL -> MySQL. */
        const QString &driverNamePrintable(const QString &connection = "");
        /*! Return the name of the connected database. */
        const QString &databaseName(const QString &connection = "");
        /*! Return the hostname of the connected database. */
        const QString &hostName(const QString &connection = "");

        /*! Get the QtTimeZoneConfig for the current connection. */
        const QtTimeZoneConfig &
        qtTimeZone(const QString &connection = "");
        /*! Set the QtTimeZoneConfig for the current connection (override qt_timezone). */
        DatabaseConnection &
        setQtTimeZone(const QVariant &timezone, const QString &connection = "");
        /*! Set the QtTimeZoneConfig for the current connection (override qt_timezone). */
        DatabaseConnection &
        setQtTimeZone(QtTimeZoneConfig &&timezone, const QString &connection = "");
        /*! Determine whether the QDateTime time zone should be converted. */
        bool isConvertingTimeZone(const QString &connection = "");

        /* Connection configurations - saved in the DatabaseManager */
        /*! Get an original configuration option value for the given connection
            (passed to the DB::create, original/unchanged). */
        QVariant originalConfigValue(const QString &option,
                                     const QString &connection = "") const;
        /*! Get an original configuration for the given connection
            (passed to the DB::create, original/unchanged). */
        const QVariantHash &originalConfig(const QString &connection = "") const;
        /*! Get the number of registered connection configurations. */
        std::size_t originalConfigsSize() const;

        /* Connection configurations - proxies to the DatabaseConnection */
        /*! Get an option value from the configuration options. */
        QVariant getConfigValue(const QString &option, const QString &connection = "");
        /*! Get the configuration for the current connection. */
        const QVariantHash &getConfig(const QString &connection = "");
        /*! Check whether the configuration contains the given option. */
        bool hasConfigValue(const QString &option, const QString &connection = "");

        /* Pretending */
        /*! Execute the given callback in "dry run" mode. */
        QList<Log> pretend(const std::function<void()> &callback,
                           const QString &connection = "");
        /*! Execute the given callback in "dry run" mode. */
        QList<Log> pretend(const std::function<void(DatabaseConnection &)> &callback,
                           const QString &connection = "");

        /* Records were modified */
        /*! Check if any records have been modified. */
        bool getRecordsHaveBeenModified(const QString &connection = "");
        /*! Indicates if any records have been modified. */
        void recordsHaveBeenModified(bool value = true, const QString &connection = "");
        /*! Reset the record modification state. */
        void forgetRecordModificationState(const QString &connection = "");

        /* Logging */
        /*! Get the connection query log. */
        std::shared_ptr<QList<Log>>
        getQueryLog(const QString &connection = "");
        /*! Clear the query log. */
        void flushQueryLog(const QString &connection = "");
        /*! Enable the query log on the connection. */
        void enableQueryLog(const QString &connection = "");
        /*! Disable the query log on the connection. */
        void disableQueryLog(const QString &connection = "");
        /*! Determine whether we're logging queries. */
        bool logging(const QString &connection = "");
        /*! The current order value for a query log record. */
        std::size_t getQueryLogOrder() const noexcept;

        /* Queries execution time counter */
        /*! Determine whether we're counting queries execution time. */
        bool countingElapsed(const QString &connection = "");
        /*! Enable counting queries execution time on the current connection. */
        DatabaseConnection &enableElapsedCounter(const QString &connection = "");
        /*! Disable counting queries execution time on the current connection. */
        DatabaseConnection &disableElapsedCounter(const QString &connection = "");
        /*! Obtain queries execution time. */
        qint64 getElapsedCounter(const QString &connection = "");
        /*! Obtain and reset queries execution time. */
        qint64 takeElapsedCounter(const QString &connection = "");
        /*! Reset queries execution time. */
        DatabaseConnection &resetElapsedCounter(const QString &connection = "");

        /*! Determine whether any connection is counting queries execution time. */
        bool anyCountingElapsed();
        /*! Enable counting queries execution time on all connections. */
        void enableAllElapsedCounters();
        /*! Disable counting queries execution time on all connections. */
        void disableAllElapsedCounters();
        /*! Obtain queries execution time from all connections. */
        qint64 getAllElapsedCounters();
        /*! Obtain and reset queries execution time on all active connections. */
        qint64 takeAllElapsedCounters();
        /*! Reset queries execution time on all active connections. */
        void resetAllElapsedCounters();

        /*! Enable counting queries execution time on given connections. */
        void enableElapsedCounters(const QStringList &connections);
        /*! Disable counting queries execution time on given connections. */
        void disableElapsedCounters(const QStringList &connections);
        /*! Obtain queries execution time from given connections. */
        qint64 getElapsedCounters(const QStringList &connections);
        /*! Obtain and reset queries execution time on given connections. */
        qint64 takeElapsedCounters(const QStringList &connections);
        /*! Reset queries execution time on given connections. */
        void resetElapsedCounters(const QStringList &connections);

        /* Queries executed counter */
        /*! Determine whether we're counting the number of executed queries. */
        bool countingStatements(const QString &connection = "");
        /*! Enable counting the number of executed queries on the current connection. */
        DatabaseConnection &
        enableStatementsCounter(const QString &connection = "");
        /*! Disable counting the number of executed queries on the current connection. */
        DatabaseConnection &
        disableStatementsCounter(const QString &connection = "");
        /*! Obtain the number of executed queries. */
        const StatementsCounter &
        getStatementsCounter(const QString &connection = "");
        /*! Obtain and reset the number of executed queries. */
        StatementsCounter
        takeStatementsCounter(const QString &connection = "");
        /*! Reset the number of executed queries. */
        DatabaseConnection &
        resetStatementsCounter(const QString &connection = "");

        /*! Determine whether any connection is counting the number of executed
            queries. */
        bool anyCountingStatements();
        /*! Enable counting the number of executed queries on all connections. */
        void enableAllStatementCounters();
        /*! Disable counting the number of executed queries on all connections. */
        void disableAllStatementCounters();
        /*! Obtain the number of executed queries on all active connections. */
        StatementsCounter getAllStatementCounters();
        /*! Obtain and reset the number of executed queries on all active connections. */
        StatementsCounter takeAllStatementCounters();
        /*! Reset the number of executed queries on all active connections. */
        void resetAllStatementCounters();

        /*! Enable counting the number of executed queries on given connections. */
        void enableStatementCounters(const QStringList &connections);
        /*! Disable counting the number of executed queries on given connections. */
        void disableStatementCounters(const QStringList &connections);
        /*! Obtain the number of executed queries on given connections. */
        StatementsCounter getStatementCounters(const QStringList &connections);
        /*! Obtain and reset the number of executed queries on given connections. */
        StatementsCounter takeStatementCounters(const QStringList &connections);
        /*! Reset the number of executed queries on given connections. */
        void resetStatementCounters(const QStringList &connections);

    private:
        /*! Private constructor to create DatabaseManager instance and set a default
            connection at once. */
        explicit DatabaseManager(const QString &defaultConnection);
        /*! Private constructor to create DatabaseManager instance and register a new
            connection as default connection at once. */
        explicit DatabaseManager(const QVariantHash &config, const QString &connection,
                                 const QString &defaultConnection);
        /*! Private constructor to create DatabaseManager instance and set connections
            at once. */
        explicit DatabaseManager(const ConfigurationsType &configs,
                                 const QString &defaultConnection);

        /*! Setup the default database connection reconnector. */
        DatabaseManager &setupDefaultReconnector();

        /*! Parse the connection into the string of the name and read / write type. */
        const QString &parseConnectionName(const QString &name) const;

        /*! Make the database connection instance. */
        std::shared_ptr<DatabaseConnection>
        makeConnection(const QString &connection);

        /*! Get the configuration for a connection. */
        QVariantHash &configuration(const QString &connection);
        /*! Throw if a given database connection doesn't have any configuration. */
        void throwIfNoConfiguration(const QString &connection) const;

        /*! Prepare the database connection instance. */
        std::shared_ptr<DatabaseConnection>
        configure(std::shared_ptr<DatabaseConnection> &&connection) const;

        /*! Refresh an underlying Q/SqlDatabase connection resolver on a given
            TinyORM connection. */
        DatabaseConnection &refreshSqlConnection(const QString &connection);

        /*! Throw an exception if DatabaseManager instance already exists. */
        static void checkInstance();

        /*! Database configuration. */
        Configuration m_configuration {};
        /*! Active database connection instances for the current thread. */
        Support::DatabaseConnectionsMap m_connections {};
        /*! The callback to be executed to reconnect to a database. */
        ReconnectorType m_reconnector = nullptr;

        /*! Shared pointer to the DatabaseManager instance. */
        static std::shared_ptr<DatabaseManager> m_instance;
    };

    /* public */

    /* Expression factories */

    Query::Expression
    DatabaseManager::raw(const QVariant &value) const // NOLINT(readability-convert-member-functions-to-static)
    {
        return Query::Expression(value);
    }

    Query::Expression
    DatabaseManager::raw(QVariant &&value) const noexcept // NOLINT(readability-convert-member-functions-to-static)
    {
        return Query::Expression(std::move(value));
    }

    /* DatabaseManager */

    DatabaseConnection &DatabaseManager::on(const QString &connection)
    {
        return this->connection(connection);
    }

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DATABASEMANAGER_HPP
