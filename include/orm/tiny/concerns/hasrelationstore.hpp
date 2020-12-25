#ifndef HASRELATIONSTORE_H
#define HASRELATIONSTORE_H

#include "orm/ormtypes.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny
{
    template<typename Model>
    class Builder;
    template<typename Model>
    using TinyBuilder = Builder<Model>;

namespace Concerns
{
    template<typename Model, typename ...AllRelations>
    class HasRelationStore
    {
    protected:
        /*! Type of data saved in the relation store. */
        enum struct RelationStoreType
        {
            EAGER,
            PUSH,
        };

        // TODO future try to rewrite this by templated class to avoid polymorfic class, like described here http://groups.di.unipi.it/~nids/docs/templates_vs_inheritance.html silverqx
        class BaseRelationStore
        {
        protected:
            BaseRelationStore(const RelationStoreType storeType);

        public:
            virtual ~BaseRelationStore() = default;

            inline RelationStoreType getStoreType() const
            { return m_storeType; }

        private:
            /*! Store type held by relation store. */
            RelationStoreType m_storeType;
        };

        /*! Helps to avoid passing variables to the Model::relationVisitor(). */
        class EagerRelationStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(EagerRelationStore)

        public:
            EagerRelationStore(
                    const WithItem &relation, Tiny::TinyBuilder<Model> &builder,
                    QVector<Model> &models);

            const WithItem           &relation;
            Tiny::TinyBuilder<Model> &builder;
            /*! Models to eager load, hydrated models obtained from database. */
            QVector<Model>           &models;
        };

        /*! Helps to avoid passing variables to the Model::relationVisitor(). */
        class PushRelationStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(PushRelationStore)

        public:
            explicit PushRelationStore(RelationsType<AllRelations...> &models);

            /*! Models to push, the reference to the relation in m_relations hash. */
            RelationsType<AllRelations...> &models;
            /*! The result of a push. */
            bool result = false;
        };

        /*! Factory method to create eager store. */
        void createEagerStore(const WithItem &relation, Tiny::TinyBuilder<Model> &builder,
                              QVector<Model> &models);
        /*! Factory method to create push store. */
        void createPushStore(RelationsType<AllRelations...> &models);

        /*! Releases the ownership and destroy all relation stores. */
        void resetRelationStore();

        /*! Store to save values to, before Model::relationVisitor() is called. */
        std::shared_ptr<BaseRelationStore> m_relationStore;
        /*! Store to save values to, before Model::relationVisitor() is called. */
        std::shared_ptr<EagerRelationStore> m_eagerStore;
        /*! Store to save values to, before Model::relationVisitor() is called. */
        std::shared_ptr<PushRelationStore> m_pushStore;
    };

    template<typename Model, typename ...AllRelations>
    void HasRelationStore<Model, AllRelations...>::createEagerStore(
            const WithItem &relation, Tiny::TinyBuilder<Model> &builder,
            QVector<Model> &models)
    {
        Q_ASSERT(!m_relationStore && !m_eagerStore);

        m_relationStore = m_eagerStore =
                std::make_shared<EagerRelationStore>(relation, builder, models);
    }

    template<typename Model, typename ...AllRelations>
    void HasRelationStore<Model, AllRelations...>::createPushStore(
            RelationsType<AllRelations...> &models)
    {
        Q_ASSERT(!m_relationStore && !m_pushStore);

        m_relationStore = m_pushStore =
                std::make_shared<PushRelationStore>(models);
    }

    template<typename Model, typename ...AllRelations>
    void HasRelationStore<Model, AllRelations...>::resetRelationStore()
    {
        /* Releases the ownership and destroy all relation stores. */
        m_relationStore.reset();
        m_eagerStore.reset();
        m_pushStore.reset();
    }

    template<typename Model, typename ...AllRelations>
    HasRelationStore<Model, AllRelations...>::BaseRelationStore::BaseRelationStore(
            const RelationStoreType storeType)
        : m_storeType(storeType)
    {}

    template<typename Model, typename ...AllRelations>
    HasRelationStore<Model, AllRelations...>::EagerRelationStore::EagerRelationStore(
            const WithItem &relation, Tiny::TinyBuilder<Model> &builder,
            QVector<Model> &models)
        : BaseRelationStore(RelationStoreType::EAGER)
        , relation(relation)
        , builder(builder)
        , models(models)
    {}

    template<typename Model, typename ...AllRelations>
    HasRelationStore<Model, AllRelations...>::PushRelationStore::PushRelationStore(
            RelationsType<AllRelations...> &models)
        : BaseRelationStore(RelationStoreType::PUSH)
        , models(models)
    {}

} // namespace Orm::Tiny::Concerns
} // namespace Orm::Tiny
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // HASRELATIONSTORE_H
