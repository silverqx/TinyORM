#include "databases.hpp"

#include "orm/db.hpp"
#include "orm/utils/configuration.hpp"
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
using Orm::Constants::UTF8MB4Unicode520ci;
using Orm::Constants::Version;
using Orm::Constants::application_name;
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
using Orm::Constants::search_path;
using Orm::Constants::qt_timezone;
using Orm::Constants::strict_;
using Orm::Constants::timezone_;
using Orm::Constants::username_;

using Orm::DB;
using Orm::DatabaseManager;

using Orm::Exceptions::RuntimeError;

using ConfigUtils = Orm::Utils::Configuration;

using ConfigurationsType = TestUtils::Databases::ConfigurationsType;

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

/* private */

std::shared_ptr<DatabaseManager> Databases::m_dm;
ConfigurationsType Databases::m_configurations;

/* public */

/* Create connection/s for the whole unit test case */

QStringList Databases::createConnections(const QStringList &connections)
{
    throwIfConnectionsInitialized();

    // Ownership of a shared_ptr()
    m_dm = DB::create();

    /* The default connection is empty for tests, there is no default connection
       because it can produce hard to find bugs, I have to be explicit about
       the connection which will be used. */
    m_dm->addConnections(createConfigurationsHash(connections), EMPTY);

    return m_dm->connectionNames();
}

QString Databases::createConnection(const QString &connection)
{
    auto connections = createConnections({connection});

    if (connections.isEmpty())
        return {};

    return std::move(connections.first());
}

/* Create a connection for one test method */

namespace
{
    /*! Create the connection name from the given parts (for temporary connection). */
    inline QString
    connectionNameForTemp(const QString &connection,
                          const Databases::ConnectionNameParts &connectionParts)
    {
        return QStringLiteral("%1-%2-%3").arg(connection, connectionParts.className,
                                              connectionParts.methodName);
    }
} // namespace

/* Differences between following three methods:
   1. Creates a totally new connection with custom configuration
   2. Creates a new connection from our predefined configuration
   3. Like 2. but allows to customize the configuration */

std::optional<QString>
Databases::createConnectionTemp(
        const QString &connection, ConnectionNameParts &&connectionParts,
        const QVariantHash &configuration)
{
    Q_ASSERT(configuration.contains(driver_));

    const auto driver = configuration[driver_].value<QString>().toUpper();

    if (!isDriverAvailable(driver) ||
        !envVariablesDefined(envVariables(driver, connection))
    )
        return std::nullopt;

    auto connectionName = connectionNameForTemp(connection, connectionParts);

    // Add a new database connection
    m_dm->addConnection(configuration, connectionName);

    return connectionName;
}

std::optional<QString>
Databases::createConnectionTempFrom(const QString &fromConfiguration,
                                    ConnectionNameParts &&connection)
{
    const auto configuration = Databases::configuration(fromConfiguration);

    // Nothing to do, no configuration exists
    if (!configuration)
        return std::nullopt;

    auto connectionName = connectionNameForTemp(fromConfiguration, connection);

    // Add a new database connection
    m_dm->addConnection(*configuration, connectionName);

    return connectionName;
}

std::optional<QString>
Databases::createConnectionTempFrom(
        const QString &fromConfiguration, ConnectionNameParts &&connection,
        std::unordered_map<QString, QVariant> &&optionsToUpdate,
        const std::vector<QString> &optionsToRemove)
{
    const auto configurationOriginal = Databases::configuration(fromConfiguration);

    // Nothing to do, no configuration exists
    if (!configurationOriginal)
        return std::nullopt;

    // Make configuration copy so I can modify it
    auto configuration = configurationOriginal->get();

    // Add, modify, or remove options in the configuration
    updateConfigurationForTemp(configuration, std::move(optionsToUpdate),
                               optionsToRemove);

    auto connectionName = connectionNameForTemp(fromConfiguration, connection);

    // Add a new database connection
    m_dm->addConnection(configuration, connectionName);

    return connectionName;
}

std::optional<std::reference_wrapper<const QVariantHash>>
Databases::configuration(const QString &connection)
{
    if (!m_configurations.contains(connection))
        return std::nullopt;

    return m_configurations.at(connection);
}

