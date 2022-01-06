#pragma once
#ifndef ORM_SCHEMA_MYSQLSCHEMABUILDER_HPP
#define ORM_SCHEMA_MYSQLSCHEMABUILDER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/schema/schemabuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Schema
{

    /*! MySql schema repository class. */
    class SHAREDLIB_EXPORT MySqlSchemaBuilder : public SchemaBuilder
    {
        Q_DISABLE_COPY(MySqlSchemaBuilder)

    public:
        /*! Inherit constructors. */
        using SchemaBuilder::SchemaBuilder;

        /*! Virtual destructor. */
        inline ~MySqlSchemaBuilder() override = default;

        /*! Get the column listing for a given table. */
        QStringList getColumnListing(const QString &table) const override;
    };

} // namespace Orm::Schema

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_MYSQLSCHEMABUILDER_HPP
