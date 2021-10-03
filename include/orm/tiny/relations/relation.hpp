#pragma once
#ifndef RELATION_HPP
#define RELATION_HPP

#include <QtSql/QSqlQuery>

#include <optional>

#include <range/v3/action/sort.hpp>
#include <range/v3/action/unique.hpp>

#include "orm/exceptions/runtimeerror.hpp"
#include "orm/tiny/relations/relationproxies.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny
{

    template<typename Derived, AllRelationsConcept ...AllRelations>
    class Model;

namespace Concerns
{
    template<typename Model>
    class QueriesRelationships;
}

namespace Relations
{

    /*! The tag for one type relation. */
    class OneRelation
    {
    public:
        /*! Pure virtual destructor. */
        virtual ~OneRelation() = 0;
    };

    /*! The tag for many type relation. */
    class ManyRelation
    {
    public:
        /*! Pure virtual destructor. */
        virtual ~ManyRelation() = 0;
    };

    /*! The tag for the relation which contains pivot table, like many-to-many. */
    class PivotRelation
    {
    public:
        /*! Pure virtual destructor. */
        virtual ~PivotRelation() = 0;
    };

    /*! Base relations class. */
    template<class Model, class Related>
    class Relation : public RelationProxies<Model, Related>
    {
        // Used by QueriesRelationships::getHasQueryByExistenceCheck()
        friend Orm::Tiny::Concerns::QueriesRelationships<Model>;

        /*! Expression alias. */
        using Expression = Orm::Query::Expression;

    protected:
        /*! Protected constructor. */
        Relation(std::unique_ptr<Related> &&related, Model &parent,
                 const QString &relatedKey = "");

    public:
        /*! Related instance type passed to the relation. */
        using RelatedType = Related;

        /*! Pure virtual destructor. */
        virtual ~Relation() = 0;

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
        virtual void match(QVector<Model> &models, QVector<Related> results,
                           const QString &relation) const = 0;
        /*! Get the results of the relationship. */
        virtual std::variant<QVector<Related>, std::optional<Related>>
        getResults() const = 0;

        /*! Get the relationship for eager loading. */
        QVector<Related> getEager() const;
        /*! Execute the query as a "select" statement. */
        virtual QVector<Related>
        get(const QVector<Column> &columns = {ASTERISK}) const;

        /* Getters / Setters */
        /*! Get the underlying query for the relation. */
        Builder<Related> &getQuery() const;
        /*! Get the base QueryBuilder driving the TinyBuilder. */
        QueryBuilder &getBaseQuery() const;

        /*! Get the parent model of the relation. */
        const Model &getParent() const;
        /*! Get the related model of the relation. */
        const Related &getRelated() const;
        /*! Get the related model of the relation. */
        Related &getRelated();
        /*! Get the name of the "created at" column. */
        const QString &createdAt() const;
        /*! Get the name of the "updated at" column. */
        const QString &updatedAt() const;
        /*! Get the name of the related model's "updated at" column. */
        const QString &relatedUpdatedAt() const;
        /*! Get the related key for the relationship. */
        const QString &getRelatedKeyName() const;
        /*! Get the fully qualified parent key name. */
        virtual QString getQualifiedParentKeyName() const;
        /*! Get the key for comparing against the parent key in "has" query. */
        virtual QString getExistenceCompareKey() const;

        /* Others */
        /*! Touch all of the related models for the relationship. */
        virtual void touch() const;
        /*! Run a raw update against the base query. */
        std::tuple<int, QSqlQuery>
        rawUpdate(const QVector<UpdateItem> &values = {}) const;

        /*! The textual representation of the Relation type. */
        virtual QString relationTypeName() const = 0;

    protected:
        /*! Initialize a Relation instance. */
        void init() const;

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
        const std::unique_ptr<Related> m_related;
        /*! The key name of the related model. */
        QString m_relatedKey;
        // TODO next would be good to use TinyBuilder alias instead of Builder silverqx
        /*! The TinyORM TinyBuilder instance. */
        std::unique_ptr<Builder<Related>> m_query;
        /*! Indicates if the relation is adding constraints. */
        static bool constraints;
    };

    inline OneRelation::~OneRelation() = default;

    inline ManyRelation::~ManyRelation() = default;

    inline PivotRelation::~PivotRelation() = default;

    template<class Model, class Related>
    bool Relation<Model, Related>::constraints = true;

    template<class Model, class Related>
    inline Relation<Model, Related>::~Relation() = default;

    template<class Model, class Related>
    Relation<Model, Related>::Relation(std::unique_ptr<Related> &&related, Model &parent,
                                       const QString &relatedKey)
        : m_parent(parent)
        , m_related(std::move(related))
        , m_relatedKey(relatedKey.isEmpty() ? m_related->getKeyName() : relatedKey)
        , m_query(m_related->newQuery())
    {}

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
    inline QVector<Related>
    Relation<Model, Related>::getEager() const
    {
        return get();
    }

    template<class Model, class Related>
    inline QVector<Related>
    Relation<Model, Related>::get(const QVector<Column> &columns) const
    {
        return m_query->get(columns);
    }

    template<class Model, class Related>
    inline Builder<Related> &Relation<Model, Related>::getQuery() const
    {
        return *m_query;
    }

    template<class Model, class Related>
    inline QueryBuilder &Relation<Model, Related>::getBaseQuery() const
    {
        return m_query->getQuery();
    }

    template<class Model, class Related>
    inline const Model &Relation<Model, Related>::getParent() const
    {
        return m_parent;
    }

    template<class Model, class Related>
    inline const Related &Relation<Model, Related>::getRelated() const
    {
        return *m_related;
    }

    template<class Model, class Related>
    inline Related &Relation<Model, Related>::getRelated()
    {
        return *m_related;
    }

    template<class Model, class Related>
    inline const QString &Relation<Model, Related>::createdAt() const
    {
        return m_parent.getCreatedAtColumn();
    }

    template<class Model, class Related>
    inline const QString &Relation<Model, Related>::updatedAt() const
    {
        return m_parent.getUpdatedAtColumn();
    }

    template<class Model, class Related>
    inline const QString &Relation<Model, Related>::relatedUpdatedAt() const
    {
        return m_related->getUpdatedAtColumn();
    }

    template<class Model, class Related>
    inline const QString &Relation<Model, Related>::getRelatedKeyName() const
    {
        return m_relatedKey;
    }

    template<class Model, class Related>
    inline QString Relation<Model, Related>::getQualifiedParentKeyName() const
    {
        return m_parent.getQualifiedKeyName();
    }

    template<class Model, class Related>
    inline QString Relation<Model, Related>::getExistenceCompareKey() const
    {
        throw Orm::Exceptions::RuntimeError(
                    QStringLiteral("Method %1() is not implemented for '%2' "
                                   "relation type.")
                    .arg(__func__, relationTypeName()));
    }

    template<class Model, class Related>
    void Relation<Model, Related>::touch() const
    {
        const auto &model = getRelated();

        if (!model.isIgnoringTouch())
            rawUpdate({
                {model.getUpdatedAtColumn(), model.freshTimestampString()}
            });
    }

    template<class Model, class Related>
    inline std::tuple<int, QSqlQuery>
    Relation<Model, Related>::rawUpdate(const QVector<UpdateItem> &values) const
    {
        // FEATURE scopes silverqx
        return m_query->update(values);
    }

    template<class Model, class Related>
    inline void Relation<Model, Related>::init() const
    {
        addConstraints();
    }

    template<class Model, class Related>
    QVector<QVariant>
    Relation<Model, Related>::getKeys(const QVector<Model> &models,
                                      const QString &key) const
    {
        QVector<QVariant> keys;

        for (const auto &model : models)
            keys.append(key.isEmpty() ? model.getKey()
                                      : model.getAttribute(key));

        using namespace ranges;
        return keys |= actions::sort(less {}, [](auto key_)
        {
            return key_.template value<typename Model::KeyType>();
        })
                       | actions::unique;
    }

    template<class Model, class Related>
    std::unique_ptr<Builder<Related>>
    Relation<Model, Related>::getRelationExistenceQuery(
            std::unique_ptr<Builder<Related>> &&query, const Builder<Model> &,
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
                                          {Expression("count(*)")});

        query->getQuery().setBindings({}, BindingType::SELECT);

        return std::move(query);
    }

} // namespace Orm::Tiny::Relations
} // namespace Orm::Tiny
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // RELATION_HPP
