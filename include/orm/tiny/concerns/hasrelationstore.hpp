#pragma once
#ifndef ORM_TINY_CONCERNS_HASRELATIONSTORE_HPP
#define ORM_TINY_CONCERNS_HASRELATIONSTORE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <stack>

#include "orm/tiny/macros/crtpmodelwithbase.hpp"
#include "orm/tiny/relations/relation.hpp"
#include "orm/utils/notnull.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Concerns
{

    template<typename Derived, AllRelationsConcept ...AllRelations>
    class HasRelationships;

    template<typename Model>
    class QueriesRelationships;

    /*! Concept to check the relationship method. */
    template<typename Method, typename Derived>
    concept RelationshipMethod =
            ModelConcept<Derived> &&
            std::is_member_function_pointer_v<Method> &&
            std::is_convertible_v<std::invoke_result_t<Method, Derived>,
                                  std::unique_ptr<Relations::IsRelation>>;

    // FUTURE relationstore, cache results, eg. cache Relation instance and return copy of this cached Relation instance, Related parameter can be obtained from cached Relation instance silverqx
    /*! Relation store, handles mapping from a relation name to the Model's relation
        method, also calls visited method with Related parameter when needed. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class HasRelationStore // clazy:exclude=rule-of-three
    {
        // To access createXyzStore(), xyzStore() getters, resetRelationStore(), ...
        friend HasRelationships<Derived, AllRelations...>;

        /*! Alias for the NotNull. */
        template<typename T>
        using NotNull = Orm::Utils::NotNull<T>;
        /*! Alias for the type utils. */
        using TypeUtils = Orm::Utils::Type;

        /*! Default constructor. */
        inline HasRelationStore() = default;
        /*! Default destructor. */
        inline ~HasRelationStore() = default;

        /*! Copy constructor. */
        inline HasRelationStore(const HasRelationStore &other);
        /*! Copy assignment operator. */
        inline HasRelationStore &operator=(const HasRelationStore &other);

        /*! Move constructor. */
        inline HasRelationStore(HasRelationStore &&other) noexcept;
        /*! Move assignment operator. */
        inline HasRelationStore &operator=(HasRelationStore &&other) noexcept;

        /*! Type of data saved in the relation store. */
        enum struct RelationStoreType
        {
            EAGER,
            EAGER_POINTERS,
            PUSH,
            TOUCH_OWNERS,
            LAZY_RESULTS,
            BELONGSTOMANY_RELATED_TABLE,
            QUERIES_RELATIONSHIPS_QUERY,
            QUERIES_RELATIONSHIPS_TINY,
            QUERIES_RELATIONSHIPS_TINY_NESTED,
        };

        /* Forward declarations */
        /*! The store for loading eager relations. */
        template<SameDerivedCollectionModel<Derived> CollectionModel>
        class EagerRelationStore;
        class PushRelationStore;
        class TouchOwnersRelationStore;
        template<typename Related>
        class LazyRelationStore;
        class BelongsToManyRelatedTableStore;
        template<typename Related>
        class QueriesRelationshipsStore;

        /*! Base class for relation stores. */
        class BaseRelationStore
        {
            Q_DISABLE_COPY(BaseRelationStore)

        protected:
            /*! Constructor. */
            BaseRelationStore(NotNull<HasRelationStore *> hasRelationStore,
                              RelationStoreType storeType);

        public:
            /*! Pure virtual destructor. */
            inline virtual ~BaseRelationStore() = 0;

            /*! Visit the given relation. */
            void visit(const QString &relation);

            /*! Called from Model::u_relations to pass reference to the relation
                method, an enter point of the visitation. */
            template<RelationshipMethod<Derived> Method>
            void operator()(Method method);

        protected:
            /*! Currently held store type. */
            inline RelationStoreType getStoreType() const;

            /*! Reference to the parent HasRelationStore instance. */
            NotNull<HasRelationStore *> m_hasRelationStore;

        private:
            /*! Store type held by relation store. */
            /*const*/ RelationStoreType m_storeType;
        };

        /*! The store for loading eager relations. */
        template<SameDerivedCollectionModel<Derived> CollectionModel>
        class EagerRelationStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(EagerRelationStore)

        public:
            /*! Constructor. */
            EagerRelationStore(
                    NotNull<HasRelationStore *> hasRelationStore,
                    const Tiny::TinyBuilder<Derived> &builder,
                    ModelsCollection<CollectionModel> &models, const WithItem &relation);
            /*! Virtual destructor. */
            inline virtual ~EagerRelationStore() final = default;

            /*! Method called after visitation. */
            template<RelationshipMethod<Derived> Method>
            void visited(Method method) const;

        private:
            /*! The Tiny builder instance to which the visited relation will be
                dispatched. */
            NotNull<const Tiny::TinyBuilder<Derived> *> m_builder;
            /*! Models on which to do an eager load, hydrated models that were obtained
                from the database and these models will be passed as parameter
                to the TinyBuilder. */
            NotNull<ModelsCollection<CollectionModel> *> m_models;
            /*! The WithItem that will be passed as parameter to the TinyBuilder. */
            NotNull<const WithItem *> m_relation;
        };

        /*! The store for the Model push() method. */
        class PushRelationStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(PushRelationStore)

        public:
            /*! Constructor. */
            PushRelationStore(NotNull<HasRelationStore *> hasRelationStore,
                              RelationsType<AllRelations...> &models);
            /*! Virtual destructor. */
            inline virtual ~PushRelationStore() final = default;

            /*! Method called after visitation. */
            template<RelationshipMethod<Derived> Method>
            void visited(Method /*unused*/) const;

            /*! Models to push, the reference to the relation in the m_relations hash. */
            NotNull<RelationsType<AllRelations...> *> m_models;
            /*! The result of a push. */
            bool m_result = false;
        };

        /*! The store for touching owner's timestamps. */
        class TouchOwnersRelationStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(TouchOwnersRelationStore)

        public:
            /*! Constructor. */
            TouchOwnersRelationStore(NotNull<HasRelationStore *> hasRelationStore,
                                     const QString &relation);
            /*! Virtual destructor. */
            inline virtual ~TouchOwnersRelationStore() final = default;

            /*! Method called after visitation. */
            template<RelationshipMethod<Derived> Method>
            void visited(Method method) const;

            /*! Models to touch timestamps for, the reference to the relation name/key
                in the m_relations hash. */
            NotNull<const QString *> m_relation;
        };

        /*! The store for the lazy loading. */
        template<typename Related>
        class LazyRelationStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(LazyRelationStore)

        public:
            /*! Constructor. */
            explicit LazyRelationStore(NotNull<HasRelationStore *> hasRelationStore);
            /*! Virtual destructor. */
            inline virtual ~LazyRelationStore() final = default;

            /*! Method called after visitation. */
            template<RelationshipMethod<Derived> Method>
            void visited(Method method);

            // TODO templated LazyRelationStore by Container too, ModelsCollection to Container silverqx
            /*! The result of lazy load. */
            std::variant<ModelsCollection<Related>, std::optional<Related>> m_result;
        };

        /*! The store to obtain the related table name for BelongsToMany relation. */
        class BelongsToManyRelatedTableStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(BelongsToManyRelatedTableStore)

        public:
            /*! Constructor. */
            explicit
            BelongsToManyRelatedTableStore(NotNull<HasRelationStore *> hasRelationStore);
            /*! Virtual destructor. */
            inline virtual ~BelongsToManyRelatedTableStore() final = default;

            /*! Method called after visitation. */
            template<RelationshipMethod<Derived> Method>
            void visited(Method /*unused*/);

            /*! The related table name result. */
            std::optional<QString> m_result = std::nullopt;
        };

        /*! The store for obtaining a Relation instance for QueriesRelationships. */
        template<typename Related>
        class QueriesRelationshipsStore final : public BaseRelationStore
        {
            Q_DISABLE_COPY(QueriesRelationshipsStore)

        public:
            /*! QueriesRelationshipsStore constructor. */
            QueriesRelationshipsStore(
                    NotNull<HasRelationStore *> hasRelationStore,
                    QueriesRelationships<Derived> &origin, const QString &comparison,
                    qint64 count, const QString &condition,
                    const std::function<
                            void(QueriesRelationshipsCallback<Related> &)> &callback,
                    std::optional<std::reference_wrapper<
                            QStringList>> relations = std::nullopt);
            /*! Virtual destructor. */
            inline virtual ~QueriesRelationshipsStore() final = default;

            /*! Method called after visitation. */
            template<typename RelatedFromMethod, typename Method>
            void visited(Method method);

        protected:
            /*! Store type initializer. */
            constexpr static RelationStoreType initStoreType();

            /*! Served store type, this class can handle two store types. */
            T_THREAD_LOCAL
            constexpr static const RelationStoreType STORE_TYPE = initStoreType();

        private:
            /*! The QueriesRelationships instance to which the visited relation will be
                dispatched. */
            NotNull<QueriesRelationships<Derived> *> m_origin;
            /*! Comparison operator, used during querying relationship exitence. */
            NotNull<const QString *> m_comparison;
            /*! Required number of records, used during querying relationship
                exitence. */
            /*const*/ qint64 m_count;
            /*! Condition operator, used during querying relationship exitence. */
            NotNull<const QString *> m_condition;
            /*! Builder callback, used during querying relationship exitence. */
            NotNull<const std::function<void(
                    QueriesRelationshipsCallback<Related> &)> *> m_callback;
            /*! Nested relations for hasNested() method. */
            QStringList *m_relations;
        };

        /* Factory methods for Relation stores */
        /*! Factory method to create an eager store. */
        template<SameDerivedCollectionModel<Derived> CollectionModel>
        BaseRelationStore &
        createEagerStore(
                const Tiny::TinyBuilder<Derived> &builder,
                ModelsCollection<CollectionModel> &models, const WithItem &relation);
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
                std::optional<std::reference_wrapper<
                        QStringList>> relations = std::nullopt);

        /*! Release the ownership and destroy the top relation store on the stack. */
        void resetRelationStore();

        /* Getters for Relation stores */
        /*! Reference to the push store. */
        inline PushRelationStore &pushStore();
        /*! Cont reference to the touch owners relation store. */
        inline const TouchOwnersRelationStore &touchOwnersStore() const;
        /*! Const reference to the lazy store. */
        template<typename Related>
        inline const LazyRelationStore<Related> &lazyStore() const;
        /*! Reference to the BelongsToMany related table name store. */
        inline const BelongsToManyRelatedTableStore &
        belongsToManyRelatedTableStore() const;
        /*! Const reference to the QueriesRelationships store. */
        template<typename Related = void>
        inline const QueriesRelationshipsStore<Related> &
        queriesRelationshipsStore() const;

        /*! Type of the template message to generate. */
        enum struct CopyMoveTemplateType
        {
            COPY,
            MOVE,
        };
        /*! Template message for the Q_ASSERT_X() check in copy/move constructors. */
        static QString
        relationStoreCopyMoveTemplate(CopyMoveTemplateType type);

        /* Static cast this to a child's instance type (CRTP) */
        TINY_CRTP_MODEL_WITH_BASE_DECLARATIONS

        /*! The store where the values will be saved, before BaseRelationStore::visit()
            is called. */
        std::stack<std::shared_ptr<BaseRelationStore>> m_relationStore;
    };

    /* BaseRelationStore */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    HasRelationStore<Derived, AllRelations...>::BaseRelationStore
                                              ::~BaseRelationStore() = default;

    template<typename Derived, AllRelationsConcept ...AllRelations>
    HasRelationStore<Derived, AllRelations...>::BaseRelationStore::BaseRelationStore(
            NotNull<HasRelationStore *> hasRelationStore,
            const RelationStoreType storeType
    )
        : m_hasRelationStore(hasRelationStore)
        , m_storeType(storeType)
    {}

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasRelationStore<Derived, AllRelations...>::BaseRelationStore
                                                   ::visit(const QString &relation)
    {
        std::invoke(m_hasRelationStore->basemodel().getUserRelations()
                    .find(relation).value(),
                    *this);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<RelationshipMethod<Derived> Method>
    void HasRelationStore<Derived, AllRelations...>::BaseRelationStore
                                                   ::operator()(const Method method)
    {
        const auto storeType = getStoreType();

        switch (storeType) {
        case RelationStoreType::EAGER:
            static_cast<EagerRelationStore<Derived> *>(this)->visited(method);
            break;

        case RelationStoreType::EAGER_POINTERS:
            static_cast<EagerRelationStore<Derived *> *>(this)->visited(method);
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
            case RelationStoreType::QUERIES_RELATIONSHIPS_TINY_NESTED:
                static_cast<QueriesRelationshipsStore<void> *>(this)
                        ->template visited<Related>(method);
                break;

            case RelationStoreType::QUERIES_RELATIONSHIPS_TINY:
                static_cast<QueriesRelationshipsStore<Related> *>(this)
                        ->template visited<Related>(method);
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

    template<typename Derived, AllRelationsConcept ...AllRelations>
    typename HasRelationStore<Derived, AllRelations...>::RelationStoreType
    HasRelationStore<Derived, AllRelations...>::BaseRelationStore::getStoreType() const
    {
        return m_storeType;
    }

    /* EagerRelationStore */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<SameDerivedCollectionModel<Derived> CollectionModel>
    HasRelationStore<Derived, AllRelations...>::EagerRelationStore<CollectionModel>
                                              ::EagerRelationStore(
            NotNull<HasRelationStore *> hasRelationStore,
            const Tiny::TinyBuilder<Derived> &builder,
            ModelsCollection<CollectionModel> &models, const WithItem &relation
    )
        : BaseRelationStore(hasRelationStore, std::is_pointer_v<CollectionModel>
                                              ? RelationStoreType::EAGER_POINTERS
                                              : RelationStoreType::EAGER)
        , m_builder(&builder)
        , m_models(&models)
        , m_relation(&relation)
    {}

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<SameDerivedCollectionModel<Derived> CollectionModel>
    template<RelationshipMethod<Derived> Method>
    void HasRelationStore<Derived, AllRelations...>::EagerRelationStore<CollectionModel>
                                                   ::visited(
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
        auto dummyModel = this->m_hasRelationStore->model().newInstance();

        /* We want to run a relationship query without any constraints so that we will
           not have to remove these where clauses manually which gets really hacky
           and error prone. We don't want constraints because we add eager ones. */
        auto relationInstance =
                Relations::Relation<Derived, Related>::noConstraints(
                    [&method, &dummyModel]
        {
            return std::invoke(method, dummyModel);
        });

        m_builder->eagerLoadRelationVisited(std::move(relationInstance),
                                            *m_models, *m_relation);
    }

    /* PushRelationStore */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    HasRelationStore<Derived, AllRelations...>::PushRelationStore::PushRelationStore(
            NotNull<HasRelationStore *> hasRelationStore,
            RelationsType<AllRelations...> &models
    )
        : BaseRelationStore(hasRelationStore, RelationStoreType::PUSH)
        , m_models(&models)
    {}

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<RelationshipMethod<Derived> Method>
    void HasRelationStore<Derived, AllRelations...>::PushRelationStore::visited(
            const Method /*unused*/) const
    {
        using Related = typename std::invoke_result_t<Method, Derived>
                                    ::element_type::RelatedType;

        this->m_hasRelationStore->basemodel().template pushVisited<Related>();
    }

    /* TouchOwnersRelationStore */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    HasRelationStore<Derived, AllRelations...>::TouchOwnersRelationStore
                                              ::TouchOwnersRelationStore(
            NotNull<HasRelationStore *> hasRelationStore, const QString &relation
    )
        : BaseRelationStore(hasRelationStore, RelationStoreType::TOUCH_OWNERS)
        , m_relation(&relation)
    {}

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<RelationshipMethod<Derived> Method>
    void HasRelationStore<Derived, AllRelations...>::TouchOwnersRelationStore::visited(
            const Method method) const
    {
        auto relationInstance = std::invoke(method, this->m_hasRelationStore->model());

        using Related = typename std::invoke_result_t<Method, Derived>
                                    ::element_type::RelatedType;

        this->m_hasRelationStore->basemodel()
                .template touchOwnersVisited<Related>(std::move(relationInstance));
    }

    /* LazyRelationStore */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    HasRelationStore<Derived, AllRelations...>::LazyRelationStore<Related>
                                              ::LazyRelationStore(
            NotNull<HasRelationStore *> hasRelationStore
    )
        : BaseRelationStore(hasRelationStore, RelationStoreType::LAZY_RESULTS)
    {}

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    template<RelationshipMethod<Derived> Method>
    void
    HasRelationStore<Derived, AllRelations...>::LazyRelationStore<Related>::visited(
            const Method method)
    {
        m_result = std::invoke(method, this->m_hasRelationStore->model())->getResults();
    }

    /* BelongsToManyRelatedTableStore */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    HasRelationStore<Derived, AllRelations...>::BelongsToManyRelatedTableStore
                                              ::BelongsToManyRelatedTableStore(
            NotNull<HasRelationStore *> hasRelationStore
    )
        : BaseRelationStore(hasRelationStore,
                            RelationStoreType::BELONGSTOMANY_RELATED_TABLE)
    {}

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<RelationshipMethod<Derived> Method>
    void HasRelationStore<Derived, AllRelations...>::BelongsToManyRelatedTableStore
                                                   ::visited(const Method /*unused*/)
    {
        using Relation = typename std::invoke_result_t<Method, Derived>::element_type;

        if constexpr (std::is_base_of_v<Relations::IsPivotRelation, Relation>)
            m_result = typename Relation::RelatedType().getTable();
    }

    /* QueriesRelationshipsStore */

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

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    HasRelationStore<Derived, AllRelations...>::QueriesRelationshipsStore<Related>
                                              ::QueriesRelationshipsStore(
            NotNull<HasRelationStore *> hasRelationStore,
            QueriesRelationships<Derived> &origin,
            const QString &comparison, const qint64 count, const QString &condition,
            const std::function<void(QueriesRelationshipsCallback<Related> &)> &callback,
            const std::optional<std::reference_wrapper<QStringList>> relations
    )
        : BaseRelationStore(hasRelationStore,
                            relations
                            ? RelationStoreType::QUERIES_RELATIONSHIPS_TINY_NESTED
                            : STORE_TYPE)
        , m_origin(&origin)
        , m_comparison(&comparison)
        , m_count(count)
        , m_condition(&condition)
        , m_callback(&callback)
        , m_relations(relations ? &relations->get() : nullptr)
    {}

    template<typename Derived, AllRelationsConcept ...AllRelations>
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

        // We want to run a relationship query without any constraints
        auto relationInstance =
                Relations::Relation<Derived, RelatedFromMethod>::noConstraints(
                    [this, &method]
        {
            return std::invoke(method, this->m_hasRelationStore->model());
        });

        // Nested store type, used by hasNested()
        if (this->getStoreType() == RelationStoreType::QUERIES_RELATIONSHIPS_TINY_NESTED)
            m_origin->template hasInternalVisited<RelatedFromMethod>(
                        std::move(relationInstance), *m_comparison, m_count,
                        *m_condition, *m_relations);
        else
            m_origin->template has<RelatedFromMethod>(
                        std::move(relationInstance), *m_comparison, m_count,
                        *m_condition, *m_callback);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    constexpr typename
    HasRelationStore<Derived, AllRelations...>::RelationStoreType
    HasRelationStore<Derived, AllRelations...>::QueriesRelationshipsStore<Related>
                                              ::initStoreType()
    {
        if constexpr (std::is_void_v<Related>)
            return RelationStoreType::QUERIES_RELATIONSHIPS_QUERY;
        else
            return RelationStoreType::QUERIES_RELATIONSHIPS_TINY;
    }

    /* HasRelationStore */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    HasRelationStore<Derived, AllRelations...>::HasRelationStore( // NOLINT(modernize-use-equals-delete)
#ifdef TINYORM_DEBUG
            const HasRelationStore &other)
    {
        // This is real porn 😂
        Q_ASSERT_X(m_relationStore.empty() && other.m_relationStore.empty(),
                   "Relation store copy constructor",
                   relationStoreCopyMoveTemplate(CopyMoveTemplateType::COPY)
                   .toUtf8().constData());

        m_relationStore = {};
    }
#else
            const HasRelationStore &/*unused*/)
        : m_relationStore()
    {}
#endif

    template<typename Derived, AllRelationsConcept ...AllRelations>
    HasRelationStore<Derived, AllRelations...> &
    HasRelationStore<Derived, AllRelations...>::operator=( // NOLINT(modernize-use-equals-delete)
#ifdef TINYORM_DEBUG
            const HasRelationStore &other)
    {
        Q_ASSERT_X(m_relationStore.empty() && other.m_relationStore.empty(),
                   "Relation store copy assignment",
                   relationStoreCopyMoveTemplate(CopyMoveTemplateType::COPY)
                   .toUtf8().constData());
#else
            const HasRelationStore &/*unused*/)
    {
#endif
        m_relationStore = {};

        return *this;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    HasRelationStore<Derived, AllRelations...>::HasRelationStore( // NOLINT(modernize-use-equals-delete)
#ifdef TINYORM_DEBUG
            HasRelationStore &&other) noexcept
    {
        Q_ASSERT_X(m_relationStore.empty() && other.m_relationStore.empty(),
                   "Relation store move constructor",
                   relationStoreCopyMoveTemplate(CopyMoveTemplateType::MOVE)
                   .toUtf8().constData());

        m_relationStore = {};
    }
#else
            HasRelationStore &&/*unused*/) noexcept
        : m_relationStore()
    {}
#endif

    template<typename Derived, AllRelationsConcept ...AllRelations>
    HasRelationStore<Derived, AllRelations...> &
    HasRelationStore<Derived, AllRelations...>::operator=( // NOLINT(modernize-use-equals-delete)
#ifdef TINYORM_DEBUG
            HasRelationStore &&other) noexcept
    {
        Q_ASSERT_X(m_relationStore.empty() && other.m_relationStore.empty(),
                   "Relation store move assignment",
                   relationStoreCopyMoveTemplate(CopyMoveTemplateType::MOVE)
                   .toUtf8().constData());
#else
            HasRelationStore &&/*unused*/) noexcept
    {
#endif
        m_relationStore = {};

        return *this;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<SameDerivedCollectionModel<Derived> CollectionModel>
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::createEagerStore(
            const Tiny::TinyBuilder<Derived> &builder,
            ModelsCollection<CollectionModel> &models, const WithItem &relation)
    {
        m_relationStore.push(std::make_shared<EagerRelationStore<CollectionModel>>(
                                 this, builder, models, relation));

        return *m_relationStore.top();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::createPushStore(
            RelationsType<AllRelations...> &models)
    {
        m_relationStore.push(std::make_shared<PushRelationStore>(this, models));

        return *m_relationStore.top();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::createTouchOwnersStore(
            const QString &relation)
    {
        m_relationStore.push(
                    std::make_shared<TouchOwnersRelationStore>(this, relation));

        return *m_relationStore.top();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::createLazyStore()
    {
        m_relationStore.push(std::make_shared<LazyRelationStore<Related>>(this));

        return *m_relationStore.top();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::createBelongsToManyRelatedTableStore()
    {
        m_relationStore.push(std::make_shared<BelongsToManyRelatedTableStore>(this));

        return *m_relationStore.top();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::createQueriesRelationshipsStore(
            QueriesRelationships<Derived> &origin, const QString &comparison,
            const qint64 count, const QString &condition,
            const std::function<void(QueriesRelationshipsCallback<Related> &)> &callback,
            const std::optional<std::reference_wrapper<QStringList>> relations)
    {
        m_relationStore.push(std::make_shared<QueriesRelationshipsStore<Related>>(
                                 this, origin, comparison, count, condition, callback,
                                 relations));

        return *m_relationStore.top();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasRelationStore<Derived, AllRelations...>::resetRelationStore()
    {
        m_relationStore.pop();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    typename HasRelationStore<Derived, AllRelations...>::PushRelationStore &
    HasRelationStore<Derived, AllRelations...>::pushStore()
    {
        return *std::static_pointer_cast<PushRelationStore>(m_relationStore.top());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const typename HasRelationStore<Derived, AllRelations...>::TouchOwnersRelationStore &
    HasRelationStore<Derived, AllRelations...>::touchOwnersStore() const
    {
        return *std::static_pointer_cast<
                const TouchOwnersRelationStore>(m_relationStore.top());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    const typename HasRelationStore<Derived, AllRelations...>::
          template LazyRelationStore<Related> &
    HasRelationStore<Derived, AllRelations...>::lazyStore() const
    {
        return *std::static_pointer_cast<
                const LazyRelationStore<Related>>(m_relationStore.top());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const typename HasRelationStore<Derived, AllRelations...>::
                   BelongsToManyRelatedTableStore &
    HasRelationStore<Derived, AllRelations...>::belongsToManyRelatedTableStore() const
    {
        return *std::static_pointer_cast<
                const BelongsToManyRelatedTableStore>(m_relationStore.top());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    const typename HasRelationStore<Derived, AllRelations...>::
          template QueriesRelationshipsStore<Related> &
    HasRelationStore<Derived, AllRelations...>::queriesRelationshipsStore() const
    {
        return *std::static_pointer_cast<
                const QueriesRelationshipsStore<Related>>(m_relationStore.top());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    HasRelationStore<Derived, AllRelations...>::relationStoreCopyMoveTemplate(
#ifdef TINYORM_DEBUG
            const CopyMoveTemplateType type)
#else
            const CopyMoveTemplateType /*unused*/)
#endif
    {
#ifdef TINYORM_DEBUG
        static const auto message =
                QStringLiteral(
                    "You can not %1 the %2 model in the middle of any relation store "
                    "operation.");

        static const auto className = TypeUtils::classPureBasename<Derived>();

        switch (type) {
        case CopyMoveTemplateType::COPY:
            return message.arg(QStringLiteral("copy"), className);
        case CopyMoveTemplateType::MOVE:
            return message.arg(QStringLiteral("move"), className);
        default:
            Q_UNREACHABLE();
        }
#endif

        return "";
    }

    /* Static cast this to a child's instance type (CRTP) */
    TINY_CRTP_MODEL_WITH_BASE_DEFINITIONS(HasRelationStore)

} // namespace Orm::Tiny::Concerns

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_CONCERNS_HASRELATIONSTORE_HPP
