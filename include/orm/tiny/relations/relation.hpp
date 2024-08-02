#pragma once
#ifndef ORM_TINY_RELATIONS_RELATION_HPP
#define ORM_TINY_RELATIONS_RELATION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <range/v3/action/sort.hpp>
#include <range/v3/action/unique.hpp>

#include "orm/exceptions/runtimeerror.hpp"
#include "orm/macros/threadlocal.hpp"
#include "orm/tiny/relations/relationproxies.hpp"
#include "orm/tiny/relations/relationtypes.hpp"
#include "orm/utils/notnull.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny
{
namespace Concerns
{
    template<typename Model>
    class QueriesRelationships;
}

namespace Relations
{

    /*! Base relations class. */
    template<class Model, class Related>
    class Relation : public RelationProxies<Model, Related>,
                     public IsRelation
    {
        // Used by QueriesRelationships::getHasQueryByExistenceCheck()
        friend Tiny::Concerns::QueriesRelationships<Model>;

        /*! Alias for the Expression. */
        using Expression = Orm::Query::Expression;
        /*! Alias for the NotNull. */
        template<typename T>
        using NotNull = Orm::Utils::NotNull<T>;

    protected:
        /*! Protected constructor. */
        Relation(std::unique_ptr<Related> &&related, Model &parent,
                 const QString &relatedKey = "");

        /*! Relation's copy constructor (used by BelongsToMany::clone()). */
        Relation(const Relation &) = default;

    public:
        /*! Parent Model type. */
        using ModelType = Model;
        /*! Related type. */
        using RelatedType = Related;

        /*! Pure virtual destructor. */
        inline ~Relation() override = 0;

        /*! Relation's move constructor. */
        Relation(Relation &&) = delete;

        /*! Relation's copy assignment operator. */
        Relation &operator=(const Relation &) = delete;
        /*! Relation's move assignment operator. */
        Relation &operator=(Relation &&) = delete;

        /* Relation related operations */
        /*! Set the base constraints on the relation query. */
        virtual void addConstraints() const = 0;

        /*! Run a callback with constraints disabled on the relation. */
        static std::unique_ptr<Relation<Model, Related>>
        noConstraints(const std::function<
                      std::unique_ptr<Relation<Model, Related>>()> &callback);

        // Note at the bottom of the file
        /*! Set the constraints for an eager load of the relation. */
        virtual void addEagerConstraints(const ModelsCollection<Model> &models) = 0;
        /*! Set the constraints for an eager load of the relation. */
        virtual void addEagerConstraints(const ModelsCollection<Model *> &models) = 0;

        /*! Initialize the relation on a set of models. */
        virtual ModelsCollection<Model> &
        initRelation(ModelsCollection<Model> &models,
                     const QString &relation) const = 0;
        /*! Initialize the relation on a set of models. */
        virtual ModelsCollection<Model *> &
        initRelation(ModelsCollection<Model *> &models,
                     const QString &relation) const = 0;

        /*! Match the eagerly loaded results to their parents. */
        virtual void
        match(ModelsCollection<Model> &models, ModelsCollection<Related> &&results,
              const QString &relation) const = 0;
        /*! Match the eagerly loaded results to their parents. */
        virtual void
        match(ModelsCollection<Model *> &models, ModelsCollection<Related> &&results,
              const QString &relation) const = 0;

        /*! Get the results of the relationship. */
        virtual std::variant<ModelsCollection<Related>, std::optional<Related>>
        getResults() const = 0;

        /*! Get the relationship for eager loading. */
        inline ModelsCollection<Related> getEager() const;
        /*! Execute the query as a "select" statement. */
        inline virtual ModelsCollection<Related>
        get(const QList<Column> &columns = {ASTERISK}) const; // NOLINT(google-default-arguments)

        /* Getters / Setters */
        /*! Get the underlying query for the relation. */
        inline Builder<Related> &getQuery() const noexcept;
        /*! Get the base QueryBuilder driving the TinyBuilder. */
        inline QueryBuilder &getBaseQuery() const noexcept;

        /*! Get the parent model of the relation. */
        inline const Model &getParent() const noexcept;
        /*! Get the related model of the relation. */
        inline const Related &getRelated() const noexcept;
        /*! Get the related model of the relation. */
        inline Related &getRelated() noexcept;
        /*! Get the name of the "created at" column. */
        inline const QString &createdAt() const;
        /*! Get the name of the "updated at" column. */
        inline const QString &updatedAt() const;
        /*! Get the name of the related model's "updated at" column. */
        inline const QString &relatedUpdatedAt() const;
        /*! Get the related key for the relationship. */
        inline const QString &getRelatedKeyName() const noexcept;
        /*! Get the fully qualified parent key name. */
        inline virtual QString getQualifiedParentKeyName() const;
        /*! Get the key for comparing against the parent key in "has" query. */
        inline virtual QString getExistenceCompareKey() const;

        /* Timestamps */
        /*! Touch all of the related models for the relationship. */
        virtual void touch() const;

        /* Others */
        /*! Run a raw update against the base query. */
        inline std::tuple<int, TSqlQuery>
        rawUpdate(const QList<UpdateItem> &values = {}) const;

        /*! The textual representation of the Relation type. */
        virtual const QString &relationTypeName() const = 0;

    protected:
        /* Relation related operations */
        /*! Initialize a Relation instance. */
        inline void init() const;

        /*! Add a whereIn eager constraint for a given set of model keys to be loaded. */
        void whereInEager(const QString &key, const QList<QVariant> &modelKeys);

        /*! Get all of the primary keys for the vector of models. */
        template<SameDerivedCollectionModel<Model> CollectionModel>
        QList<QVariant>
        getKeys(const ModelsCollection<CollectionModel> &models,
                const QString &key = "") const;

        /*! Convert the Model pointer to the pointer (no-op). */
        constexpr static Model *toPointer(Model *model);
        /*! Convert the Model pointer to the pointer (no-op). */
        constexpr static const Model *toPointer(const Model *model);
        /*! Convert the Model reference to the pointer. */
        inline static Model *toPointer(Model &model) noexcept;
        /*! Convert the const Model reference to the pointer. */
        inline static const Model *toPointer(const Model &model) noexcept;

        /* Querying Relationship Existence/Absence */
        /*! Add the constraints for an internal relationship existence query.
            Essentially, these queries compare on column names like whereColumn. */
        virtual std::unique_ptr<Builder<Related>>
        getRelationExistenceQuery( // NOLINT(google-default-arguments)
                std::unique_ptr<Builder<Related>> &&query,
                const Builder<Model> &parentQuery,
                const QList<Column> &columns = {ASTERISK}) const;
        /*! Add the constraints for a relationship count query. */
        std::unique_ptr<Builder<Related>>
        getRelationExistenceCountQuery(
                std::unique_ptr<Builder<Related>> &&query,
                const Builder<Model> &parentQuery) const;

        /* During eager load, we secure m_parent to not become a dangling reference in
           EagerRelationStore::visited() by help of the dummyModel local variable.
           It has to be the reference, because eg. BelongsTo::associate() directly
           modifies attributes of m_parent. */
        /*! The parent model instance. */
        NotNull<Model *> m_parent;
        /*! The related model instance. */
        NotNull<std::shared_ptr<Related>> m_related;
        /*! The key name of the related model. */
        QString m_relatedKey;
        // TODO next would be good to use TinyBuilder alias instead of Builder silverqx
        /*! The TinyORM TinyBuilder instance. */
        std::shared_ptr<Builder<Related>> m_query;
        /*! Indicates if the relation is adding constraints. */
        T_THREAD_LOCAL
        inline static bool constraints = true;

    private:
        /*! Indicates whether the eagerly loaded relation should implicitly return
            an empty collection. */
        bool m_eagerKeysWereEmpty = false;
    };

    /* protected */

    template<class Model, class Related>
    Relation<Model, Related>::Relation(std::unique_ptr<Related> &&related, Model &parent,
                                       const QString &relatedKey)
        : m_parent(&parent)
        , m_related(std::move(related))
        , m_relatedKey(relatedKey.isEmpty() ? m_related->getKeyName() : relatedKey)
        , m_query(m_related->newQuery())
    {}

    /* public */

    template<class Model, class Related>
    Relation<Model, Related>::~Relation() = default;

    /* Relation related operations */

    template<class Model, class Related>
    std::unique_ptr<Relation<Model, Related>>
    Relation<Model, Related>::noConstraints(
            const std::function<std::unique_ptr<Relation<Model, Related>>()> &callback)
    {
        const auto previous = constraints;

        constraints = false;
        auto relation = std::invoke(callback);
        constraints = previous;

        return relation;
    }

    template<class Model, class Related>
    ModelsCollection<Related>
    Relation<Model, Related>::getEager() const
    {
        // Avoid querying the database if the keys are empty (IN () aka. where 0 = 1)
        if (m_eagerKeysWereEmpty)
            return {};

        return get();
    }

    template<class Model, class Related>
    ModelsCollection<Related>
    Relation<Model, Related>::get(const QList<Column> &columns) const // NOLINT(google-default-arguments)
    {
        return m_query->get(columns);
    }

    /* Getters / Setters */

    template<class Model, class Related>
    Builder<Related> &Relation<Model, Related>::getQuery() const noexcept
    {
        return *m_query;
    }

    template<class Model, class Related>
    QueryBuilder &Relation<Model, Related>::getBaseQuery() const noexcept
    {
        return m_query->getQuery();
    }

    template<class Model, class Related>
    const Model &Relation<Model, Related>::getParent() const noexcept
    {
        return *m_parent;
    }

    template<class Model, class Related>
    const Related &Relation<Model, Related>::getRelated() const noexcept
    {
        return *m_related;
    }

    template<class Model, class Related>
    Related &Relation<Model, Related>::getRelated() noexcept
    {
        return *m_related;
    }

    template<class Model, class Related>
    const QString &Relation<Model, Related>::createdAt() const
    {
        return m_parent->getCreatedAtColumn();
    }

    template<class Model, class Related>
    const QString &Relation<Model, Related>::updatedAt() const
    {
        return m_parent->getUpdatedAtColumn();
    }

    template<class Model, class Related>
    const QString &Relation<Model, Related>::relatedUpdatedAt() const
    {
        return m_related->getUpdatedAtColumn();
    }

    template<class Model, class Related>
    const QString &Relation<Model, Related>::getRelatedKeyName() const noexcept
    {
        return m_relatedKey;
    }

    template<class Model, class Related>
    QString Relation<Model, Related>::getQualifiedParentKeyName() const
    {
        return m_parent->getQualifiedKeyName();
    }

    template<class Model, class Related>
    QString Relation<Model, Related>::getExistenceCompareKey() const
    {
        throw Orm::Exceptions::RuntimeError(
                    QStringLiteral("Method %1() is not implemented for '%2' "
                                   "relation type in %3().")
                    /* Don't use the __tiny_func__ here, use the __func__ to only show
                       a function name w/o a class name. */
                    .arg(__func__, relationTypeName(), __tiny_func__));
    }

    /* Timestamps */

    template<class Model, class Related>
    void Relation<Model, Related>::touch() const
    {
        if (Related::isIgnoringTouch())
            return;

        rawUpdate({
            {Related::getUpdatedAtColumn(), getRelated().freshTimestampString()}
        });
    }

    /* Others */

    template<class Model, class Related>
    std::tuple<int, TSqlQuery>
    Relation<Model, Related>::rawUpdate(const QList<UpdateItem> &values) const
    {
        // FEATURE scopes silverqx
        return m_query->update(values);
    }

    /* protected */

    /* Relation related operations */

    template<class Model, class Related>
    void Relation<Model, Related>::init() const
    {
        addConstraints();
    }

    template<class Model, class Related>
    void Relation<Model, Related>::whereInEager(const QString &key,
                                                const QList<QVariant> &modelKeys)
    {
        getBaseQuery().whereIn(key, modelKeys);

        // Set empty keys flag
        if (modelKeys.isEmpty())
            m_eagerKeysWereEmpty = true;
    }

    template<class Model, class Related>
    template<SameDerivedCollectionModel<Model> CollectionModel>
    QList<QVariant>
    Relation<Model, Related>::getKeys(const ModelsCollection<CollectionModel> &models,
                                      const QString &key) const
    {
        QList<QVariant> keys;
        keys.reserve(models.size());

        /*! Const Model type used in the for-ranged loops. */
        using ConstModelLoopType = typename ModelsCollection<CollectionModel>::
                                            ConstModelLoopType;

        for (ConstModelLoopType model : models)
            keys.append(key.isEmpty() ? toPointer(model)->getKey()
                                      : toPointer(model)->getAttribute(key));

        return keys |= ranges::actions::sort(ranges::less {}, [](const auto &key_)
        {
            return key_.template value<typename Model::KeyType>();
        })
                | ranges::actions::unique;
    }

    template<class Model, class Related>
    constexpr Model *
    Relation<Model, Related>::toPointer(Model *const model)
    {
        // I don't have enough courage to remove this
        Q_CHECK_PTR(model);

        return model;
    }

    template<class Model, class Related>
    constexpr const Model *
    Relation<Model, Related>::toPointer(const Model *const model)
    {
        // I don't have enough courage to remove this
        Q_CHECK_PTR(model);

        return model;
    }

    template<class Model, class Related>
    Model *
    Relation<Model, Related>::toPointer(Model &model) noexcept
    {
        return &model;
    }

    template<class Model, class Related>
    const Model *
    Relation<Model, Related>::toPointer(const Model &model) noexcept
    {
        return &model;
    }

    /* Querying Relationship Existence/Absence */

    template<class Model, class Related>
    std::unique_ptr<Builder<Related>>
    Relation<Model, Related>::getRelationExistenceQuery( // NOLINT(google-default-arguments)
            std::unique_ptr<Builder<Related>> &&query, const Builder<Model> &/*unused*/,
            const QList<Column> &columns) const
    {
        query->select(columns).whereColumnEq(getQualifiedParentKeyName(),
                                             getExistenceCompareKey());

        return std::move(query);
    }

    template<class Model, class Related>
    std::unique_ptr<Builder<Related>>
    Relation<Model, Related>::getRelationExistenceCountQuery(
            std::unique_ptr<Builder<Related>> &&query,
            const Builder<Model> &parentQuery) const
    {
        // Ownership of a unique_ptr()
        query = getRelationExistenceQuery(std::move(query), parentQuery,
                                          {Expression(QStringLiteral("count(*)"))});

        query->getQuery().setBindings({}, BindingType::SELECT);

        return std::move(query);
    }

} // namespace Relations
} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_RELATION_HPP

/* I don't use templated versions of the addEagerConstraints(), initRelation(), and
   match() because the std::unique_ptr<Relation<>> can be returned from the relationship
   methods (instead of eg. std::unique_ptr<HasMany<>>) and this is only possible because
   of polymorphism. It can be considered an additional feature and would be lost if
   the templated version of these methods were used. */
