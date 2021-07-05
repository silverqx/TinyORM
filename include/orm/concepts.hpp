#ifndef ORM_CONCEPTS_HPP
#define ORM_CONCEPTS_HPP

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

namespace Query
{
    class Builder;
    class Expression;
}
    using QueryBuilder = Query::Builder;

    /*! Concept for the subquery, used in the from clause (tablename), join clause, ... */
    template<typename T>
    concept SubQuery = std::convertible_to<T, Orm::QueryBuilder &> ||
                       std::convertible_to<T, QString> ||
                       std::invocable<T, Orm::QueryBuilder &>;

    /*! Concept for the join's table. */
    template<typename T>
    concept JoinTable = std::same_as<T, Query::Expression> ||
                        std::convertible_to<T, QString>;

    /*! Concept for the table column. */
    template<typename T>
    concept ColumnConcept =
            std::convertible_to<T, std::variant<QString, Query::Expression>>;

    /*! Concept for the QString. */
    template<typename T>
    concept QStringConcept = std::convertible_to<T, const QString &> ||
                             std::convertible_to<T, QString>;

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // ORM_CONCEPTS_HPP
