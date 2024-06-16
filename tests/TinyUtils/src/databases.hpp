#pragma once
#ifndef TINYUTILS_DATABASES_HPP
#define TINYUTILS_DATABASES_HPP

#include <memory>
#include <optional>

#ifdef TINYORM_USING_TINYDRIVERS
#  include "orm/drivers/sqldatabase.hpp"
#endif

#include "orm/support/databaseconfiguration.hpp"

#include "export.hpp"

namespace Orm
{
    class DatabaseManager;
}

namespace TestUtils
{
    /* I will not create a separate constants file for these two constants, define them
       here is ok because every test that will include databases.hpp will also use
       one of these constants. */

    /*! Template message for the QSKIP() for one connection. */
    TINYUTILS_EXPORT extern const QString AutoTestSkipped;
    /*! Template message for the QSKIP() for more connections. */
    TINYUTILS_EXPORT extern const QString AutoTestSkippedAny;

    /*! Database connections factory for unit tests (library class). */
    class TINYUTILS_EXPORT Databases
    {
        Q_DISABLE_COPY_MOVE(Databases)

        /*! Alias for the DatabaseManager. */
        using DatabaseManager = Orm::DatabaseManager;
#ifdef TINYORM_USING_TINYDRIVERS
        /*! Alias for the SqlDatabase. */
        using SqlDatabase = Orm::Drivers::SqlDatabase;
#endif

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Databases() = delete;
        /*! Deleted destructor. */
        ~Databases() = delete;

        /*! Type used for Database Connections map. */
        using ConfigurationsType = Orm::Support::DatabaseConfiguration
                                               ::ConfigurationsType;

        /*! MySQL connection name for TinyORM tests. */
        static const QString MYSQL;
        /*! MariaDB connection name for TinyORM tests. */
        static const QString MARIADB;
        /*! SQLite connection name for TinyORM tests. */
        static const QString SQLITE;
        /*! PostgreSQL connection name for TinyORM tests. */
        static const QString POSTGRESQL;

#ifdef TINYORM_USING_TINYDRIVERS
        /*! MySQL connection name for TinyDrivers tests. */
        static const QString MYSQL_DRIVERS;
        /*! MariaDB connection name for TinyDrivers tests. */
        static const QString MARIADB_DRIVERS;
        /*! SQLite connection name for TinyDrivers tests. */
        static const QString SQLITE_DRIVERS;
        /*! PostgreSQL connection name for TinyDrivers tests. */
        static const QString POSTGRESQL_DRIVERS;
#endif

        /* Create connection/s for the whole unit test case */
        /*! Create all database connections which will be tested for TinyORM tests. */
        static QStringList createConnections(const QStringList &connections = {});
        /*! Create a database connection for TinyORM tests. */
        static QString createConnection(const QString &connection);

#ifdef TINYORM_USING_TINYDRIVERS
        /*! Create all database connections which will be tested for TinyDrivers tests. */
        static QStringList createDriversConnections(const QStringList &connections = {});
        /*! Create a database connection for TinyDrivers tests. */
        static QString createDriversConnection(const QString &connection);
#endif

        /* Create a connection for one test method */
        /*! Connection name parts to generate the connection name for one test method. */
        struct ConnectionNameParts
        {
            /*! Unit test case class name. */
            const char *className;
            /*! Unit test method name. */
            QString methodName;
        };

        /*! Create a temporary database connection for one test method, TinyORM tests. */
        static std::optional<QString>
        createConnectionTemp(
                const QString &connection, const ConnectionNameParts &connectionParts,
                const QVariantHash &configuration);
        /*! Create a temp. DB connection for one test method from config., TinyORM. */
        static std::optional<QString>
        createConnectionTempFrom(
                const QString &fromConfiguration,
                const ConnectionNameParts &connectionParts);
        /*! Create a temporary database connection for one test method from config. */
        static std::optional<QString>
        createConnectionTempFrom(
                const QString &fromConfiguration,
                const ConnectionNameParts &connectionParts,
                std::unordered_map<QString, QVariant> &&optionsToUpdate,
                const std::vector<QString> &optionsToRemove = {});

#ifdef TINYORM_USING_TINYDRIVERS
        /*! Create temp. database connection for one test method, TinyDrivers tests. */
        static std::optional<QString>
        createDriversConnectionTemp(
                const QString &connectionName, const ConnectionNameParts &connectionParts,
                const QVariantHash &configuration, bool open = true);
        /*! Create temp. DB connection for one test method from config., TinyDrivers. */
        static std::optional<QString>
        createDriversConnectionTempFrom(
                const QString &fromConfiguration,
                const ConnectionNameParts &connectionParts, bool open = true);
        /*! Create a temp. DB connection for one test method from config., TinyDrivers */
        static std::optional<QString>
        createDriversConnectionTempFrom(
                const QString &fromConfiguration,
                const ConnectionNameParts &connectionParts,
                std::unordered_map<QString, QVariant> &&optionsToUpdate,
                const std::vector<QString> &optionsToRemove = {},
                bool open = true);
#endif

        /*! Get a configuration for the given connection for XyzConnectionTemp methods. */
        static std::optional<std::reference_wrapper<const QVariantHash>>
        configurationForTemp(const QString &connection, bool forTinyDrivers);

