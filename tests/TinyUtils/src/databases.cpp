#include "databases.hpp"

#include "orm/constants.hpp"
#include "orm/db.hpp"
#include "orm/exceptions/runtimeerror.hpp"
#include "orm/utils/type.hpp"

using Orm::Constants::EMPTY;
using Orm::Constants::H127001;
using Orm::Constants::InnoDB;
using Orm::Constants::P3306;
using Orm::Constants::P5432;
using Orm::Constants::PUBLIC;
using Orm::Constants::QMYSQL;
using Orm::Constants::QPSQL;
using Orm::Constants::QSQLITE;
using Orm::Constants::ROOT;
using Orm::Constants::TZ00;
using Orm::Constants::UTC;
using Orm::Constants::UTF8;
using Orm::Constants::UTF8MB4;
using Orm::Constants::UTF8MB40900aici;
using Orm::Constants::Version;
using Orm::Constants::database_;
using Orm::Constants::driver_;
using Orm::Constants::charset_;
using Orm::Constants::check_database_exists;
using Orm::Constants::collation_;
using Orm::Constants::engine_;
using Orm::Constants::foreign_key_constraints;
using Orm::Constants::host_;
using Orm::Constants::isolation_level;
using Orm::Constants::options_;
using Orm::Constants::password_;
using Orm::Constants::port_;
using Orm::Constants::postgres_;
using Orm::Constants::prefix_;
using Orm::Constants::prefix_indexes;
using Orm::Constants::return_qdatetime;
using Orm::Constants::qt_timezone;
using Orm::Constants::schema_;
using Orm::Constants::strict_;
using Orm::Constants::timezone_;
using Orm::Constants::username_;

using Orm::DB;

using Orm::Exceptions::RuntimeError;

#ifndef TINYORM_SQLITE_DATABASE
#  define TINYORM_SQLITE_DATABASE ""
#endif

