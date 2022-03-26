#pragma once
#ifndef ORM_SCHEMA_GRAMMARS_SQLITESCHEMAGRAMMAR_HPP
#define ORM_SCHEMA_GRAMMARS_SQLITESCHEMAGRAMMAR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/schema/grammars/schemagrammar.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs::Grammars
{

    /*! SQLite schemma grammar. */
    class SHAREDLIB_EXPORT SQLiteSchemaGrammar : public SchemaGrammar
    {
        Q_DISABLE_COPY(SQLiteSchemaGrammar)

    public:
        /*! Default constructor. */
        inline SQLiteSchemaGrammar() = default;
        /*! Virtual destructor. */
        inline ~SQLiteSchemaGrammar() override = default;

        /*! Check if this Grammar supports schema changes wrapped in a transaction. */
        inline bool supportsSchemaTransactions() const noexcept override;

        /* Compile methods for the SchemaBuilder */
        /*! Compile the command to enable foreign key constraints. */
        QString compileEnableForeignKeyConstraints() const override;
        /*! Compile the command to disable foreign key constraints. */
        QString compileDisableForeignKeyConstraints() const override;

        /*! Compile the query to determine the list of columns. */
        QString compileColumnListing(const QString &table = "") const override;

        /* Compile methods for commands */
        /*! Compile a foreign key command. */
        QVector<QString>
        compileForeign(const Blueprint &blueprint,
                       const ForeignKeyCommand &command) const override;

        /*! Run command's compile method and return SQL queries. */
        QVector<QString>
        invokeCompileMethod(const CommandDefinition &command,
                            const DatabaseConnection &connection,
                            const Blueprint &blueprint) const override;

    protected:
        /*! Add the column modifiers to the definition. */
        QString addModifiers(QString &&sql,
                             const ColumnDefinition &column) const override;

        /*! Get the SQL for the column data type. */
        QString getType(const ColumnDefinition &column) const override;
    };

    bool SQLiteSchemaGrammar::supportsSchemaTransactions() const noexcept
    {
        return false;
    }

} // namespace Orm::SchemaNs::Grammars

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_GRAMMARS_SQLITESCHEMAGRAMMAR_HPP
