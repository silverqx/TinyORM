#pragma once
#ifndef ORM_TINY_RELATIONS_STORES_BELONGSTOMANYRELATEDTABLESTORE_HPP
#define ORM_TINY_RELATIONS_STORES_BELONGSTOMANYRELATEDTABLESTORE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/tiny/support/stores/baserelationstore.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{
namespace Relations
{
    class IsPivotRelation;
}

namespace Support::Stores
{

    /*! The store to obtain the related table name for BelongsToMany relation. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class BelongsToManyRelatedTableStore final :
            public BaseRelationStore<Derived, AllRelations...>
    {
        Q_DISABLE_COPY(BelongsToManyRelatedTableStore)

        /*! Alias for the NotNull. */
        template<typename T>
        using NotNull = Orm::Utils::NotNull<T>;

        /* Aliases to shorten type names */
        /*! Alias for the BaseRelationStore (for shorter name). */
        using BaseRelationStore = BaseRelationStore<Derived, AllRelations...>;
        /*! Alias for the HasRelationStore (for shorter name). */
        using HasRelationStore = Concerns::HasRelationStore<Derived, AllRelations...>;

    public:
        /*! Constructor. */
        explicit
        BelongsToManyRelatedTableStore(NotNull<HasRelationStore *> hasRelationStore);
        /*! Default destructor. */
        inline ~BelongsToManyRelatedTableStore() = default;

        /*! Method called after visitation. */
        template<RelationshipMethod<Derived> Method>
        void visited(Method /*unused*/);

        /*! The related table name result. */
        std::optional<QString> m_result = std::nullopt;
    };

    /* public */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    BelongsToManyRelatedTableStore<Derived, AllRelations...>::
    BelongsToManyRelatedTableStore(NotNull<HasRelationStore *> hasRelationStore)
        : BaseRelationStore(hasRelationStore,
                            RelationStoreType::BELONGSTOMANY_RELATED_TABLE)
    {}

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<RelationshipMethod<Derived> Method>
    void BelongsToManyRelatedTableStore<Derived, AllRelations...>::visited(
            const Method /*unused*/)
    {
        using Relation = typename std::invoke_result_t<Method, Derived>::element_type;

        if constexpr (std::is_base_of_v<Relations::IsPivotRelation, Relation>)
            m_result = typename Relation::RelatedType().getTable();
    }

} // namespace Support::Stores
} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_STORES_BELONGSTOMANYRELATEDTABLESTORE_HPP
