#pragma once
#ifndef SQLITESCHEMABUILDER_HPP
#define SQLITESCHEMABUILDER_HPP

#include "orm/schema/schemabuilder.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Schema
{

    /*! SQLite schema repository class. */
    class SHAREDLIB_EXPORT SQLiteSchemaBuilder : public SchemaBuilder
    {
        Q_DISABLE_COPY(SQLiteSchemaBuilder)

    public:
        using SchemaBuilder::SchemaBuilder;
    };

} // namespace Orm::Schema
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // SQLITESCHEMABUILDER_HPP
