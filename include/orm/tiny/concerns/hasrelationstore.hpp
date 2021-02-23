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

    /*! The type in which are saved relationships. */
    template<typename ...AllRelations>
    using RelationsType = std::variant<std::monostate,
                                       QVector<AllRelations>...,
                                       std::optional<AllRelations>...>;

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
            TOUCH_OWNERS,
            IS_PIVOT_MODEL,
        };

        // TODO future try to rewrite this by templated class to avoid polymorfic class, like described here http://groups.di.unipi.it/~nids/docs/templates_vs_inheritance.html silverqx
        class BaseRelationStore
        {
        protected:
            BaseRelationStore(const RelationStoreType storeType);

        public:
            inline virtual ~BaseRelationStore() = default;

            inline RelationStoreType getStoreType() const
            { return m_storeType; }

        private:
            /*! Store type held by relation store. */
            RelationStoreType m_storeType;
        };

        /*! The store for loading eager relations, helps to avoid passing variables
            to the Model::relationVisitor(). */
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

        /*! The store for the Model push() method, helps to avoid passing variables
            to the Model::relationVisitor(). */
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

        /*! The store for touching owner's timestamps, helps to avoid passing variables
            to the Model::relationVisitor(). */
        class TouchOwnersRelationStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(TouchOwnersRelationStore)

        public:
            explicit TouchOwnersRelationStore(const QString &relation);

            /*! Models to touch timestamps for, the reference to the relation
                in the m_relations hash. */
            const QString &relation;
        };

        /*! The store to determine if the relation stores the pivot model, the result
            can be obtained by result data member. */
        class IsPivotModelRelationStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(IsPivotModelRelationStore)

        public:
            IsPivotModelRelationStore();

            /*! The result of a push. */
            bool result = false;
        };

        /*! Factory method to create eager store. */
        void createEagerStore(const WithItem &relation, Tiny::TinyBuilder<Model> &builder,
                              QVector<Model> &models);
        /*! Factory method to create push store. */
        void createPushStore(RelationsType<AllRelations...> &models);
        /*! Factory method to create touch owners store. */
        void createTouchOwnersStore(const QString &relation);
        /*! Factory method to create 'is pivot model' store. */
        void createIsPivotModelStore();

        /*! Releases the ownership and destroy all relation stores. */
        void resetRelationStore();

        /*! Store to save values to, before Model::relationVisitor() is called. */
        std::shared_ptr<BaseRelationStore> m_relationStore;
        /*! Store to save values to, before Model::relationVisitor() is called. */
        std::shared_ptr<EagerRelationStore> m_eagerStore;
        /*! Store to save values to, before Model::relationVisitor() is called. */
        std::shared_ptr<PushRelationStore> m_pushStore;
        /*! Store to save values to, before Model::relationVisitor() is called. */
        std::shared_ptr<TouchOwnersRelationStore> m_touchOwnersStore;
        /*! Store to obtain result, after the Model::relationVisitor() is called. */
        std::shared_ptr<IsPivotModelRelationStore> m_isPivotModelStore;
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
    void HasRelationStore<Model, AllRelations...>::createTouchOwnersStore(
            const QString &relation)
    {
        Q_ASSERT(!m_relationStore && !m_touchOwnersStore);

        m_relationStore = m_touchOwnersStore =
                std::make_shared<TouchOwnersRelationStore>(relation);
    }

    template<typename Model, typename ...AllRelations>
    void HasRelationStore<Model, AllRelations...>::createIsPivotModelStore()
    {
        Q_ASSERT(!m_relationStore && !m_isPivotModelStore);

        m_relationStore = m_isPivotModelStore =
                std::make_shared<IsPivotModelRelationStore>();
    }

    template<typename Model, typename ...AllRelations>
    void HasRelationStore<Model, AllRelations...>::resetRelationStore()
    {
        /* Releases the ownership and destroy all relation stores. */
        m_relationStore.reset();
        m_eagerStore.reset();
        m_pushStore.reset();
        m_touchOwnersStore.reset();
        m_isPivotModelStore.reset();
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

    template<typename Model, typename ...AllRelations>
    HasRelationStore<Model, AllRelations...>::TouchOwnersRelationStore
                                            ::TouchOwnersRelationStore(
            const QString &relation)
        : BaseRelationStore(RelationStoreType::TOUCH_OWNERS)
        , relation(relation)
    {}

    template<typename Model, typename ...AllRelations>
    HasRelationStore<Model, AllRelations...>::IsPivotModelRelationStore
                                            ::IsPivotModelRelationStore()
        : BaseRelationStore(RelationStoreType::IS_PIVOT_MODEL)
    {}

} // namespace Orm::Tiny::Concerns
} // namespace Orm::Tiny
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // HASRELATIONSTORE_H
