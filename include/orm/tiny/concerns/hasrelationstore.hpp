#pragma once
#ifndef ORM_TINY_CONCERNS_HASRELATIONSTORE_HPP
#define ORM_TINY_CONCERNS_HASRELATIONSTORE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <stack>

#include "orm/tiny/macros/crtpmodelwithbase.hpp"
#include "orm/tiny/support/stores/belongstomanyrelatedtablestore.hpp" // IWYU pragma: keep
#include "orm/tiny/support/stores/eagerrelationstore.hpp" // IWYU pragma: keep
#include "orm/tiny/support/stores/lazyrelationstore.hpp" // IWYU pragma: keep
#include "orm/tiny/support/stores/pushrelationstore.hpp" // IWYU pragma: keep
#include "orm/tiny/support/stores/queriesrelationshipsstore.hpp"
#include "orm/tiny/support/stores/serializerelationstore.hpp" // IWYU pragma: keep
#include "orm/tiny/support/stores/touchownersrelationstore.hpp" // IWYU pragma: keep

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Concerns
{

    // FUTURE relationstore, cache results, eg. cache Relation instance and return copy of this cached Relation instance, Related parameter can be obtained from cached Relation instance silverqx
    /*! Relation store, handles mapping from a relation name to the Model's relation
        method, also calls visited method with Related parameter when needed. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class HasRelationStore // clazy:exclude=rule-of-three
    {
        /*! Alias for the type utils. */
        using TypeUtils = Orm::Utils::Type;

        /* Aliases to shorten type names, yes yes templated c++ 😵‍💫 */
        /*! Alias for the BaseRelationStore (for shorter name). */
        using BaseRelationStore =
              Support::Stores::BaseRelationStore<Derived, AllRelations...>;

        TINY_RELATIONSTORES_ALIASES

        // To access model() and basemodel()
        friend BaseRelationStore;
        // To access createXyzStore(), xyzStore() getters, resetRelationStore(), ...
        friend HasRelationships<Derived, AllRelations...>;

        /* Constructors */
        /*! Default constructor. */
        HasRelationStore() = default;
        /*! Default destructor. */
        ~HasRelationStore() = default;

        /*! Copy constructor. */
        inline HasRelationStore(const HasRelationStore &other);
        /*! Copy assignment operator. */
        inline HasRelationStore &operator=(const HasRelationStore &other);

        /*! Move constructor. */
        inline HasRelationStore(HasRelationStore &&other) noexcept;
        /*! Move assignment operator. */
        inline HasRelationStore &operator=(HasRelationStore &&other) noexcept;

        /* Factory methods for Relation stores */
        /*! Factory method to create an eager store. */
        template<SameDerivedCollectionModel<Derived> CollectionModel>
        BaseRelationStore &
        createEagerStore(
                const Tiny::TinyBuilder<Derived> &builder,
                ModelsCollection<CollectionModel> &models,
                const WithItem &relation) const;
        /*! Factory method to create the push store. */
        BaseRelationStore &createPushStore(RelationsType<AllRelations...> &models) const;
        /*! Factory method to create the touch owners store. */
        BaseRelationStore &createTouchOwnersStore(const QString &relation) const;
        /*! Factory method to create the lazy store. */
        template<typename Related>
        BaseRelationStore &createLazyStore() const;
        /*! Factory method to create the store to obtain BelongsToMany related model
            table name. */
        BaseRelationStore &createBelongsToManyRelatedTableStore() const;
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
                        QStringList>> relations = std::nullopt) const;
        /*! Factory method to create the store for serializing relationship. */
        template<SerializedAttributes C>
        BaseRelationStore &
        createSerializeRelationStore(
                const QString &relation, const RelationsType<AllRelations...> &models,
                C &attributes) const;

        /*! Release the ownership and destroy the top relation store on the stack. */
        void resetRelationStore() const;

        /* Getters for Relation stores */
        /*! Reference to the push store. */
        inline PushRelationStore &pushStore() const;
        /*! Cont reference to the touch owners relation store. */
        inline const TouchOwnersRelationStore &touchOwnersStore() const;
        /*! Const reference to the lazy store. */
        template<typename Related>
        const LazyRelationStore<Related> &lazyStore() const;
        /*! Reference to the BelongsToMany related table name store. */
        inline BelongsToManyRelatedTableStore &
        belongsToManyRelatedTableStore() const;
        /*! Const reference to the QueriesRelationships store. */
        template<typename Related = void>
        const QueriesRelationshipsStore<Related> &
        queriesRelationshipsStore() const;
        /*! Const reference to the serialize relation store. */
        template<SerializedAttributes C>
        const SerializeRelationStore<C> &serializeRelationStore() const;

        /*! Type of the template message to generate. */
        enum struct CopyMoveTemplateType : quint8
        {
            /*! Tags copy constructor and assignment operator. */
            COPY,
            /*! Tags move constructor and assignment operator. */
            MOVE,
        };
        /*! Template message for the Q_ASSERT_X() check in copy/move constructors. */
#ifdef TINYORM_NO_DEBUG
        [[maybe_unused]]
