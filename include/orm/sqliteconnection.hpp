#pragma once
#ifndef SQLITECONNECTION_HPP
#define SQLITECONNECTION_HPP

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
        SQLiteConnection(
                std::function<Connectors::ConnectionName()> &&connection,
                const QString &database = "", const QString &tablePrefix = "",
                const QVariantHash &config = {});
        /*! Virtual destructor. */
        inline virtual ~SQLiteConnection() = default;

        /*! Get a schema builder instance for the connection. */
        std::unique_ptr<SchemaBuilder> getSchemaBuilder() override;

    protected:
        /*! Get the default query grammar instance. */
        std::unique_ptr<QueryGrammar> getDefaultQueryGrammar() const override;
        /*! Get the default schema grammar instance. */
        std::unique_ptr<SchemaGrammar> getDefaultSchemaGrammar() const override;
        /*! Get the default post processor instance. */
        std::unique_ptr<QueryProcessor> getDefaultPostProcessor() const override;
    };

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // SQLITECONNECTION_HPP
