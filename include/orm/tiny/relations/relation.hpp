#ifndef RELATION_H
#define RELATION_H

#include <QtSql/QSqlQuery>

#include <range/v3/action/sort.hpp>
#include <range/v3/action/unique.hpp>

#include "orm/ormtypes.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{
namespace Query
{
    class JoinClause;
}
namespace Tiny
{
    template<class Model>
    class Builder;

    template<class Model, typename ...AllRelations>
    class BaseModel;

namespace Relations
{

    using JoinClause = Orm::Query::JoinClause;

    // TODO next proxy all TinyBuilder methods aaaa 😑 silverqx
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
        inline QVector<Related> getEager() const
        { return get(); }
        /*! Execute the query as a "select" statement. */
        virtual inline QVector<Related> get(const QStringList &columns = {"*"}) const
        { return m_query->get(columns); }

        /* Getters / Setters */
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

        /* Others */
        /*! Touch all of the related models for the relationship. */
        void touch() const;
        /*! Run a raw update against the base query. */
        std::tuple<int, QSqlQuery>
        rawUpdate(const QVector<UpdateItem> &values = {}) const;

        /* TinyBuilder proxy methods */
        std::optional<Model>
        find(const QVariant &id, const QStringList &columns = {"*"});
        /*! Find a model by its primary key or return fresh model instance. */
        Model findOrNew(const QVariant &id, const QStringList &columns = {"*"});
        /*! Find a model by its primary key or throw an exception. */
        Model findOrFail(const QVariant &id, const QStringList &columns = {"*"});

        /*! Get the first record matching the attributes or instantiate it. */
        Model firstOrNew(const QVector<WhereItem> &attributes = {},
                         const QVector<AttributeItem> &values = {});
        /*! Get the first record matching the attributes or create it. */
        Model firstOrCreate(const QVector<WhereItem> &attributes = {},
                            const QVector<AttributeItem> &values = {});
        /*! Execute the query and get the first result or throw an exception. */
        Model firstOrFail(const QStringList &columns = {"*"});

        /*! Add a basic where clause to the query, and return the first result. */
        std::optional<Model>
        firstWhere(const QString &column, const QString &comparison,
                   const QVariant &value, const QString &condition = "and");

        /*! Get a single column's value from the first result of a query. */
        QVariant value(const QString &column);

        /*! Set the relationships that should be eager loaded. */
        Builder<Related> &with(const QVector<WithItem> &relations);
        /*! Set the relationships that should be eager loaded. */
        Builder<Related> &with(const QString &relation);
        /*! Prevent the specified relations from being eager loaded. */
        Builder<Related> &without(const QVector<QString> &relations);
        /*! Prevent the specified relations from being eager loaded. */
        Builder<Related> &without(const QString &relation);

        /*! Save a new model and return the instance. */
        Model create(const QVector<AttributeItem> &attributes);

        /* Proxies to TinyBuilder -> BuildsQueries */
        /*! Execute the query and get the first result. */
        std::optional<Model> first(const QStringList &columns = {"*"});

        /* Proxies to TinyBuilder -> QueryBuilder */
        /* Insert, Update, Delete */
        /*! Insert new records into the database. */
        std::tuple<bool, std::optional<QSqlQuery>>
        insert(const QVector<AttributeItem> &attributes) const;
        /*! Insert a new record and get the value of the primary key. */
        quint64 insertGetId(const QVector<AttributeItem> &attributes) const;

        /*! Update records in the database. */
        std::tuple<int, QSqlQuery>
        update(const QVector<UpdateItem> &values) const;

        /*! Delete records from the database. */
        std::tuple<int, QSqlQuery> remove();
        /*! Delete records from the database. */
        std::tuple<int, QSqlQuery> deleteModels();

        /*! Run a truncate statement on the table. */
        std::tuple<bool, QSqlQuery> truncate();

        /* Select */
        /*! Set the columns to be selected. */
        Builder<Related> &select(const QStringList columns = {"*"});
        /*! Set the column to be selected. */
        Builder<Related> &select(const QString column);
        /*! Add new select columns to the query. */
        Builder<Related> &addSelect(const QStringList &columns);
        /*! Add a new select column to the query. */
        Builder<Related> &addSelect(const QString &column);

