#pragma once
#ifndef ORM_TINY_RELATIONS_STORES_TOUCHOWNERSRELATIONSTORE_HPP
#define ORM_TINY_RELATIONS_STORES_TOUCHOWNERSRELATIONSTORE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/tiny/support/stores/baserelationstore.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Support::Stores
{

    /*! The store for touching owner's timestamps. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class TouchOwnersRelationStore final :
            public BaseRelationStore<Derived, AllRelations...>
    {
        Q_DISABLE_COPY_MOVE(TouchOwnersRelationStore)

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
        TouchOwnersRelationStore(NotNull<HasRelationStore *> hasRelationStore,
                                 const QString &relation);
        /*! Default destructor. */
        ~TouchOwnersRelationStore() = default;

    private:
        /*! Method called after visitation. */
        template<RelationshipMethod<Derived> Method>
        void visited(Method method);

        /*! Models to touch timestamps for, the reference to the relation name/key
            in the m_relations hash. */
        NotNull<const QString *> m_relation;
    };

    /* public */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    TouchOwnersRelationStore<Derived, AllRelations...>::TouchOwnersRelationStore(
            NotNull<HasRelationStore *> hasRelationStore, const QString &relation
    )
        : BaseRelationStore_(hasRelationStore, RelationStoreType::TOUCH_OWNERS)
        , m_relation(&relation)
    {}

    /* private */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<RelationshipMethod<Derived> Method>
    void
    TouchOwnersRelationStore<Derived, AllRelations...>::visited(const Method method)
    {
        auto relationInstance = std::invoke(method, this->model());

        using Related = typename std::invoke_result_t<Method, Derived>
                                    ::element_type::RelatedType;

        this->basemodel().template touchOwnersVisited<Related>(relationInstance,
                                                               *m_relation);
    }

} // namespace Orm::Tiny::Support::Stores

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_STORES_TOUCHOWNERSRELATIONSTORE_HPP
