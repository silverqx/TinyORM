#include "orm/connectors/connectionfactory.hpp"

#include "orm/connectors/mysqlconnector.hpp"
#include "orm/connectors/sqliteconnector.hpp"
#include "orm/mysqlconnection.hpp"
#include "orm/sqliteconnection.hpp"

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

std::unique_ptr<ConnectorInterface>
ConnectionFactory::createConnector(const QVariantHash &config) const
{
    // This method is public, so I left this check here
    if (!config.contains("driver"))
        throw std::domain_error("A 'driver' configuration parameter must be specified.");

    const auto driver = config["driver"].value<QString>();

    if (driver == "QMYSQL")
        return std::make_unique<MySqlConnector>();
//    else if (driver == "QPSQL")
//        return std::make_unique<PostgresConnector>();
    else if (driver == "QSQLITE")
        return std::make_unique<SQLiteConnector>();
//    else if (driver == "SQLSRV")
//        return std::make_unique<SqlServerConnector>();
    else
        throw std::domain_error(
                "Unsupported driver '" + driver.toStdString() + "'.");
}

QVariantHash &
ConnectionFactory::parseConfig(QVariantHash &config, const QString &name) const
{
    // Insert/normalize needed configuration values
    config.insert("name", name);

    normalizeDriverName(config);

    if (!config.contains("database"))
        config.insert("database", QString(""));

    if (!config.contains("prefix"))
        config.insert("prefix", QString(""));

    if (!config.contains("options"))
        config.insert("options", QVariantHash());

    return config;
}

void ConnectionFactory::normalizeDriverName(QVariantHash &config) const
{
    if (!config.contains("driver"))
        config.insert("driver", QString(""));

    else {
        auto &driver = config["driver"];

        driver = driver.value<QString>().toUpper();
    }
}

std::unique_ptr<DatabaseConnection>
ConnectionFactory::createSingleConnection(QVariantHash &config) const
{
    return createConnection(
                config["driver"].value<QString>(), createQSqlDatabaseResolver(config),
                config["database"].value<QString>(), config["prefix"].value<QString>(),
                config);
}

std::function<ConnectionName()>
ConnectionFactory::createQSqlDatabaseResolver(QVariantHash &config) const
{
    return config.contains("host")
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
                configCopy["host"] = host;

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
        const std::function<ConnectionName()> &connection,
        const QString &database, const QString &prefix,
        const QVariantHash &config) const
{
    if (driver == "QMYSQL")
        return std::make_unique<MySqlConnection>(connection, database, prefix, config);
//    else if (driver == "QPSQL")
//        return std::make_unique<PostgresConnection>(connection, database, prefix, config);
    else if (driver == "QSQLITE")
        return std::make_unique<SQLiteConnection>(connection, database, prefix, config);
//    else if (driver == "SQLSRV")
//        return std::make_unique<SqlServerConnection>(connection, database, prefix, config);
    else
        throw std::domain_error(
                "Unsupported driver '" + driver.toStdString() + "'.");
}

QStringList ConnectionFactory::parseHosts(const QVariantHash &config) const
{
    if (!config.contains("host"))
        // TODO now unify exception, std::domain_error is for user code, create own exceptions and use InvalidArgumentError, or runtime/logic error silverqx
        throw std::domain_error("Database 'host' configuration parameter is required.");

    const auto hosts = config["host"].value<QStringList>();

    validateHosts(hosts);

    return hosts;
}

void ConnectionFactory::validateHosts(const QStringList &hosts) const
{
    for (const auto &host : hosts)
        if (host.isEmpty())
            throw std::domain_error("Database 'host' configuration parameter "
                                        "can not contain empty value.");
}

} // namespace Orm::Connectors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
