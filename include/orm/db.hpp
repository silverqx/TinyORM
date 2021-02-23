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

        /*! Reconnect to the given database. */
        static ConnectionInterface &reconnect(QString name = "");
        /*! Disconnect from the given database. */
        static void disconnect(QString name = "");

        /*! Get all of the support drivers. */
        static const QStringList supportedDrivers();

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
        static inline Query::Expression raw(const QVariant &value)
        { return value; }

        /*! Run a select statement against the database. */
        static std::tuple<bool, QSqlQuery>
        select(const QString &query, const QVector<QVariant> &bindings = {});
        /*! Run a select statement and return a single result. */
        static std::tuple<bool, QSqlQuery>
        selectOne(const QString &query, const QVector<QVariant> &bindings = {});
        /*! Run an insert statement against the database. */
        static std::tuple<bool, QSqlQuery>
        insert(const QString &query, const QVector<QVariant> &bindings = {});
        /*! Run an update statement against the database. */
        static std::tuple<int, QSqlQuery>
        update(const QString &query, const QVector<QVariant> &bindings = {});
        /*! Run a delete statement against the database. */
        static std::tuple<int, QSqlQuery>
        remove(const QString &query, const QVector<QVariant> &bindings = {});
        /*! Execute an SQL statement and return the boolean result and QSqlQuery. */
        static std::tuple<bool, QSqlQuery>
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
    };

} // namespace Orm

using DB = Orm::DB;

#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif


#endif // DB_HPP
