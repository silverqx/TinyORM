#pragma once
#ifndef ORM_POSTGRESSCHEMABUILDER_HPP
#define ORM_POSTGRESSCHEMABUILDER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/schema/schemabuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Schema
{

    /*! PostgreSql schema repository class. */
    class SHAREDLIB_EXPORT PostgresSchemaBuilder : public SchemaBuilder
    {
        Q_DISABLE_COPY(PostgresSchemaBuilder)

    public:
        /*! Inherit constructors. */
        using SchemaBuilder::SchemaBuilder;

        /*! Virtual destructor. */
        inline ~PostgresSchemaBuilder() override = default;

        /*! Get the column listing for a given table. */
        QStringList getColumnListing(const QString &table) const override;

    protected:
        /*! Parse the table name and extract the schema and table. */
        std::pair<QString, QString>
        parseSchemaAndTable(const QString &table) const;
    };

} // namespace Orm::Schema

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_POSTGRESSCHEMABUILDER_HPP
