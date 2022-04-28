#pragma once
#ifndef ORM_ORMTYPES_HPP
#define ORM_ORMTYPES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QSharedPointer>
#include <QVariant>
#include <QVector>

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
    enum struct BindingType
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
    enum struct WhereType
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
    };

    /*! Where clause item, primarily used in grammars to build sql query. */
    struct WhereConditionItem
    {
        Column                       column      {};
        QVariant                     value       {};
        QString                      comparison  {Orm::Constants::EQ};
        QString                      condition   {Orm::Constants::AND};
        WhereType                    type        {WhereType::UNDEFINED};
        QSharedPointer<QueryBuilder> nestedQuery {nullptr};
        QVector<QVariant>            values      {};
        Column                       columnTwo   {};
        QString                      sql         {};
    };

    /*! Supported having types. */
    enum struct HavingType
    {
        UNDEFINED = -1,
        BASIC,
        RAW,
    };

    /*! Having clause item. */
    struct HavingConditionItem
    {
        Column     column     {};
        QVariant   value      {};
        QString    comparison {Orm::Constants::EQ};
        QString    condition  {Orm::Constants::AND};
        HavingType type       {HavingType::UNDEFINED};
        QString    sql        {};
    };

    /*! Order by clause item. */
    struct OrderByItem
    {
        Column      column    {};
        QString     direction {Orm::Constants::ASC};
        QString     sql       {};
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
        QString  condition  {};
    };

    /*! Where item to compare two columns, primarily used in vector overloads. */
    struct WhereColumnItem
    {
        Column  first;
        Column  second;
        QString comparison {Orm::Constants::EQ};
        QString condition  {};
    };

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#ifdef TINYORM_COMMON_NAMESPACE
// NOLINTNEXTLINE(performance-no-int-to-ptr, misc-no-recursion)
Q_DECLARE_METATYPE(TINYORM_COMMON_NAMESPACE::Orm::WhereConditionItem)
#else
// NOLINTNEXTLINE(performance-no-int-to-ptr, misc-no-recursion)
Q_DECLARE_METATYPE(Orm::WhereConditionItem)
#endif

#endif // ORM_ORMTYPES_HPP