namespace TestUtils
{

/* The whole class is designed so that a Database::createConnections/createConnection
   methods can be called only once. You can pass connection name/s to these methods
   and they create TinyORM database connections.
   Only those connections will be created, for which are environment variables defined
   correctly.
   Tests don't fail but are skipped when a connection is not available. */

namespace
{
    /*! DatabaseManager instance. */
    Q_GLOBAL_STATIC_WITH_ARGS(std::shared_ptr<Orm::DatabaseManager>, db, {nullptr});
}

/* public */

const QStringList &Databases::createConnections(const QStringList &connections)
{
    throwIfConnectionsInitialized();

    // Ownership of a shared_ptr()
    static const auto manager = *db = DB::create();

    /* The default connection is empty for tests, there is no default connection
       because it can produce hard to find bugs, I have to be explicit about
       the connection which will be used. */
    manager->addConnections(createConfigurationsHash(connections), EMPTY);

    static const auto cachedConnectionNames = manager->connectionNames();

    return cachedConnectionNames;
}

QString Databases::createConnection(const QString &connection)
{
    const auto &connections = createConnections({connection});

    if (connections.size() > 1)
        throw RuntimeError(
                QStringLiteral("Returned more than one connection in %1().")
                .arg(__tiny_func__));

    if (!connections.isEmpty())
        return connections.first();

    return {};
}

bool Databases::allEnvVariablesEmpty(const std::vector<const char *> &envVariables)
{
    return std::all_of(envVariables.cbegin(), envVariables.cend(),
                       [](const auto *envVariable)
    {
        return qEnvironmentVariableIsEmpty(envVariable);
    });
}

const std::shared_ptr<Orm::DatabaseManager> &Databases::manager()
{
    if (db() == nullptr)
        throw RuntimeError(
                QStringLiteral("The global static 'db' was already destroyed in %1().")
                .arg(__tiny_func__));

    return *db;
}

/* private */

const Databases::ConfigurationsType &
Databases::createConfigurationsHash(const QStringList &connections)
{
    static ConfigurationsType configurations;

    const auto shouldCreateConnection = [&connections]
                                        (const auto &connection, auto &&driver)
    {
        const auto isAvailable = DB::isDriverAvailable(driver);

        if (!isAvailable)
            qWarning("%s driver not available, all tests for this database will be "
                     "skipped.", driver.toLatin1().constData());

        return isAvailable &&
                (connections.isEmpty() || connections.contains(connection));
    };

    if (shouldCreateConnection(MYSQL, QMYSQL))
        if (auto [config, envDefined] = mysqlConfiguration(); envDefined)
            configurations[MYSQL] = std::move(config);

    if (shouldCreateConnection(SQLITE, QSQLITE))
        if (auto [config, envDefined] = sqliteConfiguration(); envDefined)
            configurations[SQLITE] = std::move(config);

    if (shouldCreateConnection(POSTGRESQL, QPSQL))
        if (auto [config, envDefined] = postgresConfiguration(); envDefined)
            configurations[POSTGRESQL] = std::move(config);

    return configurations;
}

std::pair<std::reference_wrapper<const QVariantHash>, bool>
Databases::mysqlConfiguration()
{
    static const QVariantHash config {
        {driver_,         QMYSQL},
        {host_,           qEnvironmentVariable("DB_MYSQL_HOST",      H127001)},
        {port_,           qEnvironmentVariable("DB_MYSQL_PORT",      P3306)},
        {database_,       qEnvironmentVariable("DB_MYSQL_DATABASE",  EMPTY)},
        {username_,       qEnvironmentVariable("DB_MYSQL_USERNAME",  ROOT)},
        {password_,       qEnvironmentVariable("DB_MYSQL_PASSWORD",  EMPTY)},
        {charset_,        qEnvironmentVariable("DB_MYSQL_CHARSET",   UTF8MB4)},
        {collation_,      qEnvironmentVariable("DB_MYSQL_COLLATION", UTF8MB40900aici)},
        // Very important for tests
        {timezone_,       TZ00},
        // Specifies what time zone all QDateTime-s will have
        {qt_timezone,     QVariant::fromValue(Qt::UTC)},
        {prefix_,         EMPTY},
        {prefix_indexes,  true},
        {strict_,         true},
        {isolation_level, QStringLiteral("REPEATABLE READ")},
        {engine_,         InnoDB},
        {Version,         {}}, // Autodetect
        {options_,        QVariantHash()},
        // FUTURE remove, when unit tested silverqx
        // Example
//        {options_, "MYSQL_OPT_CONNECT_TIMEOUT = 5 ; MYSQL_OPT_RECONNECT=1"},
//        {options_, QVariantHash {{"MYSQL_OPT_RECONNECT", 1},
//                                 {"MYSQL_OPT_READ_TIMEOUT", 10}}},
    };

    // Environment variables were undefined
    const std::vector<const char *> envVariables {
        "DB_MYSQL_HOST", "DB_MYSQL_PORT", "DB_MYSQL_DATABASE", "DB_MYSQL_USERNAME",
        "DB_MYSQL_PASSWORD", "DB_MYSQL_CHARSET", "DB_MYSQL_COLLATION"
    };

    if (allEnvVariablesEmpty(envVariables))
        return {std::cref(config), false};

    // Environment variables was defined
    return {std::cref(config), true};
}

std::pair<std::reference_wrapper<const QVariantHash>, bool>
Databases::sqliteConfiguration()
{
    static const QVariantHash config {
        {driver_,                 QSQLITE},
        {database_,               qEnvironmentVariable("DB_SQLITE_DATABASE",
                                                       TINYORM_SQLITE_DATABASE)},
        {foreign_key_constraints, true},
        {check_database_exists,   true},
        // Specifies what time zone all QDateTime-s will have
        {qt_timezone,             QVariant::fromValue(Qt::UTC)},
        {return_qdatetime,        true},
        {prefix_,                 EMPTY},
        // FUTURE schema sqlite, prefix_indexes and sqlite, works it? test silverqx
    };

    // Environment variables were undefined
    const std::vector<const char *> envVariables {
        "DB_SQLITE_DATABASE", "DB_SQLITE_FOREIGN_KEYS"
    };

    if (allEnvVariablesEmpty(envVariables))
        return {std::cref(config), false};

    // Environment variables was defined
    return {std::cref(config), true};
}

std::pair<std::reference_wrapper<const QVariantHash>, bool>
Databases::postgresConfiguration()
{
    static const QVariantHash config {
        {driver_,        QPSQL},
        {host_,          qEnvironmentVariable("DB_PGSQL_HOST",     H127001)},
        {port_,          qEnvironmentVariable("DB_PGSQL_PORT",     P5432)},
        {database_,      qEnvironmentVariable("DB_PGSQL_DATABASE", EMPTY)},
        {schema_,        qEnvironmentVariable("DB_PGSQL_SCHEMA",   PUBLIC)},
        {username_,      qEnvironmentVariable("DB_PGSQL_USERNAME", postgres_)},
        {password_,      qEnvironmentVariable("DB_PGSQL_PASSWORD", EMPTY)},
        {charset_,       qEnvironmentVariable("DB_PGSQL_CHARSET",  UTF8)},
        {timezone_,      UTC},
        // Specifies what time zone all QDateTime-s will have
        {qt_timezone,    QVariant::fromValue(Qt::UTC)},
        {prefix_,        EMPTY},
        {prefix_indexes, true},
        // ConnectionFactory provides a default value for this, this is only for reference
//        {dont_drop,      QStringList {QStringLiteral("spatial_ref_sys")}},
        {options_,       QVariantHash()},
    };

    // Environment variables were undefined
    const std::vector<const char *> envVariables {
        "DB_PGSQL_HOST", "DB_PGSQL_PORT", "DB_PGSQL_DATABASE", "DB_PGSQL_SCHEMA",
        "DB_PGSQL_USERNAME", "DB_PGSQL_PASSWORD", "DB_PGSQL_CHARSET"
    };

    if (allEnvVariablesEmpty(envVariables))
        return {std::cref(config), false};

    // Environment variables was defined
    return {std::cref(config), true};
}

void Databases::throwIfConnectionsInitialized()
{
    /*! Determines whether connections were initialized. */
    static bool initialized = false;

    if (initialized)
        throw RuntimeError(
                QStringLiteral("Databases::createConnections/createConnection methods "
                               "can be called only once in %1().")
                .arg(__tiny_func__));

    initialized = true;
}

} // namespace TestUtils
