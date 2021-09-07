#pragma once
#ifndef HASRELATIONSTORE_H
#define HASRELATIONSTORE_H

#include <optional>
#include <stack>

#include "orm/exceptions/runtimeerror.hpp"
#include "orm/tiny/relations/relation.hpp"
#include "orm/tiny/tinytypes.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny
{

    /*! The type in which are saved relationships. */
    template<typename ...AllRelations>
    using RelationsType = std::variant<std::monostate,
                                       QVector<AllRelations>...,
                                       std::optional<AllRelations>...>;

namespace Concerns
{

    template<typename Model>
    class QueriesRelationships;

    // FEATURE concepts, stackoverflow how the hell should I check this silverqx
//    template<typename Model, typename Method>
//    concept RelationMethod = std::is_member_function_pointer_v<Method> &&
//        (requires(Method m) {
//            {std::invoke(m, Model())} -> std::convertible_to<std::unique_ptr<Relations::OneRelation>>;
//        } ||
//        requires(Method m) {
//            {std::invoke(m, Model())} -> std::convertible_to<std::unique_ptr<Relations::ManyRelation>>;
//        });

    // FUTURE relationstore, cache results, eg. cache Relation instance and return copy of this cached Relation instance, Related parameter can be obtained from cached Relation instance silverqx
    /*! Relation store, handles mapping from a relation name to the Model's relation
        method, also calls visited method with Related parameter when needed. */
    template<typename Derived, typename ...AllRelations>
    class HasRelationStore
    {
        // Used by Model, all is private to not pollute user's CA in model classes
        friend Model<Derived, AllRelations...>;

        /*! Type of data saved in the relation store. */
        enum struct RelationStoreType
        {
            EAGER,
            PUSH,
            TOUCH_OWNERS,
            LAZY_RESULTS,
            BELONGSTOMANY_RELATED_TABLE,
            QUERIES_RELATIONSHIPS_QUERY,
            QUERIES_RELATIONSHIPS_TINY,
            QUERIES_RELATIONSHIPS_TINY_NESTED,
        };

        /* Forward declarations */
        class EagerRelationStore;
        class PushRelationStore;
        class TouchOwnersRelationStore;
        template<typename Related>
        class LazyRelationStore;
        class BelongsToManyRelatedTableStore;
        template<typename Related>
        class QueriesRelationshipsStore;

        // FUTURE try to rewrite this by templated class to avoid polymorfic class, like described here http://groups.di.unipi.it/~nids/docs/templates_vs_inheritance.html silverqx
        /*! Base class for relation stores. */
        class BaseRelationStore
        {
        protected:
            /*! Constructor. */
            BaseRelationStore(HasRelationStore &hasRelationStore,
                              const RelationStoreType storeType);

        public:
            /*! Virtual destructor. */
            inline virtual ~BaseRelationStore() = default;

            /*! Visit the given relation. */
            void visit(const QString &relation);

            /*! Called from Model::u_relations to pass reference to the relation
                method, an enter point of the visitation. */
            template<typename Method>
            requires std::is_member_function_pointer_v<Method>
            void operator()(const Method method);

        protected:
            /*! Currently held store type. */
            RelationStoreType getStoreType() const;

            /*! Reference to the parent HasRelationStore instance. */
            HasRelationStore &m_hasRelationStore;

        private:
            /*! Store type held by relation store. */
            const RelationStoreType m_storeType;
        };

        /*! The store for loading eager relations. */
        class EagerRelationStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(EagerRelationStore)

        public:
            /*! Constructor. */
            EagerRelationStore(
                    HasRelationStore &hasRelationStore,
                    const Tiny::TinyBuilder<Derived> &builder,
                    QVector<Derived> &models, const WithItem &relation);

            /*! Method called after visitation. */
            template<typename Method>
            void visited(const Method method) const;

        private:
            /*! The Tiny builder instance to which the visited relation will be
                dispatched. */
            const Tiny::TinyBuilder<Derived> &m_builder;
            /*! Models on which to do an eager load, hydrated models that were obtained
                from the database and these models will be passed as parameter
                to the TinyBuilder. */
            QVector<Derived> &m_models;
            /*! The WithItem that will be passed as parameter to the TinyBuilder. */
            const WithItem &m_relation;
        };

        /*! The store for the Model push() method. */
        class PushRelationStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(PushRelationStore)

        public:
            /*! Constructor. */
            explicit PushRelationStore(HasRelationStore &hasRelationStore,
                                       RelationsType<AllRelations...> &models);

            /*! Method called after visitation. */
            template<typename Method>
            void visited(const Method) const;

            /*! Models to push, the reference to the relation in m_relations hash. */
            RelationsType<AllRelations...> &m_models;
            /*! The result of a push. */
            bool m_result = false;
        };

        /*! The store for touching owner's timestamps. */
        class TouchOwnersRelationStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(TouchOwnersRelationStore)

        public:
            /*! Constructor. */
            explicit TouchOwnersRelationStore(HasRelationStore &hasRelationStore,
                                              const QString &relation);

            /*! Method called after visitation. */
            template<typename Method>
            void visited(const Method method) const;

            /*! Models to touch timestamps for, the reference to the relation name/key
                in the m_relations hash. */
            const QString &m_relation;
        };

        /*! The store for the lazy loading. */
        template<typename Related>
        class LazyRelationStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(LazyRelationStore)

        public:
            /*! Constructor. */
            LazyRelationStore(HasRelationStore &hasRelationStore);

            /*! Method called after visitation. */
            template<typename Method>
            void visited(const Method method);

            // TODO templated LazyRelationStore by Container too, QVector to Container silverqx
            /*! The result of lazy load. */
            std::variant<QVector<Related>, std::optional<Related>> m_result;
        };

        /*! The store to obtain the related table name for BelongsToMany relation. */
        class BelongsToManyRelatedTableStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(BelongsToManyRelatedTableStore)

        public:
            /*! Constructor. */
            explicit BelongsToManyRelatedTableStore(HasRelationStore &hasRelationStore);

            /*! Method called after visitation. */
            template<typename Method>
            void visited(const Method);

            /*! The related table name result. */
            std::optional<QString> m_result;
        };

        /*! The store for obtaining a Relation instance for QueriesRelationships. */
        template<typename Related>
        class QueriesRelationshipsStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(QueriesRelationshipsStore)

        public:
            /*! QueriesRelationshipsStore constructor. */
            QueriesRelationshipsStore(
                    HasRelationStore &hasRelationStore,
                    QueriesRelationships<Derived> &origin, const QString &comparison,
                    qint64 count, const QString &condition,
                    const std::function<
                            void(QueriesRelationshipsCallback<Related> &)> &callback,
                    const std::optional<std::reference_wrapper<
                                        QStringList>> relations = std::nullopt);

            /*! Method called after visitation. */
            template<typename RelatedFromMethod, typename Method>
            void visited(const Method method);

        protected:
            /*! Store type initializer. */
            constexpr static RelationStoreType initStoreType();

            /*! Served store type, this class can handle two store types. */
            thread_local constexpr static RelationStoreType STORE_TYPE = initStoreType();

        private:
            /*! The QueriesRelationships instance to which the visited relation will be
                dispatched. */
            QueriesRelationships<Derived> &m_origin;
            /*! Comparison operator, used during querying relationship exitence. */
            const QString &m_comparison;
            /*! Required number of records, used during querying relationship
                exitence. */
            const qint64 m_count;
            /*! Condition operator, used during querying relationship exitence. */
            const QString &m_condition;
            /*! Builder callback, used during querying relationship exitence. */
            const std::function<void(
                    QueriesRelationshipsCallback<Related> &)> &m_callback;
            /*! Nested relations for hasNested() method. */
            std::optional<std::reference_wrapper<QStringList>> m_relations;
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
        /*! Factory method to create the store to obtain BelongsToMany related model
            table name. */
        BaseRelationStore &createBelongsToManyRelatedTableStore();
        /*! Factory method to create the QueriesRelationships store with a Tiny
            callback. */
        template<typename Related = void>
        BaseRelationStore &
        createQueriesRelationshipsStore(
                QueriesRelationships<Derived> &origin, const QString &comparison,
                qint64 count, const QString &condition,
                const std::function<
                        void(QueriesRelationshipsCallback<Related> &)> &callback,
                const std::optional<std::reference_wrapper<
                        QStringList>> relations = std::nullopt);

        /*! Release the ownership and destroy the top relation store on the stack. */
        void resetRelationStore();

        /* Getters for Relation stores */
        /*! Reference to the push store. */
        PushRelationStore &pushStore();
        /*! Cont reference to the touch owners relation store. */
        const TouchOwnersRelationStore &touchOwnersStore() const;
        /*! Const reference to the lazy store. */
        template<typename Related>
        const LazyRelationStore<Related> &lazyStore() const;
        /*! Reference to the BelongsToMany related table name store. */
        const BelongsToManyRelatedTableStore &belongsToManyRelatedTableStore() const;
        /*! Const reference to the QueriesRelationships store. */
        template<typename Related = void>
        const QueriesRelationshipsStore<Related> &queriesRelationshipsStore() const;

        /*! Static cast this to a child's instance Model type. */
        Model<Derived, AllRelations...> &basemodel();
        /*! Static cast this to a child's instance Model type, const version. */
        const Model<Derived, AllRelations...> &basemodel() const;

        /*! Static cast this to a child's instance type (CRTP). */
        Derived &model();
        /*! Static cast this to a child's instance type (CRTP), const version. */
        const Derived &model() const;

        // BUG this is bad, disable Model's copy/assignment ctors if m_relationStore is not empty, or empty the m_relationStore on copy?, have to think about this 🤔 silverqx
        /*! The store where the values will be saved, before BaseRelationStore::visit()
            is called. */
        std::stack<std::shared_ptr<BaseRelationStore>> m_relationStore;
    };

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

    // CUR1 add constraint to also check Method return value, should be std::unique_ptr<Relation> silverqx
    template<typename Derived, typename ...AllRelations>
    template<typename Method>
    requires std::is_member_function_pointer_v<Method>
    void HasRelationStore<Derived, AllRelations...>::BaseRelationStore
                                                   ::operator()(const Method method)
    {
        const auto storeType = getStoreType();

        switch (storeType) {
        case RelationStoreType::EAGER:
            static_cast<EagerRelationStore *>(this)->visited(method);
            break;

        case RelationStoreType::TOUCH_OWNERS:
            static_cast<TouchOwnersRelationStore *>(this)->visited(method);
            break;

        case RelationStoreType::PUSH:
            static_cast<PushRelationStore *>(this)->visited(method);
            break;

        case RelationStoreType::BELONGSTOMANY_RELATED_TABLE:
            static_cast<BelongsToManyRelatedTableStore *>(this)->visited(method);
            break;

        case RelationStoreType::LAZY_RESULTS:
        case RelationStoreType::QUERIES_RELATIONSHIPS_QUERY:
        case RelationStoreType::QUERIES_RELATIONSHIPS_TINY:
        case RelationStoreType::QUERIES_RELATIONSHIPS_TINY_NESTED:
        {
            using Related = typename std::invoke_result_t<Method, Derived>
                                        ::element_type::RelatedType;

            switch (storeType) {
            case RelationStoreType::LAZY_RESULTS:
                static_cast<LazyRelationStore<Related> *>(this)->visited(method);
                break;

            case RelationStoreType::QUERIES_RELATIONSHIPS_QUERY:
                static_cast<QueriesRelationshipsStore<void> *>(this)
                        ->template visited<Related>(method);
                break;

            case RelationStoreType::QUERIES_RELATIONSHIPS_TINY:
            case RelationStoreType::QUERIES_RELATIONSHIPS_TINY_NESTED:
                static_cast<QueriesRelationshipsStore<Related> *>(this)
                        ->template visited<Related>(method);
                break;

            default:
                break;
            }
        }
            break;

        default:
            throw Orm::Exceptions::RuntimeError("Unknown store type.");
        }
    }

    template<typename Derived, typename ...AllRelations>
    inline typename HasRelationStore<Derived, AllRelations...>::RelationStoreType
    HasRelationStore<Derived, AllRelations...>::BaseRelationStore::getStoreType() const
    {
        return m_storeType;
    }

    template<typename Derived, typename ...AllRelations>
    HasRelationStore<Derived, AllRelations...>::EagerRelationStore::EagerRelationStore(
            HasRelationStore &hasRelationStore,
            const Tiny::TinyBuilder<Derived> &builder,
            QVector<Derived> &models, const WithItem &relation
    )
        : BaseRelationStore(hasRelationStore, RelationStoreType::EAGER)
        , m_builder(builder)
        , m_models(models)
        , m_relation(relation)
    {}

    template<typename Derived, typename ...AllRelations>
    template<typename Method>
    void HasRelationStore<Derived, AllRelations...>::EagerRelationStore::visited(
            const Method method) const
    {
        using Related = typename std::invoke_result_t<Method, Derived>
                                    ::element_type::RelatedType;

        // TODO static_assert() check silverqx
//        static_assert (std::is_convertible_v<std::invoke_result_t<Method, Derived>,
//                std::unique_ptr<Relations::OneRelation>> ||
//                std::is_convertible_v<std::invoke_result_t<Method, Derived>,
//                                std::unique_ptr<Relations::ManyRelation>>, "xyz");

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
                    [&method, &dummyModel]
        {
            return std::invoke(method, dummyModel);
        });

        m_builder.eagerLoadRelationVisited(std::move(relationInstance),
                                           m_models, m_relation);
    }

    template<typename Derived, typename ...AllRelations>
    HasRelationStore<Derived, AllRelations...>::PushRelationStore::PushRelationStore(
            HasRelationStore &hasRelationStore,
            RelationsType<AllRelations...> &models
    )
        : BaseRelationStore(hasRelationStore, RelationStoreType::PUSH)
        , m_models(models)
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
        , m_relation(relation)
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
        : BaseRelationStore(hasRelationStore, RelationStoreType::LAZY_RESULTS)
    {}

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    template<typename Method>
    void
    HasRelationStore<Derived, AllRelations...>::LazyRelationStore<Related>::visited(
            const Method method)
    {
        m_result = std::invoke(method, this->m_hasRelationStore.model())
                ->getResults();
    }

    template<typename Derived, typename ...AllRelations>
    HasRelationStore<Derived, AllRelations...>::BelongsToManyRelatedTableStore
                                              ::BelongsToManyRelatedTableStore(
            HasRelationStore &hasRelationStore
    )
        : BaseRelationStore(hasRelationStore,
                            RelationStoreType::BELONGSTOMANY_RELATED_TABLE)
    {}

    template<typename Derived, typename ...AllRelations>
    template<typename Method>
    void HasRelationStore<Derived, AllRelations...>::BelongsToManyRelatedTableStore
                                                   ::visited(const Method)
    {
        using Relation = typename std::invoke_result_t<Method, Derived>::element_type;

        if constexpr (!std::is_base_of_v<Relations::PivotRelation, Relation>)
            return;

        // TODO mystery, warning: C4702: unreachable code, I don't know what cause it, I think I'm missing some forward declaration of model in some other model, but who knows silverqx
        m_result = typename Relation::RelatedType().getTable();
    }

    /*
       QueriesRelationshipsStore<Related> is templated by Related, because it needs to
       save a reference to std::function() which takes as an argument a QueryBuilder & or
       TinyBuilder<Related> &.
       If the Related parameter is the void type, then it is served as
       the QUERIES_RELATIONSHIPS_QUERY store type and will save a QueryBuilder &.
       If the Related parameter is not void, then it is served as
       the QUERIES_RELATIONSHIPS_TINY / QUERIES_RELATIONSHIPS_TINY_NESTED and will save
       a TinyBuilder<Related> &.
       The same is true for the factory method and store's getter method, if the Related
       parameter is the void type, then the QUERIES_RELATIONSHIPS_QUERY store type
       will be created / returned, ... 🙏🙌😎
       The true is that design this was super challenge 💪🚀🔥.
       Added a new store type QUERIES_RELATIONSHIPS_TINY_NESTED for nested relations for
       hasNested() method, so don't be confused by QueriesRelationshipsStore::STORE_TYPE,
       it only holds store type which can be handled by currently instantiated
       QueriesRelationshipsStore (instantiated means template instance generated by
       the compiler, not a class instance), because the QueriesRelationshipsStore class
       can handle 3 store types, the nested store type is used on the base of ctor's
       'relations' argument.
    */

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    HasRelationStore<Derived, AllRelations...>::QueriesRelationshipsStore<Related>
                                              ::QueriesRelationshipsStore(
            HasRelationStore &hasRelationStore,  QueriesRelationships<Derived> &origin,
            const QString &comparison, const qint64 count, const QString &condition,
            const std::function<void(QueriesRelationshipsCallback<Related> &)> &callback,
            const std::optional<std::reference_wrapper<QStringList>> relations
    )
        : BaseRelationStore(hasRelationStore,
                            relations
                            ? RelationStoreType::QUERIES_RELATIONSHIPS_TINY_NESTED
                            : STORE_TYPE)
        , m_origin(origin)
        , m_comparison(comparison)
        , m_count(count)
        , m_condition(condition)
        , m_callback(callback)
        , m_relations(relations)
    {}

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    template<typename RelatedFromMethod, typename Method>
    void
    HasRelationStore<Derived, AllRelations...>::QueriesRelationshipsStore<Related>
                                              ::visited(const Method method)
    {
        // This check can be done for the QUERIES_RELATIONSHIPS_TINY store type only
        if constexpr (!std::is_void_v<Related>)
            static_assert (std::is_same_v<Related, RelatedFromMethod>,
                    "How did this happen? Is a mystery 😎, a Related type has to be "
                    "the same as a RelatedFromMethod.");

        // We want to run a relationship query without any constrains
        auto relationInstance =
                Relations::Relation<Derived, RelatedFromMethod>::noConstraints(
                    [this, &method]
        {
            return std::invoke(method, this->m_hasRelationStore.model());
        });

        // Nested store type, used by hasNested()
        if (this->getStoreType() == RelationStoreType::QUERIES_RELATIONSHIPS_TINY_NESTED)
            m_origin.template hasInternalVisited<RelatedFromMethod>(
                        std::move(relationInstance), m_comparison, m_count, m_condition,
                        *m_relations);
        else
            m_origin.template has<RelatedFromMethod>(
                        std::move(relationInstance), m_comparison, m_count, m_condition,
                        m_callback);
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    inline constexpr typename
    HasRelationStore<Derived, AllRelations...>::RelationStoreType
    HasRelationStore<Derived, AllRelations...>::QueriesRelationshipsStore<Related>
                                              ::initStoreType()
    {
        if constexpr (std::is_void_v<Related>)
            return RelationStoreType::QUERIES_RELATIONSHIPS_QUERY;
        else
            return RelationStoreType::QUERIES_RELATIONSHIPS_TINY;
    }

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
        m_relationStore.push(
                    std::make_shared<TouchOwnersRelationStore>(*this, relation));

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
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::createBelongsToManyRelatedTableStore()
    {
        m_relationStore.push(std::make_shared<BelongsToManyRelatedTableStore>(*this));

        return *m_relationStore.top();
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::createQueriesRelationshipsStore(
            QueriesRelationships<Derived> &origin, const QString &comparison,
            const qint64 count, const QString &condition,
            const std::function<void(QueriesRelationshipsCallback<Related> &)> &callback,
            const std::optional<std::reference_wrapper<QStringList>> relations)
    {
        m_relationStore.push(std::make_shared<QueriesRelationshipsStore<Related>>(
                                 *this, origin, comparison, count, condition, callback,
                                 relations));

        return *m_relationStore.top();
    }

    template<typename Derived, typename ...AllRelations>
    void HasRelationStore<Derived, AllRelations...>::resetRelationStore()
    {
        m_relationStore.pop();
    }

    template<typename Derived, typename ...AllRelations>
    inline typename HasRelationStore<Derived, AllRelations...>::PushRelationStore &
    HasRelationStore<Derived, AllRelations...>::pushStore()
    {
        return *std::static_pointer_cast<PushRelationStore>(m_relationStore.top());
    }

    template<typename Derived, typename ...AllRelations>
    inline const typename HasRelationStore<Derived, AllRelations...>
                          ::TouchOwnersRelationStore &
    HasRelationStore<Derived, AllRelations...>::touchOwnersStore() const
    {
        return *std::static_pointer_cast<
                const TouchOwnersRelationStore>(m_relationStore.top());
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    inline const typename HasRelationStore<Derived, AllRelations...>
                          ::template LazyRelationStore<Related> &
    HasRelationStore<Derived, AllRelations...>::lazyStore() const
    {
        return *std::static_pointer_cast<
                const LazyRelationStore<Related>>(m_relationStore.top());
    }

    template<typename Derived, typename ...AllRelations>
    inline const typename HasRelationStore<Derived, AllRelations...>
                          ::BelongsToManyRelatedTableStore &
    HasRelationStore<Derived, AllRelations...>::belongsToManyRelatedTableStore() const
    {
        return *std::static_pointer_cast<
                const BelongsToManyRelatedTableStore>(m_relationStore.top());
    }

    template<typename Derived, typename ...AllRelations>
    template<typename Related>
    inline const typename HasRelationStore<Derived, AllRelations...>
    ::template QueriesRelationshipsStore<Related> &
    HasRelationStore<Derived, AllRelations...>::queriesRelationshipsStore() const
    {
        return *std::static_pointer_cast<
                const QueriesRelationshipsStore<Related>>(m_relationStore.top());
    }

    template<typename Derived, typename ...AllRelations>
    inline Model<Derived, AllRelations...> &
    HasRelationStore<Derived, AllRelations...>::basemodel()
    {
        return static_cast<Model<Derived, AllRelations...> &>(*this);
    }

    template<typename Derived, typename ...AllRelations>
    inline const Model<Derived, AllRelations...> &
    HasRelationStore<Derived, AllRelations...>::basemodel() const
    {
        return static_cast<const Model<Derived, AllRelations...> &>(*this);
    }

    template<typename Derived, typename ...AllRelations>
    inline Derived &
    HasRelationStore<Derived, AllRelations...>::model()
    {
        return static_cast<Derived &>(*this);
    }

    template<typename Derived, typename ...AllRelations>
    inline const Derived &
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
