#pragma once
#ifndef ORM_TINY_RELATIONS_STORES_SERIALIZERELATIONSTORE_HPP
#define ORM_TINY_RELATIONS_STORES_SERIALIZERELATIONSTORE_HPP

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

    /*! The store for serializing relationship. */
    template<SerializedAttributes C, typename Derived,
             AllRelationsConcept ...AllRelations>
    class SerializeRelationStore final :
            public BaseRelationStore<Derived, AllRelations...>
    {
        Q_DISABLE_COPY_MOVE(SerializeRelationStore)

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
        SerializeRelationStore(
                NotNull<HasRelationStore *> hasRelationStore, const QString &relation,
                const RelationsType<AllRelations...> &models, C &attributes);
        /*! Default destructor. */
        ~SerializeRelationStore() = default;

    private:
        /*! Method called after visitation. */
        template<RelationshipMethod<Derived> Method>
        void visited(Method /*unused*/) const;

        /*! Store type initializer. */
        constexpr static RelationStoreType initStoreType();

        /*! Currently served store type, this class can handle two store types. */
        constexpr static const RelationStoreType STORE_TYPE = initStoreType(); // thread_local not needed

        /*! The name of the relationship to serialize. */
        NotNull<const QString *> m_relation;
        /*! Models to serialize, the reference to the relation in the m_relations hash. */
        NotNull<const RelationsType<AllRelations...> *> m_models;
        /*! The reference to the container that will store serialized attributes. */
        NotNull<C *> m_attributes;
    };

    /* public */

    template<SerializedAttributes C, typename Derived,
             AllRelationsConcept ...AllRelations>
    SerializeRelationStore<C, Derived, AllRelations...>::SerializeRelationStore(
            NotNull<HasRelationStore *> hasRelationStore, const QString &relation,
            const RelationsType<AllRelations...> &models, C &attributes
    )
        : BaseRelationStore_(hasRelationStore, STORE_TYPE)
        , m_relation(&relation)
        , m_models(&models)
        , m_attributes(&attributes)
    {}

    /* private */

    template<SerializedAttributes C, typename Derived,
             AllRelationsConcept ...AllRelations>
    template<RelationshipMethod<Derived> Method>
    void SerializeRelationStore<C, Derived, AllRelations...>::visited(
            const Method /*unused*/) const
    {
        using Relation = typename std::invoke_result_t<Method, Derived>::element_type;
        using Related  = typename Relation::RelatedType;

        /* Here is the last and only one chance where we can obtain the PivotType
           for the belongs-to-many relation, so we need to pass it down, so that
           the toMap() or toList() can obtain the correct pivot model type
           from the m_relations map's std::variant. */

        // belongs-to-many
        if constexpr (std::is_base_of_v<Relations::IsPivotRelation, Relation>)
            this->basemodel()
                    .template serializeRelationVisited<Related, C,
                                                       typename Relation::PivotTypeType>(
                        *m_relation, *m_models, *m_attributes);

        // has-one, has-many, and belongs-to
        else
            this->basemodel()
                    .template serializeRelationVisited<Related, C, void>(
                        *m_relation, *m_models, *m_attributes);
    }

    template<SerializedAttributes C, typename Derived,
             AllRelationsConcept ...AllRelations>
    constexpr RelationStoreType
    SerializeRelationStore<C, Derived, AllRelations...>::initStoreType()
    {
        if constexpr (std::is_same_v<C, QVariantMap>)
            return RelationStoreType::RELATION_TO_MAP;

        else if constexpr (std::is_same_v<C, QList<AttributeItem>>)
            return RelationStoreType::RELATION_TO_VECTOR;

        else
            Q_UNREACHABLE(); // Ok as the C template argument is checked by concept

            // FUTURE static_assert doesn't work here (in else block) and it should, find out why silverqx
            // This should never happen :/
            // static_assert (false, "Invalid C template argument.");
    }

} // namespace Support::Stores
} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_STORES_SERIALIZERELATIONSTORE_HPP
