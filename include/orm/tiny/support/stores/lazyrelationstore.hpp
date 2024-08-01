#pragma once
#ifndef ORM_TINY_RELATIONS_STORES_LAZYRELATIONSTORE_HPP
#define ORM_TINY_RELATIONS_STORES_LAZYRELATIONSTORE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/tiny/support/stores/baserelationstore.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Support::Stores
{

    /*! The store for the lazy loading. */
    template<typename Derived, typename Related, AllRelationsConcept ...AllRelations>
    class LazyRelationStore final : public BaseRelationStore<Derived, AllRelations...>
    {
        Q_DISABLE_COPY_MOVE(LazyRelationStore)

        // To access result()
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
        explicit LazyRelationStore(NotNull<HasRelationStore *> hasRelationStore);
        /*! Default destructor. */
        ~LazyRelationStore() = default;

    private:
        /*! Get the result of lazy load. */
        inline const std::variant<ModelsCollection<Related>, std::optional<Related>> &
        result() const noexcept;

        /*! Method called after visitation. */
        template<RelationshipMethod<Derived> Method>
        void visited(Method method);

        /*! The result of lazy load. */
        std::variant<ModelsCollection<Related>, std::optional<Related>> m_result;
    };

    /* public */

    template<typename Derived, typename Related, AllRelationsConcept ...AllRelations>
    LazyRelationStore<Derived, Related, AllRelations...>::LazyRelationStore(
            NotNull<HasRelationStore *> hasRelationStore
    )
        : BaseRelationStore_(hasRelationStore, RelationStoreType::LAZY_RESULTS)
    {}

    template<typename Derived, typename Related, AllRelationsConcept ...AllRelations>
    const std::variant<ModelsCollection<Related>, std::optional<Related>> &
    LazyRelationStore<Derived, Related, AllRelations...>::result() const noexcept
    {
        return m_result;
    }

    /* private */

    template<typename Derived, typename Related, AllRelationsConcept ...AllRelations>
    template<RelationshipMethod<Derived> Method>
    void
    LazyRelationStore<Derived, Related, AllRelations...>::visited(const Method method)
    {
        m_result = std::invoke(method, this->model())->getResults();
    }

} // namespace Orm::Tiny::Support::Stores

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_STORES_LAZYRELATIONSTORE_HPP
