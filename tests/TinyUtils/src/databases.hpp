#pragma once
#ifndef TINYUTILS_DATABASES_HPP
#define TINYUTILS_DATABASES_HPP

#include <memory>

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
                           "for '%2' connection have not been defined.");
    /*! Template message for the QSKIP() for more connections. */
    inline const QString AutoTestSkippedAny =
            QStringLiteral("%1 autotest skipped, environment variables "
                           "for ANY connection have not been defined.");

    class TINYUTILS_EXPORT Databases
    {
        Q_DISABLE_COPY(Databases)

        /*! Alias for the DatabaseManager. */
        using DatabaseManager = Orm::DatabaseManager;

    public:
        /*! Type used for Database Connections map. */
        using ConfigurationsType = Orm::Support::DatabaseConfiguration
                                               ::ConfigurationsType;

        /*! MySQL connection name. */
        inline static const QString MYSQL      = "tinyorm_mysql_tests";
        /*! SQLite connection name. */
        inline static const QString SQLITE     = "tinyorm_sqlite_tests";
        /*! PostgreSQL connection name. */
        inline static const QString POSTGRESQL = "tinyorm_pgsql_tests";

        /*! Create all database connections which will be tested. */
        static const QStringList &
        createConnections(const QStringList &connections = {});
        /*! Create database connection. */
        static QString createConnection(const QString &connection);

        /*! Get a configuration for the given connection. */
        static std::optional<std::reference_wrapper<const QVariantHash>>
        configuration(const QString &connection);

        /*! Check whether all env. variables are empty. */
        static bool allEnvVariablesEmpty(const std::vector<const char *> &envVariables);

        /*! Get a reference to the database manager. */
        static const std::shared_ptr<Orm::DatabaseManager> &manager();

    private:
        /*! Create database configurations hash. */
        static const ConfigurationsType &
        createConfigurationsHash(const QStringList &connections);

        /*! Create MySQL configuration hash. */
        static std::pair<std::reference_wrapper<const QVariantHash>, bool>
        mysqlConfiguration();
        /*! Create SQLite configuration hash. */
        static std::pair<std::reference_wrapper<const QVariantHash>, bool>
        sqliteConfiguration();
        /*! Create PostgreSQL configuration hash. */
        static std::pair<std::reference_wrapper<const QVariantHash>, bool>
        postgresConfiguration();

        /*! Throw exception when database connections were already initialized. */
        static void throwIfConnectionsInitialized();

        /*! Shared pointer to the DatabaseManager instance. */
        static std::shared_ptr<Orm::DatabaseManager> m_instance;
        /*! Database configurations map. */
        static ConfigurationsType m_configurations;
    };

} // namespace TestUtils

#endif // TINYUTILS_DATABASES_HPP
