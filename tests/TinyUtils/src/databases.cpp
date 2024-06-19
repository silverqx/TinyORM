#include "databases.hpp"

#ifdef TINYORM_USING_TINYDRIVERS
#  include <range/v3/view/map.hpp>
#endif

#include "orm/db.hpp"
#include "orm/utils/configuration.hpp"
#include "orm/utils/type.hpp"

#ifdef TINYORM_USING_TINYDRIVERS
#  include "orm/exceptions/runtimeerror.hpp"
#endif

#ifndef sl
/*! Alias for the QStringLiteral(). */
#  define sl(str) QStringLiteral(str)
#endif

#ifdef TINYORM_USING_TINYDRIVERS
using Orm::Drivers::SqlDatabase;
#endif

using Orm::Constants::EMPTY;
using Orm::Constants::H127001;
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
using Orm::Constants::UTF8MB4Uca1400aici;
using Orm::Constants::InnoDB;
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

#ifdef TINYORM_USING_TINYDRIVERS
using Orm::Constants::EQ_C;
using Orm::Constants::SEMICOLON;
using Orm::Constants::SSL_CA;
using Orm::Constants::SSL_CERT;
using Orm::Constants::SSL_KEY;
using Orm::Constants::SSL_MODE;
#endif

using Orm::DB;
using Orm::DatabaseManager;
using Orm::Exceptions::RuntimeError;
using Orm::TTimeZone;

using ConfigUtils = Orm::Utils::Configuration;

using ConfigurationsType = TestUtils::Databases::ConfigurationsType;

#ifndef TINYORM_SQLITE_DATABASE
#  define TINYORM_SQLITE_DATABASE ""
#endif

