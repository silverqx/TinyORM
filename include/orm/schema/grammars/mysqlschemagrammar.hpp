#pragma once
#ifndef ORM_SCHEMA_GRAMMARS_MYSQLSCHEMAGRAMMAR_HPP
#define ORM_SCHEMA_GRAMMARS_MYSQLSCHEMAGRAMMAR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/schema/grammars/schemagrammar.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{
    class AutoIncrementStartingValueCommand;
    class DropColumnsCommand;
    class RenameCommand;

namespace Grammars
{

    /*! MySQL schema grammar. */
    class TINYORM_EXPORT MySqlSchemaGrammar : public SchemaGrammar
    {
        Q_DISABLE_COPY_MOVE(MySqlSchemaGrammar)

    public:
        /*! Constructor. */
        inline explicit MySqlSchemaGrammar(bool isMaria = false);
        /*! Virtual destructor. */
        ~MySqlSchemaGrammar() override = default;

        /*! Check if this Grammar supports schema changes wrapped in a transaction. */
        inline bool supportsSchemaTransactions() const noexcept override;

        /* Compile methods for the SchemaBuilder */
        /*! Compile a create database command. */
        QString compileCreateDatabase(const QString &name,
                                      DatabaseConnection &connection) const override;
        /*! Compile a drop database if exists command. */
        QString compileDropDatabaseIfExists(const QString &name) const override;

        /*! Compile the SQL needed to drop all tables. */
        QString compileDropAllTables(const QList<QString> &tables) const override;
        /*! Compile the SQL needed to drop all views. */
        QString compileDropAllViews(const QList<QString> &views) const override;

        /*! Compile the SQL needed to retrieve all table names. */
        QString compileGetAllTables( // NOLINT(google-default-arguments)
                    const QList<QString> &databases = {}) const override;
        /*! Compile the SQL needed to retrieve all view names. */
        QString compileGetAllViews( // NOLINT(google-default-arguments)
                    const QList<QString> &databases = {}) const override;

        /*! Compile the command to enable foreign key constraints. */
        QString compileEnableForeignKeyConstraints() const override;
        /*! Compile the command to disable foreign key constraints. */
        QString compileDisableForeignKeyConstraints() const override;

        /*! Compile the query to determine the list of tables. */
        QString compileTableExists() const override;
        /*! Compile the query to determine the list of columns. */
        QString compileColumnListing(const QString &table = "") const override; // NOLINT(google-default-arguments)

        /* Compile methods for commands */
        /*! Compile a create table command. */
        QList<QString> compileCreate(const Blueprint &blueprint,
                                     const DatabaseConnection &connection) const;

        /*! Compile a rename table command. */
        QList<QString> compileRename(const Blueprint &blueprint,
                                     const RenameCommand &command) const;

        /*! Compile an add column command. */
        QList<QString> compileAdd(const Blueprint &blueprint,
                                  const BasicCommand &command) const;
        /*! Compile a change column command. */
        QList<QString> compileChange(const Blueprint &blueprint,
                                     const BasicCommand &command) const;

        /*! Compile a drop column command. */
        QList<QString> compileDropColumn(const Blueprint &blueprint,
                                         const DropColumnsCommand &command) const;
        /*! Compile a rename column command. */
        QList<QString> compileRenameColumn(const Blueprint &blueprint,
                                           const RenameCommand &command) const;

        /*! Compile a primary key command. */
        QList<QString> compilePrimary(const Blueprint &blueprint,
                                      const IndexCommand &command) const;
        /*! Compile a unique key command. */
        QList<QString> compileUnique(const Blueprint &blueprint,
                                     const IndexCommand &command) const;
        /*! Compile a plain index key command. */
        QList<QString> compileIndex(const Blueprint &blueprint,
                                    const IndexCommand &command) const;
        /*! Compile a fulltext index key command. */
        QList<QString> compileFullText(const Blueprint &blueprint,
                                       const IndexCommand &command) const override;
        /*! Compile a spatial index key command. */
        QList<QString> compileSpatialIndex(const Blueprint &blueprint,
                                           const IndexCommand &command) const;

        /*! Compile a drop primary key command. */
        QList<QString> compileDropPrimary(const Blueprint &blueprint,
                                          const IndexCommand &command) const;
        /*! Compile a drop unique key command. */
        inline QList<QString> compileDropUnique(const Blueprint &blueprint,
                                                const IndexCommand &command) const;
        /*! Compile a drop index command. */
        QList<QString> compileDropIndex(const Blueprint &blueprint,
                                        const IndexCommand &command) const;
        /*! Compile a drop fulltext index command. */
        inline QList<QString>
        compileDropFullText(const Blueprint &blueprint,
                            const IndexCommand &command) const override;
        /*! Compile a drop spatial index command. */
        inline QList<QString>
        compileDropSpatialIndex(const Blueprint &blueprint,
                                const IndexCommand &command) const;