bool Databases::hasConfiguration(const QString &connection)
{
    return m_configurations.contains(connection);
}

/* Common */

bool Databases::removeConnection(const QString &connection)
{
    return m_dm->removeConnection(connection);
}

bool Databases::envVariablesDefined(const std::vector<const char *> &envVariables)
{
    return std::any_of(envVariables.cbegin(), envVariables.cend(),
                       [](const char *envVariable)
    {
        return !qEnvironmentVariableIsEmpty(envVariable);
    });
}

Orm::DatabaseManager &Databases::manager()
{
    throwIfNoManagerInstance();

    return *m_dm;
}

std::shared_ptr<DatabaseManager> Databases::managerShared()
{
    throwIfNoManagerInstance();

    return m_dm;
}

/* private */

const ConfigurationsType &
Databases::createConfigurationsHash(const QStringList &connections)
{
    /*! Determine whether a connection for the given driver should be created. */
    const auto shouldCreateConnection = [&connections]
                                        (const QString &connection,
                                         const QString &driver)
    {
        // connections.isEmpty() means create all connections
        const auto createAllConnections = connections.isEmpty();

        return isDriverAvailable(driver) &&
                (createAllConnections || connections.contains(connection));
    };

    // This connection must be to the MySQL database server (not MariaDB)
    if (shouldCreateConnection(MYSQL, QMYSQL))
        if (auto [config, envDefined] = mysqlConfiguration(); envDefined)
            m_configurations[MYSQL] = std::move(config);

    // This connection must be to the MariaDB database server (not MySQL)
    if (shouldCreateConnection(MARIADB, QMYSQL))
        if (auto [config, envDefined] = mariaConfiguration(); envDefined)
            m_configurations[MARIADB] = std::move(config);

    if (shouldCreateConnection(SQLITE, QSQLITE))
        if (auto [config, envDefined] = sqliteConfiguration(); envDefined)
            m_configurations[SQLITE] = std::move(config);

    if (shouldCreateConnection(POSTGRESQL, QPSQL))
        if (auto [config, envDefined] = postgresConfiguration(); envDefined)
            m_configurations[POSTGRESQL] = std::move(config);

    return m_configurations;
}

std::pair<QVariantHash, bool>
Databases::mysqlConfiguration()
{
    /* This connection must be to the MySQL database server (not MariaDB), because
       some auto tests depend on it and also the TinyOrmPlayground project. */
    QVariantHash config {
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
        /* Specifies what time zone all QDateTime-s will have, the overridden default
           is the Qt::UTC, set to the Qt::LocalTime or QtTimeZoneType::DontConvert to
           use the system local time. */
        {qt_timezone,     QVariant::fromValue(Qt::UTC)},
        {prefix_,         EMPTY},
        {prefix_indexes,  false},
        {strict_,         true},
        {isolation_level, QStringLiteral("REPEATABLE READ")}, // MySQL default is REPEATABLE READ for InnoDB
        {engine_,         InnoDB},
        {Version,         {}}, // Autodetect
        {options_,        ConfigUtils::mysqlSslOptions()},
        // FUTURE remove, when unit tested silverqx
        // Example
//        {options_, "MYSQL_OPT_CONNECT_TIMEOUT = 5 ; MYSQL_OPT_RECONNECT=1"},
//        {options_, QVariantHash {{"MYSQL_OPT_RECONNECT", 1},
//                                 {"MYSQL_OPT_READ_TIMEOUT", 10}}},
    };

    return {std::move(config), envVariablesDefined(mysqlEnvVariables())};
}