namespace TestUtils
{

/* The whole class is designed so that a Database::createConnections()/createConnection()
   methods can be called only once. You can pass connection name/s to these methods
   and they create TinyORM database connections.
   Only those connections will be created, for which the environment variables are defined
   correctly.
   Tests don't fail but are skipped when a connection is not available.

   The createConnections()/createConnection() are supposed to be called from
   the initTestCase() method (can be called only once).
   The createConnectionTemp/From() can be called from test methods and connections don't
   need to be initialized first in the initTestCase(). */

/* Global */

const QString AutoTestSkipped =
        sl("%1 autotest skipped, environment variables for '%2' connection "
           "have not been defined or the Qt sql driver is not available.");
const QString AutoTestSkippedAny =
        sl("%1 autotest skipped, environment variables for ANY connection "
           "have not been defined.");

/* Databases */

/* private */

std::shared_ptr<DatabaseManager> Databases::m_dm;
ConfigurationsType Databases::m_configurations;

/* public */

const QString Databases::MYSQL      = sl("tinyorm_mysql_tests");
const QString Databases::MARIADB    = sl("tinyorm_maria_tests");
const QString Databases::SQLITE     = sl("tinyorm_sqlite_tests");
const QString Databases::POSTGRESQL = sl("tinyorm_postgres_tests");

#ifdef TINYORM_USING_TINYDRIVERS
const QString Databases::MYSQL_DRIVERS      = sl("tinydrivers_mysql_tests");
const QString Databases::MARIADB_DRIVERS    = sl("tinydrivers_maria_tests");
const QString Databases::SQLITE_DRIVERS     = sl("tinydrivers_sqlite_tests");
const QString Databases::POSTGRESQL_DRIVERS = sl("tinydrivers_postgres_tests");
#endif

/* Create connection/s for the whole unit test case */

QStringList Databases::createConnections(const QStringList &connections)
{
    throwIfConnectionsInitialized();

    createDatabaseManager();

    /* The default connection is empty for tests, there is no default connection
       because it can produce hard to find bugs, I have to be explicit about
       the connection which will be used. */
    m_dm->addConnections(createConfigurationsHash(connections, false), EMPTY);

    return m_dm->connectionNames();
}

QString Databases::createConnection(const QString &connection)
{
    auto connections = createConnections({connection});

    // Nothing to do
    if (connections.isEmpty())
        return {};

    return std::move(connections.first());
}

#ifdef TINYORM_USING_TINYDRIVERS
QStringList Databases::createDriversConnections(const QStringList &connections)
{
    throwIfConnectionsInitialized();

    const auto &configurations = createConfigurationsHash(connections, true);

    // Nothing to do
    if (configurations.empty())
        return {};

    for (const auto &[connection, configuration] : configurations)
        createDriversConnectionInternal(connection, configuration);

    return configurations | ranges::views::keys | ranges::to<QStringList>();
}

QString Databases::createDriversConnection(const QString &connection)
{
    throwIfConnectionsInitialized();

#if defined(__GNUG__) && !defined(__clang__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wdangling-reference"
#endif
    const auto &configurations = createConfigurationsHash({connection}, true);
#if defined(__GNUG__) && !defined(__clang__)
#  pragma GCC diagnostic pop
#endif

    // Nothing to do
    if (configurations.empty())
        return {};

    Q_ASSERT(configurations.size() == 1);

    createDriversConnectionInternal(connection, configurations.find(connection)->second);

    return connection;
}
#endif

/* Create a connection for one test method */

namespace
{
    /*! Create the connection name from the given parts (for temporary connection). */
    inline QString
    connectionNameForTemp(const QString &connection,
                          const Databases::ConnectionNameParts &connectionParts)
    {
        return sl("%1-%2-%3").arg(connection, connectionParts.className,
                                              connectionParts.methodName);
    }
} // namespace

/* Differences between following three methods:
   1. Creates a totally new connection with custom configuration
   2. Creates a new connection from our predefined configuration
   3. Like 2. but allows to customize the configuration */

std::optional<QString>
Databases::createConnectionTemp(
        const QString &connection, const ConnectionNameParts &connectionParts,
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
                                    const ConnectionNameParts &connectionParts)
{
    const auto configuration = Databases::configurationForTemp(fromConfiguration, false);

    // Nothing to do, no configuration exists
    if (!configuration)
        return std::nullopt;

    auto connectionName = connectionNameForTemp(fromConfiguration, connectionParts);

    // Add a new database connection
    m_dm->addConnection(*configuration, connectionName);

    return connectionName;
}

std::optional<QString>
Databases::createConnectionTempFrom(
        const QString &fromConfiguration, const ConnectionNameParts &connectionParts,
        std::unordered_map<QString, QVariant> &&optionsToUpdate,
        const std::vector<QString> &optionsToRemove)
{
    const auto
    configurationOriginal = Databases::configurationForTemp(fromConfiguration, false);

    // Nothing to do, no configuration exists
    if (!configurationOriginal)
        return std::nullopt;

    // Make configuration copy so I can modify it
    auto configuration = configurationOriginal->get();

    // Add, modify, or remove options in the configuration
    updateConfigurationForTemp(configuration, std::move(optionsToUpdate),
                               optionsToRemove);

    auto connectionName = connectionNameForTemp(fromConfiguration, connectionParts);

    // Add a new database connection
    m_dm->addConnection(configuration, connectionName);

    return connectionName;
}

#ifdef TINYORM_USING_TINYDRIVERS
std::optional<QString>
Databases::createDriversConnectionTemp(
        const QString &connection, const ConnectionNameParts &connectionParts,
        const QVariantHash &configuration, const bool open)
{
    Q_ASSERT(configuration.contains(driver_));

    const auto driver = configuration[driver_].value<QString>().toUpper();

    if (!isDriverAvailable(driver) ||
        !envVariablesDefined(envVariables(driver, connection))
    )
        return std::nullopt;

    auto connectionName = connectionNameForTemp(connection, connectionParts);

    // Add a new TinyDrivers database connection
    createDriversConnectionInternal(connectionName, configuration, open);

    return connectionName;
}

std::optional<QString>
Databases::createDriversConnectionTempFrom(
        const QString &fromConfiguration, const ConnectionNameParts &connectionParts,
        const bool open)
{
    const auto configuration = Databases::configurationForTemp(fromConfiguration, true);

    // Nothing to do, no configuration exists
    if (!configuration)
        return std::nullopt;

    auto connectionName = connectionNameForTemp(fromConfiguration, connectionParts);

    // Add a new TinyDrivers database connection
    createDriversConnectionInternal(connectionName, *configuration, open);

    return connectionName;
}

std::optional<QString>
Databases::createDriversConnectionTempFrom(
        const QString &fromConfiguration, const ConnectionNameParts &connectionParts,
        std::unordered_map<QString, QVariant> &&optionsToUpdate,
        const std::vector<QString> &optionsToRemove, const bool open)
{
    const auto
    configurationOriginal = Databases::configurationForTemp(fromConfiguration, true);

    // Nothing to do, no configuration exists
    if (!configurationOriginal)
        return std::nullopt;

    // Make configuration copy so I can modify it
    auto configuration = configurationOriginal->get();

    // Add, modify, or remove options in the configuration
    updateConfigurationForTemp(configuration, std::move(optionsToUpdate),
                               optionsToRemove);

    auto connectionName = connectionNameForTemp(fromConfiguration, connectionParts);

    // Add a new TinyDrivers database connection
    createDriversConnectionInternal(connectionName, configuration, open);

    return connectionName;
}
#endif

std::optional<std::reference_wrapper<const QVariantHash>>
Databases::configurationForTemp(const QString &connection, const bool forTinyDrivers)
{
    // Never call this method from createConnection/s() or createDriversConnection/s()

    // Connection already exists
    if (hasConfiguration(connection))
        return m_configurations.at(connection);

    // The XyzConnectionTemp() methods are allowed to create new configurations
    createConfigurationsHash({connection}, forTinyDrivers);

    // Nothing to do, creating the configuration failed
    if (!hasConfiguration(connection))
        return std::nullopt;

    return std::as_const(m_configurations).at(connection);
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

#ifdef TINYORM_USING_TINYDRIVERS
void Databases::removeDriversConnection(const QString &connection)
{
    SqlDatabase::removeDatabase(connection);
}

/* This method is needed and must be used the with build_static_drivers option because
   using the SqlDatabase::database() directly causes that the g_connections global
   variable has different addresses in TinyOrm and TinyUtils libraries.
   The reason is that the TinyUtils, TinyOrm, and auto test executables are all linking
   against the TinyDrivers library and if this library is linked statically into
   the TinyUtils, TinyOrm, and auto tests executables then the g_connections global will
   have different address everywhere, there is nothing what can be done about this
   because this is how static libraries work, solution is to use shared libraries or
   secure that only one instance will be used and this is what this and a few methods
   below are doing. */
SqlDatabase Databases::driversConnection(const QString &connection, const bool open)
{
    return SqlDatabase::database(connection, open);
}

QStringList Databases::driversConnectionNames()
{
    return SqlDatabase::connectionNames();
}

QStringList Databases::driversOpenedConnectionNames()
{
    return SqlDatabase::openedConnectionNames();
}

bool Databases::driversIsThreadCheck() noexcept
{
    return SqlDatabase::isThreadCheckEnabled();
}
#endif

bool Databases::envVariablesDefined(const std::vector<const char *> &envVariables)
{
    return std::any_of(envVariables.cbegin(), envVariables.cend(),
                       [](const char *envVariable)
    {
        return !qEnvironmentVariableIsEmpty(envVariable);
    });
}

std::shared_ptr<Databases::DatabaseManager> Databases::createDatabaseManager()
{
    // Ownership of a shared_ptr()
    m_dm = DB::create(EMPTY);

    return m_dm;
}

DatabaseManager &Databases::manager()
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

#ifdef TINYORM_USING_TINYDRIVERS
void Databases::createDriversConnectionInternal(
        const QString &connection, const QVariantHash &configuration, const bool open)
{
    Q_ASSERT(configuration.contains(driver_));

    const auto driver = configuration[driver_].value<QString>().toUpper();

    if (driver == QMYSQL) {
        if (connection.startsWith(MYSQL_DRIVERS))
            createDriversMySQLConnection(
                        connection, configuration, mysqlSslEnvVariables(), open);

        else if (connection.startsWith(MARIADB_DRIVERS))
            createDriversMariaConnection(
                        connection, configuration, mariaSslEnvVariables(), open);
    }
    // else if (driver == QPSQL)
    //     createDriversPostgresConnection(connection, configuration, open);

    // else if (driver == QSQLITE)
    //     createDriversSQLiteConnection(connection, configuration, open);

    else
        throw RuntimeError(
                sl("Creating of the '%1' connection for TinyDrivers failed in %2().")
                .arg(connection, __tiny_func__));
}

void Databases::createDriversMySQLConnection(
        const QString &connection, const QVariantHash &configuration,
        const std::vector<const char *> &sslEnvVariables, const bool open)
{
    auto db = SqlDatabase::addDatabase(configuration[driver_].value<QString>().toUpper(),
                                       connection);

    if (configuration.contains(host_))
        db.setHostName(    configuration[host_].value<QString>());
    if (configuration.contains(port_))
        db.setPort(        configuration[port_].value<int>());
    if (configuration.contains(database_))
        db.setDatabaseName(configuration[database_].value<QString>());
    if (configuration.contains(username_))
        db.setUserName(    configuration[username_].value<QString>());
    if (configuration.contains(password_))
        db.setPassword(    configuration[password_].value<QString>());

    // Check if all SSL-related Environment variables are defined
    if (configuration.contains(options_) && envVariablesDefined(sslEnvVariables))
        db.setConnectOptions(createMySQLOrMariaSslOptions(configuration));

    if (open)
        db.open();
}

QString Databases::createMySQLOrMariaSslOptions(const QVariantHash &configuration)
{
    const auto options = configuration[options_].value<QVariantHash>();

    QStringList result;
    result.reserve(3);

    if (options.contains(SSL_CERT))
        result << SSL_CERT % EQ_C % options[SSL_CERT].value<QString>();

    if (options.contains(SSL_KEY))
        result << SSL_KEY % EQ_C % options[SSL_KEY].value<QString>();

    if (options.contains(SSL_CA))
        result << SSL_CA % EQ_C % options[SSL_CA].value<QString>();

    if (options.contains(SSL_MODE))
        result << SSL_MODE % EQ_C % options[SSL_MODE].value<QString>();

    return result.join(SEMICOLON);
}
#endif

const ConfigurationsType &
Databases::createConfigurationsHash(const QStringList &connections,
                                    const bool forTinyDrivers)
{
#ifdef TINYORM_USING_QTSQLDRIVERS
    Q_UNUSED(forTinyDrivers)
#endif
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

    m_configurations.reserve(computeReserveForConfigurationsHash(connections));

    // This connection must be to the MySQL database server (not MariaDB)
#ifdef TINYORM_USING_QTSQLDRIVERS
    if (const auto connection = MYSQL;
#elif defined(TINYORM_USING_TINYDRIVERS)
    if (const auto connection = forTinyDrivers ? MYSQL_DRIVERS : MYSQL;
#else
#  error Missing include "orm/macros/sqldrivermappings.hpp".
#endif
        shouldCreateConnection(connection, QMYSQL)
    )
        if (auto &&[config, envDefined] = mysqlConfiguration(); envDefined)
            m_configurations.try_emplace(connection, std::move(config));

    // This connection must be to the MariaDB database server (not MySQL)
#ifdef TINYORM_USING_QTSQLDRIVERS
    if (const auto connection = MARIADB;
#elif defined(TINYORM_USING_TINYDRIVERS)
    if (const auto connection = forTinyDrivers ? MARIADB_DRIVERS : MARIADB;
#else
#  error Missing include "orm/macros/sqldrivermappings.hpp".
#endif
        shouldCreateConnection(connection, QMYSQL)
    )
        if (auto &&[config, envDefined] = mariaConfiguration(); envDefined)
            m_configurations.try_emplace(connection, std::move(config));

#ifdef TINYORM_USING_QTSQLDRIVERS
    if (const auto connection = SQLITE;
#elif defined(TINYORM_USING_TINYDRIVERS)
    if (const auto connection = forTinyDrivers ? SQLITE_DRIVERS : SQLITE;
#else
#  error Missing include "orm/macros/sqldrivermappings.hpp".
#endif
        shouldCreateConnection(connection, QSQLITE)
    )
        if (auto &&[config, envDefined] = sqliteConfiguration(); envDefined)
            m_configurations.try_emplace(connection, std::move(config));

#ifdef TINYORM_USING_QTSQLDRIVERS
    if (const auto connection = POSTGRESQL;
#elif defined(TINYORM_USING_TINYDRIVERS)
    if (const auto connection = forTinyDrivers ? POSTGRESQL_DRIVERS : POSTGRESQL;
#else
#  error Missing include "orm/macros/sqldrivermappings.hpp".
#endif
        shouldCreateConnection(connection, QPSQL)
    )
        if (auto &&[config, envDefined] = postgresConfiguration(); envDefined)
            m_configurations.try_emplace(connection, std::move(config));

    return m_configurations;
}

ConfigurationsType::size_type
Databases::computeReserveForConfigurationsHash(const QStringList &connections)
{
    /*! Alias for the configurations hash size type. */
    using SizeType = decltype (m_configurations)::size_type;

    if (!connections.isEmpty())
        return static_cast<SizeType>(connections.size());

    /* For all connections enabled (this logic is enough, I will not complicate it).
       +1 because the QMYSQL driver is also used for MariaDB connection. */
#ifdef TINYORM_USING_QTSQLDRIVERS
    return static_cast<SizeType>(m_dm->supportedDrivers().size()) + 1;
#elif defined(TINYORM_USING_TINYDRIVERS)
    return static_cast<SizeType>(SqlDatabase::drivers().size()) + 1;
#else
#  error Missing include "orm/macros/sqldrivermappings.hpp".
#endif
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
           is the QTimeZone::UTC, set to the QTimeZone::LocalTime or
           QtTimeZoneType::DontConvert to use the system local time.
           Also, don't use the QtTimeZoneConfig::utc() here because
           of tst_DatabaseConnection::timezone_And_qt_timezone() test to pass, we need to
           test if the value is correctly parsed. */
        {qt_timezone,     QVariant::fromValue(TTimeZone::UTC)},
        {prefix_,         EMPTY},
        {prefix_indexes,  false},
        {strict_,         true},
        {isolation_level, sl("REPEATABLE READ")}, // MySQL default is REPEATABLE READ for InnoDB
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
                                               UTF8MB4Uca1400aici)},
        // Very important for tests
        {timezone_,       TZ00},
        /* Specifies what time zone all QDateTime-s will have, the overridden default
           is the QTimeZone::UTC, set to the QTimeZone::LocalTime or
           QtTimeZoneType::DontConvert to use the system local time.
           Also, don't use the QtTimeZoneConfig::utc() here because
           of tst_DatabaseConnection::timezone_And_qt_timezone() test to pass, we need to
           test if the value is correctly parsed. */
        {qt_timezone,     QVariant::fromValue(TTimeZone::UTC)},
        {prefix_,         EMPTY},
        {prefix_indexes,  false},
        {strict_,         true},
        {isolation_level, sl("REPEATABLE READ")}, // MySQL default is REPEATABLE READ for InnoDB
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
           is the QTimeZone::UTC, set to the QTimeZone::LocalTime or
           QtTimeZoneType::DontConvert to use the system local time.
           Also, don't use the QtTimeZoneConfig::utc() here because
           of tst_DatabaseConnection::timezone_And_qt_timezone() test to pass, we need to
           test if the value is correctly parsed. */
        {qt_timezone,             QVariant::fromValue(TTimeZone::UTC)},
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
        {application_name,   sl("TinyORM tests (TinyUtils)")},
        {host_,              qEnvironmentVariable("DB_PGSQL_HOST",       H127001)},
        {port_,              qEnvironmentVariable("DB_PGSQL_PORT",       P5432)},
        {database_,          qEnvironmentVariable("DB_PGSQL_DATABASE",   EMPTY)},
        {search_path,        qEnvironmentVariable("DB_PGSQL_SEARCHPATH", PUBLIC)},
        {username_,          qEnvironmentVariable("DB_PGSQL_USERNAME",   postgres_)},
        {password_,          qEnvironmentVariable("DB_PGSQL_PASSWORD",   EMPTY)},
        {charset_,           qEnvironmentVariable("DB_PGSQL_CHARSET",    UTF8)},
        {timezone_,          UTC},
        /* Specifies what time zone all QDateTime-s will have, the overridden default
           is the QTimeZone::UTC, set to the QTimeZone::LocalTime or
           QtTimeZoneType::DontConvert to use the system local time.
           Also, don't use the QtTimeZoneConfig::utc() here because
           of tst_DatabaseConnection::timezone_And_qt_timezone() test to pass, we need to
           test if the value is correctly parsed. */
        {qt_timezone,        QVariant::fromValue(TTimeZone::UTC)},
        {prefix_,            EMPTY},
        {prefix_indexes,     false},
//        {isolation_level,    sl("REPEATABLE READ")}, // Postgres default is READ COMMITTED
//        {synchronous_commit, sl("off")}, // Postgres default is on
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
#ifdef TINYORM_USING_QTSQLDRIVERS
        if (connection == MYSQL)
#elif defined(TINYORM_USING_TINYDRIVERS)
        if (connection == MYSQL || connection == MYSQL_DRIVERS)
#else
#  error Missing include "orm/macros/sqldrivermappings.hpp".
#endif
            return mysqlEnvVariables();

#ifdef TINYORM_USING_QTSQLDRIVERS
        if (connection == MARIADB)
#elif defined(TINYORM_USING_TINYDRIVERS)
        if (connection == MARIADB || connection == MARIADB_DRIVERS)
#else
#  error Missing include "orm/macros/sqldrivermappings.hpp".
#endif
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
        "DB_MYSQL_PASSWORD", "DB_MYSQL_CHARSET", "DB_MYSQL_COLLATION",
    };

    return cached;
}

const std::vector<const char *> &Databases::mariaEnvVariables()
{
    // Environment variables to check if all are empty (no need to check SSL variables)
    static const std::vector<const char *> cached {
        "DB_MARIA_HOST", "DB_MARIA_PORT", "DB_MARIA_DATABASE", "DB_MARIA_USERNAME",
        "DB_MARIA_PASSWORD", "DB_MARIA_CHARSET", "DB_MARIA_COLLATION",
    };

    return cached;
}

const std::vector<const char *> &Databases::sqliteEnvVariables()
{
    // Environment variables to check if all are empty
    static const std::vector<const char *> cached {
        "DB_SQLITE_DATABASE", "DB_SQLITE_FOREIGN_KEYS",
    };

    return cached;
}

const std::vector<const char *> &Databases::postgresEnvVariables()
{
    // Environment variables to check if all are empty (no need to check SSL variables)
    static const std::vector<const char *> cached {
        "DB_PGSQL_HOST", "DB_PGSQL_PORT", "DB_PGSQL_DATABASE", "DB_PGSQL_SEARCHPATH",
        "DB_PGSQL_USERNAME", "DB_PGSQL_PASSWORD", "DB_PGSQL_CHARSET",
    };

    return cached;
}

#ifdef TINYORM_USING_TINYDRIVERS
const std::vector<const char *> &Databases::mysqlSslEnvVariables()
{
    // SSL-related Environment variables to check if all are empty
    static const std::vector<const char *> cached {
        "DB_MYSQL_SSL_CA", "DB_MYSQL_SSL_CERT", "DB_MYSQL_SSL_KEY", "DB_MYSQL_SSL_MODE",
    };

    return cached;
}

const std::vector<const char *> &Databases::mariaSslEnvVariables()
{
    /* We are using MySQL client library to connect to the MariaDB server so
       the MYSQL_OPT_SSL_MODE can't be defined because MariaDB server doesn't recognize
       this option, MariaDB has the MYSQL_OPT_SSL_VERIFY_SERVER_CERT but it can't be used
       either becuase MySQL client library doesn't recognize it. */

    // SSL-related Environment variables to check if all are empty
    static const std::vector<const char *> cached {
        "DB_MARIA_SSL_CA", "DB_MARIA_SSL_CERT", "DB_MARIA_SSL_KEY",
    };

    return cached;
}
#endif

bool Databases::isDriverAvailable(const QString &driver)
{
    static std::unordered_map<QString, bool> isAvailableCache;

    // Return a cached value/result
    if (isAvailableCache.contains(driver))
        return isAvailableCache.at(driver);

    const auto isAvailable = m_dm->isDriverAvailable(driver);

    // TinyDrivers currently supports only the MySQL database so this warning is useless
#ifdef TINYORM_USING_QTSQLDRIVERS
    if (!isAvailable)
        qWarning("The '%s' driver not available, all tests for this database will "
                 "be skipped.", driver.toLatin1().constData());
#endif

    // Cache a result and return it
    const auto [it, ok] = isAvailableCache.emplace(driver, isAvailable);
    Q_ASSERT(ok);

    return it->second;
}

void Databases::updateConfigurationForTemp(
        QVariantHash &configuration,
        std::unordered_map<QString, QVariant> &&optionsToUpdate, // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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
                sl("The DatabaseManager instance has not yet been created, create it "
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
                sl("Databases::createConnections/createConnection methods can be called "
                   "only once in %1().")
                .arg(__tiny_func__));

    initialized = true;
}

} // namespace TestUtils
