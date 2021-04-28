#ifndef HASRELATIONSTORE_H
#define HASRELATIONSTORE_H

#include <optional>
#include <stack>

#include "orm/ormtypes.hpp"
#include "orm/runtimeerror.hpp"
#include "orm/tiny/relations/relation.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny
{
    template<typename Model>
    class Builder;

    template<typename Derived, typename ...AllRelations>
    class Model;

    template<typename Model>
    using TinyBuilder = Builder<Model>;

    /*! The type in which are saved relationships. */
    template<typename ...AllRelations>
    using RelationsType = std::variant<std::monostate,
                                       QVector<AllRelations>...,
                                       std::optional<AllRelations>...>;

namespace Concerns
{
    template<typename Derived, typename ...AllRelations>
    class HasRelationStore
    {
        /*! Type of data saved in the relation store. */
        enum struct RelationStoreType
        {
            EAGER,
            PUSH,
            TOUCH_OWNERS,
            LAZY,
        };

        /* Forward declarations */
        class EagerRelationStore;
        class PushRelationStore;
        class TouchOwnersRelationStore;
        template<typename Related>
        class LazyRelationStore;

    protected:
        // FUTURE try to rewrite this by templated class to avoid polymorfic class, like described here http://groups.di.unipi.it/~nids/docs/templates_vs_inheritance.html silverqx
        class BaseRelationStore
        {
        protected:
            BaseRelationStore(HasRelationStore &hasRelationStore,
                              const RelationStoreType storeType);

        public:
            // CUR make all default dtors inline silverqx
            inline virtual ~BaseRelationStore() = default;

            /*! Currently held store type. */
            inline RelationStoreType getStoreType() const
            { return m_storeType; }

            /*! Visit the given relation. */
            void visit(const QString &relation);

            /*! Called from Model::u_relations to pass reference to the relation
                method, an enter point of the visitation. */
            template<typename Method>
            requires std::is_member_function_pointer_v<Method>
            void operator()(const Method method);

        protected:
            /*! Reference to the parent HasRelationStore instance. */
            HasRelationStore &m_hasRelationStore;

        private:
            /*! Store type held by relation store. */
            const RelationStoreType m_storeType;
        };

        /* Factory methods for Relation stores */
        /*! Factory method to create an eager store. */
        BaseRelationStore &
        createEagerStore(const Tiny::TinyBuilder<Derived> &builder,
                         QVector<Derived> &models, const WithItem &relation);
        /*! Factory method to create the push store. */
        BaseRelationStore &createPushStore(RelationsType<AllRelations...> &models);
        /*! Factory method to create the touch owners store. */
        BaseRelationStore &createTouchOwnersStore(const QString &relation);
        /*! Factory method to create the lazy store. */
        template<typename Related>
        BaseRelationStore &createLazyStore();

        /*! Releases the ownership and destroy currently held relation store. */
        void resetRelationStore();

        /* Getters for Relation stores */
        /*! Reference to the push store. */
        inline PushRelationStore &pushStore();
        /*! Cont reference to the touch owners relation store. */
        inline const TouchOwnersRelationStore &touchOwnersStore() const;
        /*! Const reference to the lazy store. */
        template<typename Related>
        inline const LazyRelationStore<Related> &lazyStore() const;

    private:
        /*! The store for loading eager relations. */
        class EagerRelationStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(EagerRelationStore)

        public:
            EagerRelationStore(
                    HasRelationStore &hasRelationStore,
                    const Tiny::TinyBuilder<Derived> &builder,
                    QVector<Derived> &models, const WithItem &relation);

            /*! The Tiny builder instance to which the visited relation will be
                dispatched. */
            const Tiny::TinyBuilder<Derived> &builder;
            /*! Models on which to do an eager load, hydrated models that were obtained
                from the database and these models will be passed as parameter
                to the TinyBuilder. */
            QVector<Derived> &models;
            /*! The WithItem that will be passed as parameter to the TinyBuilder. */
            const WithItem &relation;

            /*! Method called after visitation. */
            template<typename Method>
            void visited(const Method method) const;
        };

        /*! The store for the Model push() method. */
        class PushRelationStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(PushRelationStore)

        public:
            explicit PushRelationStore(HasRelationStore &hasRelationStore,
                                       RelationsType<AllRelations...> &models);

            /*! Models to push, the reference to the relation in m_relations hash. */
            RelationsType<AllRelations...> &models;
            /*! The result of a push. */
            bool result = false;

            /*! Method called after visitation. */
            template<typename Method>
            void visited(const Method) const;
        };

        /*! The store for touching owner's timestamps. */
        class TouchOwnersRelationStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(TouchOwnersRelationStore)

        public:
            explicit TouchOwnersRelationStore(HasRelationStore &hasRelationStore,
                                              const QString &relation);

            /*! Models to touch timestamps for, the reference to the relation name/key
                in the m_relations hash. */
            const QString &relation;

            /*! Method called after visitation. */
            template<typename Method>
            void visited(const Method method) const;
        };

        /*! The store for the lazy loading. */
        template<typename Related>
        class LazyRelationStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(LazyRelationStore)

        public:
            LazyRelationStore(HasRelationStore &hasRelationStore);

            /*! The result of lazy load. */
            std::variant<QVector<Related>, std::optional<Related>> result;

            /*! Method called after visitation. */
            template<typename Method>
            void visited(const Method method);
        };

        // BUG this is bad, disable Model's copy/assignment ctors if m_relationStore is not empty, or empty the m_relationStore on copy?, have to think about this 🤔 silverqx
        /*! The store where the values will be saved, before BaseRelationStore::visit()
            is called. */
        std::stack<std::shared_ptr<BaseRelationStore>> m_relationStore;

        /*! Static cast this to a child's instance type (CRTP). */
        inline Model<Derived, AllRelations...> &basemodel();
        /*! Static cast this to a child's instance type (CRTP), const version. */
        inline const Model<Derived, AllRelations...> &basemodel() const;

        /*! Static cast this to a child's instance type (CRTP). */
        inline Derived &model();
        /*! Static cast this to a child's instance type (CRTP), const version. */
        inline const Derived &model() const;
    };

    template<typename Derived, typename ...AllRelations>
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::createEagerStore(
            const Tiny::TinyBuilder<Derived> &builder, QVector<Derived> &models,
            const WithItem &relation)
    {
        m_relationStore.push(std::make_shared<EagerRelationStore>(
                                 *this, builder, models, relation));

        return *m_relationStore.top();
    }

    template<typename Derived, typename ...AllRelations>
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::createPushStore(
            RelationsType<AllRelations...> &models)
    {
        m_relationStore.push(std::make_shared<PushRelationStore>(*this, models));

        return *m_relationStore.top();
    }

    template<typename Derived, typename ...AllRelations>
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::createTouchOwnersStore(
            const QString &relation)
    {
        m_relationStore.push(std::make_shared<TouchOwnersRelationStore>(*this, relation));

        return *m_relationStore.top();
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::createLazyStore()
    {
        m_relationStore.push(std::make_shared<LazyRelationStore<Related>>(*this));

        return *m_relationStore.top();
    }

    template<typename Derived, typename ...AllRelations>
    void HasRelationStore<Derived, AllRelations...>::resetRelationStore()
    {
        // Releases the ownership and destroy the top relation store on the stack
        m_relationStore.pop();
    }

    template<typename Derived, typename ...AllRelations>
    typename HasRelationStore<Derived, AllRelations...>::PushRelationStore &
    HasRelationStore<Derived, AllRelations...>::pushStore()
    {
        return *std::static_pointer_cast<PushRelationStore>(m_relationStore.top());
    }

    template<typename Derived, typename ...AllRelations>
    const typename HasRelationStore<Derived, AllRelations...>::TouchOwnersRelationStore &
    HasRelationStore<Derived, AllRelations...>::touchOwnersStore() const
    {
        return *std::static_pointer_cast<
                const TouchOwnersRelationStore>(m_relationStore.top());
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    const typename HasRelationStore<Derived, AllRelations...>
                   ::template LazyRelationStore<Related> &
    HasRelationStore<Derived, AllRelations...>::lazyStore() const
    {
        return *std::static_pointer_cast<
                const LazyRelationStore<Related>>(m_relationStore.top());
    }

    template<typename Derived, typename ...AllRelations>
    HasRelationStore<Derived, AllRelations...>::BaseRelationStore::BaseRelationStore(
            HasRelationStore &hasRelationStore, const RelationStoreType storeType
    )
        : m_hasRelationStore(hasRelationStore)
        , m_storeType(storeType)
    {}

    template<typename Derived, typename ...AllRelations>
    void HasRelationStore<Derived, AllRelations...>::BaseRelationStore
                                                   ::visit(const QString &relation)
    {
        std::invoke(m_hasRelationStore.basemodel().getRelationsRawMapInternal()
                    .find(relation).value(),
                    *this);
    }


    template<typename Derived, typename ...AllRelations>
    template<typename Method>
    requires std::is_member_function_pointer_v<Method>
    void HasRelationStore<Derived, AllRelations...>::BaseRelationStore
                                                   ::operator()(const Method method)
    {
        switch (getStoreType()) {
        case RelationStoreType::EAGER:
            static_cast<EagerRelationStore *>(this)->visited(method);
            break;

        case RelationStoreType::TOUCH_OWNERS:
            static_cast<TouchOwnersRelationStore *>(this)->visited(method);
            break;

        case RelationStoreType::PUSH:
            static_cast<PushRelationStore *>(this)->visited(method);
            break;

        case RelationStoreType::LAZY:
        {
            using Related = typename std::invoke_result_t<Method, Derived>
                                        ::element_type::RelatedType;

            static_cast<LazyRelationStore<Related> *>(this)->visited(method);
        }
            break;

        default:
            throw RuntimeError("Unknown store type.");
        }
    }

    template<typename Derived, typename ...AllRelations>
    HasRelationStore<Derived, AllRelations...>::EagerRelationStore::EagerRelationStore(
            HasRelationStore &hasRelationStore,
            const Tiny::TinyBuilder<Derived> &builder,
            QVector<Derived> &models, const WithItem &relation
    )
        : BaseRelationStore(hasRelationStore, RelationStoreType::EAGER)
        , relation(relation)
        , builder(builder)
        , models(models)
    {}

    template<typename Derived, typename ...AllRelations>
    template<typename Method>
    void HasRelationStore<Derived, AllRelations...>::EagerRelationStore::visited(
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
        auto dummyModel = this->m_hasRelationStore.model().newInstance();

        /* We want to run a relationship query without any constrains so that we will
           not have to remove these where clauses manually which gets really hacky
           and error prone. We don't want constraints because we add eager ones. */
        auto relationInstance =
                Relations::Relation<Derived, Related>::noConstraints(
                    [this, &method, &dummyModel]
        {
            return std::invoke(method, dummyModel);
        });

        builder.eagerLoadRelationVisited(std::move(relationInstance), models, relation);
    }

    template<typename Derived, typename ...AllRelations>
    HasRelationStore<Derived, AllRelations...>::PushRelationStore::PushRelationStore(
            HasRelationStore &hasRelationStore,
            RelationsType<AllRelations...> &models
    )
        : BaseRelationStore(hasRelationStore, RelationStoreType::PUSH)
        , models(models)
    {}

    template<typename Derived, typename ...AllRelations>
    template<typename Method>
    void HasRelationStore<Derived, AllRelations...>::PushRelationStore::visited(
            const Method) const
    {
        using Related = typename std::invoke_result_t<Method, Derived>
                                    ::element_type::RelatedType;

        this->m_hasRelationStore.basemodel().template pushVisited<Related>();
    }

    template<typename Derived, typename ...AllRelations>
    HasRelationStore<Derived, AllRelations...>::TouchOwnersRelationStore
                                              ::TouchOwnersRelationStore(
            HasRelationStore &hasRelationStore, const QString &relation
    )
        : BaseRelationStore(hasRelationStore, RelationStoreType::TOUCH_OWNERS)
        , relation(relation)
    {}

    template<typename Derived, typename ...AllRelations>
    template<typename Method>
    void HasRelationStore<Derived, AllRelations...>::TouchOwnersRelationStore::visited(
            const Method method) const
    {
        auto relationInstance = std::invoke(method, this->m_hasRelationStore.model());

        using Related = typename std::invoke_result_t<Method, Derived>
                                    ::element_type::RelatedType;

        this->m_hasRelationStore.basemodel()
                .template touchOwnersVisited<Related>(std::move(relationInstance));
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    HasRelationStore<Derived, AllRelations...>::LazyRelationStore<Related>
                                              ::LazyRelationStore(
            HasRelationStore &hasRelationStore
    )
        : BaseRelationStore(hasRelationStore, RelationStoreType::LAZY)
    {}


    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    template<typename Method>
    void
    HasRelationStore<Derived, AllRelations...>::LazyRelationStore<Related>::visited(
            const Method method)
    {
        result = std::invoke(method, this->m_hasRelationStore.model())
                 ->getResults();
    }

    template<typename Derived, typename ...AllRelations>
    Model<Derived, AllRelations...> &
    HasRelationStore<Derived, AllRelations...>::basemodel()
    {
        return static_cast<Model<Derived, AllRelations...> &>(*this);
    }

    template<typename Derived, typename ...AllRelations>
    const Model<Derived, AllRelations...> &
    HasRelationStore<Derived, AllRelations...>::basemodel() const
    {
        return static_cast<const Model<Derived, AllRelations...> &>(*this);
    }

    template<typename Derived, typename ...AllRelations>
    Derived &
    HasRelationStore<Derived, AllRelations...>::model()
    {
        return static_cast<Derived &>(*this);
    }

    template<typename Derived, typename ...AllRelations>
    const Derived &
    HasRelationStore<Derived, AllRelations...>::model() const
    {
        return static_cast<const Derived &>(*this);
    }

} // namespace Orm::Tiny::Concerns
} // namespace Orm::Tiny
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // HASRELATIONSTORE_H
