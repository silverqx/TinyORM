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
    class TINYORM_EXPORT MySqlConnection final : public DatabaseConnection
    {
        Q_DISABLE_COPY_MOVE(MySqlConnection)

        /*! Private constructor. */
        explicit MySqlConnection(
                std::function<Connectors::ConnectionName()> &&connection,
                QString &&database = "", QString &&tablePrefix = "",
                QtTimeZoneConfig &&qtTimeZone = QtTimeZoneConfig::utc(),
                QVariantHash &&config = {});

    public:
        /*! Virtual destructor. */
        ~MySqlConnection() final = default;

        /*! Factory method for MySQL database connection. */
        [[nodiscard]] inline static
        std::shared_ptr<MySqlConnection>
        create(std::function<Connectors::ConnectionName()> &&connection,
               QString &&database = "", QString &&tablePrefix = "",
               QtTimeZoneConfig &&qtTimeZone = QtTimeZoneConfig::utc(),
               QVariantHash &&config = {});

        /* Getters / Setters */
        /*! Get the MySQL server version. */
        std::optional<QString> version();
        /*! Is currently connected the MariaDB database server? */
        bool isMaria();
        /*! Determine whether to use the upsert alias (by MySQL version >=8.0.19). */
        bool useUpsertAlias();
#ifdef TINYORM_TESTS_CODE
        /*! Override the version database configuration value. */
        void setConfigVersion(const QString &value);
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

    /* public */

    std::shared_ptr<MySqlConnection>
    MySqlConnection::create(
            std::function<Connectors::ConnectionName()> &&connection,
            QString &&database, QString &&tablePrefix, QtTimeZoneConfig &&qtTimeZone,
            QVariantHash &&config)
    {
        // Can't use the std::make_shared<> because the constructor is private
        return std::shared_ptr<MySqlConnection>(
                    new MySqlConnection(
                        std::move(connection),  std::move(database),
                        std::move(tablePrefix), std::move(qtTimeZone),
                        std::move(config)));
    }

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_MYSQLCONNECTION_HPP
