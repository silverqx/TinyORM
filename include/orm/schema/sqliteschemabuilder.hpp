#pragma once
#ifndef ORM_SCHEMA_SQLITESCHEMABUILDER_HPP
#define ORM_SCHEMA_SQLITESCHEMABUILDER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/schema/schemabuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{

    /*! SQLite schema builder class. */
    class SQLiteSchemaBuilder : public SchemaBuilder
    {
        Q_DISABLE_COPY(SQLiteSchemaBuilder)

    public:
        /*! Inherit constructors. */
        using SchemaBuilder::SchemaBuilder;

        /*! Virtual destructor. */
        inline ~SQLiteSchemaBuilder() override = default;
    };

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_SQLITESCHEMABUILDER_HPP
