#pragma once
#ifndef ORM_DB_HPP
#define ORM_DB_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/databasemanager.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

    /*! Facade class for the DatabaseManager. */
    class TINYORM_EXPORT DB final
    {
        Q_DISABLE_COPY_MOVE(DB)

        /*! Type for the Database Configuration. */
        using Configuration = Orm::Support::DatabaseConfiguration;

    public:
        /*! Type used for Database Connections map. */
        using ConfigurationsType = Configuration::ConfigurationsType;

        /*! Deleted default constructor, this is a pure library class. */
        DB() = delete;
        /*! Deleted destructor. */
        ~DB() = delete;

        /* DatabaseManager factories */
        /*! Factory method to create DatabaseManager instance and set a default connection
            at once. */
        static std::shared_ptr<DatabaseManager>
        create(const QString &defaultConnection = Configuration::defaultConnectionName);
        /*! Factory method to create DatabaseManager instance and register
            a new connection as default connection at once. */
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
        inline static Query::Expression raw(const QVariant &value);
        /*! Create a new raw query expression. */
        inline static Query::Expression raw(QVariant &&value) noexcept;

        /* Proxy methods to the DatabaseConnection */
        /*! Begin a fluent query against a database table for the connection. */
        static std::shared_ptr<QueryBuilder>
        table(const QString &table, const QString &connection = "");
        /*! Begin a fluent query against a database table for the connection. */
        static std::shared_ptr<QueryBuilder>
        tableAs(const QString &table, const QString &as = "",
                const QString &connection = "");

        /*! Get a new query builder instance for the connection. */
        static std::shared_ptr<QueryBuilder> query(const QString &connection = "");
        /*! Get a new Q/SqlQuery instance for the connection. */
        static TSqlQuery sqlQuery(const QString &connection = "");

        /*! Run a select statement against the database. */
        static SqlQuery
        select(const QString &query, QList<QVariant> bindings = {},
               const QString &connection = "");
        /*! Run a select statement against the database. */
        static SqlQuery
        selectFromWriteConnection(const QString &query, QList<QVariant> bindings = {},
                                  const QString &connection = "");

        /*! Run a select statement and return a single result. */
        static SqlQuery
        selectOne(const QString &query, QList<QVariant> bindings = {},
                  const QString &connection = "");
        /*! Run a select statement and return the first column of the first row. */
        static QVariant
        scalar(const QString &query, QList<QVariant> bindings = {},
               const QString &connection = "");

        /*! Run an insert statement against the database. */
        static SqlQuery
        insert(const QString &query, QList<QVariant> bindings = {},
               const QString &connection = "");
        /*! Run an update statement against the database. */
        static std::tuple<int, TSqlQuery>
        update(const QString &query, QList<QVariant> bindings = {},
               const QString &connection = "");
        /*! Run a delete statement against the database. */
        static std::tuple<int, TSqlQuery>
        remove(const QString &query, QList<QVariant> bindings = {},
               const QString &connection = "");

        /*! Execute an SQL statement and return the boolean result and SqlQuery. */
        static SqlQuery
        statement(const QString &query, QList<QVariant> bindings = {},
                  const QString &connection = "");
        /*! Run an SQL statement and get the number of rows affected. */
        static std::tuple<int, TSqlQuery>
        affectingStatement(const QString &query, QList<QVariant> bindings = {},
                           const QString &connection = "");

        /*! Run a raw, unprepared query against the database. */
        static SqlQuery
        unprepared(const QString &query, const QString &connection = "");

        /*! Start a new database transaction. */
        static bool beginTransaction(const QString &connection = "");
        /*! Commit the active database transaction. */
        static bool commit(const QString &connection = "");
        /*! Rollback the active database transaction. */
        static bool rollBack(const QString &connection = "");
        /*! Start a new named transaction savepoint. */
        static bool savepoint(const QString &id, const QString &connection = "");
        /*! Start a new named transaction savepoint. */
        static bool savepoint(std::size_t id, const QString &connection = "");
        /*! Rollback to a named transaction savepoint. */
        static bool rollbackToSavepoint(const QString &id,
                                        const QString &connection = "");
        /*! Rollback to a named transaction savepoint. */
        static bool rollbackToSavepoint(std::size_t id,
                                        const QString &connection = "");
        /*! Get the number of active transactions. */
        static std::size_t transactionLevel(const QString &connection = "");

        /*! Determine whether the database connection is currently open. */
        static bool isOpen(const QString &connection = "");
        /*! Check database connection and show warnings when the state changed. */
        static bool pingDatabase(const QString &connection = "");

        /*! Get the SQL database driver used to access the database connection (const). */
        static const TSqlDriver *driver(const QString &connection = "");
#ifdef TINYORM_USING_TINYDRIVERS
        /*! Get the SQL database driver used to access the database connection (const). */
        static std::weak_ptr<const TSqlDriver> driverWeak(const QString &connection = "");
#endif

        /* Proxy methods to the DatabaseManager */
        /*! Get a database connection instance. */
        static DatabaseConnection &connection(const QString &name = "");
        /*! Begin a fluent query against the database on a given connection (alias for
            the connection() method). */
        static DatabaseConnection &on(const QString &connection);
        /*! Register a connection with the manager. */
        static DatabaseManager &
        addConnection(const QVariantHash &config,
                      const QString &name = Configuration::defaultConnectionName);
        /*! Register connections with the manager. */
        static DatabaseManager &
        addConnections(const ConfigurationsType &configs);
        /*! Register connections with the manager and also set a default connection. */
        static DatabaseManager &
        addConnections(const ConfigurationsType &configs,
                       const QString &defaultConnection);
        /*! Remove the given connection from the manager. */
        static bool removeConnection(const QString &name = "");
        /*! Determine whether a given connection is already registered. */
        static bool containsConnection(const QString &name = "");

        /*! Reconnect to the given database. */
        static DatabaseConnection &reconnect(const QString &name = "");
        /*! Disconnect from the given database. */
        static void disconnect(const QString &name = "");
        /*! Force connection to the database (creates physical connection), doesn't have
            to be called before querying a database. */
        static void connectEagerly(const QString &name = "");

        /*! Returns a list containing the names of all connections. */
        static QStringList connectionNames();
        /*! Returns a list containing the names of opened connections. */
        static QStringList openedConnectionNames();
        /*! Get the number of opened connections. */
        static std::size_t openedConnectionsSize();

        /*! Get all of the support drivers. */
        static QStringList supportedDrivers();
        /*! Get all of the available drivers (loadable). */
        static QStringList drivers();
        /*! Is the given driver name available? */
        static bool isDriverAvailable(const QString &driverName);
        /*! Is a driver for the given connection available? */
        static bool isConnectionDriverAvailable(const QString &connection);

        /*! Get the default connection name. */
        static const QString &getDefaultConnection();
        /*! Set the default connection name. */
        static void setDefaultConnection(const QString &defaultConnection);
        /*! Reset the default connection name. */
        static void resetDefaultConnection();

        /*! Set the database reconnector callback. */
        static DatabaseManager &setReconnector(const ReconnectorType &reconnector);

        /* Getters / Setters */
        /*! Return the connection's driver name. */
        static QString driverName(const QString &connection = "");
        /*! Return connection's driver name in printable format eg. QMYSQL -> MySQL. */
        static const QString &driverNamePrintable(const QString &connection = "");
        /*! Return the name of the connected database. */
        static const QString &databaseName(const QString &connection = "");
        /*! Return the hostname of the connected database. */
        static const QString &hostName(const QString &connection = "");

        /*! Get the QtTimeZoneConfig for the current connection. */
        static const QtTimeZoneConfig &
        qtTimeZone(const QString &connection = "");
        /*! Set the QtTimeZoneConfig for the current connection (override qt_timezone). */
        static DatabaseConnection &
        setQtTimeZone(const QVariant &timezone, const QString &connection = "");
        /*! Set the QtTimeZoneConfig for the current connection (override qt_timezone). */
        static DatabaseConnection &
        setQtTimeZone(QtTimeZoneConfig &&timezone, const QString &connection = "");
        /*! Determine whether the QDateTime time zone should be converted. */
        static bool isConvertingTimeZone(const QString &connection = "");

        /* Connection configurations - saved in the DatabaseManager */
        /*! Get a configuration option value from the configuration for a connection. */
        static QVariant originalConfigValue(const QString &option,
                                            const QString &connection = "");
        /*! Get the configuration for a connection. */
        static const QVariantHash &originalConfig(const QString &connection = "");
        /*! Get the number of registered connection configurations. */
        static std::size_t originalConfigsSize();

        /* Connection configurations - proxies to the DatabaseConnection */
        /*! Get an option value from the configuration options. */
        static QVariant getConfigValue(const QString &option,
                                       const QString &connection = "");
        /*! Get the configuration for the current connection. */
        static const QVariantHash &getConfig(const QString &connection = "");
        /*! Check whether the configuration contains the given option. */
        static bool hasConfigValue(const QString &option, const QString &connection = "");

        /* Pretending */
        /*! Execute the given callback in "dry run" mode. */
        static QList<Log>
        pretend(const std::function<void()> &callback,
                const QString &connection = "");
        /*! Execute the given callback in "dry run" mode. */
        static QList<Log>
        pretend(const std::function<void(DatabaseConnection &)> &callback,
                const QString &connection = "");

        /* Records were modified */
        /*! Check if any records have been modified. */
        static bool getRecordsHaveBeenModified(const QString &connection = "");
        /*! Indicates if any records have been modified. */
        static void recordsHaveBeenModified(bool value = true,
                                            const QString &connection = "");
        /*! Reset the record modification state. */
        static void forgetRecordModificationState(const QString &connection = "");

        /* Logging */
        /*! Get the connection query log. */
        static std::shared_ptr<QList<Log>>
        getQueryLog(const QString &connection = "");
        /*! Clear the query log. */
        static void flushQueryLog(const QString &connection = "");
        /*! Enable the query log on the connection. */
        static void enableQueryLog(const QString &connection = "");
        /*! Disable the query log on the connection. */
        static void disableQueryLog(const QString &connection = "");
        /*! Determine whether we're logging queries. */
        static bool logging(const QString &connection = "");
        /*! The current order value for a query log record. */
        static std::size_t getQueryLogOrder() noexcept;

        /* Queries execution time counter */
        /*! Determine whether we're counting queries execution time. */
        static bool
        countingElapsed(const QString &connection = "");
        /*! Enable counting queries elapsed time on the current connection. */
        static DatabaseConnection &
        enableElapsedCounter(const QString &connection = "");
        /*! Disable counting queries elapsed time on the current connection. */
        static DatabaseConnection &
        disableElapsedCounter(const QString &connection = "");
        /*! Obtain queries elapsed time. */
        static qint64
        getElapsedCounter(const QString &connection = "");
        /*! Obtain and reset queries elapsed time. */
        static qint64
        takeElapsedCounter(const QString &connection = "");
        /*! Reset queries elapsed time. */
        static DatabaseConnection &
        resetElapsedCounter(const QString &connection = "");

        /*! Determine whether any connection is counting queries execution time. */
        static bool anyCountingElapsed();
        /*! Enable counting queries execution time on all connections. */
        static void enableAllElapsedCounters();
        /*! Disable counting queries execution time on all connections. */
        static void disableAllElapsedCounters();
        /*! Obtain queries execution time from all connections. */
        static qint64 getAllElapsedCounters();
        /*! Obtain and reset queries execution time on all active connections. */
        static qint64 takeAllElapsedCounters();
        /*! Reset queries execution time on all active connections. */
        static void resetAllElapsedCounters();

        /*! Enable counting queries execution time on given connections. */
        static void enableElapsedCounters(const QStringList &connections);
        /*! Disable counting queries execution time on given connections. */
        static void disableElapsedCounters(const QStringList &connections);
        /*! Obtain queries execution time from given connections. */
        static qint64 getElapsedCounters(const QStringList &connections);
        /*! Obtain and reset queries execution time on given connections. */
        static qint64 takeElapsedCounters(const QStringList &connections);
        /*! Reset queries execution time on given connections. */
        static void resetElapsedCounters(const QStringList &connections);

        /* Queries executed counter */
        /*! Determine whether we're counting the number of executed queries. */
        static bool
        countingStatements(const QString &connection = "");
        /*! Enable counting the number of executed queries on the current connection. */
        static DatabaseConnection &
        enableStatementsCounter(const QString &connection = "");
        /*! Disable counting the number of executed queries on the current connection. */
        static DatabaseConnection &
        disableStatementsCounter(const QString &connection = "");
        /*! Obtain the number of executed queries. */
        static const StatementsCounter &
        getStatementsCounter(const QString &connection = "");
        /*! Obtain and reset the number of executed queries. */
        static StatementsCounter
        takeStatementsCounter(const QString &connection = "");
        /*! Reset the number of executed queries. */
        static DatabaseConnection &
        resetStatementsCounter(const QString &connection = "");

        /*! Determine whether any connection is counting the number of executed
            queries. */
        static bool anyCountingStatements();
        /*! Enable counting the number of executed queries on all connections. */
        static void enableAllStatementCounters();
        /*! Disable counting the number of executed queries on all connections. */
        static void disableAllStatementCounters();
        /*! Obtain the number of executed queries on all active connections. */
        static StatementsCounter getAllStatementCounters();
        /*! Obtain and reset the number of executed queries on all active connections. */
        static StatementsCounter takeAllStatementCounters();
        /*! Reset the number of executed queries on all active connections. */
        static void resetAllStatementCounters();

        /*! Enable counting the number of executed queries on given connections. */
        static void enableStatementCounters(const QStringList &connections);
        /*! Disable counting the number of executed queries on given connections. */
        static void disableStatementCounters(const QStringList &connections);
        /*! Obtain the number of executed queries on given connections. */
        static StatementsCounter getStatementCounters(const QStringList &connections);
        /*! Obtain and reset the number of executed queries on given connections. */
        static StatementsCounter takeStatementCounters(const QStringList &connections);
        /*! Reset the number of executed queries on given connections. */
        static void resetStatementCounters(const QStringList &connections);

        /* DB */
        /*! Releases the ownership of the DatabaseManager managed object. */
        static void free() noexcept;

    private:
        /*! Get a reference to the DatabaseManager. */
        static DatabaseManager &manager();

        /*! Pointer to the DatabaseManager. */
        static std::shared_ptr<DatabaseManager> m_manager;
    };

    /* public */

    /* Expression factories */

    Query::Expression DB::raw(const QVariant &value)
    {
        return Query::Expression(value);
    }

    Query::Expression DB::raw(QVariant &&value) noexcept
    {
        return Query::Expression(std::move(value));
    }

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DB_HPP
