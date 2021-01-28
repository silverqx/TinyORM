#ifndef RELATION_H
#define RELATION_H

#include <QtSql/QSqlQuery>

#include "orm/ormtypes.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny
{
    template<class Model>
    class Builder;

    template<class Model, typename ...AllRelations>
    class BaseModel;

namespace Relations
{

    template<class Model, class Related>
    class Relation
    {
    protected:
        Relation(std::unique_ptr<Related> &&related, const Model &parent);

    public:
        inline virtual ~Relation() = default;

        /*! Set the base constraints on the relation query. */
        virtual void addConstraints() const = 0;

        /*! Run a callback with constraints disabled on the relation. */
        static std::unique_ptr<Relation<Model, Related>>
        noConstraints(const std::function<std::unique_ptr<Relation<Model, Related>>()> &callback);

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
        inline QVector<Related> getEager() const
        { return get(); }
        /*! Execute the query as a "select" statement. */
        inline QVector<Related> get(const QStringList columns = {"*"}) const
        { return m_query->get(columns); }

        /*! Get the underlying query for the relation. */
        inline Builder<Related> &getQuery() const
        { return *m_query; }
        /*! Get the base query builder driving the Eloquent builder. */
        inline QueryBuilder &getBaseQuery() const
        { return m_query->getQuery(); }

        /*! Get the parent model of the relation. */
        const Model &getParent() const
        { return m_parent; }
        /*! Get the related model of the relation. */
        const Related &getRelated() const
        { return *m_related; }
        /*! Get the name of the "created at" column. */
        const QString &createdAt() const
        { return m_parent.getCreatedAtColumn(); }
        /*! Get the name of the "updated at" column. */
        const QString &updatedAt() const
        { return m_parent.getUpdatedAtColumn(); }
        /*! Get the name of the related model's "updated at" column. */
        const QString &relatedUpdatedAt() const
        { return m_related->getUpdatedAtColumn(); }

        /*! Touch all of the related models for the relationship. */
        void touch() const;
        /*! Run a raw update against the base query. */
        std::tuple<int, QSqlQuery>
        rawUpdate(const QVector<UpdateItem> &values = {}) const;

    protected:
        /*! Initialize a Relation instance. */
        void init() const
        { addConstraints(); }

        /* Much safer to make a copy here than save references, original objects get
           out of scope, because they are defined in member function blocks.
           This is true for all constructor parameters counting ctor parameters in
           derived classes, like m_parent, m_child, m_related, m_ownerKey,
           m_foreignKey, ... */
        /*! The parent model instance. */
        const Model m_parent;
        /*! The related model instance. */
        const std::unique_ptr<Related> m_related;
        // TODO next 👆👇 the same for m_related silverqx
        // TODO next reconsider unique_ptr here, shared pointer would be good to? I hit this when I implemented getBaseQuery(), or leave this unique and return shared pointer from getBaseQuery() silverqx
        /*! The Eloquent query builder instance. */
        std::unique_ptr<Builder<Related>> m_query;
        /*! Indicates if the relation is adding constraints. */
        static bool constraints;
    };

    class OneRelation
    {};

    class ManyRelation
    {};

    template<class Model, class Related>
    bool Relation<Model, Related>::constraints = true;

    template<class Model, class Related>
    Relation<Model, Related>::Relation(std::unique_ptr<Related> &&related,
                                       const Model &parent)
        : m_parent(parent)
        , m_related(std::move(related))
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
    void Relation<Model, Related>::touch() const
    {
        const auto &model = getRelated();

        if (!model.isIgnoringTouch())
            rawUpdate({
                {model.getUpdatedAtColumn(), model.freshTimestampString()}
            });
    }

    template<class Model, class Related>
    std::tuple<int, QSqlQuery>
    Relation<Model, Related>::rawUpdate(const QVector<UpdateItem> &values) const
    {
        return m_query->update(values);
    }

} // namespace Orm::Tiny::Relations
} // namespace Orm::Tiny
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // RELATION_H
