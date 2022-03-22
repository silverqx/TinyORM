#include "orm/connectors/connectionfactory.hpp"

#include "orm/connectors/mysqlconnector.hpp"
#include "orm/connectors/postgresconnector.hpp"
#include "orm/connectors/sqliteconnector.hpp"
#include "orm/mysqlconnection.hpp"
#include "orm/postgresconnection.hpp"
#include "orm/sqliteconnection.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Connectors
{

std::unique_ptr<DatabaseConnection>
ConnectionFactory::make(QVariantHash &config, const QString &name) const
{
    parseConfig(config, name);

    return createSingleConnection(config);
}

std::unique_ptr<ConnectorInterface>
ConnectionFactory::createConnector(const QVariantHash &config) const
{
    // This method is public, so I left this check here
    if (!config.contains(driver_))
        throw Exceptions::RuntimeError(
                "A 'driver' configuration parameter must be specified.");

    const auto driver = config[driver_].value<QString>();

    if (driver == QMYSQL)
        return std::make_unique<MySqlConnector>();

    if (driver == QPSQL)
        return std::make_unique<PostgresConnector>();

    if (driver == QSQLITE)
        return std::make_unique<SQLiteConnector>();

//    if (driver == "SQLSRV")
//        return std::make_unique<SqlServerConnector>();

    throw Exceptions::RuntimeError(QStringLiteral("Unsupported driver '%1'.")
                                   .arg(driver));
}

QVariantHash &
ConnectionFactory::parseConfig(QVariantHash &config, const QString &name) const
{
    // Insert/normalize needed configuration values
    config.insert(NAME, name);

    normalizeDriverName(config);

    if (!config.contains(database_))
        config.insert(database_, QString(""));

    if (!config.contains(prefix_))
        config.insert(prefix_, QString(""));

    if (!config.contains(options_))
        config.insert(options_, QVariantHash());

    if (!config.contains(prefix_indexes))
        config.insert(prefix_indexes, false);

    // FUTURE connector, this can be enhanced, eg. add default values per driver, eg. engine_ for mysql is missing, can not be added because is driver specific silverqx

    return config;
}

void ConnectionFactory::normalizeDriverName(QVariantHash &config) const
{
    if (!config.contains(driver_))
        config.insert(driver_, QString(""));

    else {
        auto &driver = config[driver_];

        driver = driver.value<QString>().toUpper();
    }
}

std::unique_ptr<DatabaseConnection>
ConnectionFactory::createSingleConnection(QVariantHash &config) const
{
    return createConnection(
                config[driver_].value<QString>(), createQSqlDatabaseResolver(config),
                config[database_].value<QString>(), config[prefix_].value<QString>(),
                config);
}

std::function<ConnectionName()>
ConnectionFactory::createQSqlDatabaseResolver(QVariantHash &config) const
{
    return config.contains(host_)
            ? createQSqlDatabaseResolverWithHosts(config)
            : createQSqlDatabaseResolverWithoutHosts(config);
}

std::function<ConnectionName()>
ConnectionFactory::createQSqlDatabaseResolverWithHosts(const QVariantHash &config) const
{
    return [this, &config]() -> ConnectionName
    {
        // Do not overwrite original config
        auto configCopy = config;

        const auto hosts = parseHosts(configCopy);
        std::exception_ptr lastException;

        // FUTURE add support for multiple hosts and connect randomly to one of them, in this step have to take into account also the sticky config paramater silverqx
        /* This for statement do nothing for now, it purpose is to randomly
           shuffle hosts and try to connect to them one be one, until the connection
           will be successful. */
        for (const auto &host : hosts)
            try {
                configCopy[host_] = host;

                return createConnector(configCopy)->connect(configCopy);

            }  catch (const std::exception &) {
                // Save last exception to be able to re-throw
                lastException = std::current_exception();
                continue;
            }

        std::rethrow_exception(lastException);
    };
}

std::function<ConnectionName()>
ConnectionFactory::createQSqlDatabaseResolverWithoutHosts(
        const QVariantHash &config) const
{
    return [this, &config]() -> ConnectionName
    {
        return createConnector(config)->connect(config);
    };
}

std::unique_ptr<DatabaseConnection>
ConnectionFactory::createConnection(
        const QString &driver,
        std::function<ConnectionName()> &&connection,
        const QString &database, const QString &prefix,
        const QVariantHash &config) const
{
    if (driver == QMYSQL)
        return std::make_unique<MySqlConnection>(std::move(connection), database, prefix,
                                                 config);
    if (driver == QPSQL)
        return std::make_unique<PostgresConnection>(std::move(connection), database,
                                                    prefix, config);
    if (driver == QSQLITE)
        return std::make_unique<SQLiteConnection>(std::move(connection), database,
                                                  prefix, config);
//    if (driver == "SQLSRV")
//        return std::make_unique<SqlServerConnection>(std::move(connection), database,
//                                                     prefix, config);

    throw Exceptions::RuntimeError(QStringLiteral("Unsupported driver '%1'.")
                                   .arg(driver));
}

QStringList ConnectionFactory::parseHosts(const QVariantHash &config) const
{
    if (!config.contains(host_))
        throw Exceptions::RuntimeError(
                "Database 'host' configuration parameter is required.");

    auto hosts = config[host_].value<QStringList>();

    validateHosts(hosts);

    return hosts;
}

void ConnectionFactory::validateHosts(const QStringList &hosts) const
{
    for (const auto &host : hosts)
        if (host.isEmpty())
            throw Exceptions::RuntimeError(
                    "Database 'host' configuration parameter can not contain empty "
                    "value.");
}

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE
