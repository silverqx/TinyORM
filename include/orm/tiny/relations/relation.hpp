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
#include "orm/utils/type.hpp"

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

    protected:
        /*! Protected constructor. */
        Relation(std::unique_ptr<Related> &&related, Model &parent,
                 const QString &relatedKey = "");

        /*! Relation's copy constructor (used by BelongsToMany::clone()). */
        inline Relation(const Relation &) = default;

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

        /*! Set the constraints for an eager load of the relation. */
        virtual void addEagerConstraints(const QVector<Model> &models) const = 0;
        /*! Initialize the relation on a set of models. */
        virtual QVector<Model> &
        initRelation(QVector<Model> &models, const QString &relation) const = 0;
        /*! Match the eagerly loaded results to their parents. */
        virtual void match(QVector<Model> &models, QVector<Related> &&results,
                           const QString &relation) const = 0;
        /*! Get the results of the relationship. */
        virtual std::variant<QVector<Related>, std::optional<Related>>
        getResults() const = 0;

        /*! Get the relationship for eager loading. */
        inline QVector<Related> getEager() const;
        /*! Execute the query as a "select" statement. */
        inline virtual QVector<Related>
        get(const QVector<Column> &columns = {ASTERISK}) const;

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
        inline std::tuple<int, QSqlQuery>
        rawUpdate(const QVector<UpdateItem> &values = {}) const;

        /*! The textual representation of the Relation type. */
        virtual const QString &relationTypeName() const = 0;

    protected:
        /* Relation related operations */
        /*! Initialize a Relation instance. */
        inline void init() const;

        /*! Get all of the primary keys for the vector of models. */
        QVector<QVariant>
        getKeys(const QVector<Model> &models, const QString &key = "") const;

        /* Querying Relationship Existence/Absence */
        /*! Add the constraints for an internal relationship existence query.
            Essentially, these queries compare on column names like whereColumn. */
        virtual std::unique_ptr<Builder<Related>>
        getRelationExistenceQuery(
                std::unique_ptr<Builder<Related>> &&query,
                const Builder<Model> &parentQuery,
                const QVector<Column> &columns = {ASTERISK}) const;
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
        Model &m_parent;
        /*! The related model instance. */
        std::shared_ptr<Related> m_related;
        /*! The key name of the related model. */
        QString m_relatedKey;
        // TODO next would be good to use TinyBuilder alias instead of Builder silverqx
        /*! The TinyORM TinyBuilder instance. */
        std::shared_ptr<Builder<Related>> m_query;
        /*! Indicates if the relation is adding constraints. */
        T_THREAD_LOCAL
        inline static bool constraints = true;
    };

    /* protected */

    template<class Model, class Related>
    Relation<Model, Related>::Relation(std::unique_ptr<Related> &&related, Model &parent,
                                       const QString &relatedKey)
        : m_parent(parent)
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
    QVector<Related>
    Relation<Model, Related>::getEager() const
    {
        return get();
    }

    template<class Model, class Related>
    QVector<Related>
    Relation<Model, Related>::get(const QVector<Column> &columns) const
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
        return m_parent;
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
        return m_parent.getCreatedAtColumn();
    }

    template<class Model, class Related>
    const QString &Relation<Model, Related>::updatedAt() const
    {
        return m_parent.getUpdatedAtColumn();
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
        return m_parent.getQualifiedKeyName();
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
    std::tuple<int, QSqlQuery>
    Relation<Model, Related>::rawUpdate(const QVector<UpdateItem> &values) const
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
    QVector<QVariant>
    Relation<Model, Related>::getKeys(const QVector<Model> &models,
                                      const QString &key) const
    {
        QVector<QVariant> keys;
        keys.reserve(models.size());

        for (const auto &model : models)
            keys << key.isEmpty() ? model.getKey()
                                  : model.getAttribute(key);

        return keys |= ranges::actions::sort(ranges::less {}, [](const auto &key_)
        {
            return key_.template value<typename Model::KeyType>();
        })
                | ranges::actions::unique;
    }

    /* Querying Relationship Existence/Absence */

    template<class Model, class Related>
    std::unique_ptr<Builder<Related>>
    Relation<Model, Related>::getRelationExistenceQuery(
            std::unique_ptr<Builder<Related>> &&query, const Builder<Model> &/*unused*/,
            const QVector<Column> &columns) const
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
