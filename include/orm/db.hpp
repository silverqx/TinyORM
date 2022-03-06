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
    class SHAREDLIB_EXPORT DB final
    {
        Q_DISABLE_COPY(DB)

        /*! Type for the Database Configuration. */
        using Configuration = Orm::Support::DatabaseConfiguration;
        /*! Type used for Database Connections map. */
        using ConfigurationsType = Configuration::ConfigurationsType;

    public:
        /*! Deleted default constructor, this is a pure library class. */
        DB() = delete;
        /*! Deleted destructor. */
        ~DB() = delete;

        /* Proxy methods to the DatabaseManager */
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

        /*! Get a database connection instance. */
        static DatabaseConnection &connection(const QString &name = "");
        /*! Begin a fluent query against the database on a given connection (alias for
            the connection() method). */
        static DatabaseConnection &on(const QString &name);
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
        static QSqlDatabase connectEagerly(const QString &name = "");

        /*! Get all of the support drivers. */
        static QStringList supportedDrivers();
        /*! Returns a list containing the names of all connections. */
        static QStringList connectionNames();
        /*! Returns a list containing the names of opened connections. */
        static QStringList openedConnectionNames();

        /*! Get the default connection name. */
        static const QString &getDefaultConnection();
        /*! Set the default connection name. */
        static void setDefaultConnection(const QString &defaultConnection);
        /*! Reset the default connection name. */
        static void resetDefaultConnection();

        /*! Set the database reconnector callback. */
        static DatabaseManager &setReconnector(const ReconnectorType &reconnector);

        /* Proxy methods to the DatabaseConnection */
        /*! Begin a fluent query against a database table for the connection. */
        static QSharedPointer<QueryBuilder>
        table(const QString &table, const QString &connection = "");
        /*! Begin a fluent query against a database table for the connection. */
        static QSharedPointer<QueryBuilder>
        tableAs(const QString &table, const QString &as = "",
                const QString &connection = "");

        /*! Get a new query builder instance for the connection. */
        static QSharedPointer<QueryBuilder> query(const QString &connection = "");
        /*! Get a new QSqlQuery instance for the connection. */
        static QSqlQuery qtQuery(const QString &connection = "");

        /*! Create a new raw query expression. */
        inline static Query::Expression raw(const QVariant &value);

        /*! Run a select statement against the database. */
        static QSqlQuery
        select(const QString &query, const QVector<QVariant> &bindings = {},
               const QString &connection = "");
        /*! Run a select statement and return a single result. */
        static QSqlQuery
        selectOne(const QString &query, const QVector<QVariant> &bindings = {},
                  const QString &connection = "");
        /*! Run an insert statement against the database. */
        static QSqlQuery
        insert(const QString &query, const QVector<QVariant> &bindings = {},
               const QString &connection = "");
        /*! Run an update statement against the database. */
        static std::tuple<int, QSqlQuery>
        update(const QString &query, const QVector<QVariant> &bindings = {},
               const QString &connection = "");
        /*! Run a delete statement against the database. */
        static std::tuple<int, QSqlQuery>
        remove(const QString &query, const QVector<QVariant> &bindings = {},
               const QString &connection = "");

        /*! Execute an SQL statement and return the boolean result and QSqlQuery. */
        static QSqlQuery
        statement(const QString &query, const QVector<QVariant> &bindings = {},
                  const QString &connection = "");
        /*! Run an SQL statement and get the number of rows affected. */
        static std::tuple<int, QSqlQuery>
        affectingStatement(const QString &query, const QVector<QVariant> &bindings = {},
                           const QString &connection = "");

        /*! Run a raw, unprepared query against the database. */
        static QSqlQuery
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

        /* Logging */
        /*! Get the connection query log. */
        static std::shared_ptr<QVector<Log>>
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
        static std::size_t getQueryLogOrder();

        /* Getters */
        /*! Return the connection's driver name. */
        static QString driverName(const QString &connection = "");
        /*! Return connection's driver name in printable format eg. QMYSQL -> MySQL. */
        static const QString &driverNamePrintable(const QString &connection = "");
        /*! Return the name of the connected database. */
        static const QString &databaseName(const QString &connection = "");
        /*! Return the host name of the connected database. */
        static const QString &hostName(const QString &connection = "");

        /* Others */
        /*! Execute the given callback in "dry run" mode. */
        static QVector<Log>
        pretend(const std::function<void()> &callback,
                const QString &connection = "");
        /*! Execute the given callback in "dry run" mode. */
        static QVector<Log>
        pretend(const std::function<void(DatabaseConnection &)> &callback,
                const QString &connection = "");

        /*! Check if any records have been modified. */
        static bool getRecordsHaveBeenModified(const QString &connection = "");
        /*! Indicate if any records have been modified. */
        static void recordsHaveBeenModified(bool value = true,
                                            const QString &connection = "");
        /*! Reset the record modification state. */
        static void forgetRecordModificationState(const QString &connection = "");

    private:
        /*! Get a reference to the DatabaseManager. */
        static DatabaseManager &manager();

        /*! Pointer to the DatabaseManager. */
        static std::shared_ptr<DatabaseManager> m_manager;
    };

    Query::Expression DB::raw(const QVariant &value)
    {
        return Query::Expression(value);
    }

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DB_HPP
