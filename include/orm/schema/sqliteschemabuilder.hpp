#pragma once
#ifndef ORM_SQLITESCHEMABUILDER_HPP
#define ORM_SQLITESCHEMABUILDER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/schema/schemabuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Schema
{

    /*! SQLite schema repository class. */
    class SHAREDLIB_EXPORT SQLiteSchemaBuilder : public SchemaBuilder
    {
        Q_DISABLE_COPY(SQLiteSchemaBuilder)

    public:
        using SchemaBuilder::SchemaBuilder;

        /*! Virtual destructor. */
        inline ~SQLiteSchemaBuilder() override = default;
    };

} // namespace Orm::Schema

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SQLITESCHEMABUILDER_HPP
