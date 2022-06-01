#pragma once
#ifndef ORM_SCHEMA_SCHEMABUILDER_HPP
#define ORM_SCHEMA_SCHEMABUILDER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtSql/QSqlQuery>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"
// Include the blueprint here so a user doesn't have to (it can be forward declared)
#include "orm/schema/blueprint.hpp"

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
    class SHAREDLIB_EXPORT SchemaBuilder
    {
        Q_DISABLE_COPY(SchemaBuilder)

    public:
        /*! Constructor. */
        explicit SchemaBuilder(DatabaseConnection &connection);
        /*! Virtual destructor. */
        inline virtual ~SchemaBuilder() = default;

        /*! Create a database in the schema. */
        virtual QSqlQuery createDatabase(const QString &name) const;
        /*! Drop a database from the schema if the database exists. */
        virtual QSqlQuery dropDatabaseIfExists(const QString &name) const;

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
        void dropColumns(const QString &table, const QVector<QString> &columns) const;
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
        virtual QSqlQuery getAllTables() const;
        /*! Get all of the view names for the database. */
        virtual QSqlQuery getAllViews() const;

        /*! Enable foreign key constraints. */
        QSqlQuery enableForeignKeyConstraints() const;
        /*! Disable foreign key constraints. */
        QSqlQuery disableForeignKeyConstraints() const;

        /*! Get the column listing for a given table. */
        virtual QStringList getColumnListing(const QString &table) const;

        /*! Determine if the given table exists. */
        virtual bool hasTable(const QString &table) const;
        /*! Determine if the given table has a given column. */
        bool hasColumn(const QString &table, const QString &column) const;
        /*! Determine if the given table has given columns. */
        bool hasColumns(const QString &table, const QVector<QString> &columns) const;

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
        DatabaseConnection &m_connection;
        /*! The schema grammar instance. */
        const SchemaGrammar &m_grammar;
    };

    template<QStringConcept ...Args>
    void SchemaBuilder::dropColumns(const QString &table, Args &&...columns)
    {
        dropColumns(table, {std::forward<Args>(columns)...});
    }

    DatabaseConnection &SchemaBuilder::getConnection()
    {
        return m_connection;
    }

    const DatabaseConnection &SchemaBuilder::getConnection() const
    {
        return m_connection;
    }

} // namespace SchemaNs
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_SCHEMABUILDER_HPP
