#pragma once
#ifndef SQLITESCHEMABUILDER_HPP
#define SQLITESCHEMABUILDER_HPP

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
    };

} // namespace Orm::Schema

TINYORM_END_COMMON_NAMESPACE

#endif // SQLITESCHEMABUILDER_HPP