        /*! Compile a drop foreign key command. */
        QList<QString> compileDropForeign(const Blueprint &blueprint,
                                          const IndexCommand &command) const;

        /*! Compile a rename index command. */
        QList<QString> compileRenameIndex(const Blueprint &blueprint,
                                          const RenameCommand &command) const;

        /*! Compile a table comment command. */
        QList<QString>
        compileTableComment(const Blueprint &blueprint,
                            const TableCommentCommand &command) const override;

        /*! Map a command name to SchemaGrammar::compileXx() methods. */
        QList<QString> invokeCompileMethod(const CommandDefinition &command,
                                           const DatabaseConnection &connection,
                                           const Blueprint &blueprint) const override;

        /*! Get the fluent commands for the grammar. */
        const std::vector<FluentCommandItem> &getFluentCommands() const override;

    protected:
        // BUG schema, change after upgrade to QtCreator with clang 13 silverqx
        /*! Expose column types. */
//        using enum ColumnType;

        /*! Create the main create table clause. */
        QString compileCreateTable(const Blueprint &blueprint) const;
        /*! Add the column modifiers to the definition. */
        QString addModifiers(QString &&sql,
                             const ColumnDefinition &column) const override;

        /*! Append the character set specifications to a command. */
        static void
        compileCreateEncoding(QString &sql, const DatabaseConnection &connection,
                              const Blueprint &blueprint);
        /*! Append the engine specifications to a command. */
        static void
        compileCreateEngine(QString &sql, const DatabaseConnection &connection,
                            const Blueprint &blueprint);
        /*! Compile the auto-incrementing column starting value. */
        QList<QString>
        compileAutoIncrementStartingValue(
                const Blueprint &blueprint,
                const AutoIncrementStartingValueCommand &command) const;

        /*! Compile an index creation command. */
        QString compileKey(const Blueprint &blueprint, const IndexCommand &command,
                           const QString &type) const;

        /*! Wrap a single string in keyword identifiers. */
        QString wrapValue(QString value) const override;

        /*! Escape special characters (used by the defaultValue and comment). */
        QString escapeString(QString value) const override;

        /*! Get the SQL for the column data type. */
        QString getType(ColumnDefinition &column) const override;

        /*! Create the column definition for a char type. */
        QString typeChar(const ColumnDefinition &column) const;
        /*! Create the column definition for a string type. */
        QString typeString(const ColumnDefinition &column) const;
        /*! Create the column definition for a tiny text type. */
        QString typeTinyText(const ColumnDefinition &column) const;
        /*! Create the column definition for a text type. */
        QString typeText(const ColumnDefinition &column) const;
        /*! Create the column definition for a medium text type. */
        QString typeMediumText(const ColumnDefinition &column) const;
        /*! Create the column definition for a long text type. */
        QString typeLongText(const ColumnDefinition &column) const;
        /*! Create the column definition for a big integer type. */
        QString typeBigInteger(const ColumnDefinition &column) const;
        /*! Create the column definition for an integer type. */
        QString typeInteger(const ColumnDefinition &column) const;
        /*! Create the column definition for a medium integer type. */
        QString typeMediumInteger(const ColumnDefinition &column) const;
        /*! Create the column definition for a tiny integer type. */
        QString typeTinyInteger(const ColumnDefinition &column) const;
        /*! Create the column definition for a small integer type. */
        QString typeSmallInteger(const ColumnDefinition &column) const;
        /*! Create the column definition for a float type. */
        QString typeFloat(const ColumnDefinition &column) const;
        /*! Create the column definition for a double type. */
        QString typeDouble(const ColumnDefinition &column) const;
        /*! Create the column definition for a decimal type. */
        QString typeDecimal(const ColumnDefinition &column) const;
        /*! Create the column definition for a boolean type. */
        QString typeBoolean(const ColumnDefinition &column) const;
        /*! Create the column definition for an enumeration type. */
        QString typeEnum(const ColumnDefinition &column) const;
        /*! Create the column definition for a set enumeration type. */
        QString typeSet(const ColumnDefinition &column) const;
        /*! Create the column definition for a json type. */
        QString typeJson(const ColumnDefinition &column) const;
        /*! Create the column definition for a jsonb type. */
        QString typeJsonb(const ColumnDefinition &column) const;
        /*! Create the column definition for a date type. */
        QString typeDate(const ColumnDefinition &column) const;
        /*! Create the column definition for a date-time type. */
        QString typeDateTime(ColumnDefinition &column) const;
        /*! Create the column definition for a date-time (with time zone) type. */
        QString typeDateTimeTz(ColumnDefinition &column) const;
        /*! Create the column definition for a time type. */
        QString typeTime(const ColumnDefinition &column) const;
        /*! Create the column definition for a time (with time zone) type. */
        QString typeTimeTz(const ColumnDefinition &column) const;
        /*! Create the column definition for a timestamp type. */
        QString typeTimestamp(ColumnDefinition &column) const;
        /*! Create the column definition for a timestamp (with time zone) type. */
        QString typeTimestampTz(ColumnDefinition &column) const;
        /*! Create the column definition for a year type. */
        QString typeYear(const ColumnDefinition &column) const;
        /*! Create the column definition for a tiny binary type. */
        QString typeTinyBinary(const ColumnDefinition &column) const;
        /*! Create the column definition for a binary type. */
        QString typeBinary(const ColumnDefinition &column) const;
        /*! Create the column definition for a medium binary type. */
        QString typeMediumBinary(const ColumnDefinition &column) const;
        /*! Create the column definition for a long binary type. */
        QString typeLongBinary(const ColumnDefinition &column) const;
        /*! Create the column definition for a uuid type. */
        QString typeUuid(const ColumnDefinition &column) const;
        /*! Create the column definition for an IP address type. */
        QString typeIpAddress(const ColumnDefinition &column) const;
        /*! Create the column definition for a MAC address type. */
        QString typeMacAddress(const ColumnDefinition &column) const;
        /*! Create the column definition for a spatial Geometry type. */
        QString typeGeometry(const ColumnDefinition &column) const;
        /*! Create the column definition for a spatial Point type. */
        QString typePoint(const ColumnDefinition &column) const;
        /*! Create the column definition for a spatial LineString type. */
        QString typeLineString(const ColumnDefinition &column) const;
        /*! Create the column definition for a spatial Polygon type. */
        QString typePolygon(const ColumnDefinition &column) const;
        /*! Create the column definition for a spatial GeometryCollection type. */
        QString typeGeometryCollection(const ColumnDefinition &column) const;
        /*! Create the column definition for a spatial MultiPoint type. */
        QString typeMultiPoint(const ColumnDefinition &column) const;
        /*! Create the column definition for a spatial MultiLineString type. */
        QString typeMultiLineString(const ColumnDefinition &column) const;
        /*! Create the column definition for a spatial MultiPolygon type. */
        QString typeMultiPolygon(const ColumnDefinition &column) const;
        /*! Create the column definition for a generated, computed column type. */
        QString typeComputed(const ColumnDefinition &column) const override;

