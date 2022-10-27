#pragma once
#ifndef ORM_CONNCECTORS_CONNECTIONFACTORY_HPP
#define ORM_CONNCECTORS_CONNECTIONFACTORY_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <memory>

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
        Q_DISABLE_COPY(ConnectionFactory)

    public:
        /*! Default constructor. */
        inline ConnectionFactory() = default;
        /*! Default destructor. */
        inline ~ConnectionFactory() = default;

        /*! Establish a QSqlDatabase connection based on the configuration. */
        std::unique_ptr<DatabaseConnection>
        make(QVariantHash &config, const QString &connection) const;

        /*! Create a connector instance based on the configuration. */
        static std::unique_ptr<ConnectorInterface>
        createConnector(const QVariantHash &config);

    protected:
        /*! Parse and prepare the database configuration. */
        QVariantHash
        parseConfig(QVariantHash &config, const QString &connection) const;

        /*! Modify the driver name to uppercase. */
        static void normalizeDriverName(QVariantHash &config);
        /*! Parse the qt_timezone configuration option. */
        static void parseQtTimeZone(QVariantHash &config, const QString &connection);

        /*! Create a single database connection  instance. */
        std::unique_ptr<DatabaseConnection>
        createSingleConnection(QVariantHash &&config) const;
        /*! Create a new Closure that resolves to a QSqlDatabase instance
            ( only a connection name returned ). */
        std::function<ConnectionName()>
        createQSqlDatabaseResolver(const QVariantHash &config) const;

        /*! Create a new Closure that resolves to a QSqlDatabase instance ( only
            a connection name returned ) with a specific host or a vector of hosts. */
        std::function<ConnectionName()>
        createQSqlDatabaseResolverWithHosts(const QVariantHash &config) const;
        /*! Create a new Closure that resolves to a QSqlDatabase instance
            ( only a connection name returned ) where there is no configured host. */
        std::function<ConnectionName()>
        createQSqlDatabaseResolverWithoutHosts(const QVariantHash &config) const;

        /*! Create a new connection instance. */
        static std::unique_ptr<DatabaseConnection>
        createConnection(
                QString &&driver, std::function<ConnectionName()> &&connection,
                QString &&database, QString &&tablePrefix = "",
                QtTimeZoneConfig &&qtTimeZone = {QtTimeZoneType::DontConvert},
                QVariantHash &&config = {},
                std::optional<bool> &&returnQDateTime = std::nullopt);

        /*! Parse the hosts configuration item into the QStringList and validate hosts. */
        QStringList parseHosts(const QVariantHash &config) const;
        /*! Check if the hosts configuration item has right format. */
        static void validateHosts(const QStringList &hosts);
    };

} // namespace Connectors
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONNCECTORS_CONNECTIONFACTORY_HPP
