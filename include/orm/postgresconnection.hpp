#pragma once
#ifndef ORM_POSTGRESCONNECTION_HPP
#define ORM_POSTGRESCONNECTION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/concerns/parsessearchpath.hpp"
#include "orm/databaseconnection.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

    /*! PostgreSQL database connection. */
    class TINYORM_EXPORT PostgresConnection final : public DatabaseConnection,
                                                    public Concerns::ParsesSearchPath
    {
        Q_DISABLE_COPY_MOVE(PostgresConnection)

        /*! Private constructor. */
        explicit PostgresConnection(
                std::function<Connectors::ConnectionName()> &&connection,
                QString &&database = "", QString &&tablePrefix = "",
                QtTimeZoneConfig &&qtTimeZone = QtTimeZoneConfig::utc(),
                QVariantHash &&config = {});

    public:
        /*! Virtual destructor. */
        ~PostgresConnection() final = default;

        /*! Factory method for PostgreSQL database connection. */
        [[nodiscard]] inline static
        std::shared_ptr<PostgresConnection>
        create(std::function<Connectors::ConnectionName()> &&connection,
               QString &&database = "", QString &&tablePrefix = "",
               QtTimeZoneConfig &&qtTimeZone = QtTimeZoneConfig::utc(),
               QVariantHash &&config = {});

        /* Getters */
        /*! Get the PostgreSQL server 'search_path' for the current connection
            (with the "$user" variable resolved as the config['username']). */
        QStringList searchPath(bool flushCache = false);
        /*! Get the PostgreSQL server search_path for the current connection
            (without resolving the "$user" variable). */
        QStringList searchPathRaw(bool flushCache = false);

    protected:
        /*! Get the default query grammar instance. */
        std::unique_ptr<QueryGrammar> getDefaultQueryGrammar() const final;
        /*! Get the default schema grammar instance. */
        std::unique_ptr<SchemaGrammar> getDefaultSchemaGrammar() final;
        /*! Get the default schema builder instance. */
        std::unique_ptr<SchemaBuilder> getDefaultSchemaBuilder() final;
        /*! Get the default post processor instance. */
        std::unique_ptr<QueryProcessor> getDefaultPostProcessor() const final;

    private:
        /*! Get the PostgreSQL server 'search_path' (for pretend mode). */
        QStringList searchPathRawForPretending() const;
        /*! Obtain the 'search_path' from the PostgreSQL database. */
        QStringList searchPathRawDb();

        /*! The PostgreSQL server 'search_path' for the current connection. */
        std::optional<QStringList> m_searchPath = std::nullopt;
    };

    /* public */

    std::shared_ptr<PostgresConnection>
    PostgresConnection::create(
            std::function<Connectors::ConnectionName()> &&connection,
            QString &&database, QString &&tablePrefix, QtTimeZoneConfig &&qtTimeZone,
            QVariantHash &&config)
    {
        // Can't use the std::make_shared<> because the constructor is private
        return std::shared_ptr<PostgresConnection>(
                    new PostgresConnection(
                        std::move(connection),  std::move(database),
                        std::move(tablePrefix), std::move(qtTimeZone),
                        std::move(config)));
    }

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_POSTGRESCONNECTION_HPP