        /*! Force the query to only return distinct results. */
        Builder<Related> &distinct();

        /*! Add a join clause to the query. */
        Builder<Related> &join(const QString &table, const QString &first,
                               const QString &comparison, const QString &second,
                               const QString &type = "inner", bool where = false);
        /*! Add an advanced join clause to the query. */
        Builder<Related> &join(const QString &table,
                               const std::function<void(JoinClause &)> &callback,
                               const QString &type = "inner");
        /*! Add a "join where" clause to the query. */
        Builder<Related> &joinWhere(const QString &table, const QString &first,
                                    const QString &comparison, const QString &second,
                                    const QString &type = "inner");
        /*! Add a left join to the query. */
        Builder<Related> &leftJoin(const QString &table, const QString &first,
                                   const QString &comparison, const QString &second);
        /*! Add an advanced left join to the query. */
        Builder<Related> &leftJoin(const QString &table,
                                   const std::function<void(JoinClause &)> &callback);
        /*! Add a "join where" clause to the query. */
        Builder<Related> &leftJoinWhere(const QString &table, const QString &first,
                                        const QString &comparison,
                                        const QString &second);
        /*! Add a right join to the query. */
        Builder<Related> &rightJoin(const QString &table, const QString &first,
                                    const QString &comparison, const QString &second);
        /*! Add an advanced right join to the query. */
        Builder<Related> &rightJoin(const QString &table,
                                    const std::function<void(JoinClause &)> &callback);
        /*! Add a "right join where" clause to the query. */
        Builder<Related> &rightJoinWhere(const QString &table, const QString &first,
                                         const QString &comparison,
                                         const QString &second);
        /*! Add a "cross join" clause to the query. */
        Builder<Related> &crossJoin(const QString &table, const QString &first,
                                    const QString &comparison, const QString &second);
        /*! Add an advanced "cross join" clause to the query. */
        Builder<Related> &crossJoin(const QString &table,
                                    const std::function<void(JoinClause &)> &callback);

        /*! Add a basic where clause to the query. */
        Builder<Related> &where(const QString &column, const QString &comparison,
                                const QVariant &value, const QString &condition = "and");
        /*! Add an "or where" clause to the query. */
        Builder<Related> &orWhere(const QString &column, const QString &comparison,
                                  const QVariant &value);
        /*! Add a basic equal where clause to the query. */
        Builder<Related> &whereEq(const QString &column, const QVariant &value,
                                  const QString &condition = "and");
        /*! Add an equal "or where" clause to the query. */
        Builder<Related> &orWhereEq(const QString &column, const QVariant &value);
        /*! Add a nested where clause to the query. */
        Builder<Related> &where(const std::function<void(Builder<Related> &)> &callback,
                                const QString &condition = "and");
        /*! Add a nested "or where" clause to the query. */
        Builder<Related> &orWhere(
                const std::function<void(Builder<Related> &)> &callback);

        /*! Add an array of basic where clauses to the query. */
        Builder<Related> &where(const QVector<WhereItem> &values,
                                const QString &condition = "and");
        /*! Add an array of basic "or where" clauses to the query. */
        Builder<Related> &orWhere(const QVector<WhereItem> &values);

        /*! Add an array of where clauses comparing two columns to the query. */
        Builder<Related> &whereColumn(const QVector<WhereColumnItem> &values,
                                      const QString &condition = "and");
        /*! Add an array of "or where" clauses comparing two columns to the query. */
        Builder<Related> &orWhereColumn(const QVector<WhereColumnItem> &values);

        /*! Add a "where" clause comparing two columns to the query. */
        Builder<Related> &whereColumn(const QString &first, const QString &comparison,
                                      const QString &second,
                                      const QString &condition = "and");
        /*! Add a "or where" clause comparing two columns to the query. */
        Builder<Related> &orWhereColumn(const QString &first, const QString &comparison,
                                        const QString &second);
        /*! Add an equal "where" clause comparing two columns to the query. */
        Builder<Related> &whereColumnEq(const QString &first, const QString &second,
                                        const QString &condition = "and");
        /*! Add an equal "or where" clause comparing two columns to the query. */
        Builder<Related> &orWhereColumnEq(const QString &first, const QString &second);