#endif
        static QString
        relationStoreCopyMoveTemplate(CopyMoveTemplateType type);

        /* Static cast this to a child's instance type (CRTP) */
        TINY_CRTP_MODEL_WITH_BASE_DECLARATIONS

        /*! The store where the values will be saved, before BaseRelationStore::visit()
            is called. */
        mutable std::stack<std::shared_ptr<BaseRelationStore>> m_relationStore;
    };

    /* I have tried twice to make the m_relationStore to be only one instance per thread
       (thread_local and first time inline static and second static local variable), but
       I reverted it because it's slower for some mysterious reasons. 🤯
       I tested it with the TinyOrmPlayground that had 1594 DB queries and it was ~20ms
       slower. */

    /* private */

    /* Constructors */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    HasRelationStore<Derived, AllRelations...>::HasRelationStore( // NOLINT(modernize-use-equals-delete)
#ifdef TINYORM_DEBUG
            const HasRelationStore &other)
        : m_relationStore()
    {
        // This is real porn 😂
        Q_ASSERT_X(other.m_relationStore.empty(),
                   "Relation store copy constructor",
                   relationStoreCopyMoveTemplate(CopyMoveTemplateType::COPY)
                   .toUtf8().constData());
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
        : m_relationStore()
    {
        Q_ASSERT_X(other.m_relationStore.empty(),
                   "Relation store move constructor",
                   relationStoreCopyMoveTemplate(CopyMoveTemplateType::MOVE)
                   .toUtf8().constData());
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

    /* private */

    /* Factory methods for Relation stores */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<SameDerivedCollectionModel<Derived> CollectionModel>
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::createEagerStore(
            const Tiny::TinyBuilder<Derived> &builder,
            ModelsCollection<CollectionModel> &models, const WithItem &relation) const
    {
        m_relationStore.push(
            std::make_shared<EagerRelationStore<CollectionModel>>(
                const_cast<HasRelationStore *>(this), builder, models, relation)); // NOLINT(cppcoreguidelines-pro-type-const-cast)

        return *m_relationStore.top();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::createPushStore(
            RelationsType<AllRelations...> &models) const
    {
        m_relationStore.push(
            std::make_shared<PushRelationStore>(const_cast<HasRelationStore *>(this), // NOLINT(cppcoreguidelines-pro-type-const-cast)
                                                models));

        return *m_relationStore.top();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::createTouchOwnersStore(
            const QString &relation) const
    {
        m_relationStore.push(std::make_shared<TouchOwnersRelationStore>(
                                 const_cast<HasRelationStore *>(this), relation)); // NOLINT(cppcoreguidelines-pro-type-const-cast)

        return *m_relationStore.top();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::createLazyStore() const
    {
        m_relationStore.push(std::make_shared<LazyRelationStore<Related>>(
                                 const_cast<HasRelationStore *>(this))); // NOLINT(cppcoreguidelines-pro-type-const-cast)

        return *m_relationStore.top();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::
    createBelongsToManyRelatedTableStore() const
    {
        m_relationStore.push(std::make_shared<BelongsToManyRelatedTableStore>(
                                 const_cast<HasRelationStore *>(this))); // NOLINT(cppcoreguidelines-pro-type-const-cast)

        return *m_relationStore.top();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::createQueriesRelationshipsStore(
            QueriesRelationships<Derived> &origin, const QString &comparison,
            const qint64 count, const QString &condition,
            const std::function<void(QueriesRelationshipsCallback<Related> &)> &callback,
            const std::optional<std::reference_wrapper<QStringList>> relations) const
    {
        m_relationStore.push(std::make_shared<QueriesRelationshipsStore<Related>>(
                                 const_cast<HasRelationStore *>(this), origin, // NOLINT(cppcoreguidelines-pro-type-const-cast)
                                 comparison, count, condition, callback, relations));

        return *m_relationStore.top();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<SerializedAttributes C>
    typename HasRelationStore<Derived, AllRelations...>::BaseRelationStore &
    HasRelationStore<Derived, AllRelations...>::createSerializeRelationStore(
            const QString &relation, const RelationsType<AllRelations...> &models,
            C &attributes) const
    {
        m_relationStore.push(std::make_shared<SerializeRelationStore<C>>(
                                 const_cast<HasRelationStore *>(this), relation, models, // NOLINT(cppcoreguidelines-pro-type-const-cast)
                                 attributes));

        return *m_relationStore.top();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void HasRelationStore<Derived, AllRelations...>::resetRelationStore() const
    {
        m_relationStore.pop();
    }

    /* Getters for Relation stores */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    typename HasRelationStore<Derived, AllRelations...>::PushRelationStore &
    HasRelationStore<Derived, AllRelations...>::pushStore() const
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
    typename HasRelationStore<Derived, AllRelations...>::
             BelongsToManyRelatedTableStore &
    HasRelationStore<Derived, AllRelations...>::belongsToManyRelatedTableStore() const
    {
        return *std::static_pointer_cast<
                BelongsToManyRelatedTableStore>(m_relationStore.top());
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
    template<SerializedAttributes C>
    const typename HasRelationStore<Derived, AllRelations...>::
          template SerializeRelationStore<C> &
    HasRelationStore<Derived, AllRelations...>::serializeRelationStore() const
    {
        return *std::static_pointer_cast<
                const SerializeRelationStore<C>>(m_relationStore.top());
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
        // Don't make it static
        const auto className = TypeUtils::classPureBasename<Derived>();

        switch (type) {
        case CopyMoveTemplateType::COPY:
            return message.arg(QStringLiteral("copy"), className);
        case CopyMoveTemplateType::MOVE:
            return message.arg(QStringLiteral("move"), className);
        default:
            Q_UNREACHABLE();
        }
#endif

        // This method is used in Debug builds only
        return {};
    }

    /* Static cast this to a child's instance type (CRTP) */

    TINY_CRTP_MODEL_WITH_BASE_DEFINITIONS(HasRelationStore)

} // namespace Orm::Tiny::Concerns

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_CONCERNS_HASRELATIONSTORE_HPP
