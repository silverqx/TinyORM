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
        QString name {};
    };

    /*! Drop columns command. */
    class DropColumnsCommand : public CommandDefinition
    {
    public:
        /*! Command name. */
        QString name {};

        /*! Columns to drop. */
        QVector<Column> columns;
    };

    /*! Rename table, column or index command. */
    class RenameCommand : public CommandDefinition
    {
    public:
        /*! Command name. */
        QString name {};

        /*! Rename from. */
        QString from {};
        /*! Rename to. */
        QString to;
    };

    /*! Create primary, index, unique, fulltext or spatialIndex command. */
    class IndexCommand : public CommandDefinition
    {
    public:
        /*! Command name. */
        QString name {};

        /*! Index name. */
        QString index;
        /*! Columns for which to create an index. */
        QVector<Column> columns;
        /*! Algorithm to use during index creation. */
        QString algorithm {};
    };

    /*! Foreign key constraints command. */
    class ForeignKeyCommand : public CommandDefinition
    {
    public:
        /*! Command name. */
        QString name {};

        /*! Index name for the foreign key. */
        QString index;
        // CUR schema, check if foreign key can use more columns, if not use QString silverqx
        /*! Columns for which to create the foreign key. */
        QVector<Column> columns;

        /*! Specifies the referenced columns. */
        QVector<QString> references {};
        /*! Specifies the referenced table. */
        QString on {};
        /*! Specifies ON DELETE action (cascade/restrict/set null/no action/
            set default). */
        QString onDelete {};
        /*! Specifies ON UPDATE action (cascade/restrict/set null/no action/
            set default). */
        QString onUpdate {};
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
        QString name {};
        /*! Indicates whether a column will be changed or created. */
        bool change = false;

        /* Column type specific */
        /*! Allowed index values for Enumaration Literals (enum/set). */
        QVector<QString> allowed {};
        /*! Value for a generated, computed column type (SQL Server). */
        QString expression {};
        /*! Length of the char or varchar column. */
        int length = DefaultStringLength;
        /*! Number of digits after the decimal point for floating-point types. */
        std::optional<int> places = std::nullopt;
        /*! Determine fractional Seconds in Time Values (MySQL 0-6). */
        int precision = -1;
        /* srid max. value should be 2^32-1 as is described here, so unsigned int
           should be ok:
           https://dev.mysql.com/doc/refman/8.0/en/spatial-function-argument-handling.html */
        /*! The spatial reference identifier (SRID) of a geometry identifies the SRS
            in which the geometry is defined. */
        std::optional<quint32> srid = std::nullopt;
        /*! Number of digits before the decimal point for floating-point types. */
        std::optional<int> total    = std::nullopt;

        /* Indexes - used by blueprint command for indexes or fluent indexes on column */
        /*! Add an index. */
        std::variant<std::monostate, QString, bool> index        {};
        /*! Add a primary index. */
        std::variant<std::monostate, QString, bool> primary      {};
        /*! Add a fulltext index. */
        std::variant<std::monostate, QString, bool> fulltext     {};
        /*! Add a spatial index. */
        std::variant<std::monostate, QString, bool> spatialIndex {};
        /*! Add a unique index. */
        std::variant<std::monostate, QString, bool> unique       {};

        /* Column definition */
        /*! Determine "after" which column to place a current column (MySQL). */
        QString after         {};
        /*! Specify a character set for the column (MySQL). */
        QString charset       {};
        /*! Specify a collation for the column (MySQL/PostgreSQL/SQL Server). */
        QString collation     {};
        /*! Add a comment to the column (MySQL/PostgreSQL). */
        QString comment       {};
        /*! Specify a "default" value for the column. */
        QVariant defaultValue {};
        /*! Set the starting value of an auto-incrementing field (MySQL / PostgreSQL). */
        std::optional<quint64> from          = std::nullopt;
        /*! Set the starting value of an auto-incrementing field (MySQL / PostgreSQL). */
        std::optional<quint64> startingValue = std::nullopt;
        /*! Create a stored generated column (MySQL/PostgreSQL/SQLite). */
        QString storedAs  {};
        /*! Create a virtual generated column (MySQL/PostgreSQL/SQLite). */
        QString virtualAs {};

        // Place boolean data members at the end to avoid excessive padding
        /*! Determine whether the INTEGER column is auto-increment (primary key). */
        bool autoIncrement      = false;
        /*! Place the column "first" in the table (MySQL). */
        bool first              = false;
        /*! Specify that the column should be invisible to "SELECT *" (MySQL). */
        bool invisible          = false;
        /*! Determine whether the INTEGER column is UNSIGNED (MySQL). */
        bool isUnsigned         = false;
        /*! Allow NULL values to be inserted into the column. */
        bool nullable           = false;
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
       I had to reject designated initializers with commands because of gcc throws
       -Wmissing-field-initializers warning with aggregates that have a base class
       even empty base class, so I had to use classic aggregate initialization with
       commands, it's not a big deal as all data members of commands have to be
       initialized anyway as command aggregate classes are tailor-made for a particular
       commands. I didn't want to suppress this warning around every addCommand()
       method call. */

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_COLUMNDEFINITION_HPP
