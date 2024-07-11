#pragma once
#ifndef ORM_ORMTYPES_HPP
#define ORM_ORMTYPES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QTimeZone>

#include <memory>
#include <variant>

#include "orm/constants.hpp"
#include "orm/macros/sqldrivermappings.hpp" // IWYU pragma: export
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
    using BindingsMap = QMap<BindingType, QList<QVariant>>;

    /*! Aggregate item. */
    struct AggregateItem
    {
        QString       function;
        QList<Column> columns;
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
        QList<Column>                 columns        {}; // NOLINT(readability-redundant-member-init)
        QList<QVariant>               values         {}; // NOLINT(readability-redundant-member-init)
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
        /*! QtTimeZoneConfig contains the QTimeZone, use toTimeZone() for conversion. */
        QTimeZone,
        /*! QtTimeZoneConfig contains seconds, use toOffsetFromUtc() for conversion. */
        OffsetFromUtc,
#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
        /*! QtTimeZoneConfig contains Qt::TimeSpec, use toTimeSpec() for conversion. */
        QtTimeSpec,
#endif
    };

/* The TTimeZone alias can be used on Qt <v6.5 and also Qt >=v6.5.
   It helps to support both Qt versions still. */
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    /*! Alias for QTimeZone::Initialization. */
    using TTimeZone = QTimeZone::Initialization;
#else
    /*! Alias for Qt::TimeSpec. */
    using TTimeZone = Qt::TimeSpec;
#endif

    /*! Determine how the QDateTime time zone will be converted, it's saved
        in the qt_timezone database connection's configuration option. */
    struct QtTimeZoneConfig
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        /*! Time zone type saved in the value data member. */
        QtTimeZoneType type  {QtTimeZoneType::QTimeZone};
        /*! Time zone value (UTC by default). */
        QVariant       value {QVariant::fromValue(QTimeZone(QTimeZone::UTC))}; // UTC is the default timezone, not the QTimeZone::LocalTime (it overrides this default)
#else
        /*! Time zone type saved in the value data member. */
        QtTimeZoneType type  {QtTimeZoneType::QtTimeSpec};
        /*! Time zone value (UTC by default). */
        QVariant       value {Qt::UTC}; // UTC is the default timezone, not the Qt::LocalTime (it overrides this default)
#endif

        /*! Equality comparison operator for the QtTimeZoneConfig. */
        bool operator==(const QtTimeZoneConfig &) const = default;

        /*! Factory method to create QtTimeZoneConfig as QTimeZone::UTC instance. */
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        inline static QtTimeZoneConfig utc() noexcept;
#else
        inline static QtTimeZoneConfig utc();
#endif

        /*! Factory method to create QtTimeZoneConfig as QTimeZone::LocalTime instance. */
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        inline static QtTimeZoneConfig localTime() noexcept;
#else
        inline static QtTimeZoneConfig localTime();
#endif
    };

    /* QtTimeZoneConfig */

    /* public */

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    QtTimeZoneConfig QtTimeZoneConfig::utc()
    noexcept(std::is_nothrow_copy_constructible_v<QTimeZone>)
#else
    QtTimeZoneConfig QtTimeZoneConfig::utc()
#endif
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        return {QtTimeZoneType::QTimeZone,
                QVariant::fromValue(QTimeZone(QTimeZone::UTC))};
#else
        return {QtTimeZoneType::QtTimeSpec, Qt::UTC};
#endif
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    QtTimeZoneConfig QtTimeZoneConfig::localTime()
    noexcept(std::is_nothrow_copy_constructible_v<QTimeZone>)
#else
    QtTimeZoneConfig QtTimeZoneConfig::localTime()
#endif
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        return {QtTimeZoneType::QTimeZone,
                QVariant::fromValue(QTimeZone(QTimeZone::LocalTime))};
#else
        return {QtTimeZoneType::QtTimeSpec, Qt::LocalTime};
#endif
    }

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_ORMTYPES_HPP
