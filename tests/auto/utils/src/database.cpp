#include "database.hpp"

#include "orm/db.hpp"
#include "orm/logicerror.hpp"

using Orm::LogicError;

namespace TestUtils
{

const QStringList &Database::createConnections(const QStringList &connections)
{
    // Ownership of a unique_ptr()
    /* The default connection is empty for tests, there is no default connection
       because it can produce hard to find bugs, I have to be explicit about
       the connection which will be used. */
    static const auto manager = DB::create(getConfigurations(connections), "");

    static const auto cachedConnectionNames = manager->connectionNames();

    return cachedConnectionNames;
}

const Database::ConfigurationsType &
Database::getConfigurations(const QStringList &connections)
{
    static auto configurations = ConfigurationsType {
        {"tinyorm_mysql_tests", {
            {"driver",    "QMYSQL"},
            {"host",      qEnvironmentVariable("DB_MYSQL_HOST", "127.0.0.1")},
            {"port",      qEnvironmentVariable("DB_MYSQL_PORT", "3306")},
            {"database",  qEnvironmentVariable("DB_MYSQL_DATABASE", "")},
            {"username",  qEnvironmentVariable("DB_MYSQL_USERNAME", "root")},
            {"password",  qEnvironmentVariable("DB_MYSQL_PASSWORD", "")},
            {"charset",   qEnvironmentVariable("DB_MYSQL_CHARSET", "utf8mb4")},
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
        }},

        {"tinyorm_sqlite_tests", {
            {"driver",    "QSQLITE"},
            {"database",  qEnvironmentVariable("DB_SQLITE_DATABASE",
                                               TINYORM_SQLITE_DATABASE)},
            {"foreign_key_constraints", qEnvironmentVariable("DB_SQLITE_FOREIGN_KEYS",
                                                             "true")},
            {"check_database_exists",   true},
        }},
    };

    // When connections variable is empty, then return all configurations
    if (connections.isEmpty())
        return configurations;

    // Throw if the connection is not defined
    for (const auto &connection : connections)
        if (!configurations.contains(connection))
            throw LogicError(
                    QStringLiteral("Connection '%1' doesn't exist.").arg(connection));

    // Remove unwanted connection configurations
    auto it = configurations.begin();
    while (it != configurations.end())
        if (!connections.contains(it.key()))
            it = configurations.erase(it);
        else
            ++it;

    return configurations;
}

} // namespace TestUtils
