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

    /*! Database column definition. */
    class ColumnDefinition
    {
        /*! Type for the database column. */
        using Column = std::variant<QString, Query::Expression>;

    public:
        /*! Column type. */
        ColumnType type;
        /*! Column name. */
        QString name;
        /*! Indicates whether a column will be changed or created. */
        bool change = false;

        /* Commands */
        // indexes, and columns also used in dropColumns()
        /*! Index name. */
        QString index;
        /*! Columns for which to create an index. */
        QVector<Column> columns; // foreign key
        /*! Algorith to use during index creation. */
        QString algorithm;

        // rename index
        /*! Rename index from. */
        QString from_;
        /*! Rename index to. */
        QString to;

        // foreign key
        /*! Specifies the referenced columns. */
        QVector<QString> references;
        /*! Specifies the referenced table. */
        QString on;
        /*! Specifies ON DELETE action (cascade/restrict/set null/no action/
            set default). */
        QString onDelete;
        /*! Specifies ON UPDATE action (cascade/restrict/set null/no action/
            set default). */
        QString onUpdate;

        /* Internal - used from blueprint */
        /*! Allowed index values for Enumaration Literals (enum/set). */
        QVector<QString> allowed;
        /*! Value for a generated, computed column type (SQL Server). */
        QString expression;
        /*! Length of the char or varchar column. */
        int length = DefaultStringLength;
        /*! Number of digits after the decimal point for floating-point types. */
        std::optional<int> places;
        /*! Determine fractional Seconds in Time Values (MySQL 0-6). */
        int precision;
        /* srid max. value should be 2^32-1 as is described here, so unsigned int
           should be ok:
           https://dev.mysql.com/doc/refman/8.0/en/spatial-function-argument-handling.html */
        /*! The spatial reference identifier (SRID) of a geometry identifies the SRS
            in which the geometry is defined. */
        std::optional<quint32> srid;
        /*! Number of digits before the decimal point for floating-point types. */
        std::optional<int> total;

        /* Indexes */
        /*! Add an index. */
        std::variant<std::monostate, QString, bool> index_;
        /*! Add a primary index. */
        std::variant<std::monostate, QString, bool> primary;
        /*! Add a fulltext index. */
        std::variant<std::monostate, QString, bool> fulltext;
        /*! Add a spatial index. */
        std::variant<std::monostate, QString, bool> spatialIndex;
        /*! Add a unique index. */
        std::variant<std::monostate, QString, bool> unique;

        /* Column definitions */
        /*! Determine "after" which column to place a current column (MySQL). */
        QString after;
        /*! Specify a character set for the column (MySQL). */
        QString charset;
        /*! Specify a collation for the column (MySQL/PostgreSQL/SQL Server). */
        QString collation;
        /*! Add a comment to the column (MySQL/PostgreSQL). */
        QString comment;
        /*! Specify a "default" value for the column. */
        QVariant defaultValue;
        /*! Set the starting value of an auto-incrementing field (MySQL / PostgreSQL). */
        std::optional<quint64> from;
        /*! Set the starting value of an auto-incrementing field (MySQL / PostgreSQL). */
        std::optional<quint64> startingValue;
        /*! Create a stored generated column (MySQL/PostgreSQL/SQLite). */
        QString storedAs;
        /*! Create a virtual generated column (MySQL/PostgreSQL/SQLite). */
        QString virtualAs;

        // Place boolean data members at the end to avoid excessive padding
        /*! Determine whether the INTEGER column is auto-increment (primary key). */
        bool autoIncrement = false;
        /*! Place the column "first" in the table (MySQL). */
        bool first = false;
        /*! Specify that the column should be invisible to "SELECT *" (MySQL). */
        bool invisible = false;
        /*! Determine whether the INTEGER column is UNSIGNED (MySQL). */
        bool isUnsigned = false;
        /*! Allow NULL values to be inserted into the column. */
        bool nullable = false;
        /*! Set the TIMESTAMP column to use CURRENT_TIMESTAMP as default value. */
        bool useCurrent = false;
        /*! Set the TIMESTAMP column to use CURRENT_TIMESTAMP when updating (MySQL). */
        bool useCurrentOnUpdate = false;
    };

} // namespace Orm::SchemaNs

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_SCHEMA_COLUMNDEFINITION_HPP