        /*! Add a "where in" clause to the query. */
        Builder<Related> &whereIn(const QString &column, const QVector<QVariant> &values,
                                  const QString &condition = "and", bool nope = false);
        /*! Add an "or where in" clause to the query. */
        Builder<Related> &orWhereIn(const QString &column,
                                    const QVector<QVariant> &values);
        /*! Add a "where not in" clause to the query. */
        Builder<Related> &whereNotIn(const QString &column,
                                     const QVector<QVariant> &values,
                                     const QString &condition = "and");
        /*! Add an "or where not in" clause to the query. */
        Builder<Related> &orWhereNotIn(const QString &column,
                                       const QVector<QVariant> &values);

        /*! Add a "where null" clause to the query. */
        Builder<Related> &whereNull(const QStringList &columns = {"*"},
                                    const QString &condition = "and",
                                    bool nope = false);
        /*! Add a "where null" clause to the query. */
        Builder<Related> &whereNull(const QString &column,
                                    const QString &condition = "and",
                                    bool nope = false);
        /*! Add an "or where null" clause to the query. */
        Builder<Related> &orWhereNull(const QStringList &columns = {"*"});
        /*! Add an "or where null" clause to the query. */
        Builder<Related> &orWhereNull(const QString &column);
        /*! Add a "where not null" clause to the query. */
        Builder<Related> &whereNotNull(const QStringList &columns = {"*"},
                                       const QString &condition = "and");
        /*! Add a "where not null" clause to the query. */
        Builder<Related> &whereNotNull(const QString &column,
                                       const QString &condition = "and");
        /*! Add an "or where not null" clause to the query. */
        Builder<Related> &orWhereNotNull(const QStringList &columns = {"*"});
        /*! Add an "or where not null" clause to the query. */
        Builder<Related> &orWhereNotNull(const QString &column);

        /*! Add a "group by" clause to the query. */
        Builder<Related> &groupBy(const QStringList &groups);
        /*! Add a "group by" clause to the query. */
        Builder<Related> &groupBy(const QString &group);

        /*! Add a "having" clause to the query. */
        Builder<Related> &having(const QString &column, const QString &comparison,
                                 const QVariant &value,
                                 const QString &condition = "and");
        /*! Add an "or having" clause to the query. */
        Builder<Related> &orHaving(const QString &column, const QString &comparison,
                                   const QVariant &value);

        /*! Add an "order by" clause to the query. */
        Builder<Related> &orderBy(const QString &column,
                                  const QString &direction = "asc");
        /*! Add a descending "order by" clause to the query. */
        Builder<Related> &orderByDesc(const QString &column);
        /*! Add an "order by" clause for a timestamp to the query. */
        Builder<Related> &latest(const QString &column = "");
        /*! Add an "order by" clause for a timestamp to the query. */
        Builder<Related> &oldest(const QString &column = "");
        /*! Remove all existing orders. */
        Builder<Related> &reorder();
        /*! Remove all existing orders and optionally add a new order. */
        Builder<Related> &reorder(const QString &column,
                                  const QString &direction = "asc");

        /*! Set the "limit" value of the query. */
        Builder<Related> &limit(int value);
        /*! Alias to set the "limit" value of the query. */
        Builder<Related> &take(int value);
        /*! Set the "offset" value of the query. */
        Builder<Related> &offset(int value);
        /*! Alias to set the "offset" value of the query. */
        Builder<Related> &skip(int value);
        /*! Set the limit and offset for a given page. */
        Builder<Related> &forPage(int page, int perPage = 30);

        /*! Increment a column's value by a given amount. */
        template<typename T> requires std::is_arithmetic_v<T>
        std::tuple<int, QSqlQuery>
        increment(const QString &column, T amount = 1,
                  const QVector<UpdateItem> &extra = {});
        /*! Decrement a column's value by a given amount. */
        template<typename T> requires std::is_arithmetic_v<T>
        std::tuple<int, QSqlQuery>
        decrement(const QString &column, T amount = 1,
                  const QVector<UpdateItem> &extra = {});

