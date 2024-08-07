#pragma once
#ifndef ORM_TINY_RELATIONS_STORES_BASERELATIONSTORE_HPP
#define ORM_TINY_RELATIONS_STORES_BASERELATIONSTORE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/tiny/macros/relationstoresaliases.hpp"
#include "orm/tiny/tinytypes.hpp"
#include "orm/utils/notnull.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{
namespace Concerns
{
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class HasRelationStore;

    template<typename Derived, AllRelationsConcept ...AllRelations>
    class HasRelationships;
} // namespace Concerns
namespace Relations
{
    class IsRelation;
}

namespace Support::Stores
{

    /*! Concept to check the relationship method. */
    template<typename Method, typename Derived>
    concept RelationshipMethod =
            ModelConcept<Derived> &&
            std::is_member_function_pointer_v<Method> &&
            std::is_convertible_v<std::invoke_result_t<Method, Derived>,
                                  std::unique_ptr<Relations::IsRelation>>;

    /* Forward declarations */
    /*! The store for loading eager relations. */
    template<typename Derived,
             TINY_RSA_ERS_CLANG16(TINY_RSA_ERS_IS_CLANG16) CollectionModel,
             AllRelationsConcept ...AllRelations>
    class EagerRelationStore;
    template<typename Derived, AllRelationsConcept ...AllRelations>
    /*! The store for the Model push() method. */
    class PushRelationStore;
    template<typename Derived, AllRelationsConcept ...AllRelations>
    /*! The store for touching owner's timestamps. */
    class TouchOwnersRelationStore;
    template<typename Derived, typename Related, AllRelationsConcept ...AllRelations>
    /*! The store for the lazy loading. */
    class LazyRelationStore;
    /*! The store to obtain the related table name for BelongsToMany relation. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class BelongsToManyRelatedTableStore;
    /*! The store for obtaining a Relation instance for QueriesRelationships. */
    template<typename Derived, typename Related, AllRelationsConcept ...AllRelations>
    class QueriesRelationshipsStore;
    /*! The store for serializing relationship. */
    template<SerializedAttributes C, typename Derived,
             AllRelationsConcept ...AllRelations>
    class SerializeRelationStore;

    /*! Type of data saved in the relation store. */
    enum struct RelationStoreType : quint8
    {
        /*! The store for loading eager relations. */
        EAGER,
        /*! The store for loading eager relations (collection of pointers). */
        EAGER_POINTERS,
        /*! The store for the Model push() method. */
        PUSH,
        /*! The store for touching owner's timestamps. */
        TOUCH_OWNERS,
        /*! The store for the lazy loading. */
        LAZY_RESULTS,
        /*! The store to obtain the related table name for BelongsToMany relation. */
        BELONGSTOMANY_RELATED_TABLE,
        /*! Save arguments needed to build a query for last relation in hasNested(). */
        QUERIES_RELATIONSHIPS_QUERY,
        /*! Save arguments needed to build a query for last relation in hasNested(). */
        QUERIES_RELATIONSHIPS_TINY,
        /*! Save arguments needed to build a query for last relation in hasNested(). */
        QUERIES_RELATIONSHIPS_TINY_NESTED,
        /*! The store for serializing relationship (QVariantMap). */
        RELATION_TO_MAP,
        /*! The store for serializing relationship (QList<AttributeItem>). */
        RELATION_TO_VECTOR,
    };

    /*! Base class for relation stores. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class BaseRelationStore
    {
        Q_DISABLE_COPY_MOVE(BaseRelationStore)

        // To access visit()
        friend Concerns::HasRelationships<Derived, AllRelations...>;
        // To access operator()
        friend Derived;

        /*! Alias for the NotNull. */
        template<typename T>
        using NotNull = Orm::Utils::NotNull<T>;

        /* Aliases to shorten type names, yes yes templated c++ 😵‍💫 */
        /*! Alias for the HasRelationStore (for shorter name). */
        using HasRelationStore = Concerns::HasRelationStore<Derived, AllRelations...>;

        TINY_RELATIONSTORES_ALIASES

        // To access visit()
        friend BelongsToManyRelatedTableStore;

    protected:
        /*! Constructor. */
        BaseRelationStore(NotNull<HasRelationStore *> hasRelationStore,
                          RelationStoreType storeType);

    public:
        /*! Default destructor. */
        ~BaseRelationStore() = default;

    protected:
        /*! Called from Model::u_relations to pass reference to the relation method,
            an enter point of the visitation. */
        template<RelationshipMethod<Derived> Method>
        void operator()(Method method);

        /*! Currently held store type. */
        inline RelationStoreType getStoreType() const noexcept;

