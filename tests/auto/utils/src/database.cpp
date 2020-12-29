#include "database.hpp"

#include "orm/databaseconnection.hpp"

using namespace Utils;

Database::Database()
{}

Orm::DatabaseConnection &Database::createConnection()
{
    const QHash<QString, QVariant> config {
        {"host",      qEnvironmentVariable("DB_HOST", "127.0.0.1")},
        {"port",      qEnvironmentVariable("DB_PORT", "3306")},
        {"database",  qEnvironmentVariable("DB_DATABASE", "")},
        {"username",  qEnvironmentVariable("DB_USERNAME", "root")},
        {"password",  qEnvironmentVariable("DB_PASSWORD", "")},
        {"charset",   qEnvironmentVariable("DB_CHARSET", "utf8mb4")},
        {"collation", qEnvironmentVariable("DB_COLLATION", "utf8mb4_unicode_ci")},
        {"prefix",    ""},
        {"strict",    true},
        {"options",   ""},
    };

    return Orm::DatabaseConnection::create(config.find("database").value().toString(),
                                           config.find("prefix").value().toString(),
                                           config);
}
