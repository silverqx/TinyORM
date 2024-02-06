#include "orm/connectors/connectionfactory.hpp"

#include "orm/configurations/configurationparserfactory.hpp"
#include "orm/connectors/mysqlconnector.hpp"
#include "orm/connectors/postgresconnector.hpp"
#include "orm/connectors/sqliteconnector.hpp"
#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/mysqlconnection.hpp"
#include "orm/postgresconnection.hpp"
#include "orm/sqliteconnection.hpp"
#include "orm/utils/configuration.hpp"
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using ConfigUtils = Orm::Utils::Configuration;

namespace Orm::Connectors
{

/* public */

std::shared_ptr<DatabaseConnection>
ConnectionFactory::make(QVariantHash &config, const ConnectionName &connection)
{
    // Parse and prepare the database configuration
    auto configCopy = parseConfiguration(config, connection);

    return createSingleConnection(std::move(configCopy));
}

std::unique_ptr<ConnectorInterface>
ConnectionFactory::createConnector(const QVariantHash &config)
{
    // This method is public, so I left this check here
    if (!config.contains(driver_))
        throw Exceptions::InvalidArgumentError(
                QStringLiteral("A 'driver' configuration parameter must be specified "
                               "in %1().")
                .arg(__tiny_func__));

    const auto driver = config[driver_].value<QString>();

    if (driver == QMYSQL)
        return std::make_unique<MySqlConnector>();

    if (driver == QPSQL)
        return std::make_unique<PostgresConnector>();

    if (driver == QSQLITE)
        return std::make_unique<SQLiteConnector>();

//    if (driver == "SQLSRV")
//        return std::make_unique<SqlServerConnector>();

    throw Exceptions::InvalidArgumentError(
                QStringLiteral("Unsupported driver '%1' in %2().")
                .arg(driver, __tiny_func__));
}

/* protected */

QVariantHash
ConnectionFactory::parseConfiguration(QVariantHash &config,
                                      const ConnectionName &connection)
{
    // Look at the beginning of the configurationparser.cpp file how parsing works
    return Configurations::ConfigurationParserFactory::cachedParser(config)
            .parseConfiguration(config, connection);
}

std::shared_ptr<DatabaseConnection>
ConnectionFactory::createSingleConnection(QVariantHash &&config)
{
    // The config[return_qdatetime] is guaranteed to have a value for SQLite connection
    const auto returnQDateTime = config[driver_] == QSQLITE
                                 ? std::make_optional(config[return_qdatetime]
                                                      .value<bool>())
                                 : std::nullopt;

    return createConnection(
                config[driver_].value<QString>(), createQSqlDatabaseResolver(config),
                config[database_].value<QString>(), config[prefix_].value<QString>(),
                config[qt_timezone].value<QtTimeZoneConfig>(),
                std::move(config), returnQDateTime);
}

std::function<ConnectionName()>
ConnectionFactory::createQSqlDatabaseResolver(const QVariantHash &config)
{
    return config.contains(host_)
            ? createQSqlDatabaseResolverWithHosts(config)
            : createQSqlDatabaseResolverWithoutHosts(config);
}

std::function<ConnectionName()>
ConnectionFactory::createQSqlDatabaseResolverWithHosts(const QVariantHash &config)
{
    // Pass the config by value because it will be destroyed in the parseConfig()
    return [config = config]() mutable -> ConnectionName
    {
        const auto hosts = parseHosts(config);
        std::exception_ptr lastException;

        // FUTURE add support for multiple hosts and connect randomly to one of them, in this step have to take into account also the sticky config paramater silverqx
        /* This for statement do nothing for now, it purpose is to randomly
           shuffle hosts and try to connect to them one be one, until the connection
           will be successful. */
        for (const auto &host : hosts)
            try {
                config[host_] = host;

                return createConnector(config)->connect(config);

            } catch (const std::exception &/*unused*/) {
                // Save last exception to be able to re-throw
                lastException = std::current_exception();
                continue;
            }

        std::rethrow_exception(lastException);
    };
}

std::function<ConnectionName()>
ConnectionFactory::createQSqlDatabaseResolverWithoutHosts(
        const QVariantHash &config)
{
    // Pass the config by value because it will be destroyed in the parseConfig()
    return [config]() -> ConnectionName
    {
        return createConnector(config)->connect(config);
    };
}

std::shared_ptr<DatabaseConnection>
ConnectionFactory::createConnection(
        const QString &driver, std::function<ConnectionName()> &&connection,
        QString &&database, QString &&tablePrefix, QtTimeZoneConfig &&qtTimeZone,
        QVariantHash &&config, const std::optional<bool> returnQDateTime)
{
    if (driver == QMYSQL)
        return MySqlConnection::create(
                    std::move(connection), std::move(database), std::move(tablePrefix),
                    std::move(qtTimeZone), std::move(config));

    if (driver == QPSQL)
        return PostgresConnection::create(
                    std::move(connection), std::move(database), std::move(tablePrefix),
                    std::move(qtTimeZone), std::move(config));

    if (driver == QSQLITE)
        return SQLiteConnection::create(
                    std::move(connection), std::move(database), std::move(tablePrefix),
                    std::move(qtTimeZone), returnQDateTime,     std::move(config));

//    if (driver == "SQLSRV")
//        return SqlServerConnection::create(
//                    std::move(connection), std::move(database), std::move(tablePrefix),
//                    std::move(qtTimeZone), std::move(config));

    throw Exceptions::InvalidArgumentError(
                QStringLiteral("Unsupported driver '%1' in %2().")
                .arg(driver, __tiny_func__));
}

QStringList ConnectionFactory::parseHosts(const QVariantHash &config)
{
    if (!config.contains(host_))
        throw Exceptions::InvalidArgumentError(
                QStringLiteral("Database 'host' configuration parameter is required "
                               "in %1().")
                .arg(__tiny_func__));

    auto hosts = config[host_].value<QStringList>();

    validateHosts(hosts);

    return hosts;
}

void ConnectionFactory::validateHosts(const QStringList &hosts)
{
    for (const auto &host : hosts)
        if (host.isEmpty())
            throw Exceptions::InvalidArgumentError(
                    QStringLiteral(
                        "Database 'host' configuration parameter can not contain empty "
                        "value in %1().")
                    .arg(__tiny_func__));
}

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE
