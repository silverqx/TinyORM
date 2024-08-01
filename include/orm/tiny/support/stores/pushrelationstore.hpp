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
        Q_DISABLE_COPY_MOVE(PushRelationStore)

        // To access result(), setResult(), and models()
        friend Concerns::HasRelationships<Derived, AllRelations...>;

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
        PushRelationStore(NotNull<HasRelationStore *> hasRelationStore,
                          RelationsType<AllRelations...> &models);
        /*! Default destructor. */
        ~PushRelationStore() = default;

    private:
        /*! Get the result of a push. */
        inline bool result() const noexcept;
        /*! Set the result of a push. */
        inline void setResult(bool result) noexcept;

        /*! Get models to push, the reference to the relation in the m_relations hash. */
        inline RelationsType<AllRelations...> &models() const noexcept;

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
        : BaseRelationStore_(hasRelationStore, RelationStoreType::PUSH)
        , m_models(&models)
    {}

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool PushRelationStore<Derived, AllRelations...>::result() const noexcept
    {
        return m_result;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void
    PushRelationStore<Derived, AllRelations...>::setResult(const bool result) noexcept
    {
        m_result = result;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    RelationsType<AllRelations...> &
    PushRelationStore<Derived, AllRelations...>::models() const noexcept
    {
        return *m_models;
    }

    /* private */

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