        /*! Get the SQL for an unsigned column modifier. */
        QString modifyUnsigned(const ColumnDefinition &column) const;
        /*! Get the SQL for a character set column modifier. */
        QString modifyCharset(const ColumnDefinition &column) const;
        /*! Get the SQL for a collation column modifier. */
        QString modifyCollate(const ColumnDefinition &column) const;
        /*! Get the SQL for a generated virtual column modifier. */
        QString modifyVirtualAs(const ColumnDefinition &column) const;
        /*! Get the SQL for a generated stored column modifier. */
        QString modifyStoredAs(const ColumnDefinition &column) const;
        /*! Get the SQL for a nullable column modifier. */
        QString modifyNullable(const ColumnDefinition &column) const;
        /*! Get the SQL for an invisible column modifier. */
        QString modifyInvisible(const ColumnDefinition &column) const;
        /*! Get the SQL for a SRID column modifier. */
        QString modifySrid(const ColumnDefinition &column) const;
        /*! Get the SQL for a default column modifier. */
        QString modifyDefault(const ColumnDefinition &column) const;
        /*! Get the SQL for an "on update" column modifier (MySQL only;
            used by the datetime and timestamp). */
        QString modifyOnUpdate(const ColumnDefinition &column) const;
        /*! Get the SQL for an auto-increment column modifier. */
        QString modifyIncrement(const ColumnDefinition &column) const;
        /*! Get the SQL for a "comment" column modifier. */
        QString modifyComment(const ColumnDefinition &column) const;
        /*! Get the SQL for an "after" column modifier. */
        QString modifyAfter(const ColumnDefinition &column) const;
        /*! Get the SQL for a "first" column modifier. */
        QString modifyFirst(const ColumnDefinition &column) const;

    private:
        /*! Is this the schema grammar for the MariaDB database server? */
        bool m_isMaria;
    };

    /* public */

    MySqlSchemaGrammar::MySqlSchemaGrammar(const bool isMaria)
        : m_isMaria(isMaria)
    {}

    bool MySqlSchemaGrammar::supportsSchemaTransactions() const noexcept
    {
        return false;
    }

    QList<QString>
    MySqlSchemaGrammar::compileDropUnique(const Blueprint &blueprint,
                                          const IndexCommand &command) const
    {
        return compileDropIndex(blueprint, command);
    }

    QList<QString>
    MySqlSchemaGrammar::compileDropFullText(const Blueprint &blueprint,
                                            const IndexCommand &command) const
    {
        return compileDropIndex(blueprint, command);
    }

    QList<QString>
    MySqlSchemaGrammar::compileDropSpatialIndex(
                const Blueprint &blueprint, const IndexCommand &command) const
    {
        return compileDropIndex(blueprint, command);
    }

} // namespace Grammars
} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_GRAMMARS_MYSQLSCHEMAGRAMMAR_HPP