        /*! Add a where clause on the primary key to the query. */
        Builder<Related> &whereKey(const QVariant &id);
        /*! Add a where clause on the primary key to the query. */
        Builder<Related> &whereKeyNot(const QVariant &id);

    protected:
        /*! Initialize a Relation instance. */
        inline void init() const
        { addConstraints(); }

        /*! Get all of the primary keys for an array of models. */
        QVector<QVariant>
        getKeys(const QVector<Model> &models, const QString &key = "") const;

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
        // TODO next would be good to use TinyBuilder alias instead of Builder silverqx
        /*! The Eloquent query builder instance. */
        std::unique_ptr<Builder<Related>> m_query;
        /*! Indicates if the relation is adding constraints. */
        static bool constraints;
    };

    /*! The tag for one type relation. */
    class OneRelation
    {};

    /*! The tag for many type relation. */
    class ManyRelation
    {};

    /*! The tag for the relation which contains pivot table, like many-to-many. */
    class PivotRelation
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

    template<class Model, class Related>
    std::optional<Model>
    Relation<Model, Related>::find(const QVariant &id, const QStringList &columns)
    {
        return m_query->find(id, columns);
    }

    template<class Model, class Related>
    Model
    Relation<Model, Related>::findOrNew(const QVariant &id, const QStringList &columns)
    {
        return m_query->findOrNew(id, columns);
    }

    template<class Model, class Related>
    Model
    Relation<Model, Related>::findOrFail(const QVariant &id, const QStringList &columns)
    {
        return m_query->findOrFail(id, columns);
    }

    template<class Model, class Related>
    Model
    Relation<Model, Related>::firstOrNew(const QVector<WhereItem> &attributes,
                                         const QVector<AttributeItem> &values)
    {
        return m_query->firstOrNew(attributes, values);
    }

    template<class Model, class Related>
    Model
    Relation<Model, Related>::firstOrCreate(const QVector<WhereItem> &attributes,
                                            const QVector<AttributeItem> &values)
    {
        return m_query->firstOrCreate(attributes, values);
    }

    template<class Model, class Related>
    Model Relation<Model, Related>::firstOrFail(const QStringList &columns)
    {
        return m_query->firstOrFail(columns);
    }

    template<class Model, class Related>
    std::optional<Model>
    Relation<Model, Related>::firstWhere(const QString &column, const QString &comparison,
                                         const QVariant &value, const QString &condition)
    {
        return m_query->firstWhere(column, comparison, value, condition);
    }

