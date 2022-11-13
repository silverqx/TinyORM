#pragma once
#ifndef ORM_ORMCONCEPTS_HPP
#define ORM_ORMCONCEPTS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>
#include <QVariant>

#include <deque>
#include <set>

#include "orm/macros/commonnamespace.hpp"

namespace std
{
    template<typename ...Types>
    class variant; // NOLINT(bugprone-forwarding-reference-overload)
}

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
namespace Query
{
    class Builder;
    class Expression;
}
    using QueryBuilder = Query::Builder;

    /* Don't test for eg. QString and also const QString & in the std::converible_to<>,
       test only for the QString, it doesn't make sense to test for both. */

    /*! Concept for a queryable parameter. */
    template<typename T>
    concept Queryable = std::convertible_to<T, Orm::QueryBuilder &> ||
                        std::invocable<T, Orm::QueryBuilder &>;

    /*! Concept for whereSub()'s value parameter. */
    template<typename T>
    concept WhereValueSubQuery = Queryable<T>;

    /*! Concept for the subquery, used in the from clause (tablename), join clause, ... */
    template<typename T>
    concept SubQuery = std::convertible_to<T, QString> ||
                       Queryable<T>;

    /*! Concept for where()'s value parameter. */
    template<typename T>
    concept WhereValue = std::convertible_to<T, QVariant> ||
                         WhereValueSubQuery<T>;

    /*! Concept for the join's table parameter. */
    template<typename T>
    concept JoinTable = std::same_as<T, Query::Expression> ||
                        std::convertible_to<T, QString>;

    /*! Concept for the table column. */
    template<typename T>
    concept ColumnConcept =
            std::convertible_to<T, std::variant<QString, Query::Expression>>;

    /*! Concept for the QString. */
    template<typename T>
    concept QStringConcept = std::convertible_to<T, QString>;

    /*! Type for the database column. */
    using Column = std::variant<QString, Query::Expression>;

    /*! QString container concept (QStringList or QVector<QString>). */
    template<typename T>
    concept ColumnContainer = std::convertible_to<T, QStringList> ||
                              std::convertible_to<T, QVector<QString>> ||
                              std::convertible_to<T, QVector<Column>>;

    /*! QString container concept (used by ContainerUtils::join()). */
    template<typename T>
    concept JoinContainer = ColumnContainer<T> ||
                            std::convertible_to<T, std::set<QString>> ||
                            std::convertible_to<T, std::deque<QString>> ||
                            std::convertible_to<T, std::vector<QString>>;

    /*! Concept for delimiter for joining containers. */
    template<typename T>
    concept DelimiterConcept = std::convertible_to<T, QString> ||
                               std::convertible_to<T, QChar>;

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_ORMCONCEPTS_HPP
