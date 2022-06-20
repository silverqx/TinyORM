#pragma once
#ifndef ORM_SCHEMA_POSTGRESSCHEMABUILDER_HPP
#define ORM_SCHEMA_POSTGRESSCHEMABUILDER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/schema/schemabuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{

    /*! PostgreSql schema builder class. */
    class SHAREDLIB_EXPORT PostgresSchemaBuilder : public SchemaBuilder
    {
        Q_DISABLE_COPY(PostgresSchemaBuilder)

    public:
        /*! Inherit constructors. */
        using SchemaBuilder::SchemaBuilder;

        /*! Virtual destructor. */
        inline ~PostgresSchemaBuilder() override = default;

        /*! Create a database in the schema. */
        std::optional<QSqlQuery> createDatabase(const QString &name) const override;
        /*! Drop a database from the schema if the database exists. */
        std::optional<QSqlQuery>
        dropDatabaseIfExists(const QString &name) const override;

        /*! Drop all tables from the database. */
        void dropAllTables() const override;
        /*! Drop all views from the database. */
        void dropAllViews() const override;

        /*! Get all of the table names for the database. */
        QSqlQuery getAllTables() const override;
        /*! Get all of the view names for the database. */
        QSqlQuery getAllViews() const override;

        /*! Get the column listing for a given table. */
        QStringList getColumnListing(const QString &table) const override;

        /*! Determine if the given table exists. */
        bool hasTable(const QString &table) const override;

    protected:
        /*! Parse the table name and extract the schema and table. */
        std::tuple<QString, QString>
        parseSchemaAndTable(const QString &table) const;
    };

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_POSTGRESSCHEMABUILDER_HPP