    template<class Model, class Related>
    QVariant Relation<Model, Related>::value(const QString &column)
    {
        return m_query->value(column);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::with(const QVector<WithItem> &relations)
    {
        return m_query->with(relations);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::with(const QString &relation)
    {
        return m_query->with(relation);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::without(const QVector<QString> &relations)
    {
        return m_query->without(relations);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::without(const QString &relation)
    {
        return m_query->without(relation);
    }

    template<class Model, class Related>
    Model
    Relation<Model, Related>::create(const QVector<AttributeItem> &attributes)
    {
        return m_query->create(attributes);
    }

    template<class Model, class Related>
    std::optional<Model>
    Relation<Model, Related>::first(const QStringList &columns)
    {
        return m_query->first(columns);
    }

    template<class Model, class Related>
    std::tuple<bool, std::optional<QSqlQuery>>
    Relation<Model, Related>::insert(const QVector<AttributeItem> &attributes) const
    {
        return m_query->insert(attributes);
    }

    // TODO dilemma primarykey, Model::KeyType vs QVariant silverqx
    template<class Model, class Related>
    quint64
    Relation<Model, Related>::insertGetId(const QVector<AttributeItem> &attributes) const
    {
        return m_query->insertGetId(attributes);
    }

    template<class Model, class Related>
    std::tuple<int, QSqlQuery>
    Relation<Model, Related>::update(const QVector<UpdateItem> &values) const
    {
        return m_query->update(values);
    }

    template<class Model, class Related>
    std::tuple<int, QSqlQuery>
    Relation<Model, Related>::remove()
    {
        return m_query->remove();
    }

    template<class Model, class Related>
    std::tuple<int, QSqlQuery>
    Relation<Model, Related>::deleteModels()
    {
        return m_query->deleteModels();
    }

    template<class Model, class Related>
    std::tuple<bool, QSqlQuery>
    Relation<Model, Related>::truncate()
    {
        return m_query->truncate();
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::select(const QStringList columns)
    {
        return m_query->select(columns);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::select(const QString column)
    {
        return m_query->select(column);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::addSelect(const QStringList &columns)
    {
        return m_query->addSelect(columns);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::addSelect(const QString &column)
    {
        return m_query->addSelect(column);
    }

    template<class Model, class Related>
    Builder<Related> &Relation<Model, Related>::distinct()
    {
        return m_query->distinct();
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::join(const QString &table, const QString &first,
                                   const QString &comparison, const QString &second,
                                   const QString &type, const bool where)
    {
        return m_query->join(table, first, comparison, second, type, where);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::join(const QString &table,
                                   const std::function<void (JoinClause &)> &callback,
                                   const QString &type)
    {
        return m_query->join(table, callback, type);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::joinWhere(const QString &table, const QString &first,
                                        const QString &comparison, const QString &second,
                                        const QString &type)
    {
        return m_query->joinWhere(table, first, comparison, second, type);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::leftJoin(const QString &table, const QString &first,
                                       const QString &comparison, const QString &second)
    {
        return m_query->leftJoin(table, first, comparison, second);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::leftJoin(
            const QString &table,
            const std::function<void (JoinClause &)> &callback)
    {
        return m_query->leftJoin(table, callback);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::leftJoinWhere(
            const QString &table, const QString &first,
            const QString &comparison, const QString &second)
    {
        return m_query->leftJoinWhere(table, first, comparison, second);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::rightJoin(const QString &table, const QString &first,
                                        const QString &comparison, const QString &second)
    {
        return m_query->rightJoin(table, first, comparison, second);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::rightJoin(
            const QString &table,
            const std::function<void (JoinClause &)> &callback)
    {
        return m_query->rightJoin(table, callback);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::rightJoinWhere(
            const QString &table, const QString &first,
            const QString &comparison, const QString &second)
    {
        return m_query->rightJoinWhere(table, first, comparison, second);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::crossJoin(const QString &table, const QString &first,
                                        const QString &comparison, const QString &second)
    {
        return m_query->crossJoin(table, first, comparison, second);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::crossJoin(
            const QString &table,
            const std::function<void (JoinClause &)> &callback)
    {
        return m_query->crossJoin(table, callback);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::where(const QString &column, const QString &comparison,
                                    const QVariant &value, const QString &condition)
    {
        return m_query->where(column, comparison, value, condition);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::orWhere(const QString &column, const QString &comparison,
                                      const QVariant &value)
    {
        return m_query->orWhere(column, comparison, value);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::whereEq(const QString &column, const QVariant &value,
                                      const QString &condition)
    {
        return m_query->whereEq(column, value, condition);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::orWhereEq(const QString &column, const QVariant &value)
    {
        return m_query->orWhereEq(column, value);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::where(
            const std::function<void(Builder<Related> &)> &callback,
            const QString &condition)
    {
        return m_query->where(callback, condition);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::orWhere(
            const std::function<void(Builder<Related> &)> &callback)
    {
        return m_query->orWhere(callback);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::where(const QVector<WhereItem> &values,
                                    const QString &condition)
    {
        return m_query->where(values, condition);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::orWhere(const QVector<WhereItem> &values)
    {
        return m_query->orWhere(values);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::whereColumn(
            const QVector<WhereColumnItem> &values, const QString &condition)
    {
        return m_query->whereColumn(values, condition);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::orWhereColumn(const QVector<WhereColumnItem> &values)
    {
        return m_query->orWhereColumn(values);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::whereColumn(
            const QString &first, const QString &comparison,
            const QString &second, const QString &condition)
    {
        return m_query->whereColumn(first, comparison, second, condition);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::orWhereColumn(
            const QString &first, const QString &comparison, const QString &second)
    {
        return m_query->orWhereColumn(first, comparison, second);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::whereColumnEq(
            const QString &first, const QString &second, const QString &condition)
    {
        return m_query->whereColumnEq(first, second, condition);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::orWhereColumnEq(const QString &first, const QString &second)
    {
        return m_query->orWhereColumnEq(first, second);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::whereIn(
            const QString &column, const QVector<QVariant> &values,
            const QString &condition, const bool nope)
    {
        return m_query->whereIn(column, values, condition, nope);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::orWhereIn(const QString &column,
                                        const QVector<QVariant> &values)
    {
        return m_query->orWhereIn(column, values);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::whereNotIn(
            const QString &column, const QVector<QVariant> &values,
            const QString &condition)
    {
        return m_query->whereNotIn(column, values, condition);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::orWhereNotIn(const QString &column,
                                           const QVector<QVariant> &values)
    {
        return m_query->orWhereNotIn(column, values);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::whereNull(
            const QStringList &columns, const QString &condition, const bool nope)
    {
        return m_query->whereNull(columns, condition, nope);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::whereNull(
            const QString &column, const QString &condition, const bool nope)
    {
        return m_query->whereNull(column, condition, nope);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::orWhereNull(const QStringList &columns)
    {
        return m_query->orWhereNull(columns);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::orWhereNull(const QString &column)
    {
        return m_query->orWhereNull(column);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::whereNotNull(const QStringList &columns,
                                           const QString &condition)
    {
        return m_query->whereNotNull(columns, condition);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::whereNotNull(const QString &column,
                                           const QString &condition)
    {
        return m_query->whereNotNull(column, condition);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::orWhereNotNull(const QStringList &columns)
    {
        return m_query->orWhereNotNull(columns);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::orWhereNotNull(const QString &column)
    {
        return m_query->orWhereNotNull(column);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::groupBy(const QStringList &groups)
    {
        return m_query->groupBy(groups);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::groupBy(const QString &group)
    {
        return m_query->groupBy(group);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::having(const QString &column, const QString &comparison,
                                     const QVariant &value, const QString &condition)
    {
        return m_query->having(column, comparison, value, condition);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::orHaving(const QString &column, const QString &comparison,
                                       const QVariant &value)
    {
        return m_query->orHaving(column, comparison, value);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::orderBy(const QString &column, const QString &direction)
    {
        return m_query->orderBy(column, direction);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::orderByDesc(const QString &column)
    {
        return m_query->orderByDesc(column);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::latest(const QString &column)
    {
        return m_query->latest(column);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::oldest(const QString &column)
    {
        return m_query->oldest(column);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::reorder()
    {
        return m_query->reorder();
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::reorder(const QString &column, const QString &direction)
    {
        return m_query->reorder(column, direction);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::limit(const int value)
    {
        return m_query->limit(value);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::take(const int value)
    {
        return m_query->take(value);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::offset(const int value)
    {
        return m_query->offset(value);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::skip(const int value)
    {
        return m_query->skip(value);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::forPage(const int page, const int perPage)
    {
        return m_query->forPage(page, perPage);
    }

    template<class Model, class Related>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, QSqlQuery>
    Relation<Model, Related>::increment(const QString &column, const T amount,
                                        const QVector<UpdateItem> &extra)
    {
        return m_query->increment(column, amount, extra);
    }

    template<class Model, class Related>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, QSqlQuery>
    Relation<Model, Related>::decrement(const QString &column, const T amount,
                                        const QVector<UpdateItem> &extra)
    {
        return m_query->decrement(column, amount, extra);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::whereKey(const QVariant &id)
    {
        return m_query->whereKey(id);
    }

    template<class Model, class Related>
    Builder<Related> &
    Relation<Model, Related>::whereKeyNot(const QVariant &id)
    {
        return m_query->whereKeyNot(id);
    }

    template<class Model, class Related>
    QVector<QVariant>
    Relation<Model, Related>::getKeys(const QVector<Model> &models,
                                      const QString &key) const
    {
        QVector<QVariant> keys;

        // BUG what if key is empty/null/invalid, looks orig. implementation, there is used "->values()->unique(null, true)" silverqx
        for (const auto &model : models)
            keys.append(key.isEmpty() ? model.getKey()
                                      : model.getAttribute(key));

        using namespace ranges;
        return keys |= actions::sort(less {}, &QVariant::value<typename Model::KeyType>)
                       | actions::unique;
    }

} // namespace Orm::Tiny::Relations
} // namespace Orm::Tiny
} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // RELATION_H
