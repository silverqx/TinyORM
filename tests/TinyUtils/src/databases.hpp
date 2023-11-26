#pragma once
#ifndef TINYUTILS_DATABASES_HPP
#define TINYUTILS_DATABASES_HPP

#include <memory>
#include <optional>

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
    inline const QString AutoTestSkipped =
            QStringLiteral("%1 autotest skipped, environment variables "
                           "for '%2' connection have not been defined or the Qt sql "
                           "driver is not available.");
    /*! Template message for the QSKIP() for more connections. */
    inline const QString AutoTestSkippedAny =
            QStringLiteral("%1 autotest skipped, environment variables "
                           "for ANY connection have not been defined.");

    /*! Database connections factory for unit tests (library class). */
    class TINYUTILS_EXPORT Databases
    {
        Q_DISABLE_COPY_MOVE(Databases)

        /*! Alias for the DatabaseManager. */
        using DatabaseManager = Orm::DatabaseManager;

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Databases() = delete;
        /*! Deleted destructor. */
        ~Databases() = delete;

        /*! Type used for Database Connections map. */
        using ConfigurationsType = Orm::Support::DatabaseConfiguration
                                               ::ConfigurationsType;

        /*! MySQL connection name. */
        inline static const QString MYSQL      = "tinyorm_mysql_tests";
        /*! MariaDB connection name. */
        inline static const QString MARIADB    = "tinyorm_maria_tests";
        /*! SQLite connection name. */
        inline static const QString SQLITE     = "tinyorm_sqlite_tests";
        /*! PostgreSQL connection name. */
        inline static const QString POSTGRESQL = "tinyorm_pgsql_tests";

        /* Create connection/s for the whole unit test case */
        /*! Create all database connections which will be tested. */
        static QStringList createConnections(const QStringList &connections = {});
        /*! Create a database connection. */
        static QString createConnection(const QString &connection);

        /* Create a connection for one test method */
        /*! Connection name parts to generate the connection name for one test method. */
        struct ConnectionNameParts
        {
            /*! Unit test case class name. */
            const char *className;
            /*! Unit test method name. */
            QString methodName;
        };

        /*! Create a temporary database connection for one test method. */
        static std::optional<QString>
        createConnectionTemp(
                const QString &connectionName, const ConnectionNameParts &connectionParts,
                const QVariantHash &configuration);
        /*! Create a temporary database connection for one test method from config. */
        static std::optional<QString>
        createConnectionTempFrom(
                const QString &fromConfiguration, const ConnectionNameParts &connection);
        /*! Create a temporary database connection for one test method from config. */
        static std::optional<QString>
        createConnectionTempFrom(
                const QString &fromConfiguration, const ConnectionNameParts &connection,
                std::unordered_map<QString, QVariant> &&optionsToUpdate,
                const std::vector<QString> &optionsToRemove = {});

        /*! Get a configuration for the given connection. */
        static std::optional<std::reference_wrapper<const QVariantHash>>
        configuration(const QString &connection);

        /*! Determine whether a configuration for the given connection was defined. */
        static bool hasConfiguration(const QString &connection);

        /* Common */
        /*! Remove a database connection. */
        static bool removeConnection(const QString &connection);

        /*! Check whether environment variables are correctly set. */
        static bool envVariablesDefined(const std::vector<const char *> &envVariables);

        /*! Get a reference to the database manager. */
        static Orm::DatabaseManager &manager();
        /*! Get a database manager instance. */
        static std::shared_ptr<Orm::DatabaseManager> managerShared();

    private:
        /*! Create database configurations hash. */
        static const ConfigurationsType &
        createConfigurationsHash(const QStringList &connections);

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
        static std::shared_ptr<Orm::DatabaseManager> m_dm;
        /*! Database configurations map. */
        static ConfigurationsType m_configurations;
    };

} // namespace TestUtils

#endif // TINYUTILS_DATABASES_HPP
