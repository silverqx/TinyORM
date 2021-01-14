#include "orm/connectors/connectionfactory.hpp"

#include "orm/connectors/mysqlconnector.hpp"
#include "orm/mysqlconnection.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Connectors
{

ConnectionFactory::ConnectionFactory()
{}

std::unique_ptr<DatabaseConnection>
ConnectionFactory::make(QVariantHash &config, const QString &name) const
{
    parseConfig(config, name);

    return createSingleConnection(config);
}

// TODO now std::unique_ptr<Connector> vs std::unique_ptr<ConnectorInterface> silverqx
std::unique_ptr<ConnectorInterface>
ConnectionFactory::createConnector(const QVariantHash &config) const
{
    // This method is public, so I left this check here
    if (!config.contains("driver"))
        throw std::invalid_argument("A 'driver' configuration parameter must be specified.");

    const auto driver = config["driver"].toString();

    if (driver == "QMYSQL")
        return std::make_unique<MySqlConnector>();
//    else if (driver == "QPSQL")
//        return std::make_unique<PostgresConnector>();
//    else if (driver == "QSQLITE")
//        return std::make_unique<SQLiteConnector>();
//    else if (driver == "SQLSRV")
//        return std::make_unique<SqlServerConnector>();
    else
        // TODO now check all exception, if they have correct type silverqx
        throw std::invalid_argument(
                "Unsupported driver '" + driver.toStdString() + "'.");
}

QVariantHash &
ConnectionFactory::parseConfig(QVariantHash &config, const QString &name) const
{
    // Insert/normalize needed configuration values
    config.insert("name", name);

    normalizeDriverName(config);

    if (!config.contains("database"))
        config.insert("database", QStringLiteral(""));

    if (!config.contains("prefix"))
        config.insert("prefix", QStringLiteral(""));

    if (!config.contains("options"))
        config.insert("options", QVariantHash());

    return config;
}

void ConnectionFactory::normalizeDriverName(QVariantHash &config) const
{
    if (!config.contains("driver"))
        config.insert("driver", QStringLiteral(""));

    else {
        auto &driver = config["driver"];

        driver = driver.toString().toUpper();
    }
}

std::unique_ptr<DatabaseConnection>
ConnectionFactory::createSingleConnection(QVariantHash &config) const
{
    const auto qtConnectionResolver = createQSqlDatabaseResolver(config);

    // TODO now decide how to handle undefined driver or database silverqx
    return createConnection(
                config["driver"].toString(), qtConnectionResolver,
                config["database"].toString(), config["prefix"].toString(),
                config);
}

std::function<ConnectionName()>
ConnectionFactory::createQSqlDatabaseResolver(QVariantHash &config) const
{
    return config.contains("host")
            ? createQSqlDatabaseResolverWithHosts(config)
            : createQSqlDatabaseResolverWithoutHosts(config);
}

// TODO now const silverqx
std::function<ConnectionName()>
ConnectionFactory::createQSqlDatabaseResolverWithHosts(const QVariantHash &config) const
{
    return [this, &config]() -> ConnectionName
    {
        // Do not overwrite original config
        auto configCopy = config;

        const auto hosts = parseHosts(configCopy);
        std::exception lastException;

        // TODO future add support for multiple hosts and connect randomly to one of them, in this step have to take into account also the sticky config paramater silverqx
        // TODO docs silverqx
        for (const auto &host : hosts)
            try {
                configCopy["host"] = host;

                return createConnector(configCopy)->connect(configCopy);

            }  catch (const std::exception &e) {
                // Save last exception to be able to re-throw
                lastException = e;
                continue;
            }

        throw lastException;
    };
}

std::function<ConnectionName()>
ConnectionFactory::createQSqlDatabaseResolverWithoutHosts(const QVariantHash &) const
{
    throw std::domain_error(
                "createQSqlDatabaseResolverWithoutHosts() not yet implemented.");

//    return function () use ($config) {
//        return $this->createConnector($config)->connect($config);
//    };
}

std::unique_ptr<DatabaseConnection>
ConnectionFactory::createConnection(
        const QString &driver,
        const std::function<ConnectionName()> &connection,
        const QString &database, const QString &prefix,
        const QVariantHash &config) const
{
    if (driver == "QMYSQL")
        return std::make_unique<MySqlConnection>(connection, database, prefix, config);
//    else if (driver == "QPSQL")
//        return std::make_unique<PostgresConnection>(connection, database, prefix, config);
//    else if (driver == "QSQLITE")
//        return std::make_unique<SQLiteConnection>(connection, database, prefix, config);
//    else if (driver == "SQLSRV")
//        return std::make_unique<SqlServerConnection>(connection, database, prefix, config);
    else
        throw std::invalid_argument(
                "Unsupported driver '" + driver.toStdString() + "'.");
}

QStringList ConnectionFactory::parseHosts(const QVariantHash &config) const
{
    if (!config.contains("host"))
        throw std::invalid_argument("Database 'host' configuration parameter is required.");

    const auto hosts = config["host"].value<QStringList>();

    validateHosts(hosts);

    return hosts;
}

void ConnectionFactory::validateHosts(const QStringList &hosts) const
{
    for (const auto &host : hosts)
        if (host.isEmpty())
            throw std::invalid_argument("Database 'host' configuration parameter "
                                        "can not contain empty value.");
}

} // namespace Orm::Connectors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
