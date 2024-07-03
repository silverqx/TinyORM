#pragma once
#ifndef ORM_SCHEMA_COLUMNDEFINITION_HPP
#define ORM_SCHEMA_COLUMNDEFINITION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariant>

#include "orm/query/expression.hpp"
#include "orm/schema/schematypes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::SchemaNs
{

    /* NOTE: every command must start with the QString name; data member because of
       usage the reinterpret_cast<> from CommandDefinition & to BasicCommand &, look at
       MySqlSchemaGrammar::invokeCompileMethod(). */

    /*! Type for the database column. */
    using Column = std::variant<QString, Query::Expression>;

    /*! Base class for all command definitions. */
    class CommandDefinition
    {};

    /*! Basic command that needs only the name. */
    class BasicCommand : public CommandDefinition
    {
    public:
        /*! Command name. */
        QString name;
    };

    /*! Drop columns command. */
    class DropColumnsCommand : public CommandDefinition
    {
    public:
        /*! Command name. */
        QString name;

        /*! Columns to drop. */
        QList<Column> columns;
    };

    /*! Rename table, column or index command. */
    class RenameCommand : public CommandDefinition
    {
    public:
        /*! Command name. */
        QString name;

        /*! Rename from. */
        QString from;
        /*! Rename to. */
        QString to;
    };

    /*! Create primary, index, unique, fulltext or spatialIndex command. */
    class IndexCommand : public CommandDefinition
    {
    public:
        /*! Command name. */
        QString name;

        /*! Index name. */
        QString index;
        /*! Columns for which to create an index. */
        QList<Column> columns;
        /*! Algorithm to use during index creation. */
        QString algorithm {}; // NOLINT(readability-redundant-member-init)
        /*! Dictionary for the to_tsvector function for fulltext search (PostgreSQL). */
        QString language {}; // NOLINT(readability-redundant-member-init)
    };

    /*! Foreign key constraints command. */
    class ForeignKeyCommand : public CommandDefinition
    {
    public:
        /*! Command name. */
        QString name;

        /*! Index name for the foreign key. */
        QString index;
        /*! Columns for which to create the foreign key. */
        QList<Column> columns;

        /*! Specifies the referenced columns. */
        QList<QString> references {}; // NOLINT(readability-redundant-member-init)
        /*! Specifies the referenced table. */
        QString on {}; // NOLINT(readability-redundant-member-init)
        /*! Specifies ON DELETE action (cascade/restrict/set null/no action/
            set default). */
        QString onDelete {}; // NOLINT(readability-redundant-member-init)
        /*! Specifies ON UPDATE action (cascade/restrict/set null/no action/
            set default). */
        QString onUpdate {}; // NOLINT(readability-redundant-member-init)

        /*! Set the foreign key as deferrable (PostgreSQL). */
        std::optional<bool> deferrable = std::nullopt;
        /*! Set the default time to check the constraint (PostgreSQL). */
        std::optional<bool> initiallyImmediate = std::nullopt;
        /*! Set the skip check that all existing rows in the table satisfy the new
            constraint, skip this check on true (PostgreSQL). */
        std::optional<bool> notValid = std::nullopt;
    };

    /*! Column comment command for the PostgreSQL. */
    class CommentCommand : public CommandDefinition
    {
    public:
        /*! Command name. */
        QString name;
        /*! Column name. */
        QString column;
        /*! Column comment value. */
        QString comment;
        /*! Indicates whether a column will be changed or created. */
        bool change = false;
    };

    /*! Table auto-incrementing column starting value command (MySQL/PostgreSQL). */
    class AutoIncrementStartingValueCommand : public CommandDefinition
    {
    public:
        /*! Command name. */
        QString name;
        /*! Column name. */
        QString column;
        /*! Starting value of an auto-incrementing field (MySQL/PostgreSQL). */
        quint64 startingValue;
    };

    /*! Table comment command for the MySQL and PostgreSQL. */
    class TableCommentCommand : public CommandDefinition
    {
    public:
        /*! Command name. */
        QString name;
        /*! Table comment value. */
        QString comment;
    };

    /*! Database column definition. */
    class ColumnDefinition
    {
    public:
        /* Internal - used from the blueprint, not exposed to ColumnDefinitionReference */
        /* Every column must define type and name values */
        /*! Column type. */
        ColumnType type = ColumnType::Undefined;
        /*! Column name. */
        QString name {}; // NOLINT(readability-redundant-member-init)
        /*! Indicates whether a column will be changed or created. */
        bool change = false;

        /* Column type specific */
        /*! Allowed index values for Enumaration Literals (enum/set). */
        QList<QString> allowed {}; // NOLINT(readability-redundant-member-init)
        /*! Value for a generated, computed column type (SQL Server). */
        QString expression {}; // NOLINT(readability-redundant-member-init)
        /*! Length of the char or varchar column. */
        int length = DefaultStringLength;
        /*! Number of digits after the decimal point for floating-point types. */
        std::optional<int> places    = std::nullopt;
        /*! Determine fractional seconds in time values (MySQL 0-6). */
        std::optional<int> precision = std::nullopt;
        /* srid max. value should be 2^32-1 as is described here, so unsigned int
           should be ok:
           https://dev.mysql.com/doc/refman/9.0/en/spatial-function-argument-handling.html */
        /*! The spatial reference identifier (SRID) of a geometry identifies the SRS
            in which the geometry is defined (MySQL/PostgreSQL). */
        std::optional<quint32> srid = std::nullopt;
        /*! Number of digits before the decimal point for floating-point types. */
        std::optional<int> total    = std::nullopt;

        /* Indexes - used by blueprint command for indexes or fluent indexes on column */
        /*! Add an index. */
        std::variant<std::monostate, QString, bool> index        {}; // NOLINT(readability-redundant-member-init)
        /*! Add a primary index. */
        std::variant<std::monostate, QString, bool> primary      {}; // NOLINT(readability-redundant-member-init)
        /*! Add a fulltext index. */
        std::variant<std::monostate, QString, bool> fulltext     {}; // NOLINT(readability-redundant-member-init)
        /*! Add a spatial index. */
        std::variant<std::monostate, QString, bool> spatialIndex {}; // NOLINT(readability-redundant-member-init)
        /*! Add a unique index. */
        std::variant<std::monostate, QString, bool> unique       {}; // NOLINT(readability-redundant-member-init)

        /* Column definition */
        /*! Determine "after" which column to place a current column (MySQL). */
        QString after         {}; // NOLINT(readability-redundant-member-init)
        /*! Specify a character set for the column (MySQL). */
        QString charset       {}; // NOLINT(readability-redundant-member-init)
        /*! Specify a collation for the column (MySQL/PostgreSQL/SQL Server). */
        QString collation     {}; // NOLINT(readability-redundant-member-init)
        /*! Add a comment to the column (MySQL/PostgreSQL). */
        QString comment       {}; // NOLINT(readability-redundant-member-init)
        /*! Specify a "default" value for the column. */
        QVariant defaultValue {}; // NOLINT(readability-redundant-member-init)
        /*! Specifies a "on update" action (MySQL; used by the datetime and timestamp). */
        QVariant onUpdate     {}; // NOLINT(readability-redundant-member-init)
        /*! Create a SQL compliant identity column (PostgreSQL). */
        QString generatedAs   {}; // NOLINT(readability-redundant-member-init)
        /*! Rename a column, used with the change() method (MySQL). */
        QString renameTo      {}; // NOLINT(readability-redundant-member-init)

        /*! Set the starting value of an auto-incrementing field (MySQL/PostgreSQL),
            alias for the 'startingValue'. */
        std::optional<quint64> from          = std::nullopt;
        /*! Allow NULL values to be inserted into the column. */
        std::optional<bool> nullable         = std::nullopt; // Has to be optional because of virtualAs() and storedAs(), look at MySqlSchemaGrammar::modifyNullable()
        /*! Set the starting value of an auto-incrementing field (MySQL/PostgreSQL). */
        std::optional<quint64> startingValue = std::nullopt;
        /*! Create a stored generated column (MySQL/PostgreSQL/SQLite). */
        std::optional<QString> storedAs      = std::nullopt; // Has to be optional because of modifyStoredAs(), look at PostgresSchemaGrammar and tst_PostgreSQL_SchemaBuilder::drop_StoredAs() (to support "drop expression if exists")
        /*! Create a virtual generated column (MySQL/PostgreSQL/SQLite). */
        std::optional<QString> virtualAs     = std::nullopt; // Has to be optional because of modifyVirtualAs(), look at PostgresSchemaGrammar (to support "drop expression if exists")

        // Place boolean data members at the end to avoid excessive padding
        /*! Used as a modifier for generatedAs() (PostgreSQL). */
        bool always             = false;
        /*! Determine whether the INTEGER column is auto-increment (primary key). */
        bool autoIncrement      = false;
        /*! Place the column "first" in the table (MySQL). */
        bool first              = false;
        /*! Specify that the column should be invisible to "SELECT *" (MySQL). */
        bool invisible          = false;
        /*! Determine whether to use the geography (default, false) or
            geometry type (PostgreSQL). */
        bool isGeometry         = false;
        /*! Determine whether the INTEGER column is UNSIGNED (MySQL). */
        bool isUnsigned         = false;
        /*! Set the TIMESTAMP column to use CURRENT_TIMESTAMP as default value. */
        bool useCurrent         = false;
        /*! Set the TIMESTAMP column to use CURRENT_TIMESTAMP when updating (MySQL). */
        bool useCurrentOnUpdate = false;
    };

    /* Commands was extracted from the ColumnDefinition and has been created
       own struct for every or very similar commands. They are now allocated
       on the heap to save space because sizeof(ColumnDefinition) was 736 and
       that is too much, eg. if schema would contain 100 columns it would take
       73KB. After a commands extraction sizeof(ColumnDefinition) have decreased
       to 496 bytes (~40% decrease) 😁.
       I have decided not to use polymorphic commands, I wanted to use
       designated initializers with aggregates, the consequence of this is
       usage of reinterpret_cast() :/, but it works great.
       I had to reject designated initializers with commands because of GCC throws
       -Wmissing-field-initializers warning with aggregates that have a base class
       even empty base class, so I had to use classic aggregate initialization with
       commands, it's not a big deal as all data members of commands have to be
       initialized anyway as command aggregate classes are tailor-made for a particular
       commands. I didn't want to suppress this warning around every addCommand()
       method call. */

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_COLUMNDEFINITION_HPP
