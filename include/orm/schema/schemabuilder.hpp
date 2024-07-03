#pragma once
#ifndef ORM_SCHEMA_SCHEMABUILDER_HPP
#define ORM_SCHEMA_SCHEMABUILDER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

// Include the blueprint here so the user doesn't have to (it can be forward declared)
#include "orm/schema/blueprint.hpp"
#include "orm/types/sqlquery.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
    class DatabaseConnection;

namespace SchemaNs
{
namespace Grammars
{
    class SchemaGrammar;
}

    /*! Database schema builder base class. */
    class TINYORM_EXPORT SchemaBuilder
    {
        Q_DISABLE_COPY_MOVE(SchemaBuilder)

    public:
        /*! Constructor. */
        explicit SchemaBuilder(std::shared_ptr<DatabaseConnection> connection);
        /*! Virtual destructor, this class is used so can not be pure. */
        virtual ~SchemaBuilder() = default;

        /*! Create a database in the schema. */
        virtual std::optional<SqlQuery>
        createDatabase(const QString &name) const;
        /*! Drop a database from the schema if the database exists. */
        virtual std::optional<SqlQuery>
        dropDatabaseIfExists(const QString &name) const;

        /*! Create a new table on the schema. */
        void create(const QString &table,
                    const std::function<void(Blueprint &)> &callback) const;
        /*! Modify a table on the schema. */
        void table(const QString &table,
                   const std::function<void(Blueprint &)> &callback) const;

        /*! Drop a table from the schema. */
        void drop(const QString &table) const;
        /*! Drop a table from the schema if it exists. */
        void dropIfExists(const QString &table) const;

        /*! Rename a table on the schema. */
        void rename(const QString &from, const QString &to) const;

        /*! Drop columns from a table schema. */
        void dropColumns(const QString &table, const QList<QString> &columns) const;
        /*! Drop columns from a table schema. */
        template<QStringConcept ...Args>
        void dropColumns(const QString &table, Args &&...columns);
        /*! Drop one column from a table schema. */
        void dropColumn(const QString &table, const QString &column) const;

        /*! Rename the given column on the schema. */
        void renameColumn(const QString &table, const QString &from, const QString &to);

        /*! Drop all tables from the database. */
        virtual void dropAllTables() const;
        /*! Drop all views from the database. */
        virtual void dropAllViews() const;
        /*! Drop all types from the database. */
        virtual void dropAllTypes() const;

        /*! Get all of the table names for the database. */
        virtual SqlQuery getAllTables() const;
        /*! Get all of the view names for the database. */
        virtual SqlQuery getAllViews() const;

        /*! Enable foreign key constraints. */
        SqlQuery enableForeignKeyConstraints() const;
        /*! Disable foreign key constraints. */
        SqlQuery disableForeignKeyConstraints() const;
        /*! Disable foreign key constraints during the execution of a callback. */
        void withoutForeignKeyConstraints(const std::function<void()> &callback) const;

        /*! Get the column listing for a given table. */
        virtual QStringList getColumnListing(const QString &table) const;

        /*! Determine if the given table exists. */
        virtual bool hasTable(const QString &table) const;
        /*! Determine if the given table has a given column. */
        bool hasColumn(const QString &table, const QString &column) const;
        /*! Determine if the given table has given columns. */
        bool hasColumns(const QString &table, const QList<QString> &columns) const;

        /* Getters */
        /*! Get the database connection reference. */
        inline DatabaseConnection &getConnection();
        /*! Get the database connection reference, const version. */
        inline const DatabaseConnection &getConnection() const;

    protected:
        using SchemaGrammar = Grammars::SchemaGrammar;

        /*! Create a new command set with a Closure. */
        Blueprint
        createBlueprint(const QString &table,
                        const std::function<void(Blueprint &)> &callback = nullptr) const;
        /*! Execute the blueprint to build / modify the table. */
        void build(Blueprint &&blueprint) const;

        /*! The database connection instance. */
        std::shared_ptr<DatabaseConnection> m_connection;
        /*! The schema grammar instance. */
        std::shared_ptr<SchemaGrammar> m_grammar;
    };

    /* public */

    template<QStringConcept ...Args>
    void SchemaBuilder::dropColumns(const QString &table, Args &&...columns)
    {
        dropColumns(table, {std::forward<Args>(columns)...});
    }

    DatabaseConnection &SchemaBuilder::getConnection()
    {
        return *m_connection;
    }

    const DatabaseConnection &SchemaBuilder::getConnection() const
    {
        return *m_connection;
    }

} // namespace SchemaNs
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_SCHEMABUILDER_HPP
