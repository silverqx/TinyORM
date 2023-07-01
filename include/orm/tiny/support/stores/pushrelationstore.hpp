#pragma once
#ifndef ORM_TINY_RELATIONS_STORES_PUSHRELATIONSTORE_HPP
#define ORM_TINY_RELATIONS_STORES_PUSHRELATIONSTORE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/tiny/support/stores/baserelationstore.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Support::Stores
{

    /*! The store for the Model push() method. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class PushRelationStore final : public BaseRelationStore<Derived, AllRelations...>
    {
        Q_DISABLE_COPY(PushRelationStore)

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
        PushRelationStore(NotNull<HasRelationStore *> hasRelationStore,
                          RelationsType<AllRelations...> &models);
        /*! Default destructor. */
        inline ~PushRelationStore() = default;

        /*! Method called after visitation. */
        template<RelationshipMethod<Derived> Method>
        void visited(Method /*unused*/) const;

        /*! Models to push, the reference to the relation in the m_relations hash. */
        NotNull<RelationsType<AllRelations...> *> m_models;
        /*! The result of a push. */
        bool m_result = false;
    };

    /* public */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    PushRelationStore<Derived, AllRelations...>::PushRelationStore(
            NotNull<HasRelationStore *> hasRelationStore,
            RelationsType<AllRelations...> &models
    )
        : BaseRelationStore(hasRelationStore, RelationStoreType::PUSH)
        , m_models(&models)
    {}

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<RelationshipMethod<Derived> Method>
    void
    PushRelationStore<Derived, AllRelations...>::visited(const Method /*unused*/) const
    {
        using Related = typename std::invoke_result_t<Method, Derived>
                                    ::element_type::RelatedType;

        this->basemodel().template pushVisited<Related>();
    }

} // namespace Orm::Tiny::Support::Stores

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_STORES_PUSHRELATIONSTORE_HPP
