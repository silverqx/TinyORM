#ifndef DB_HPP
#define DB_HPP

#include "orm/databasemanager.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    /*! Facade class for the DatabaseManager. */
    class SHAREDLIB_EXPORT DB final
    {
        Q_DISABLE_COPY(DB)

        using ConfigurationsType = Orm::Configuration::ConfigurationsType;

        /*! Prohibited DB's constructor. */
        DB() = default;
        /*! Prohibited DB's destructor. */
        ~DB() = default;

        /*! Get the reference to the DatabaseManager. */
        static DatabaseManager &manager();

        /*! Pointer to the DatabaseManager. */
        static DatabaseManager *m_manager;

    public:
        /* Proxy methods to the DatabaseManager */
        /*! Factory method to create DatabaseManager instance and register
            a new connection as default connection at once. */
        static std::unique_ptr<DatabaseManager>
        create(const QVariantHash &config,
               const QString &connection =
               QLatin1String(DatabaseManager::defaultConnectionName));
        /*! Factory method to create DatabaseManager instance and set connections
            at once. */
        static std::unique_ptr<DatabaseManager>
        create(const ConfigurationsType &configs,
               const QString &defaultConnection =
               QLatin1String(DatabaseManager::defaultConnectionName));

        /*! Get a database connection instance. */
        static ConnectionInterface &connection(const QString &name = "");
        /*! Register a connection with the manager. */
        static DatabaseManager &
        addConnection(const QVariantHash &config,
                      const QString &name =
                      QLatin1String(DatabaseManager::defaultConnectionName));
        /*! Remove the given connection from the manager. */
        static bool removeConnection(QString name = "");

        /*! Reconnect to the given database. */
        static ConnectionInterface &reconnect(QString name = "");
        /*! Disconnect from the given database. */
        static void disconnect(QString name = "");

        /*! Get all of the support drivers. */
        static const QStringList supportedDrivers();
        /*! Returns a list containing the names of all connections. */
        static QStringList connectionNames();
        /*! Returns a list containing the names of opened connections. */
        static QStringList openedConnectionNames();

        /*! Get the default connection name. */
        static const QString &getDefaultConnection();
        /*! Set the default connection name. */
        static void setDefaultConnection(const QString &defaultConnection);

        /*! Set the database reconnector callback. */
        static DatabaseManager &setReconnector(
                const DatabaseManager::ReconnectorType &reconnector);

        /* Proxy methods to the DatabaseConnection */
        /*! Begin a fluent query against a database table for the connection. */
        static QSharedPointer<QueryBuilder>
        table(const QString &table, const QString &as = "",
              const QString &connection = "");

        /*! Get a new query builder instance for the connection. */
        static QSharedPointer<QueryBuilder> query(const QString &connection = "");
        /*! Get a new QSqlQuery instance for the connection. */
        static QSqlQuery qtQuery(const QString &connection = "");

        /*! Create a new raw query expression. */
        inline static Query::Expression raw(const QVariant &value)
        { return value; }

        /*! Run a select statement against the database. */
        static QSqlQuery
        select(const QString &query, const QVector<QVariant> &bindings = {});
        /*! Run a select statement and return a single result. */
        static QSqlQuery
        selectOne(const QString &query, const QVector<QVariant> &bindings = {});
        /*! Run an insert statement against the database. */
        static QSqlQuery
        insert(const QString &query, const QVector<QVariant> &bindings = {});
        /*! Run an update statement against the database. */
        static std::tuple<int, QSqlQuery>
        update(const QString &query, const QVector<QVariant> &bindings = {});
        /*! Run a delete statement against the database. */
        static std::tuple<int, QSqlQuery>
        remove(const QString &query, const QVector<QVariant> &bindings = {});
        /*! Execute an SQL statement and return the boolean result and QSqlQuery. */
        static QSqlQuery
        statement(const QString &query,
                  const QVector<QVariant> &bindings = {});

        /*! Start a new database transaction. */
        static bool beginTransaction(const QString &connection = "");
        /*! Commit the active database transaction. */
        static bool commit(const QString &connection = "");
        /*! Rollback the active database transaction. */
        static bool rollBack(const QString &connection = "");
        /*! Start a new named transaction savepoint. */
        static bool savepoint(const QString &id, const QString &connection = "");
        /*! Start a new named transaction savepoint. */
        static bool savepoint(size_t id, const QString &connection = "");
        /*! Rollback to a named transaction savepoint. */
        static bool rollbackToSavepoint(const QString &id,
                                        const QString &connection = "");
        /*! Rollback to a named transaction savepoint. */
        static bool rollbackToSavepoint(size_t id,
                                        const QString &connection = "");
        /*! Get the number of active transactions. */
        static uint transactionLevel(const QString &connection = "");

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
    };

} // namespace Orm

using DB = Orm::DB;

#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // DB_HPP
