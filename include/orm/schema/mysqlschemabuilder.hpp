#pragma once
#ifndef ORM_SCHEMA_MYSQLSCHEMABUILDER_HPP
#define ORM_SCHEMA_MYSQLSCHEMABUILDER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/schema/schemabuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{

    /*! MySQL schema builder class. */
    class TINYORM_EXPORT MySqlSchemaBuilder : public SchemaBuilder
    {
        Q_DISABLE_COPY_MOVE(MySqlSchemaBuilder)

    public:
        /*! Inherit constructors. */
        using SchemaBuilder::SchemaBuilder;

        /*! Virtual destructor. */
        ~MySqlSchemaBuilder() override = default;

        /*! Create a database in the schema. */
        std::optional<SqlQuery>
        createDatabase(const QString &name) const override;
        /*! Drop a database from the schema if the database exists. */
        std::optional<SqlQuery>
        dropDatabaseIfExists(const QString &name) const override;

        /*! Drop all tables from the database. */
        void dropAllTables() const override;
        /*! Drop all views from the database. */
        void dropAllViews() const override;

        /*! Get all of the table names for the database. */
        SqlQuery getAllTables() const override;
        /*! Get all of the view names for the database. */
        SqlQuery getAllViews() const override;

        /*! Get the column listing for a given table. */
        QStringList getColumnListing(const QString &table) const override;

        /*! Determine if the given table exists. */
        bool hasTable(const QString &table) const override;
    };

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_MYSQLSCHEMABUILDER_HPP
