#pragma once
#ifndef ORM_ORMTYPES_HPP
#define ORM_ORMTYPES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QTimeZone>

#include <memory>
#include <variant>

#include "orm/constants.hpp"
#include "orm/query/expression.hpp"

// TODO types, divide to public/private silverqx

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
    // NOLINTNEXTLINE(google-build-using-namespace)
    using namespace Orm::Constants;

    class DatabaseConnection;

namespace Query
{
    class Builder;
}
    using QueryBuilder = Query::Builder;

    /*! Type for the DatabaseConnection Reconnector (lambda). */
    using ReconnectorType = std::function<void(const DatabaseConnection &)>;

    /*! Type for the database column. */
    using Column = std::variant<QString, Query::Expression>;

    /*! Type for the from clause. */
    using FromClause = std::variant<std::monostate, QString, Query::Expression>;

    /*! Binding types. */
    enum struct BindingType : quint8
    {
        SELECT,
        FROM,
        JOIN,
        WHERE,
        GROUPBY,
        HAVING,
        ORDER,
        UNION,
        UNIONORDER,
    };

    /*! Type for the query value bindings. */
    using BindingsMap = QMap<BindingType, QVector<QVariant>>;

    /*! Aggregate item. */
    struct AggregateItem
    {
        QString         function;
        QVector<Column> columns;
    };

    /*! Supported where clause types. */
    enum struct WhereType : qint8
    {
        UNDEFINED = -1,
        BASIC,
        NESTED,
        COLUMN,
        IN_,
        NOT_IN,
        NULL_,
        NOT_NULL,
        RAW,
        EXISTS,
        NOT_EXISTS,
        ROW_VALUES,
        BETWEEN,
        BETWEEN_COLUMNS,
        DATE,
        TIME,
        DAY,
        MONTH,
        YEAR,
    };

    /*! Supported having types. */
    enum struct HavingType : qint8
    {
        UNDEFINED = -1,
        BASIC,
        RAW,
    };

    /*! Having clause item. */
    struct HavingConditionItem
    {
        Column     column     {}; // NOLINT(readability-redundant-member-init)
        QVariant   value      {}; // NOLINT(readability-redundant-member-init)
        QString    comparison {Orm::Constants::EQ};
        QString    condition  {Orm::Constants::AND};
        HavingType type       {HavingType::UNDEFINED};
        QString    sql        {}; // for the raw version; NOLINT(readability-redundant-member-init)
    };

    /*! Order by clause item. */
    struct OrderByItem
    {
        Column      column    {}; // NOLINT(readability-redundant-member-init)
        QString     direction {Orm::Constants::ASC};
        QString     sql       {}; // for the raw version; NOLINT(readability-redundant-member-init)
    };

    /*! Update item. */
    struct UpdateItem
    {
        QString  column;
        QVariant value;
    };

    /*! Where value/attribute item. */
    struct WhereItem
    {
        Column   column;
        QVariant value;
        QString  comparison {Orm::Constants::EQ};
        QString  condition  {}; // NOLINT(readability-redundant-member-init)
    };

    /*! Where item to compare two columns, primarily used in vector overloads. */
    struct WhereColumnItem
    {
        Column  first;
        Column  second;
        QString comparison {Orm::Constants::EQ};
        QString condition  {}; // NOLINT(readability-redundant-member-init)
    };

    /*! Where item that stores values for the where between clause. */
    struct WhereBetweenItem
    {
        QVariant min;
        QVariant max;
    };

    /*! Where item that stores column names for the where between clause. */
    struct WhereBetweenColumnsItem
    {
        Column min;
        Column max;
    };

    /*! Where clause item, primarily used in grammars to build sql query. */
    struct WhereConditionItem
    {
        Column                        column         {}; // NOLINT(readability-redundant-member-init)
        QVariant                      value          {}; // NOLINT(readability-redundant-member-init)
        QString                       comparison     {Orm::Constants::EQ};
        QString                       condition      {Orm::Constants::AND};
        WhereType                     type           {WhereType::UNDEFINED};
        std::shared_ptr<QueryBuilder> nestedQuery    {nullptr};
        QVector<Column>               columns        {}; // NOLINT(readability-redundant-member-init)
        QVector<QVariant>             values         {}; // NOLINT(readability-redundant-member-init)
        Column                        columnTwo      {}; // NOLINT(readability-redundant-member-init)
        QString                       sql            {}; // for the raw version; NOLINT(readability-redundant-member-init)
        bool                          nope           {false};
        WhereBetweenItem              between        {};
        WhereBetweenColumnsItem       betweenColumns {};
    };

    /*! Time zone type for the QtTimeZoneConfig connection configuration option. */
    enum struct QtTimeZoneType : quint8
    {
        /*! Don't convert time zone. */
        DontConvert,
        /*! QtTimeZoneConfig contains Qt::TimeSpec, use toTimeSpec() for conversion. */
        QtTimeSpec,
        /*! QtTimeZoneConfig contains the QTimeZone, use toTimeZone() for conversion. */
        QTimeZone,
        /*! QtTimeZoneConfig contains seconds, use toOffesetFromUtc() for conversion. */
        OffsetFromUtc,
    };

    /*! Determine how the QDateTime time zone will be converted, it's saved
        in the qt_timezone database connection's configuration option. */
    struct QtTimeZoneConfig
    {
        /*! Time zone type saved in the value data member. */
        QtTimeZoneType type  {QtTimeZoneType::QtTimeSpec};
        /*! Time zone value. */
        QVariant       value {Qt::UTC}; // UTC is the default timezone, not the Qt::LocalTime (it overrides this default)

        /*! Equality comparison operator for the QtTimeZoneConfig. */
        bool operator==(const QtTimeZoneConfig &) const = default;
    };

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

// Register custom QVariant types for Qt 5
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
Q_DECLARE_METATYPE(QTimeZone) // NOLINT(performance-no-int-to-ptr, misc-no-recursion)
Q_DECLARE_METATYPE(Qt::TimeSpec) // NOLINT(performance-no-int-to-ptr, misc-no-recursion)

Q_DECLARE_METATYPE(TINYORM_PREPEND_NAMESPACE(Orm::WhereConditionItem)) // NOLINT(performance-no-int-to-ptr, misc-no-recursion)
Q_DECLARE_METATYPE(TINYORM_PREPEND_NAMESPACE(Orm::QtTimeZoneConfig)) // NOLINT(performance-no-int-to-ptr, misc-no-recursion)
Q_DECLARE_METATYPE(TINYORM_PREPEND_NAMESPACE(Orm::QtTimeZoneType)) // NOLINT(performance-no-int-to-ptr, misc-no-recursion)
#endif

#endif // ORM_ORMTYPES_HPP
