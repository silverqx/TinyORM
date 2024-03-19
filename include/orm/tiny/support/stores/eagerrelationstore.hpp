#pragma once
#ifndef ORM_TINY_RELATIONS_STORES_EAGERRELATIONSTORE_HPP
#define ORM_TINY_RELATIONS_STORES_EAGERRELATIONSTORE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/tiny/support/stores/baserelationstore.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Support::Stores
{

    /* clang <16 crash during compilation during the EagerRelationStore template
       instantiation, may be it has a problem with the forward declaration above. */
    /*! The store for loading eager relations. */
    template<typename Derived,
             TINY_RSA_ERS_CLANG16(TINY_RSA_ERS_IS_CLANG16) CollectionModel,
             AllRelationsConcept ...AllRelations>
    class EagerRelationStore final : public BaseRelationStore<Derived, AllRelations...>
    {
        Q_DISABLE_COPY_MOVE(EagerRelationStore)

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
        EagerRelationStore(
                NotNull<HasRelationStore *> hasRelationStore,
                const Tiny::TinyBuilder<Derived> &builder,
                ModelsCollection<CollectionModel> &models, const WithItem &relation);
        /*! Default destructor. */
        ~EagerRelationStore() = default;

    private:
        /*! Method called after visitation. */
        template<RelationshipMethod<Derived> Method>
        void visited(Method method) const;

        /*! Store type initializer. */
        constexpr static RelationStoreType initStoreType();

        /*! Currently served store type, this class can handle two store types. */
        constexpr static const RelationStoreType STORE_TYPE = initStoreType(); // thread_local not needed

        /*! The Tiny builder instance to which the visited relation will be dispatched. */
        NotNull<const Tiny::TinyBuilder<Derived> *> m_builder;
        /*! Models on which to do an eager load, hydrated models that were obtained
            from the database and these models will be passed as parameter
            to the TinyBuilder. */
        NotNull<ModelsCollection<CollectionModel> *> m_models;
        /*! The WithItem that will be passed as parameter to the TinyBuilder. */
        NotNull<const WithItem *> m_relation;
    };

    /* public */

    template<typename Derived,
             TINY_RSA_ERS_CLANG16(TINY_RSA_ERS_IS_CLANG16) CollectionModel,
             AllRelationsConcept ...AllRelations>
    EagerRelationStore<Derived, CollectionModel, AllRelations...>::EagerRelationStore(
            NotNull<HasRelationStore *> hasRelationStore,
            const Tiny::TinyBuilder<Derived> &builder,
            ModelsCollection<CollectionModel> &models, const WithItem &relation
    )
        : BaseRelationStore_(hasRelationStore, STORE_TYPE)
        , m_builder(&builder)
        , m_models(&models)
        , m_relation(&relation)
    {}

    /* private */

    template<typename Derived,
             TINY_RSA_ERS_CLANG16(TINY_RSA_ERS_IS_CLANG16) CollectionModel,
             AllRelationsConcept ...AllRelations>
    template<RelationshipMethod<Derived> Method>
    void EagerRelationStore<Derived, CollectionModel, AllRelations...>::visited(
            const Method method) const
    {
        using Related = typename std::invoke_result_t<Method, Derived>
                                    ::element_type::RelatedType;

        /*! Helping model for eager loads, because Relation::m_parent has to be
            reference (Relation::m_parent == dummyModel), this dummy model prevents
            dangling reference, we have to secure that the model passed to the relation
            method called below inside Relation::noConstraints() will live long enough,
            to not become a dangling reference.
            Have to exists, until the 'relation->match()' is processed
            in the TinyBuilder::eagerLoadRelationVisited().
            Look at the Relation::m_parent for additional info. */
        auto dummyModel = this->basemodel().newInstance();

        /* We want to run a relationship query without any constraints so that we will
           not have to remove these where clauses manually which gets really hacky
           and error prone. We don't want constraints because we add eager ones. */
        auto relationInstance =
                Relations::Relation<Derived, Related>::noConstraints(
                    [&method, &dummyModel]
        {
            return std::invoke(method, dummyModel);
        });

        m_builder->eagerLoadRelationVisited(relationInstance, *m_models, *m_relation);
    }

    template<typename Derived,
             TINY_RSA_ERS_CLANG16(TINY_RSA_ERS_IS_CLANG16) CollectionModel,
             AllRelationsConcept ...AllRelations>
    constexpr RelationStoreType
    EagerRelationStore<Derived, CollectionModel, AllRelations...>::initStoreType()
    {
        if constexpr (std::is_pointer_v<CollectionModel>)
            return RelationStoreType::EAGER_POINTERS;
        else
            return RelationStoreType::EAGER;
    }

} // namespace Orm::Tiny::Support::Stores

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_STORES_EAGERRELATIONSTORE_HPP
