#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql/QSqlQuery>

#include "export.hpp"
#include "orm/configuration.hpp"
#include "orm/connectionresolverinterface.hpp"
#include "orm/connectors/connectionfactory.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{
namespace Query
{
    class Builder;
}
    using QueryBuilder = Query::Builder;

    class SHAREDLIB_EXPORT DatabaseManager final : public ConnectionResolverInterface
    {
        Q_DISABLE_COPY(DatabaseManager)

    public:
        /*! Default connection name. */
        static const char *defaultConnectionName;

        explicit DatabaseManager(
                const QString &defaultConnection = QLatin1String(defaultConnectionName));
        virtual ~DatabaseManager();

        /*! Factory method to create DatabaseManager instances. */
        static DatabaseManager create(
                const QVariantHash &config,
                const QString &connection = QLatin1String(defaultConnectionName),
                const QString &defaultConnection = QLatin1String(defaultConnectionName));

        /* Proxy methods to the DatabaseConnection. */
        /*! Begin a fluent query against a database table for the connection. */
        QSharedPointer<QueryBuilder>
        table(const QString &table, const QString &as = "", const QString &connection = "");

        /*! Get a new query builder instance for the connection. */
        QSharedPointer<QueryBuilder> query(const QString &connection = "");
        /*! Get a new QSqlQuery instance for the connection. */
        QSqlQuery qtQuery(const QString &connection = "");

        /* DatabaseManager. */
        /*! Obtain a database connection instance, for now it's singleton. */
//        static DatabaseManager *instance();

        /*! Get a database connection instance. */
        DatabaseConnection &connection(const QString &name = "") override;
        /*! Register a connection with the manager. */
        DatabaseManager &
        addConnection(const QVariantHash &config,
                      const QString &name = QLatin1String(defaultConnectionName));

        /*! Get all of the support drivers. */
        const QStringList supportedDrivers() const;

        /*! Get the default connection name. */
        const QString &getDefaultConnection() const override;
        /*! Set the default connection name. */
        void setDefaultConnection(const QString &defaultConnection) override;

    protected:
        explicit DatabaseManager(
                const QVariantHash &config,
                const QString &name = QLatin1String(defaultConnectionName),
                const QString &defaultConnection = QLatin1String(defaultConnectionName));

        /*! Parse the connection into an array of the name and read / write type. */
        const QString &parseConnectionName(const QString &name) const;

        /*! Make the database connection instance. */
        std::unique_ptr<DatabaseConnection>
        makeConnection(const QString &name);

        /*! Get the configuration for a connection. */
        QVariantHash &configuration(QString name);

        /*! Prepare the database connection instance. */
        std::unique_ptr<DatabaseConnection>
        configure(std::unique_ptr<DatabaseConnection> connection) const;

        /*! The database connection factory instance. */
        const Connectors::ConnectionFactory m_factory;
        /*! Database configuration. */
        Configuration m_config;
        /*! The active connection instances. */
        std::unordered_map<QString, std::unique_ptr<DatabaseConnection>> m_connections;
        /*! The callback to be executed to reconnect to a database. */
//        callable m_reconnector;

    private:
//        inline static DatabaseManager *m_instance = nullptr;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // DATABASEMANAGER_H
