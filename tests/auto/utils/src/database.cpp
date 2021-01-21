#include "database.hpp"

#include "orm/databasemanager.hpp"

using namespace TestUtils;

Database::Database()
{}

Orm::ConnectionInterface &Database::createConnection()
{
    static Orm::DatabaseManager databaseManager;

    static const QVariantHash config {
        {"driver",    "QMYSQL"},
        {"host",      qEnvironmentVariable("DB_HOST", "127.0.0.1")},
        {"port",      qEnvironmentVariable("DB_PORT", "3306")},
        {"database",  qEnvironmentVariable("DB_DATABASE", "")},
        {"username",  qEnvironmentVariable("DB_USERNAME", "root")},
        {"password",  qEnvironmentVariable("DB_PASSWORD", "")},
        {"charset",   qEnvironmentVariable("DB_CHARSET", "utf8mb4")},
        {"collation", qEnvironmentVariable("DB_COLLATION", "utf8mb4_unicode_ci")},
        {"prefix",    ""},
        {"strict",    true},
        {"options",   QVariantHash()},
        // TODO future remove, when unit tested silverqx
        // Example
//        {"options",   "MYSQL_OPT_CONNECT_TIMEOUT = 5 ; MYSQL_OPT_RECONNECT=1"},
//        {"options",   QVariantHash {{"MYSQL_OPT_RECONNECT", 1},
//                                    {"MYSQL_OPT_READ_TIMEOUT", 10}}},
    };

    static const auto &connectionName = QStringLiteral("tinyorm_mysql_tests");

    return databaseManager
            .addConnection(config, connectionName)
            .connection(connectionName);
}
