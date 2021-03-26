#include "database.hpp"

#include "orm/db.hpp"

namespace TestUtils
{

const QStringList &Database::createConnections()
{
    // Ownership of a unique_ptr()
    static const auto manager = DB::create({
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
            {"timezone",  "+00:00"},
            {"prefix",    ""},
            {"strict",    true},
            {"options",   QVariantHash()},
            // TODO future remove, when unit tested silverqx
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
    },
        /* The default connection is empty for tests, there is no default connection
           because it can produce hard to find bugs, I have to be explicit about
           the connection which will be used. */
        "");

    static const auto cachedConnectionNames = manager->connectionNames();

    return cachedConnectionNames;
}

} // namespace TestUtils
