#pragma once
#ifndef ORMTYPES_H
#define ORMTYPES_H

#include <QSharedPointer>
#include <QVariant>
#include <QVector>

#if defined(__clang__) || (defined(_MSC_VER) && _MSC_VER <= 1928)
#include <range/v3/algorithm/unique.hpp>
#endif

#include "orm/constants.hpp"
#include "orm/query/expression.hpp"
#include "orm/utils/export.hpp"

// TODO divide OrmTypes to internal and types which user will / may need, so divide to two files silverqx
/* 👆 I have good idea hot to do that, public types will be tinytypes.hpp and private will be
   types.hpp, and divide it as most as possible when needed, so eg Reconnector type to
   types/reconnectortype.hpp. */

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{
    using namespace Orm::Constants;

namespace Query
{
    class Builder;
}
    using QueryBuilder = Query::Builder;

    /*! Type for the database column. */
    using Column = std::variant<QString, Query::Expression>;

    /*! From clause defined in the QueryBuilder. */
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

    using BindingsMap = QMap<BindingType, QVector<QVariant>>;

    struct AggregateItem
    {
        QString         function;
        QVector<Column> columns;
    };

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

    struct WhereConditionItem
    {
        Column                       column      {};
        QVariant                     value       {};
        QString                      comparison  {EQ};
        QString                      condition   {AND};
        WhereType                    type        {WhereType::UNDEFINED};
        QSharedPointer<QueryBuilder> nestedQuery {nullptr};
        QVector<QVariant>            values      {};
        Column                       columnTwo   {};
        QString                      sql         {};
    };

    enum struct HavingType
    {
        UNDEFINED = -1,
        BASIC,
        RAW,
    };

    struct HavingConditionItem
    {
        Column     column     {};
        QVariant   value      {};
        QString    comparison {EQ};
        QString    condition  {AND};
        HavingType type       {HavingType::UNDEFINED};
        QString    sql        {};
    };

    struct OrderByItem
    {
        Column      column    {};
        QString     direction {ASC};
        QString     sql       {};
    };

    struct SHAREDLIB_EXPORT UpdateItem
    {
        QString  column;
        QVariant value;
    };

    struct ResultItem
    {
        QString  column;
        QVariant value;
    };

    // TODO types, also divide types by namespace, eg AttributeItem is only used in the Orm::Tiny namespace, so an user can use 'using namespace Orm::Tiny' in model files, it is not possible now, because he has to use symbols from an Orm namespace too silverqx
    // TODO pretty print in the debugger silverqx
    struct SHAREDLIB_EXPORT AttributeItem
    {
        QString  key;
        QVariant value;

        operator UpdateItem() const;
    };

    SHAREDLIB_EXPORT bool operator==(const AttributeItem &lhs, const AttributeItem &rhs);

    struct SHAREDLIB_EXPORT WhereItem
    {
        Column   column;
        QVariant value;
        QString  comparison {EQ};
        QString  condition  {};

        operator AttributeItem() const;
    };

    struct WhereColumnItem
    {
        Column  first;
        Column  second;
        QString comparison {EQ};
        QString condition  {};
    };

    // TODO types, belongs to Orm::Tiny namespace silverqx
    /*! Eager load relation item. */
    struct WithItem
    {
        QString name;
        std::function<void(QueryBuilder &)> constraints {nullptr};
    };

//    SHAREDLIB_EXPORT bool operator==(const WithItem &lhs, const WithItem &rhs);

    /*! Tag for Model::getRelation() family methods to return Related type
        directly ( not container type ). */
    struct SHAREDLIB_EXPORT One {};
    /*! Tag for Model::getRelationshipFromMethod() to return QVector<Related>
        type ( 'Many' relation types ), only internal type for now, used as the template
        tag in the Model::pushVisited. */
    struct Many {};

    /*! Options parameter type used in Model save() method. */
    struct SHAREDLIB_EXPORT SaveOptions
    {
        /*! Indicates if timestamps of parent models should be touched. */
        bool touch = true;
    };

    class SHAREDLIB_EXPORT SyncChanges final : public std::map<QString, QVector<QVariant>>
    {
    public:
        SyncChanges();

        /*! Merge changes into the current instance. */
        template<typename KeyType>
        SyncChanges &merge(SyncChanges &&changes);
        /*! Determine if the given key is supported. */
        bool supportedKey(const QString &key) const;

    protected:
        /*! Cast the given key to primary key type. */
        template<typename T>
        T castKey(const QVariant &key) const;

    private:
        /*! All of the supported keys. */
        inline static QVector<QString> SyncKeys {"attached", "detached", "updated"};
    };

    template<typename KeyType>
    SyncChanges &SyncChanges::merge(SyncChanges &&changes)
    {
        for (auto &&[key, values] : changes) {
            {
                auto &currentValues = (*this)[key];

                // If the current key value is empty, then simply move a new values
                if (supportedKey(key) && currentValues.isEmpty()) {
                    if (!values.isEmpty())
                        (*this)[key] = std::move(values);

                    continue;
                }
            }

            // Otherwise merge values
            const auto castKey = [this](const auto &id)
            {
                return this->castKey<KeyType>(id);
            };

            /* First we need to make a copy and then sort both values, vectors
               have to be sorted before the merge. */
            auto currentValues = (*this)[key];
            std::ranges::sort(currentValues, {}, castKey);
            std::ranges::sort(values, {}, castKey);

            // Then merge two vectors
            QVector<QVariant> merged;
            merged.reserve(currentValues.size() + values.size());
            std::ranges::merge(currentValues, values, std::back_inserter(merged),
                               {}, castKey, castKey);

            // Remove duplicates
            // BUG clang doesn't compile with std::ranges::unique, I don't unserstand why silverqx
#if (defined(__GNUG__) && !defined(__clang__)) || (defined(_MSC_VER) && _MSC_VER >= 1929)
            auto [first, last] = std::ranges::unique(merged, {}, castKey);
            merged.erase(first, last);
#else
            auto it = ranges::unique(merged, {}, castKey);
            merged.erase(it, ranges::end(merged));
#endif

            (*this)[key].swap(merged);
        }

        return *this;
    }

    template<typename T>
    inline T SyncChanges::castKey(const QVariant &key) const
    {
        return key.value<T>();
    }

    // CUR make order in ormtypes, eg move this to concepts silverqx
    /*! Concept for Model's AllRelations template parameter, AllRelations can not
        contain actual model type declared in the Derived template parameter. */
    template<typename Derived, typename ...AllRelations>
    concept AllRelationsConcept = (!std::same_as<Derived, AllRelations> && ...);

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#ifdef TINYORM_COMMON_NAMESPACE
Q_DECLARE_METATYPE(TINYORM_COMMON_NAMESPACE::Orm::WhereConditionItem)
#else
Q_DECLARE_METATYPE(Orm::WhereConditionItem)
#endif

#endif // ORMTYPES_H