std::pair<QVariantHash, bool>
Databases::mariaConfiguration()
{
    /* This connection must be to the MySQL database server (not MariaDB), because
       some auto tests depend on it and also the TinyOrmPlayground project. */
    QVariantHash config {
        {driver_,         QMYSQL},
        {host_,           qEnvironmentVariable("DB_MARIA_HOST",      H127001)},
        {port_,           qEnvironmentVariable("DB_MARIA_PORT",      P3306)},
        {database_,       qEnvironmentVariable("DB_MARIA_DATABASE",  EMPTY)},
        {username_,       qEnvironmentVariable("DB_MARIA_USERNAME",  ROOT)},
        {password_,       qEnvironmentVariable("DB_MARIA_PASSWORD",  EMPTY)},
        {charset_,        qEnvironmentVariable("DB_MARIA_CHARSET",   UTF8MB4)},
        {collation_,      qEnvironmentVariable("DB_MARIA_COLLATION",
                                               UTF8MB4Unicode520ci)},
        // Very important for tests
        {timezone_,       TZ00},
        /* Specifies what time zone all QDateTime-s will have, the overridden default
           is the Qt::UTC, set to the Qt::LocalTime or QtTimeZoneType::DontConvert to
           use the system local time. */
        {qt_timezone,     QVariant::fromValue(Qt::UTC)},
        {prefix_,         EMPTY},
        {prefix_indexes,  false},
        {strict_,         true},
        {isolation_level, QStringLiteral("REPEATABLE READ")}, // MySQL default is REPEATABLE READ for InnoDB
        {engine_,         InnoDB},
        {Version,         {}}, // Autodetect
        {options_,        ConfigUtils::mariaSslOptions()},
        // FUTURE remove, when unit tested silverqx
        // Example
//        {options_, "MYSQL_OPT_CONNECT_TIMEOUT = 5 ; MYSQL_OPT_RECONNECT=1"},
//        {options_, QVariantHash {{"MYSQL_OPT_RECONNECT", 1},
//                                 {"MYSQL_OPT_READ_TIMEOUT", 10}}},
    };

    return {std::move(config), envVariablesDefined(mariaEnvVariables())};
}

std::pair<QVariantHash, bool>
Databases::sqliteConfiguration()
{
    QVariantHash config {
        {driver_,                 QSQLITE},
        {database_,               qEnvironmentVariable("DB_SQLITE_DATABASE",
                                                       TINYORM_SQLITE_DATABASE)},
        {foreign_key_constraints, true},
        {check_database_exists,   true},
        /* Specifies what time zone all QDateTime-s will have, the overridden default
           is the Qt::UTC, set to the Qt::LocalTime or QtTimeZoneType::DontConvert to
           use the system local time. */
        {qt_timezone,             QVariant::fromValue(Qt::UTC)},
        /* Return a QDateTime with the correct time zone instead of the QString,
           only works when the qt_timezone isn't set to the DontConvert. */
        {return_qdatetime,        true},
        {prefix_,                 EMPTY},
        // Prefixing indexes also works with the SQLite database
        {prefix_indexes,          false},
    };

    return {std::move(config), envVariablesDefined(sqliteEnvVariables())};
}

std::pair<QVariantHash, bool>
Databases::postgresConfiguration()
{
    QVariantHash config {
        {driver_,            QPSQL},
        {application_name,   QStringLiteral("TinyORM tests (TinyUtils)")},
        {host_,              qEnvironmentVariable("DB_PGSQL_HOST",       H127001)},
        {port_,              qEnvironmentVariable("DB_PGSQL_PORT",       P5432)},
        {database_,          qEnvironmentVariable("DB_PGSQL_DATABASE",   EMPTY)},
        {search_path,        qEnvironmentVariable("DB_PGSQL_SEARCHPATH", PUBLIC)},
        {username_,          qEnvironmentVariable("DB_PGSQL_USERNAME",   postgres_)},
        {password_,          qEnvironmentVariable("DB_PGSQL_PASSWORD",   EMPTY)},
        {charset_,           qEnvironmentVariable("DB_PGSQL_CHARSET",    UTF8)},
        {timezone_,          UTC},
        /* Specifies what time zone all QDateTime-s will have, the overridden default
           is the Qt::UTC, set to the Qt::LocalTime or QtTimeZoneType::DontConvert to
           use the system local time. */
        {qt_timezone,        QVariant::fromValue(Qt::UTC)},
        {prefix_,            EMPTY},
        {prefix_indexes,     false},
//        {isolation_level,    QStringLiteral("REPEATABLE READ")}, // Postgres default is READ COMMITTED
//        {synchronous_commit, QStringLiteral("off")}, // Postgres default is on
        // ConnectionFactory provides a default value for this, this is only for reference
//        {dont_drop,          QStringList {spatial_ref_sys}},
        {options_,           ConfigUtils::postgresSslOptions()},
    };

    return {std::move(config), envVariablesDefined(postgresEnvVariables())};
}

