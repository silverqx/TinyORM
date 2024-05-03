#pragma once
#ifndef ORM_TINY_RELATIONS_STORES_BELONGSTOMANYRELATEDTABLESTORE_HPP
#define ORM_TINY_RELATIONS_STORES_BELONGSTOMANYRELATEDTABLESTORE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/threadlocal.hpp"
#include "orm/tiny/support/stores/baserelationstore.hpp"
#include "orm/utils/helpers.hpp"

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
        Q_DISABLE_COPY_MOVE(BelongsToManyRelatedTableStore)

        // To access visitWithResult()
        friend Concerns::HasRelationships<Derived, AllRelations...>;

        /*! Alias for the helper utils. */
        using Helpers = Orm::Utils::Helpers;
        /*! Alias for the NotNull. */
        template<typename T>
        using NotNull = Orm::Utils::NotNull<T>;

        /*! Alias for the BaseRelationStore (for shorter name). */
        using BaseRelationStore_ = BaseRelationStore<Derived, AllRelations...>;
        /*! Alias for the HasRelationStore (for shorter name). */
        using HasRelationStore = Concerns::HasRelationStore<Derived, AllRelations...>;

        // To access visited()
        friend BaseRelationStore_;

    public:
        /*! Constructor. */
        explicit
        BelongsToManyRelatedTableStore(NotNull<HasRelationStore *> hasRelationStore);
        /*! Default destructor. */
        ~BelongsToManyRelatedTableStore() = default;

    private:
        /*! Visit the given relation and return a result. */
        std::optional<QString> visitWithResult(const QString &relation);

        /*! Method called after visitation. */
        template<RelationshipMethod<Derived> Method>
        void visited(Method /*unused*/);

        /*! The cache key for std::unordered_map cache. */
        struct CacheKey
        {
            /*! Relation name. */
            QString relation;

            /*! Equality comparison operator for the CacheKey. */
            bool operator==(const CacheKey &) const = default;
        };

        /*! Hasher for the result cache key. */
        struct CacheKeyHasher
        {
            /*! Generate hash for the given CacheKey. */
            inline std::size_t operator()(const CacheKey &cacheKey) const noexcept;
        };

        /*! The related table name result cache type. */
        using CacheType = std::unordered_map<CacheKey, std::optional<QString>,
                                             CacheKeyHasher>;

        /*! Get the related table name result cache. */
        inline CacheType &cache() const;

        /*! The related table name result. */
        std::optional<QString> m_result = std::nullopt;
    };

    /* BelongsToManyRelatedTableStore*/

    /* public */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    BelongsToManyRelatedTableStore<Derived, AllRelations...>::
    BelongsToManyRelatedTableStore(NotNull<HasRelationStore *> hasRelationStore)
        : BaseRelationStore_(hasRelationStore,
                             RelationStoreType::BELONGSTOMANY_RELATED_TABLE)
    {}

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<QString>
    BelongsToManyRelatedTableStore<Derived, AllRelations...>::visitWithResult(
            const QString &relation)
    {
        /* Skip visitation if the result is already cached. 🕺
           I'm really surprised how well this cache works. */
        if (auto &cache = this->cache();
            cache.contains({relation})
        )
            return cache.find({relation})->second;

        BaseRelationStore_::visit(relation);

        /* Cache the result so it will visit only once for the Derived model and
           the given relation. */
        cache().template emplace<CacheKey>({relation}, m_result);

        return m_result;
    }

    /* private */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<RelationshipMethod<Derived> Method>
    void BelongsToManyRelatedTableStore<Derived, AllRelations...>::visited(
            const Method /*unused*/)
    {
        using Relation = typename std::invoke_result_t<Method, Derived>::element_type;

        if constexpr (std::is_base_of_v<Relations::IsPivotRelation, Relation>)
            m_result = typename Relation::RelatedType().getTable();
    }

    /* CacheKeyHasher */

    /* public */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::size_t
    BelongsToManyRelatedTableStore<Derived, AllRelations...>::CacheKeyHasher::
    operator()(const CacheKey &cacheKey) const noexcept
    {
        std::size_t resultHash = 0;

        const auto typeInfoHashCode = typeid (CacheKey).hash_code();

        Helpers::hashCombine<QString>(resultHash, cacheKey.relation);
        Helpers::hashCombine<std::size_t>(resultHash, typeInfoHashCode);

        return resultHash;
    }

    /* BelongsToManyRelatedTableStore*/

    /* private */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    typename BelongsToManyRelatedTableStore<Derived, AllRelations...>::CacheType &
    BelongsToManyRelatedTableStore<Derived, AllRelations...>::cache() const
    {
        /* I had also used mutex in the visitWithResult(), but I reverted it because
           I wouldn't say I liked it as the mutex was locked too long, the thread_local
           will be much faster. */
        T_THREAD_LOCAL
        static CacheType cache(static_cast<CacheType::size_type>(
                                   this->basemodel().getUserRelations().size()));

        return cache;
    }

} // namespace Support::Stores
} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_STORES_BELONGSTOMANYRELATEDTABLESTORE_HPP
