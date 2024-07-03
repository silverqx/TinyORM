#pragma once
#ifndef ORM_SCHEMA_POSTGRESSCHEMABUILDER_HPP
#define ORM_SCHEMA_POSTGRESSCHEMABUILDER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/concerns/parsessearchpath.hpp"
#include "orm/schema/schemabuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{
namespace Grammars
{
    class PostgresSchemaGrammar;
}

    /*! PostgreSQL schema builder class. */
    class TINYORM_EXPORT PostgresSchemaBuilder : public SchemaBuilder,
                                                 protected Concerns::ParsesSearchPath
    {
        Q_DISABLE_COPY_MOVE(PostgresSchemaBuilder)

        /*! Alias for the PostgresSchemaGrammar. */
        using PostgresSchemaGrammar = Grammars::PostgresSchemaGrammar;

    public:
        /*! Inherit constructors. */
        using SchemaBuilder::SchemaBuilder;

        /*! Virtual destructor. */
        ~PostgresSchemaBuilder() override = default;

        /*! Create a database in the schema. */
        std::optional<SqlQuery> createDatabase(const QString &name) const override;
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

    protected:
        /*! Parse the database object reference and extract the database, schema,
            and table. */
        std::tuple<QString, QString, QString>
        parseSchemaAndTable(const QString &reference) const;

    private:
        /*! Drop a database name for the parseSchemaAndTable(). */
        static void dropDatabaseForParse(const QString &databaseConfig,
                                         QStringList &parts, const QString &connection);
        /*! Throw if the database name differs from a database in the configuration. */
        static void throwIfDatabaseDiffers(const QString &databaseConfig,
                                           QStringList &parts, const QString &connection);
        /*! Get a schema for the parseSchemaAndTable(). */
        QString getSchemaForParse(QStringList &parts, const QString &connection) const;
        /*! Throw if the database 'search_path' is empty. */
        static void throwIfEmptySearchPath(const QStringList &searchPath,
                                           const QString &connection);

        /*! Get a set of excluded tables from the 'dont_drop' configuration option. */
        QSet<QString> excludedTables() const;
        /*! Get a set of excluded views (hardcoded). */
        const QSet<QString> &excludedViews() const;
        /*! Get the 0 and qualifiedname column values from the given query. */
        static std::tuple<QString, QString> columnValuesForDrop(TSqlQuery &query);

        /*! Get the PostgreSQL server 'search_path' for the current connection
            (with the "$user" variable resolved as the config['username']). */
        QStringList searchPath() const;
        /*! Get PostgreSQL schema grammar. */
        const PostgresSchemaGrammar &grammar() const;
    };

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_POSTGRESSCHEMABUILDER_HPP
