#pragma once
#ifndef ORM_ORMCONCEPTS_HPP
#define ORM_ORMCONCEPTS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>
#include <QVariant>

#include <deque>
#include <memory>
#include <set>
#include <variant>

#include "orm/macros/commonnamespace.hpp"

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
       test only for the QString, it doesn't make sense to test for both.
       The std::convertible_to<T, Orm::QueryBuilder &> is correct. */

    /*! Concept for a queryable parameter. */
    template<typename T>
    concept Queryable = std::convertible_to<T, Orm::QueryBuilder &> ||
                        std::invocable<T, Orm::QueryBuilder &>;

    /*! Concept for a queryable parameter (adds the std::shared_ptr<QueryBuilder>). */
    template<typename T>
    concept QueryableShared =
            Queryable<T> ||
            std::convertible_to<T, const std::shared_ptr<Orm::QueryBuilder> &>;

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

    /*! Concept for the QVariant. */
    template<typename T>
    concept QVariantConcept = std::convertible_to<T, QVariant>;

    /*! Type for the database column. */
    using Column = std::variant<QString, Query::Expression>;

    /*! QString/Column container concept (QStringList or QList<QString>). */
    template<typename T>
    concept ColumnContainer = std::convertible_to<T, QStringList>    ||
                              std::convertible_to<T, QList<QString>> ||
                              std::convertible_to<T, QList<Column>>;

    /*! QString container concept (used by ContainerUtils::join()). */
    template<typename T>
    concept JoinContainer = ColumnContainer<T>                           ||
                            std::convertible_to<T, std::set<QString>>    ||
                            std::convertible_to<T, std::deque<QString>>  ||
                            std::convertible_to<T, std::vector<QString>> ||
                            std::convertible_to<T, QList<QStringView>>   ||
                            std::convertible_to<T, QSet<QString>>;

    /*! Concept for delimiter for joining containers. */
    template<typename T>
    concept DelimiterConcept = std::convertible_to<T, QString> ||
                               std::convertible_to<T, QChar>;

    /*! QString container concept (used by ContainerUtils::countStringSizes()). */
    template<typename T>
    concept QStringContainer = requires { typename T::value_type::size_type; } &&
                               JoinContainer<T> &&
                               !std::convertible_to<T, QList<Column>>;

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_ORMCONCEPTS_HPP