const std::vector<const char *> &
Databases::envVariables(const QString &driver, const QString &connection)
{
    if (driver == QMYSQL) {
        if (connection == MYSQL)
            return mysqlEnvVariables();

        if (connection == MARIADB)
            return mariaEnvVariables();

        Q_UNREACHABLE();
    }

    if (driver == QPSQL)
        return postgresEnvVariables();

    if (driver == QSQLITE)
        return sqliteEnvVariables();

    Q_UNREACHABLE();
}

const std::vector<const char *> &Databases::mysqlEnvVariables()
{
    // Environment variables to check if all are empty (no need to check SSL variables)
    static const std::vector<const char *> cached {
        "DB_MYSQL_HOST", "DB_MYSQL_PORT", "DB_MYSQL_DATABASE", "DB_MYSQL_USERNAME",
        "DB_MYSQL_PASSWORD", "DB_MYSQL_CHARSET", "DB_MYSQL_COLLATION"
    };

    return cached;
}

const std::vector<const char *> &Databases::mariaEnvVariables()
{
    // Environment variables to check if all are empty (no need to check SSL variables)
    static const std::vector<const char *> cached {
        "DB_MARIA_HOST", "DB_MARIA_PORT", "DB_MARIA_DATABASE", "DB_MARIA_USERNAME",
        "DB_MARIA_PASSWORD", "DB_MARIA_CHARSET", "DB_MARIA_COLLATION"
    };

    return cached;
}

const std::vector<const char *> &Databases::sqliteEnvVariables()
{
    // Environment variables to check if all are empty
    static const std::vector<const char *> cached {
        "DB_SQLITE_DATABASE", "DB_SQLITE_FOREIGN_KEYS"
    };

    return cached;
}

const std::vector<const char *> &Databases::postgresEnvVariables()
{
    // Environment variables to check if all are empty (no need to check SSL variables)
    static const std::vector<const char *> cached {
        "DB_PGSQL_HOST", "DB_PGSQL_PORT", "DB_PGSQL_DATABASE", "DB_PGSQL_SEARCHPATH",
        "DB_PGSQL_USERNAME", "DB_PGSQL_PASSWORD", "DB_PGSQL_CHARSET"
    };

    return cached;
}

bool Databases::isDriverAvailable(const QString &driver)
{
    Q_ASSERT(m_dm->supportedDrivers().contains(driver));

    static std::unordered_map<QString, bool> isAvailableCache;

    // Return a cached value/result
    if (isAvailableCache.contains(driver) && isAvailableCache.at(driver))
        return true;

    const auto isAvailable = m_dm->isDriverAvailable(driver);

    if (!isAvailable)
        qWarning("The '%s' driver not available, all tests for this database will "
                 "be skipped.", driver.toLatin1().constData());

    // Cache a result and return it
    const auto [it, ok] = isAvailableCache.emplace(driver, isAvailable);
    Q_ASSERT(ok);

    return it->second;
}

void Databases::updateConfigurationForTemp(
        QVariantHash &configuration,
        std::unordered_map<QString, QVariant> &&optionsToUpdate,
        const std::vector<QString> &optionsToRemove)
{
    // Add or modify the configuration
    if (!optionsToUpdate.empty())
        for (auto &&[option, value] : optionsToUpdate)
            configuration[option] = std::move(value);

    // Remove options from the configuration
    if (!optionsToRemove.empty())
        for (const auto &option : optionsToRemove)
            if (configuration.contains(option))
                configuration.remove(option);
}

void Databases::throwIfNoManagerInstance()
{
    // Nothing to do, instance already exists
    if (m_dm)
        return;

    throw RuntimeError(
                QStringLiteral(
                    "The DatabaseManager instance has not yet been created, create it "
                    "by the Databases::createConnections/createConnection methods "
                    "in %1().")
                .arg(__tiny_func__));
}

void Databases::throwIfConnectionsInitialized()
{
    /*! Determines whether connections were initialized. */
    static auto initialized = false;

    if (initialized)
        throw RuntimeError(
                QStringLiteral("Databases::createConnections/createConnection methods "
                               "can be called only once in %1().")
                .arg(__tiny_func__));

    initialized = true;
}

} // namespace TestUtils
