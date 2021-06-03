#include "databases.hpp"

#include "orm/db.hpp"
#include "orm/logicerror.hpp"
#include "orm/runtimeerror.hpp"

using Orm::LogicError;
using Orm::RuntimeError;

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
        {"driver",    "QMYSQL"},
        {"host",      qEnvironmentVariable("DB_MYSQL_HOST",     "127.0.0.1")},
        {"port",      qEnvironmentVariable("DB_MYSQL_PORT",     "3306")},
        {"database",  qEnvironmentVariable("DB_MYSQL_DATABASE", "")},
        {"username",  qEnvironmentVariable("DB_MYSQL_USERNAME", "root")},
        {"password",  qEnvironmentVariable("DB_MYSQL_PASSWORD", "")},
        {"charset",   qEnvironmentVariable("DB_MYSQL_CHARSET",  "utf8mb4")},
        {"collation", qEnvironmentVariable("DB_MYSQL_COLLATION",
                                           "utf8mb4_0900_ai_ci")},
        // Very important for tests
        {"timezone",        "+00:00"},
        {"prefix",          ""},
        {"strict",          true},
        {"isolation_level", "REPEATABLE READ"},
        {"options",         QVariantHash()},
        // FUTURE remove, when unit tested silverqx
        // Example
//            {"options",   "MYSQL_OPT_CONNECT_TIMEOUT = 5 ; MYSQL_OPT_RECONNECT=1"},
//            {"options",   QVariantHash {{"MYSQL_OPT_RECONNECT", 1},
//                                        {"MYSQL_OPT_READ_TIMEOUT", 10}}},
    };

    // Environment variables was defined
    if (config.find("host").value().template value<QString>() != "127.0.0.1"
        && !config.find("database").value().template value<QString>().isEmpty()
        && config.find("username").value().template value<QString>() != "root"
        && config.find("password").value().template value<QString>() != ""
    )
        return {std::cref(config), true};

    return {std::cref(config), false};
}

std::pair<std::reference_wrapper<const QVariantHash>, bool>
Databases::sqliteConfiguration()
{
    static const QVariantHash config {
        {"driver",    "QSQLITE"},
        {"database",  qEnvironmentVariable("DB_SQLITE_DATABASE",
                                           TINYORM_SQLITE_DATABASE)},
        {"foreign_key_constraints", qEnvironmentVariable("DB_SQLITE_FOREIGN_KEYS",
                                                         "true")},
        {"check_database_exists",   true},
    };

    // Environment variables was defined
    if (!config.find("database").value().template value<QString>().isEmpty())
        return {std::cref(config), true};

    return {std::cref(config), false};
}

std::pair<std::reference_wrapper<const QVariantHash>, bool>
Databases::postgresConfiguration()
{
    static const QVariantHash config {
        {"driver",   "QPSQL"},
        {"host",     qEnvironmentVariable("DB_PGSQL_HOST",     "127.0.0.1")},
        {"port",     qEnvironmentVariable("DB_PGSQL_PORT",     "5432")},
        {"database", qEnvironmentVariable("DB_PGSQL_DATABASE", "")},
        {"schema",   qEnvironmentVariable("DB_PGSQL_SCHEMA",   "public")},
        {"username", qEnvironmentVariable("DB_PGSQL_USERNAME", "postgres")},
        {"password", qEnvironmentVariable("DB_PGSQL_PASSWORD", "")},
        {"charset",  qEnvironmentVariable("DB_PGSQL_CHARSET",  "utf8")},
        // I don't use timezone types in postgres anyway
        {"timezone", "UTC"},
        {"prefix",   ""},
        {"options",  QVariantHash()},
    };

    // Environment variables was defined
    if (config.find("host").value().template value<QString>() != "127.0.0.1"
        && !config.find("database").value().template value<QString>().isEmpty()
        && config.find("username").value().template value<QString>() != "postgres"
        && config.find("password").value().template value<QString>() != ""
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
