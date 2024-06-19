#pragma once
#ifndef ORM_CONNCECTORS_CONNECTIONFACTORY_HPP
#define ORM_CONNCECTORS_CONNECTIONFACTORY_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <memory>
#include <optional>

#include "orm/connectors/connectorinterface.hpp"
#include "orm/ormtypes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
    class DatabaseConnection;

namespace Connectors
{

    /*! Database connection factory. */
    class ConnectionFactory
    {
        Q_DISABLE_COPY_MOVE(ConnectionFactory)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        ConnectionFactory() = delete;
        /*! Deleted destructor. */
        ~ConnectionFactory() = delete;

        /*! Establish a QSqlDatabase connection based on the configuration. */
        static std::shared_ptr<DatabaseConnection>
        make(QVariantHash &config, const ConnectionName &connection);

        /*! Create a connector instance based on the configuration. */
        static std::unique_ptr<ConnectorInterface>
        createConnector(const QVariantHash &config);

    protected:
        /*! Parse and prepare the database configuration. */
        static QVariantHash
        parseConfiguration(QVariantHash &config, const ConnectionName &connection);

        /*! Create a single database connection  instance. */
        static std::shared_ptr<DatabaseConnection>
        createSingleConnection(QVariantHash &&config);
        /*! Create a new Closure that resolves to a QSqlDatabase instance
            ( only a connection name returned ). */
        static std::function<ConnectionName()>
        createQSqlDatabaseResolver(const QVariantHash &config);

        /*! Create a new Closure that resolves to a QSqlDatabase instance ( only
            a connection name returned ) with a specific host or a vector of hosts. */
        static std::function<ConnectionName()>
        createQSqlDatabaseResolverWithHosts(const QVariantHash &config);
        /*! Create a new Closure that resolves to a QSqlDatabase instance
            ( only a connection name returned ) where there is no configured host. */
        static std::function<ConnectionName()>
        createQSqlDatabaseResolverWithoutHosts(const QVariantHash &config);

        /*! Create a new connection instance. */
        static std::shared_ptr<DatabaseConnection>
        createConnection(
                const QString &driver, std::function<ConnectionName()> &&connection,
                QString &&database, QString &&tablePrefix = "",
                QtTimeZoneConfig &&qtTimeZone = QtTimeZoneConfig::utc(),
                QVariantHash &&config = {},
                std::optional<bool> returnQDateTime = std::nullopt);

        /*! Parse the hosts configuration item into the QStringList and validate hosts. */
        static QStringList parseHosts(const QVariantHash &config);
        /*! Check if the hosts configuration item has right format. */
        static void validateHosts(const QStringList &hosts);
    };

} // namespace Connectors
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONNCECTORS_CONNECTIONFACTORY_HPP
