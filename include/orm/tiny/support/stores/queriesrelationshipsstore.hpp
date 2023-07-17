#pragma once
#ifndef ORM_TINY_RELATIONS_STORES_QUERIESRELATIONSHIPSSTORE_HPP
#define ORM_TINY_RELATIONS_STORES_QUERIESRELATIONSHIPSSTORE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/tiny/relations/relation.hpp"
#include "orm/tiny/support/stores/baserelationstore.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{
namespace Concerns
{
    template<typename Model>
    class QueriesRelationships;
}

namespace Support::Stores
{

    /*! The store for obtaining a Relation instance for QueriesRelationships. */
    template<typename Derived, typename Related, AllRelationsConcept ...AllRelations>
    class QueriesRelationshipsStore final :
            public BaseRelationStore<Derived, AllRelations...>
    {
        Q_DISABLE_COPY(QueriesRelationshipsStore)

        /*! Alias for the NotNull. */
        template<typename T>
        using NotNull = Orm::Utils::NotNull<T>;

        /*! Alias for the BaseRelationStore (for shorter name). */
        using BaseRelationStore_ = BaseRelationStore<Derived, AllRelations...>;
        /*! Alias for the HasRelationStore (for shorter name). */
        using HasRelationStore = Concerns::HasRelationStore<Derived, AllRelations...>;

        /*! Alias for the QueriesRelationships (for shorter name). */
        template<typename Derived_> // Don't remove the Derived_ template, so the type is clearly visible below
        using QueriesRelationships = Concerns::QueriesRelationships<Derived_>;
        /*! QueriesRelationships builder type passed to the callback. */
        template<typename Related_> // Don't remove the Related_ template, so the type is clearly visible below
        using QueriesRelationshipsCallback =
              Concerns::QueriesRelationshipsCallback<Related_>;

        // To access visited()
        friend BaseRelationStore_;

    public:
        /*! QueriesRelationshipsStore constructor. */
        QueriesRelationshipsStore(
                NotNull<HasRelationStore *> hasRelationStore,
                QueriesRelationships<Derived> &origin,
                const QString &comparison, qint64 count, const QString &condition,
                const std::function<
                        void(QueriesRelationshipsCallback<Related> &)> &callback,
                std::optional<std::reference_wrapper<
                        QStringList>> relations = std::nullopt);
        /*! Default destructor. */
        inline ~QueriesRelationshipsStore() = default;

    private:
        /*! Method called after visitation. */
        template<typename RelatedFromMethod, typename Method>
        void visited(Method method);

        /*! Store type initializer. */
        constexpr static RelationStoreType initStoreType();

        /*! Currently served store type, this class can handle two store types. */
        constexpr static const RelationStoreType STORE_TYPE = initStoreType(); // thread_local not needed

        /*! The QueriesRelationships instance to which the visited relation will be
            dispatched. */
        NotNull<QueriesRelationships<Derived> *> m_origin;
        /*! Comparison operator, used during querying relationship exitence. */
        NotNull<const QString *> m_comparison;
        /*! Required number of records, used during querying relationship exitence. */
        /*const*/ qint64 m_count;
        /*! Condition operator, used during querying relationship exitence. */
        NotNull<const QString *> m_condition;
        /*! Builder callback, used during querying relationship exitence. */
        NotNull<const std::function<void(
                QueriesRelationshipsCallback<Related> &)> *> m_callback;
        /*! Nested relations for hasNested() method. */
        QStringList *m_relations;
    };

    /* QueriesRelationshipsStore<Related> is templated by Related, because it needs to
       save a reference to std::function() which takes as an argument a QueryBuilder & or
       TinyBuilder<Related> &.
       If the Related parameter is the void type, then it is served as
       the QUERIES_RELATIONSHIPS_QUERY store type and will save a QueryBuilder &.
       If the Related parameter is not void, then it is served as
       the QUERIES_RELATIONSHIPS_TINY / QUERIES_RELATIONSHIPS_TINY_NESTED and will save
       a TinyBuilder<Related> &.
       The same is true for the factory method and store's getter method, if the Related
       parameter is the void type, then the QUERIES_RELATIONSHIPS_QUERY store type
       will be created / returned, ... 🙏🙌😎
       The true is that design this was super challenge 💪🚀🔥.
       Added a new store type QUERIES_RELATIONSHIPS_TINY_NESTED for nested relations for
       hasNested() method, so don't be confused by QueriesRelationshipsStore::STORE_TYPE,
       it only holds store type which can be handled by currently instantiated
       QueriesRelationshipsStore (instantiated means template instance generated by
       the compiler, not a class instance), because the QueriesRelationshipsStore class
       can handle 3 store types, the nested store type is used on the base of ctor's
       'relations' argument. */

    /* public */

    template<typename Derived, typename Related, AllRelationsConcept ...AllRelations>
    QueriesRelationshipsStore<Derived, Related, AllRelations...>::
    QueriesRelationshipsStore(
            NotNull<HasRelationStore *> hasRelationStore,
            QueriesRelationships<Derived> &origin,
            const QString &comparison, const qint64 count, const QString &condition,
            const std::function<void(QueriesRelationshipsCallback<Related> &)> &callback,
            const std::optional<std::reference_wrapper<QStringList>> relations
    )
        : BaseRelationStore_(hasRelationStore,
                             relations
                                 ? RelationStoreType::QUERIES_RELATIONSHIPS_TINY_NESTED
                                 : STORE_TYPE)
        , m_origin(&origin)
        , m_comparison(&comparison)
        , m_count(count)
        , m_condition(&condition)
        , m_callback(&callback)
        , m_relations(relations ? &relations->get() : nullptr)
    {}

    /* private */

    template<typename Derived, typename Related, AllRelationsConcept ...AllRelations>
    template<typename RelatedFromMethod, typename Method>
    void QueriesRelationshipsStore<Derived, Related, AllRelations...>::visited(
            const Method method)
    {
        // This check can be done for the QUERIES_RELATIONSHIPS_TINY store type only
        if constexpr (!std::is_void_v<Related>)
            static_assert (std::is_same_v<Related, RelatedFromMethod>,
                    "How did this happen? Is a mystery 😎, a Related type has to be "
                    "the same as a RelatedFromMethod.");

        // We want to run a relationship query without any constraints
        auto relationInstance =
                Relations::Relation<Derived, RelatedFromMethod>::noConstraints(
                    [this, &method]
        {
            return std::invoke(method, this->model());
        });

        // Nested store type, used by hasNested()
        if (this->getStoreType() == RelationStoreType::QUERIES_RELATIONSHIPS_TINY_NESTED)
            m_origin->template hasInternalVisited<RelatedFromMethod>(
                        std::move(relationInstance), *m_comparison, m_count,
                        *m_condition, *m_relations);
        else
            m_origin->template has<RelatedFromMethod>(
                        std::move(relationInstance), *m_comparison, m_count,
                        *m_condition, *m_callback);
    }

    template<typename Derived, typename Related, AllRelationsConcept ...AllRelations>
    constexpr RelationStoreType
    QueriesRelationshipsStore<Derived, Related, AllRelations...>::initStoreType()
    {
        if constexpr (std::is_void_v<Related>)
            return RelationStoreType::QUERIES_RELATIONSHIPS_QUERY;
        else
            return RelationStoreType::QUERIES_RELATIONSHIPS_TINY;
    }

} // namespace Support::Stores
} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_STORES_QUERIESRELATIONSHIPSSTORE_HPP