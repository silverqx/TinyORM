#pragma once
#ifndef ORM_SCHEMA_GRAMMARS_SCHEMAGRAMMAR_HPP
#define ORM_SCHEMA_GRAMMARS_SCHEMAGRAMMAR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>

#include "orm/basegrammar.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{
    class BasicCommand;
    class Blueprint;
    class ColumnDefinition;
    class CommandDefinition;
    class ForeignKeyCommand;
    class IndexCommand;
    class TableCommentCommand;

namespace Grammars
{

    /*! Database schema grammar base class. */
    class TINYORM_EXPORT SchemaGrammar : public BaseGrammar
    {
        Q_DISABLE_COPY_MOVE(SchemaGrammar)

    public:
        /*! Default constructor. */
        SchemaGrammar() = default;
        /*! Pure virtual destructor. */
        inline ~SchemaGrammar() override = 0;

        /*! Check if this Grammar supports schema changes wrapped in a transaction. */
        virtual bool supportsSchemaTransactions() const noexcept = 0;

        /* Compile methods for the SchemaBuilder */
        /*! Compile a create database command. */
        virtual QString compileCreateDatabase(const QString &name,
                                              DatabaseConnection &connection) const;
        /*! Compile a drop database if exists command. */
        virtual QString compileDropDatabaseIfExists(const QString &name) const;

        /*! Compile the SQL needed to drop all tables. */
        virtual QString compileDropAllTables(const QList<QString> &tables) const;
        /*! Compile the SQL needed to drop all views. */
        virtual QString compileDropAllViews(const QList<QString> &views) const;

        /*! Compile the SQL needed to retrieve all table names. */
        virtual QString compileGetAllTables(const QList<QString> &databases = {}) const; // NOLINT(google-default-arguments)
        /*! Compile the SQL needed to retrieve all view names. */
        virtual QString compileGetAllViews(const QList<QString> &databases = {}) const; // NOLINT(google-default-arguments)

        /*! Compile the command to enable foreign key constraints. */
        virtual QString compileEnableForeignKeyConstraints() const = 0;
        /*! Compile the command to disable foreign key constraints. */
        virtual QString compileDisableForeignKeyConstraints() const = 0;

        /*! Compile the query to determine the list of tables. */
        virtual QString compileTableExists() const;
        /*! Compile the query to determine the list of columns. */
        virtual QString compileColumnListing(const QString &table = "") const = 0; // NOLINT(google-default-arguments)

        /* Compile methods for commands */
        /*! Compile a change column command. */
        QList<QString> compileChange(const Blueprint &blueprint,
                                     const BasicCommand &command) const;

        /*! Compile a drop table command. */
        QList<QString> compileDrop(const Blueprint &blueprint,
                                   const BasicCommand &command) const;
        /*! Compile a drop table (if exists) command. */
        QList<QString> compileDropIfExists(const Blueprint &blueprint,
                                           const BasicCommand &command) const;

        /*! Compile a fulltext index key command. */
        virtual QList<QString>
        compileFullText(const Blueprint &blueprint,
                        const IndexCommand &command) const;

        /*! Compile a foreign key command. */
        virtual QList<QString>
        compileForeign(const Blueprint &blueprint,
                       const ForeignKeyCommand &command) const;

        /*! Compile a drop fulltext index command. */
        virtual QList<QString>
        compileDropFullText(const Blueprint &blueprint,
                            const IndexCommand &command) const;

        /*! Compile a table comment command. */
        virtual QList<QString>
        compileTableComment(const Blueprint &blueprint,
                            const TableCommentCommand &command) const;

        /* Others */
        /*! Wrap a value in keyword identifiers. */
        QString wrap(const ColumnDefinition &column, bool prefixAlias = false) const;
        /*! Wrap a table in keyword identifiers. */
        QString wrapTable(const Blueprint &blueprint) const;
        /*! Add a prefix to an array of values. */
        template<ColumnContainer T>
        QList<QString> prefixArray(const QString &prefix, const T &values) const;

        /*! Run command's compile method and return SQL queries. */
        virtual QList<QString>
        invokeCompileMethod(const CommandDefinition &command,
                            const DatabaseConnection &connection,
                            const Blueprint &blueprint) const = 0;

        /*! Fluent command item. */
        struct FluentCommandItem
        {
            /*! Fluent command name. */
            QString commandName;
            /*! Determine whether the fluent command values are set and are not empty. */
            std::function<bool(const ColumnDefinition &column)> shouldAdd;
        };

        /*! Get the fluent commands for the grammar. */
        virtual const std::vector<FluentCommandItem> &getFluentCommands() const = 0; // Can't be noexcept because of std::function<>

    protected:
        /*! Escape special characters (used by the defaultValue and comment). */
        virtual QString escapeString(QString value) const = 0;

        /*! Determine whether should add an auto-incrementing fluent command. */
        static bool
        shouldAddAutoIncrementStartingValue(const ColumnDefinition &column) noexcept;

        /*! Get the SQL for the column data type. */
        virtual QString getType(ColumnDefinition &column) const = 0;
        /*! Compile the blueprint's added column definitions. */
        QStringList getColumns(const Blueprint &blueprint) const;

        /*! Format a value so that it can be used in "default" clauses. */
        QString getDefaultValue(const QVariant &value) const;

        /*! Add the column modifiers to the definition. */
        virtual QString
        addModifiers(QString &&sql, const ColumnDefinition &column) const = 0;

        /*! Create the column definition for a generated, computed column type. */
        virtual QString typeComputed(const ColumnDefinition &column) const;
    };

    /* public */

    SchemaGrammar::~SchemaGrammar() = default;

    /* others */

    template<ColumnContainer T>
    QList<QString>
    SchemaGrammar::prefixArray(const QString &prefix, const T &values) const
    {
        return values
                | ranges::views::transform([&prefix](const auto &value)
        {
            return SPACE_IN.arg(prefix, value);
        })
                | ranges::to<QList<QString>>();
    }

} // namespace Grammars
} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_GRAMMARS_SCHEMAGRAMMAR_HPP