        /* Static cast this to a child's instance type (CRTP) */
        /*! Static cast this to a child's instance type (CRTP). */
        inline Derived &model() noexcept;
        /*! Static cast this to a child's instance type (CRTP), const version. */
        inline const Derived &model() const noexcept;

        /*! Static cast this to a child's instance Model type. */
        inline Model<Derived, AllRelations...> &basemodel() noexcept;
        /*! Static cast this to a child's instance Model type, const version. */
        inline const Model<Derived, AllRelations...> &basemodel() const noexcept;

    private:
        /*! Visit the given relation. */
        void visit(const QString &relation);

        /*! Reference to the parent HasRelationStore instance. */
        NotNull<HasRelationStore *> m_hasRelationStore;
        /*! Store type held by relation store. */
        /*const*/ RelationStoreType m_storeType;
    };

    /* protected */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    BaseRelationStore<Derived, AllRelations...>::BaseRelationStore(
            NotNull<HasRelationStore *> hasRelationStore,
            const RelationStoreType storeType
    )
        : m_hasRelationStore(hasRelationStore)
        , m_storeType(storeType)
    {}

    /* public */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<RelationshipMethod<Derived> Method>
    void BaseRelationStore<Derived, AllRelations...>::operator()(const Method method)
    {
        const auto storeType = getStoreType();

        switch (storeType) {
        case RelationStoreType::EAGER:
            static_cast<EagerRelationStore<Derived> &>(*this).visited(method);
            break;

        case RelationStoreType::EAGER_POINTERS:
            static_cast<EagerRelationStore<Derived *> &>(*this).visited(method);
            break;

        case RelationStoreType::TOUCH_OWNERS:
            static_cast<TouchOwnersRelationStore &>(*this).visited(method);
            break;

        case RelationStoreType::PUSH:
            static_cast<PushRelationStore &>(*this).visited(method);
            break;

        case RelationStoreType::BELONGSTOMANY_RELATED_TABLE:
            static_cast<BelongsToManyRelatedTableStore &>(*this).visited(method);
            break;

        case RelationStoreType::LAZY_RESULTS:
        case RelationStoreType::QUERIES_RELATIONSHIPS_QUERY:
        case RelationStoreType::QUERIES_RELATIONSHIPS_TINY:
        case RelationStoreType::QUERIES_RELATIONSHIPS_TINY_NESTED:
        case RelationStoreType::RELATION_TO_MAP:
        case RelationStoreType::RELATION_TO_VECTOR:
        {
            using Related = typename std::invoke_result_t<Method, Derived>
                                        ::element_type::RelatedType;

            switch (storeType) {
            case RelationStoreType::LAZY_RESULTS:
                static_cast<LazyRelationStore<Related> &>(*this).visited(method);
                break;

            case RelationStoreType::QUERIES_RELATIONSHIPS_QUERY:
            case RelationStoreType::QUERIES_RELATIONSHIPS_TINY_NESTED:
                static_cast<QueriesRelationshipsStore<void> &>(*this)
                        .template visited<Related>(method);
                break;

            case RelationStoreType::QUERIES_RELATIONSHIPS_TINY:
                static_cast<QueriesRelationshipsStore<Related> &>(*this)
                        .template visited<Related>(method);
                break;

            case RelationStoreType::RELATION_TO_MAP:
                static_cast<SerializeRelationStore<QVariantMap> &>(*this)
                        .visited(method);
                break;

            case RelationStoreType::RELATION_TO_VECTOR:
                static_cast<SerializeRelationStore<QList<AttributeItem>> &>(*this)
                        .visited(method);
                break;

            default:
                Q_UNREACHABLE();
            }
        }
        break;

        default:
            Q_UNREACHABLE();
        }
    }

    /* protected */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    RelationStoreType
    BaseRelationStore<Derived, AllRelations...>::getStoreType() const noexcept
    {
        return m_storeType;
    }

    /* Static cast this to a child's instance type (CRTP) */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    BaseRelationStore<Derived, AllRelations...>::model() noexcept
    {
        return m_hasRelationStore->model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const Derived &
    BaseRelationStore<Derived, AllRelations...>::model() const noexcept
    {
        return m_hasRelationStore->model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Model<Derived, AllRelations...> &
    BaseRelationStore<Derived, AllRelations...>::basemodel() noexcept
    {
        return m_hasRelationStore->basemodel();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const Model<Derived, AllRelations...> &
    BaseRelationStore<Derived, AllRelations...>::basemodel() const noexcept
    {
        return m_hasRelationStore->basemodel();
    }

    /* private */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void BaseRelationStore<Derived, AllRelations...>::visit(const QString &relation)
    {
        std::invoke(basemodel().getUserRelations().find(relation).value(), *this);
    }

} // namespace Support::Stores
} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_STORES_BASERELATIONSTORE_HPP
