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
    class SHAREDLIB_EXPORT SQLiteConnection final : public DatabaseConnection
    {
        Q_DISABLE_COPY(SQLiteConnection)

    public:
        /*! Constructor. */
        explicit SQLiteConnection(
                std::function<Connectors::ConnectionName()> &&connection,
                QString &&database = "", QString &&tablePrefix = "",
                QtTimeZoneConfig &&qtTimeZone = {QtTimeZoneType::DontConvert},
                std::optional<bool> &&returnQDateTime = true,
                QVariantHash &&config = {});
        /*! Virtual destructor. */
        inline ~SQLiteConnection() final = default;

        /*! Get a schema builder instance for the connection. */
        std::unique_ptr<SchemaBuilder> getSchemaBuilder() final;

        /*! Determine whether to return the QDateTime or QString (SQLite only). */
        inline bool returnQDateTime() const noexcept;
        /*! Set return the QDateTime or QString (override the return_qdatetime). */
        SQLiteConnection &setReturnQDateTime(bool value);

    protected:
        /*! Get the default query grammar instance. */
        std::unique_ptr<QueryGrammar> getDefaultQueryGrammar() const final;
        /*! Get the default schema grammar instance. */
        std::unique_ptr<SchemaGrammar> getDefaultSchemaGrammar() const final;
        /*! Get the default post processor instance. */
        std::unique_ptr<QueryProcessor> getDefaultPostProcessor() const final;
    };

    /* public */

    bool SQLiteConnection::returnQDateTime() const noexcept
    {
        Q_ASSERT(m_returnQDateTime);

        return m_returnQDateTime.value_or(true);
    }

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SQLITECONNECTION_HPP
