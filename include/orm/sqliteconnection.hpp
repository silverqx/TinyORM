#pragma once
#ifndef ORM_SQLITECONNECTION_HPP
#define ORM_SQLITECONNECTION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/databaseconnection.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

    /*! SQLite database connection. */
    class TINYORM_EXPORT SQLiteConnection final : public DatabaseConnection
    {
        Q_DISABLE_COPY_MOVE(SQLiteConnection)

        /*! Private constructor. */
        explicit SQLiteConnection(
                std::function<Connectors::ConnectionName()> &&connection,
                QString &&database = "", QString &&tablePrefix = "",
                QtTimeZoneConfig &&qtTimeZone = QtTimeZoneConfig::utc(),
                std::optional<bool> returnQDateTime = true,
                QVariantHash &&config = {});

    public:
        /*! Virtual destructor. */
        ~SQLiteConnection() final = default;

        /*! Factory method for SQLite database connection. */
        [[nodiscard]] inline static
        std::shared_ptr<SQLiteConnection>
        create(std::function<Connectors::ConnectionName()> &&connection,
               QString &&database = "", QString &&tablePrefix = "",
               QtTimeZoneConfig &&qtTimeZone = QtTimeZoneConfig::utc(),
               std::optional<bool> returnQDateTime = true,
               QVariantHash &&config = {});

        /*! Determine whether to return the QDateTime/QDate or QString (SQLite only). */
        inline bool returnQDateTime() const noexcept;
        /*! Set return the QDateTime/QDate or QString (override the return_qdatetime). */
        SQLiteConnection &setReturnQDateTime(bool value);

    protected:
        /*! Get the default query grammar instance. */
        std::unique_ptr<QueryGrammar> getDefaultQueryGrammar() const final;
        /*! Get the default schema grammar instance. */
        std::unique_ptr<SchemaGrammar> getDefaultSchemaGrammar() final;
        /*! Get the default schema builder instance. */
        std::unique_ptr<SchemaBuilder> getDefaultSchemaBuilder() final;
        /*! Get the default post processor instance. */
        std::unique_ptr<QueryProcessor> getDefaultPostProcessor() const final;
    };

    /* public */

    std::shared_ptr<SQLiteConnection>
    SQLiteConnection::create(
            std::function<Connectors::ConnectionName()> &&connection,
            QString &&database, QString &&tablePrefix, QtTimeZoneConfig &&qtTimeZone,
            const std::optional<bool> returnQDateTime, QVariantHash &&config)
    {
        // Can't use the std::make_shared<> because the constructor is private
        return std::shared_ptr<SQLiteConnection>(
                    new SQLiteConnection(
                        std::move(connection),      std::move(database),
                        std::move(tablePrefix),     std::move(qtTimeZone),
                        returnQDateTime,            std::move(config)));
    }

    bool SQLiteConnection::returnQDateTime() const noexcept
    {
        Q_ASSERT(m_returnQDateTime);

        return m_returnQDateTime.value_or(true);
    }

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SQLITECONNECTION_HPP