        /*! Determine whether a configuration for the given connection was defined. */
        static bool hasConfiguration(const QString &connection);

        /* Common */
        /*! Remove a database connection for TinyORM tests. */
        static bool removeConnection(const QString &connection);
#ifdef TINYORM_USING_TINYDRIVERS
        /*! Remove a database connection for TinyDrivers tests. */
        static void removeDriversConnection(const QString &connection);

        /*! Obtain the given database connection for TinyDrivers tests. */
        static SqlDatabase driversConnection(const QString &connection, bool open = true);
        /*! Get all managed connection names for TinyDrivers tests. */
        static QStringList driversConnectionNames();
        /*! Get a list of opened connections for TinyDrivers tests. */
        static QStringList driversOpenedConnectionNames();
        /*! Determine if the same thread check during the database() call is enabled,
             for TinyDrivers tests. */
        static bool driversIsThreadCheck() noexcept;
#endif

        /*! Check whether environment variables are correctly set. */
        static bool envVariablesDefined(const std::vector<const char *> &envVariables);

        /*! Create the database manager instance. */
        static std::shared_ptr<DatabaseManager> createDatabaseManager();
        /*! Get a reference to the database manager. */
        static DatabaseManager &manager();
        /*! Get a database manager instance. */
        static std::shared_ptr<DatabaseManager> managerShared();

    private:
#ifdef TINYORM_USING_TINYDRIVERS
        /*! Create a database connection for TinyDrivers tests. */
        static void createDriversConnectionInternal(
                const QString &connection, const QVariantHash &configuration,
                bool open = true);

        /*! Create a MySQL database connection for TinyDrivers tests. */
        static void createDriversMySQLConnection(
                const QString &connection, const QVariantHash &configuration,
                const std::vector<const char *> &sslEnvVariables, bool open);
        /*! Create a MariaDB database connection for TinyDrivers tests. */
        inline static void createDriversMariaConnection(
                const QString &connection,
                const QVariantHash &configuration,
                const std::vector<const char *> &sslEnvVariables, bool open);

        /*! Get the SSL-related connection options for MySQL or MariaDB. */
        static QString createMySQLOrMariaSslOptions(const QVariantHash &configuration);
#endif

        /*! Create database configurations hash. */
        static const ConfigurationsType &
        createConfigurationsHash(const QStringList &connections, bool forTinyDrivers);
        /*! Compute the reserve size for the configurations hash. */
        static ConfigurationsType::size_type
        computeReserveForConfigurationsHash(const QStringList &connections);

        /*! Create MySQL configuration hash. */
        static std::pair<QVariantHash, bool>
        mysqlConfiguration();
        /*! Create MariaDB configuration hash. */
        static std::pair<QVariantHash, bool>
        mariaConfiguration();
        /*! Create SQLite configuration hash. */
        static std::pair<QVariantHash, bool>
        sqliteConfiguration();
        /*! Create PostgreSQL configuration hash. */
        static std::pair<QVariantHash, bool>
        postgresConfiguration();

        /*! Get all env. variable names for the given driver. */
        static const std::vector<const char *> &
        envVariables(const QString &driver, const QString &connection);

        /*! Get all MySQL environment variable names. */
        static const std::vector<const char *> &mysqlEnvVariables();
        /*! Get all MariaDB environment variable names. */
        static const std::vector<const char *> &mariaEnvVariables();
        /*! Get all SQLite environment variable names. */
        static const std::vector<const char *> &sqliteEnvVariables();
        /*! Get all PostgreSQL environment variable names. */
        static const std::vector<const char *> &postgresEnvVariables();

#ifdef TINYORM_USING_TINYDRIVERS
        /*! Get all MySQL SSL-related environment variable names for TinyDrivers tests. */
        static const std::vector<const char *> &mysqlSslEnvVariables();
        /*! Get all MariaDB SSL-related environment variable names, TinyDrivers tests. */
        static const std::vector<const char *> &mariaSslEnvVariables();
#endif

        /*! Check whether the given Qt driver is available. */
        static bool isDriverAvailable(const QString &driver);

        /*! Update options in the given configuration (for temporary connection). */
        static void
        updateConfigurationForTemp(
                QVariantHash &configuration,
                std::unordered_map<QString, QVariant> &&optionsToUpdate,
                const std::vector<QString> &optionsToRemove);

        /*! Throw an exception if the database manager instance was not yet created. */
        static void throwIfNoManagerInstance();
        /*! Throw an exception if database connections were already initialized. */
        static void throwIfConnectionsInitialized();

        /*! Shared pointer to the DatabaseManager instance. */
        static std::shared_ptr<DatabaseManager> m_dm;
        /*! Database configurations map. */
        static ConfigurationsType m_configurations;
    };

    /* private */

#ifdef TINYORM_USING_TINYDRIVERS
    /* MariaDB connection code is the same as for the MySQL connection, still leave this
       method as it has a better name (for better naming). */
    void Databases::createDriversMariaConnection(
            const QString &connection, const QVariantHash &configuration,
            const std::vector<const char *> &sslEnvVariables, const bool open)
    {
        createDriversMySQLConnection(connection, configuration, sslEnvVariables, open);
    }
#endif

} // namespace TestUtils

#endif // TINYUTILS_DATABASES_HPP
