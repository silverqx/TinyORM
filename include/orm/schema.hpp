#pragma once
#ifndef ORM_SCHEMA_HPP
#define ORM_SCHEMA_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/databasemanager.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

    /*! Facade class for the Schema builder. */
    class TINYORM_EXPORT Schema final
    {
        Q_DISABLE_COPY_MOVE(Schema)

        /*! Alias for the Blueprint. */
        using Blueprint     = SchemaNs::Blueprint;
        /*! Alias for the schema builder. */
        using SchemaBuilder = SchemaNs::SchemaBuilder;

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Schema() = delete;
        /*! Deleted destructor. */
        ~Schema() = delete;

        /* Proxy methods to the SchemaBuilder */
        /*! Create a database in the schema. */
        static std::optional<SqlQuery>
        createDatabase(const QString &name, const QString &connection = "");
        /*! Drop a database from the schema if the database exists. */
        static std::optional<SqlQuery>
        dropDatabaseIfExists(const QString &name, const QString &connection = "");

        /*! Create a new table on the schema. */
        static void create(const QString &table,
                           const std::function<void(Blueprint &)> &callback,
                           const QString &connection = "");
        /*! Modify a table on the schema. */
        static void table(const QString &table,
                          const std::function<void(Blueprint &)> &callback,
                          const QString &connection = "");
        /*! Drop a table from the schema. */
        static void drop(const QString &table,
                         const QString &connection = "");
        /*! Drop a table from the schema if it exists. */
        static void dropIfExists(const QString &table,
                                 const QString &connection = "");

        /*! Rename a table on the schema. */
        static void rename(const QString &from, const QString &to,
                           const QString &connection = "");

        /*! Drop columns from a table schema. */
        static void dropColumns(const QString &table, const QList<QString> &columns,
                                const QString &connection = "");
        /*! Drop columns from a table schema. */
        template<QStringConcept ...Args>
        static void dropColumns(const QString &table, Args &&...columns);
        /*! Drop one column from a table schema. */
        static void dropColumn(const QString &table, const QString &column,
                               const QString &connection = "");

        /*! Rename the given column on the schema. */
        static void renameColumn(const QString &table, const QString &from,
                                 const QString &to, const QString &connection = "");

        /*! Drop all tables from the database. */
        static void dropAllTables(const QString &connection = "");
        /*! Drop all views from the database. */
        static void dropAllViews(const QString &connection = "");
        /*! Drop all types from the database. */
        static void dropAllTypes(const QString &connection = "");

        /*! Get all of the table names for the database. */
        static SqlQuery getAllTables(const QString &connection = "");
        /*! Get all of the view names for the database. */
        static SqlQuery getAllViews(const QString &connection = "");

        /*! Enable foreign key constraints. */
        static SqlQuery enableForeignKeyConstraints(const QString &connection = "");
        /*! Disable foreign key constraints. */
        static SqlQuery disableForeignKeyConstraints(const QString &connection = "");
        /*! Disable foreign key constraints during the execution of a callback. */
        static void withoutForeignKeyConstraints(const std::function<void()> &callback,
                                                 const QString &connection = "");

        /*! Get the column listing for a given table. */
        static QStringList getColumnListing(const QString &table,
                                            const QString &connection = "");

        /*! Determine if the given table exists. */
        static bool hasTable(const QString &table, const QString &connection = "");
        /*! Determine if the given table has a given column. */
        static bool hasColumn(const QString &table, const QString &column,
                              const QString &connection = "");
        /*! Determine if the given table has given columns. */
        static bool hasColumns(const QString &table, const QList<QString> &columns,
                               const QString &connection = "");

        /* Schema */
        /*! Releases the ownership of the DatabaseManager managed object. */
        static void free() noexcept;

        /*! Get a schema builder instance for the default connection. */
        static SchemaBuilder &connection(const QString &name = "");
        /*! Get a schema builder instance for a connection. (alias for the connection()
            method). */
        static SchemaBuilder &on(const QString &name);

        /* Others */
        /*! Set the default string length for migrations. */
        static void defaultStringLength(int length);

    private:
        /*! Get a reference to the SchemaBuilder. */
        static SchemaBuilder &schemaBuilder(const QString &connection = "");
        /*! Get a reference to the DatabaseManager. */
        static DatabaseManager &manager();

        /*! Pointer to the DatabaseManager. */
        static std::shared_ptr<DatabaseManager> m_manager;
    };

    /* public */

    template<QStringConcept ...Args>
    void Schema::dropColumns(const QString &table, Args &&...columns)
    {
        dropColumns(table, {std::forward<Args>(columns)...});
    }

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_HPP
