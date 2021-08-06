#pragma once
#ifndef MYSQLSCHEMABUILDER_H
#define MYSQLSCHEMABUILDER_H

#include "orm/schema/schemabuilder.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Schema
{

    /*! MySql schema repository class. */
    class SHAREDLIB_EXPORT MySqlSchemaBuilder : public SchemaBuilder
    {
        Q_DISABLE_COPY(MySqlSchemaBuilder)

    public:
        using SchemaBuilder::SchemaBuilder;

        /*! Get the column listing for a given table. */
        QStringList getColumnListing(const QString &table) const override;
    };

} // namespace Orm::Schema
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // MYSQLSCHEMABUILDER_H
