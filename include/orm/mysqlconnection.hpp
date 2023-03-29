#pragma once
#ifndef ORM_MYSQLCONNECTION_HPP
#define ORM_MYSQLCONNECTION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/databaseconnection.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

    /*! MySQL database connection. */
    class SHAREDLIB_EXPORT MySqlConnection final : public DatabaseConnection
    {
        Q_DISABLE_COPY(MySqlConnection)

    public:
        /*! Constructor. */
        explicit MySqlConnection(
                std::function<Connectors::ConnectionName()> &&connection,
                QString &&database = "", QString &&tablePrefix = "",
                QtTimeZoneConfig &&qtTimeZone = {QtTimeZoneType::DontConvert},
                QVariantHash &&config = {});
        /*! Virtual destructor. */
        inline ~MySqlConnection() final = default;

        /* Getters / Setters */
        /*! Get the MySQL server version. */
        std::optional<QString> version();
        /*! Is currently connected the MariaDB database server? */
        bool isMaria();
        /*! Determine whether to use the upsert alias (by MySQL version >=8.0.19). */
        bool useUpsertAlias();
#ifdef TINYORM_TESTS_CODE
        /*! Override the version database configuration value. */
        void setConfigVersion(QString value);
#endif

        /* Others */
        /*! Check database connection and show warnings when the state changed.
            MySQL reconnection logic is disabled (MYSQL_OPT_RECONNECT), TinyORM has
            own reconnector. */
        bool pingDatabase() final;

    protected:
        /*! Get the default query grammar instance. */
        std::unique_ptr<QueryGrammar> getDefaultQueryGrammar() const final;
        /*! Get the default schema grammar instance. */
        std::unique_ptr<SchemaGrammar> getDefaultSchemaGrammar() final;
        /*! Get the default schema builder instance. */
        std::unique_ptr<SchemaBuilder> getDefaultSchemaBuilder() final;
        /*! Get the default post processor instance. */
        std::unique_ptr<QueryProcessor> getDefaultPostProcessor() const final;

        /*! MySQL server version. */
        std::optional<QString> m_version = std::nullopt;
        /*! Is currently connected the MariaDB database server? */
        std::optional<bool> m_isMaria = std::nullopt;
        /*! Determine whether to use the upsert alias (by MySQL version >=8.0.19). */
        std::optional<bool> m_useUpsertAlias = std::nullopt;
    };

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_MYSQLCONNECTION_HPP
