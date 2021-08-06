#pragma once
#ifndef CONNECTIONFACTORY_HPP
#define CONNECTIONFACTORY_HPP

#include <memory>

#include "orm/connectors/connectorinterface.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{
    class DatabaseConnection;

namespace Connectors
{

    /*! Database connection factory. */
    class ConnectionFactory
    {
    public:
        /*! Default constructor. */
        ConnectionFactory();

        /*! Establish a QSqlDatabase connection based on the configuration. */
        std::unique_ptr<DatabaseConnection>
        make(QVariantHash &config, const QString &name = "") const;

        /*! Create a connector instance based on the configuration. */
        std::unique_ptr<ConnectorInterface>
        createConnector(const QVariantHash &config) const;

    protected:
        /*! Parse and prepare the database configuration. */
        QVariantHash &
        parseConfig(QVariantHash &config, const QString &name) const;
        /*! Modify the driver name to uppercase. */
        void normalizeDriverName(QVariantHash &config) const;

        /*! Create a single database connection  instance. */
        std::unique_ptr<DatabaseConnection>
        createSingleConnection(QVariantHash &config) const;
        /*! Create a new Closure that resolves to a QSqlDatabase instance
            ( only a connection name returned ). */
        std::function<ConnectionName()>
        createQSqlDatabaseResolver(QVariantHash &config) const;

        /*! Create a new Closure that resolves to a QSqlDatabase instance ( only
            a connection name returned ) with a specific host or a vector of hosts. */
        std::function<ConnectionName()>
        createQSqlDatabaseResolverWithHosts(const QVariantHash &config) const;
        /*! Create a new Closure that resolves to a QSqlDatabase instance
            ( only a connection name returned ) where there is no configured host. */
        std::function<ConnectionName()>
        createQSqlDatabaseResolverWithoutHosts(const QVariantHash &config) const;

        /*! Create a new connection instance. */
        std::unique_ptr<DatabaseConnection>
        createConnection(const QString &driver,
                const std::function<ConnectionName()> &connection,
                const QString &database, const QString &prefix = "",
                const QVariantHash &config = {}) const;

        /*! Parse the hosts configuration item into the QStringList and validate hosts. */
        QStringList parseHosts(const QVariantHash &config) const;
        /*! Check if the hosts configuration item has right format. */
        void validateHosts(const QStringList &hosts) const;
    };

} // namespace Orm::Connectors
} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // CONNECTIONFACTORY_HPP
