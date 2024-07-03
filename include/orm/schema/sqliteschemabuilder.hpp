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
    class TINYORM_EXPORT SQLiteSchemaBuilder : public SchemaBuilder
    {
        Q_DISABLE_COPY_MOVE(SQLiteSchemaBuilder)

    public:
        /*! Inherit constructors. */
        using SchemaBuilder::SchemaBuilder;

        /*! Virtual destructor. */
        ~SQLiteSchemaBuilder() override = default;

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

        /*! Empty the database file. */
        void refreshDatabaseFile() const;
    };

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_SQLITESCHEMABUILDER_HPP
