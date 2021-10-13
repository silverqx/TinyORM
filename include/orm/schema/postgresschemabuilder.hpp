#pragma once
#ifndef POSTGRESSCHEMABUILDER_HPP
#define POSTGRESSCHEMABUILDER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/schema/schemabuilder.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Schema
{

    /*! PostgreSql schema repository class. */
    class SHAREDLIB_EXPORT PostgresSchemaBuilder : public SchemaBuilder
    {
        Q_DISABLE_COPY(PostgresSchemaBuilder)

    public:
        using SchemaBuilder::SchemaBuilder;

        /*! Get the column listing for a given table. */
        QStringList getColumnListing(const QString &table) const override;

    protected:
        /*! Parse the table name and extract the schema and table. */
        std::pair<QString, QString>
        parseSchemaAndTable(const QString &table) const;
    };

} // namespace Orm::Schema
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // POSTGRESSCHEMABUILDER_HPP
