#pragma once
#ifndef TINYUTILS_DATABASES_HPP
#define TINYUTILS_DATABASES_HPP

#include "orm/configuration.hpp"
#include "orm/connectioninterface.hpp"

#include "utils_global.hpp"

namespace TestUtils
{

    class TINYUTILS_EXPORT Databases
    {
        Q_DISABLE_COPY(Databases)

        using ConfigurationsType = Orm::Configuration::ConfigurationsType;

    public:
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

    private:
        /*! Obtain configurations for the given connection names. */
        static const ConfigurationsType &
        getConfigurations(const QStringList &connections = {});

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

        /*! Throw exception when connections were already initialized. */
        static void checkInitialized();

        /*! Determines whether connections were initialized. */
        inline static bool m_initialized = false;
    };

}

#endif // TINYUTILS_DATABASES_HPP
