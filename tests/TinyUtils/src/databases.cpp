#include "databases.hpp"

#include "orm/constants.hpp"
#include "orm/db.hpp"
#include "orm/exceptions/logicerror.hpp"
#include "orm/exceptions/runtimeerror.hpp"

using namespace Orm::Constants;

using Orm::Exceptions::LogicError;
using Orm::Exceptions::RuntimeError;

namespace TestUtils
{

/* The whole class is designed so that a Database::createConnections/createConnection
   methods can be called only once. You can pass connection name/s to these methods
   and they create TinyORM database connections.
   Only those connections will be created, for which are environment variables defined
   correctly.
   Tests don't fail but are skipped when a connection is not available. */

const QStringList &Databases::createConnections(const QStringList &connections)
{
    checkInitialized();

    // Ownership of a unique_ptr()
    /* The default connection is empty for tests, there is no default connection
       because it can produce hard to find bugs, I have to be explicit about
       the connection which will be used. */
    static const auto manager = DB::create(getConfigurations(connections), "");

    static const auto cachedConnectionNames = manager->connectionNames();

    return cachedConnectionNames;
}

QString Databases::createConnection(const QString &connection)
{
    const auto &connections = createConnections({connection});

    if (connections.size() > 1)
        throw RuntimeError("Returned more than one connection in "
                           "Databases::createConnection() method.");

    if (!connections.isEmpty())
        return connections.first();

    return {};
}

const Databases::ConfigurationsType &
Databases::getConfigurations(const QStringList &connections)
{
    static auto configurations = createConfigurationsHash(connections);

    // When connections variable is empty, then return all configurations
    if (connections.isEmpty())
        return configurations;

    return configurations;
}

const Databases::ConfigurationsType &
Databases::createConfigurationsHash(const QStringList &connections)
{
    static ConfigurationsType configurations;

    const auto shouldCreateConnection = [&connections](const auto &connection)
    {
        return connections.isEmpty() || connections.contains(connection);
    };

    if (shouldCreateConnection(MYSQL))
        if (auto [config, envDefined] = mysqlConfiguration(); envDefined)
            configurations[MYSQL] = std::move(config);

    if (shouldCreateConnection(SQLITE))
        if (auto [config, envDefined] = sqliteConfiguration(); envDefined)
            configurations[SQLITE] = std::move(config);

    if (shouldCreateConnection(POSTGRESQL))
        if (auto [config, envDefined] = postgresConfiguration(); envDefined)
            configurations[POSTGRESQL] = std::move(config);

    return configurations;
}

std::pair<std::reference_wrapper<const QVariantHash>, bool>
Databases::mysqlConfiguration()
{
    static const QVariantHash config {
        {driver_,    QMYSQL},
        {host_,      qEnvironmentVariable("DB_MYSQL_HOST",     H127001)},
        {port_,      qEnvironmentVariable("DB_MYSQL_PORT",     P3306)},
        {database_,  qEnvironmentVariable("DB_MYSQL_DATABASE", "")},
        {username_,  qEnvironmentVariable("DB_MYSQL_USERNAME", ROOT)},
        {password_,  qEnvironmentVariable("DB_MYSQL_PASSWORD", "")},
        {charset_,   qEnvironmentVariable("DB_MYSQL_CHARSET",  UTF8MB4)},
        {collation_, qEnvironmentVariable("DB_MYSQL_COLLATION",
                                          QStringLiteral("utf8mb4_0900_ai_ci"))},
        // Very important for tests
        {timezone_,       "+00:00"},
        {prefix_,         ""},
        {strict_,         true},
        {isolation_level, "REPEATABLE READ"},
        {options_,        QVariantHash()},
        // FUTURE remove, when unit tested silverqx
        // Example
//        {options_, "MYSQL_OPT_CONNECT_TIMEOUT = 5 ; MYSQL_OPT_RECONNECT=1"},
//        {options_, QVariantHash {{"MYSQL_OPT_RECONNECT", 1},
//                                 {"MYSQL_OPT_READ_TIMEOUT", 10}}},
    };

    // Environment variables was defined
    if (config.find(host_).value().template value<QString>() != H127001
        && !config.find(database_).value().template value<QString>().isEmpty()
        && config.find(username_).value().template value<QString>() != ROOT
        && config.find(password_).value().template value<QString>() != ""
    )
        return {std::cref(config), true};

    return {std::cref(config), false};
}

std::pair<std::reference_wrapper<const QVariantHash>, bool>
Databases::sqliteConfiguration()
{
    static const QVariantHash config {
        {driver_,    QSQLITE},
        {database_,  qEnvironmentVariable("DB_SQLITE_DATABASE",
                                          TINYORM_SQLITE_DATABASE)},
        {foreign_key_constraints, qEnvironmentVariable("DB_SQLITE_FOREIGN_KEYS",
                                                       QStringLiteral("true"))},
        {check_database_exists,   true},
    };

    // Environment variables was defined
    if (!config.find(database_).value().template value<QString>().isEmpty())
        return {std::cref(config), true};

    return {std::cref(config), false};
}

std::pair<std::reference_wrapper<const QVariantHash>, bool>
Databases::postgresConfiguration()
{
    static const QVariantHash config {
        {driver_,   QPSQL},
        {host_,     qEnvironmentVariable("DB_PGSQL_HOST",     H127001)},
        {port_,     qEnvironmentVariable("DB_PGSQL_PORT",     P5432)},
        {database_, qEnvironmentVariable("DB_PGSQL_DATABASE", "")},
        {schema_,   qEnvironmentVariable("DB_PGSQL_SCHEMA",   PUBLIC)},
        {username_, qEnvironmentVariable("DB_PGSQL_USERNAME",
                                         QStringLiteral("postgres"))},
        {password_, qEnvironmentVariable("DB_PGSQL_PASSWORD", "")},
        {charset_,  qEnvironmentVariable("DB_PGSQL_CHARSET",  UTF8)},
        // I don't use timezone types in postgres anyway
        {timezone_, UTC},
        {prefix_,   ""},
        {options_,  QVariantHash()},
    };

    // Environment variables was defined
    if (config.find(host_).value().template value<QString>() != H127001
        && !config.find(database_).value().template value<QString>().isEmpty()
        && config.find(username_)
                 .value().template value<QString>() != QStringLiteral("postgres")
        && config.find(password_).value().template value<QString>() != ""
    )
        return {std::cref(config), true};

    return {std::cref(config), false};
}

void Databases::checkInitialized()
{
    if (m_initialized)
        throw LogicError("Databases::createConnections/createConnection methods "
                         "can be called only once.");

    m_initialized = true;
}

} // namespace TestUtils
