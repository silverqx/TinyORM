#include "orm/connectors/connectionfactory.hpp"

#include "orm/connectors/mysqlconnector.hpp"
#include "orm/connectors/postgresconnector.hpp"
#include "orm/connectors/sqliteconnector.hpp"
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

std::unique_ptr<DatabaseConnection>
ConnectionFactory::make(QVariantHash &config, const QString &connection) const
{
    auto configCopy = parseConfig(config, connection);

    return createSingleConnection(std::move(configCopy));
}

std::unique_ptr<ConnectorInterface>
ConnectionFactory::createConnector(const QVariantHash &config)
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

    throw Exceptions::RuntimeError(QStringLiteral("Unsupported driver '%1' in %2().")
                                   .arg(driver, __tiny_func__));
}

/* protected */

QVariantHash
ConnectionFactory::parseConfig(QVariantHash &config, const QString &connection) const
{
    /* Insert/normalize needed configuration values, following inserted values will be
       also changed in the so called the original configuration that will be also saved
       in the DatabaseManager. */
    config.insert(NAME, connection);

    normalizeDriverName(config);

    if (!config.contains(database_))
        config.insert(database_, EMPTY);

    if (!config.contains(prefix_))
        config.insert(prefix_, EMPTY);

    if (!config.contains(options_))
        config.insert(options_, QVariantHash());

    if (!config.contains(prefix_indexes))
        config.insert(prefix_indexes, false);

    // FUTURE connector, this can be enhanced, eg. add default values per driver, eg. engine_ for mysql is missing, can not be added because is driver specific silverqx
    if (config[driver_] == QPSQL && !config.contains(dont_drop))
        // spatial_ref_sys table is used by the PostGIS
        config.insert(dont_drop, QStringList {QStringLiteral("spatial_ref_sys")});

    if (config[driver_] == QMYSQL && !config.contains(Version))
        config.insert(Version, {});

    if (config[driver_] == QSQLITE && !config.contains(return_qdatetime))
        config.insert(return_qdatetime, true);

    /* Changes after this point will be saved only in the DatabaseConnection
       configuration, the original DatabaseManager configuration will be untouched. */
    auto configCopy = config;

    // Parse the qt_timezone configuration option
    parseQtTimeZone(configCopy, connection);

    return configCopy;
}

void ConnectionFactory::normalizeDriverName(QVariantHash &config)
{
    if (!config.contains(driver_))
        config.insert(driver_, EMPTY);

    else {
        auto &driver = config[driver_];

        driver = driver.value<QString>().toUpper();
    }
}

void ConnectionFactory::parseQtTimeZone(QVariantHash &config, const QString &connection)
{
    auto &qtTimeZone = config[qt_timezone];

    // Nothing to parse, already contains the QtTimeZoneConfig
    if (qtTimeZone.canConvert<QtTimeZoneConfig>())
        return;

    qtTimeZone = QVariant::fromValue(
                     ConfigUtils::prepareQtTimeZone(config, connection));
}

std::unique_ptr<DatabaseConnection>
ConnectionFactory::createSingleConnection(QVariantHash &&config) const
{
    // The config[return_qdatetime] is guaranteed to have a value for SQLite connection
    auto returnQDateTime = config[driver_] == QSQLITE
                           ? std::make_optional(config[return_qdatetime].value<bool>())
                           : std::nullopt;

    return createConnection(
                config[driver_].value<QString>(), createQSqlDatabaseResolver(config),
                config[database_].value<QString>(), config[prefix_].value<QString>(),
                config[qt_timezone].value<QtTimeZoneConfig>(),
                std::move(config), std::move(returnQDateTime));
}

std::function<ConnectionName()>
ConnectionFactory::createQSqlDatabaseResolver(const QVariantHash &config) const
{
    return config.contains(host_)
            ? createQSqlDatabaseResolverWithHosts(config)
            : createQSqlDatabaseResolverWithoutHosts(config);
}

std::function<ConnectionName()>
ConnectionFactory::createQSqlDatabaseResolverWithHosts(const QVariantHash &config) const
{
    // Pass the config by value because it will be destroyed in the parseConfig()
    return [this, config = config]() mutable -> ConnectionName
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
    // Pass the config by value because it will be destroyed in the parseConfig()
    return [config]() -> ConnectionName
    {
        return createConnector(config)->connect(config);
    };
}

std::unique_ptr<DatabaseConnection>
ConnectionFactory::createConnection(
        QString &&driver, std::function<ConnectionName()> &&connection,
        QString &&database, QString &&tablePrefix, QtTimeZoneConfig &&qtTimeZone,
        QVariantHash &&config, std::optional<bool> &&returnQDateTime)
{
    if (driver == QMYSQL)
        return std::make_unique<MySqlConnection>(
                    std::move(connection), std::move(database), std::move(tablePrefix),
                    std::move(qtTimeZone), std::move(config));

    if (driver == QPSQL)
        return std::make_unique<PostgresConnection>(
                    std::move(connection), std::move(database), std::move(tablePrefix),
                    std::move(qtTimeZone), std::move(config));

    if (driver == QSQLITE)
        return std::make_unique<SQLiteConnection>(
                    std::move(connection), std::move(database), std::move(tablePrefix),
                    std::move(qtTimeZone), std::move(returnQDateTime),
                    std::move(config));

//    if (driver == "SQLSRV")
//        return std::make_unique<SqlServerConnection>(
//                    std::move(connection), std::move(database), std::move(tablePrefix),
//                    std::move(qtTimeZone), std::move(config));

    throw Exceptions::RuntimeError(QStringLiteral("Unsupported driver '%1' in %2().")
                                   .arg(std::move(driver), __tiny_func__));
}

QStringList ConnectionFactory::parseHosts(const QVariantHash &config) const
{
    if (!config.contains(host_))
        throw Exceptions::RuntimeError(
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
            throw Exceptions::RuntimeError(
                    QStringLiteral(
                        "Database 'host' configuration parameter can not contain empty "
                        "value in %1().")
                    .arg(__tiny_func__));
}

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE
