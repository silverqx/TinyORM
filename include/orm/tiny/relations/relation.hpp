#pragma once
#ifndef RELATION_H
#define RELATION_H

#include <QtSql/QSqlQuery>

#include <optional>

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

    template<typename Derived, AllRelationsConcept ...AllRelations>
    class Model;

namespace Relations
{

    /*! Base relations class. */
    template<class Model, class Related>
    class Relation
    {
        template<typename Derived>
        using BaseModel  = Orm::Tiny::Model<Derived>;
        using JoinClause = Orm::Query::JoinClause;

    protected:
        Relation(std::unique_ptr<Related> &&related, Model &parent,
                 const QString &relatedKey = "");

    public:
        /*! Related instance type passed to the relation. */
        using RelatedType = Related;

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
        inline virtual QVector<Related> get(const QVector<Column> &columns = {"*"}) const
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
        /*! Get the related key for the relationship. */
        inline const QString &getRelatedKeyName() const
        { return m_relatedKey; }

        /* Others */
        /*! Touch all of the related models for the relationship. */
        virtual void touch() const;
        /*! Run a raw update against the base query. */
        std::tuple<int, QSqlQuery>
        rawUpdate(const QVector<UpdateItem> &values = {}) const;

        /* TinyBuilder proxy methods */
        /*! Get a single column's value from the first result of a query. */
        QVariant value(const Column &column) const;

        /*! Find a model by its primary key. */
        virtual std::optional<Related>
        find(const QVariant &id, const QVector<Column> &columns = {"*"}) const;
        /*! Find a model by its primary key or return fresh model instance. */
        virtual Related
        findOrNew(const QVariant &id, const QVector<Column> &columns = {"*"}) const;
        /*! Find a model by its primary key or throw an exception. */
        virtual Related
        findOrFail(const QVariant &id, const QVector<Column> &columns = {"*"}) const;
        // findMany() is missing intentionally doesn't make sense for one type relations

        /*! Execute the query and get the first result. */
        virtual std::optional<Related>
        first(const QVector<Column> &columns = {"*"}) const;
        /*! Get the first record matching the attributes or instantiate it. */
        virtual Related
        firstOrNew(const QVector<WhereItem> &attributes = {},
                   const QVector<AttributeItem> &values = {}) const;
        /*! Get the first record matching the attributes or create it. */
        Related
        firstOrCreate(const QVector<WhereItem> &attributes = {},
                      const QVector<AttributeItem> &values = {}) const;
        /*! Execute the query and get the first result or throw an exception. */
        virtual Related firstOrFail(const QVector<Column> &columns = {"*"}) const;

        /*! Add a basic where clause to the query, and return the first result. */
        virtual std::optional<Related>
        firstWhere(const Column &column, const QString &comparison,
                   const QVariant &value, const QString &condition = "and") const;
        /*! Add a basic where clause to the query, and return the first result. */
        virtual std::optional<Related>
        firstWhereEq(const Column &column, const QVariant &value,
                     const QString &condition = "and") const;

        /*! Add a where clause on the primary key to the query. */
        const Relation &whereKey(const QVariant &id) const;
        /*! Add a where clause on the primary key to the query. */
        const Relation &whereKey(const QVector<QVariant> &ids) const;
        /*! Add a where clause on the primary key to the query. */
        const Relation &whereKeyNot(const QVariant &id) const;
        /*! Add a where clause on the primary key to the query. */
        const Relation &whereKeyNot(const QVector<QVariant> &ids) const;

        /*! Set the relationships that should be eager loaded. */
        template<typename = void>
        const Relation &with(const QVector<WithItem> &relations) const;
        /*! Set the relationships that should be eager loaded. */
        template<typename = void>
        const Relation &with(const QString &relation) const;
        /*! Begin querying a model with eager loading. */
        const Relation &with(const QVector<QString> &relations) const;
        /*! Begin querying a model with eager loading. */
        const Relation &with(QVector<QString> &&relations) const;

        /*! Prevent the specified relations from being eager loaded. */
        const Relation &without(const QVector<QString> &relations) const;
        /*! Prevent the specified relations from being eager loaded. */
        const Relation &without(const QString &relation) const;

        /*! Set the relationships that should be eager loaded while removing
            any previously added eager loading specifications. */
        const Relation &withOnly(const QVector<WithItem> &relations) const;
        /*! Set the relationship that should be eager loaded while removing
            any previously added eager loading specifications. */
        const Relation &withOnly(const QString &relation) const;

        /* Insert, Update, Delete */
        /*! Create or update a related record matching the attributes, and fill it
            with values. */
        Related updateOrCreate(const QVector<WhereItem> &attributes,
                               const QVector<AttributeItem> &values = {}) const;

        /* Proxies to TinyBuilder -> QueryBuilder */
        /*! Get the SQL representation of the query. */
        QString toSql() const;
        /*! Get the current query value bindings as flattened QVector. */
        QVector<QVariant> getBindings() const;

        /* Insert, Update, Delete */
        /*! Insert a new record into the database. */
        std::optional<QSqlQuery>
        insert(const QVector<AttributeItem> &values) const;
        /*! Insert new records into the database. */
        std::optional<QSqlQuery>
        insert(const QVector<QVector<AttributeItem>> &values) const;
        /*! Insert a new record and get the value of the primary key. */
        quint64 insertGetId(const QVector<AttributeItem> &attributes,
                            const QString &sequence = "") const;

        /*! Insert a new record into the database while ignoring errors. */
        std::tuple<int, std::optional<QSqlQuery>>
        insertOrIgnore(const QVector<AttributeItem> &values) const;
        /*! Insert new records into the database while ignoring errors. */
        std::tuple<int, std::optional<QSqlQuery>>
        insertOrIgnore(const QVector<QVector<AttributeItem>> &values) const;

        /*! Update records in the database. */
        std::tuple<int, QSqlQuery>
        update(const QVector<UpdateItem> &values) const;

        /*! Delete records from the database. */
        std::tuple<int, QSqlQuery> remove() const;
        /*! Delete records from the database. */
        std::tuple<int, QSqlQuery> deleteModels() const;

        /*! Run a truncate statement on the table. */
        void truncate() const;

        /* Select */
        /*! Set the columns to be selected. */
        const Relation &select(const QVector<Column> &columns = {"*"}) const;
        /*! Set the column to be selected. */
        const Relation &select(const Column &column) const;
        /*! Add new select columns to the query. */
        const Relation &addSelect(const QVector<Column> &columns) const;
        /*! Add a new select column to the query. */
        const Relation &addSelect(const Column &column) const;

        /*! Add a subselect expression to the query. */
        template<SubQuery T>
        const Relation &selectSub(T &&query, const QString &as) const;
        /*! Add a new "raw" select expression to the query. */
        const Relation &selectRaw(const QString &expression,
                                  const QVector<QVariant> &bindings = {}) const;

        /*! Force the query to only return distinct results. */
        const Relation &distinct() const;
        /*! Force the query to only return distinct results. */
        const Relation &distinct(const QStringList &columns) const;
        /*! Force the query to only return distinct results. */
        const Relation &distinct(QStringList &&columns) const;

        /*! Add a join clause to the query. */
        template<JoinTable T>
        const Relation &join(
                T &&table, const QString &first, const QString &comparison,
                const QString &second, const QString &type = "inner",
                bool where = false) const;
        /*! Add an advanced join clause to the query. */
        template<JoinTable T>
        const Relation &join(
                T &&table, const std::function<void(JoinClause &)> &callback,
                const QString &type = "inner") const;
        /*! Add a "join where" clause to the query. */
        template<JoinTable T>
        const Relation &joinWhere(
                T &&table, const QString &first, const QString &comparison,
                const QVariant &second, const QString &type = "inner") const;

        /*! Add a left join to the query. */
        template<JoinTable T>
        const Relation &leftJoin(
                T &&table, const QString &first, const QString &comparison,
                const QString &second) const;
        /*! Add an advanced left join to the query. */
        template<JoinTable T>
        const Relation &leftJoin(
                T &&table, const std::function<void(JoinClause &)> &callback) const;
        /*! Add a "join where" clause to the query. */
        template<JoinTable T>
        const Relation &leftJoinWhere(
                T &&table, const QString &first, const QString &comparison,
                const QVariant &second) const;

        /*! Add a right join to the query. */
        template<JoinTable T>
        const Relation &rightJoin(
                T &&table, const QString &first, const QString &comparison,
                const QString &second) const;
        /*! Add an advanced right join to the query. */
        template<JoinTable T>
        const Relation &rightJoin(
                T &&table, const std::function<void(JoinClause &)> &callback) const;
        /*! Add a "right join where" clause to the query. */
        template<JoinTable T>
        const Relation &rightJoinWhere(
                T &&table, const QString &first, const QString &comparison,
                const QVariant &second) const;

        /*! Add a "cross join" clause to the query. */
        template<JoinTable T>
        const Relation &crossJoin(
                T &&table, const QString &first, const QString &comparison,
                const QString &second) const;
        /*! Add an advanced "cross join" clause to the query. */
        template<JoinTable T>
        const Relation &crossJoin(
                T &&table, const std::function<void(JoinClause &)> &callback) const;

        /*! Add a subquery join clause to the query. */
        template<SubQuery T>
        const Relation &joinSub(
                T &&query, const QString &as, const QString &first,
                const QString &comparison, const QVariant &second,
                const QString &type = "inner", bool where = false) const;
        /*! Add a subquery left join to the query. */
        template<SubQuery T>
        const Relation &leftJoinSub(
                T &&query, const QString &as, const QString &first,
                const QString &comparison, const QVariant &second) const;
        /*! Add a subquery right join to the query. */
        template<SubQuery T>
        const Relation &rightJoinSub(
                T &&query, const QString &as, const QString &first,
                const QString &comparison, const QVariant &second) const;

        /*! Add a basic where clause to the query. */
        const Relation &where(
                const Column &column, const QString &comparison,
                const QVariant &value, const QString &condition = "and") const;
        /*! Add an "or where" clause to the query. */
        const Relation &orWhere(const Column &column, const QString &comparison,
                                const QVariant &value) const;
        /*! Add a basic equal where clause to the query. */
        const Relation &whereEq(const Column &column, const QVariant &value,
                                const QString &condition = "and") const;
        /*! Add an equal "or where" clause to the query. */
        const Relation &orWhereEq(const Column &column, const QVariant &value) const;

        /*! Add a nested where clause to the query. */
        const Relation &where(const std::function<void(Builder<Related> &)> &callback,
                              const QString &condition = "and") const;
        /*! Add a nested "or where" clause to the query. */
        const Relation &orWhere(
                const std::function<void(Builder<Related> &)> &callback) const;

        /*! Add a vector of basic where clauses to the query. */
        const Relation &where(const QVector<WhereItem> &values,
                              const QString &condition = "and") const;
        /*! Add a vector of basic "or where" clauses to the query. */
        const Relation &orWhere(const QVector<WhereItem> &values) const;

        /*! Add a vector of where clauses comparing two columns to the query. */
        const Relation &whereColumn(const QVector<WhereColumnItem> &values,
                                    const QString &condition = "and") const;
        /*! Add a vector of "or where" clauses comparing two columns to the query. */
        const Relation &orWhereColumn(const QVector<WhereColumnItem> &values) const;

        /*! Add a "where" clause comparing two columns to the query. */
        const Relation &whereColumn(const Column &first, const QString &comparison,
                                    const Column &second,
                                    const QString &condition = "and") const;
        /*! Add a "or where" clause comparing two columns to the query. */
        const Relation &orWhereColumn(const Column &first, const QString &comparison,
                                      const Column &second) const;
        /*! Add an equal "where" clause comparing two columns to the query. */
        const Relation &whereColumnEq(const Column &first, const Column &second,
                                      const QString &condition = "and") const;
        /*! Add an equal "or where" clause comparing two columns to the query. */
        const Relation &orWhereColumnEq(const Column &first,
                                        const Column &second) const;

        /*! Add a "where in" clause to the query. */
        const Relation &whereIn(
                const Column &column, const QVector<QVariant> &values,
                const QString &condition = "and", bool nope = false) const;
        /*! Add an "or where in" clause to the query. */
        const Relation &orWhereIn(const Column &column,
                                  const QVector<QVariant> &values) const;
        /*! Add a "where not in" clause to the query. */
        const Relation &whereNotIn(const Column &column,
                                   const QVector<QVariant> &values,
                                   const QString &condition = "and") const;
        /*! Add an "or where not in" clause to the query. */
        const Relation &orWhereNotIn(const Column &column,
                                     const QVector<QVariant> &values) const;

        /*! Add a "where null" clause to the query. */
        const Relation &whereNull(const QVector<Column> &columns = {"*"},
                                  const QString &condition = "and",
                                  bool nope = false) const;
        /*! Add an "or where null" clause to the query. */
        const Relation &orWhereNull(const QVector<Column> &columns = {"*"}) const;
        /*! Add a "where not null" clause to the query. */
        const Relation &whereNotNull(const QVector<Column> &columns = {"*"},
                                     const QString &condition = "and") const;
        /*! Add an "or where not null" clause to the query. */
        const Relation &orWhereNotNull(const QVector<Column> &columns = {"*"}) const;

        /*! Add a "where null" clause to the query. */
        const Relation &whereNull(const Column &column,
                                  const QString &condition = "and",
                                  bool nope = false) const;
        /*! Add an "or where null" clause to the query. */
        const Relation &orWhereNull(const Column &column) const;
        /*! Add a "where not null" clause to the query. */
        const Relation &whereNotNull(const Column &column,
                                     const QString &condition = "and") const;
        /*! Add an "or where not null" clause to the query. */
        const Relation &orWhereNotNull(const Column &column) const;

        /*! Add a "group by" clause to the query. */
        const Relation &groupBy(const QStringList &groups) const;
        /*! Add a "group by" clause to the query. */
        const Relation &groupBy(const QString &group) const;

        /*! Add a "having" clause to the query. */
        const Relation &having(const QString &column, const QString &comparison,
                               const QVariant &value,
                               const QString &condition = "and") const;
        /*! Add an "or having" clause to the query. */
        const Relation &orHaving(const QString &column, const QString &comparison,
                                 const QVariant &value) const;

        /*! Add an "order by" clause to the query. */
        const Relation &orderBy(const QString &column,
                                const QString &direction = "asc") const;
        /*! Add a descending "order by" clause to the query. */
        const Relation &orderByDesc(const QString &column) const;
        /*! Add an "order by" clause for a timestamp to the query. */
        const Relation &latest(const QString &column = "") const;
        /*! Add an "order by" clause for a timestamp to the query. */
        const Relation &oldest(const QString &column = "") const;
        /*! Remove all existing orders. */
        const Relation &reorder() const;
        /*! Remove all existing orders and optionally add a new order. */
        const Relation &reorder(const QString &column,
                                const QString &direction = "asc") const;

        /*! Set the "limit" value of the query. */
        const Relation &limit(int value) const;
        /*! Alias to set the "limit" value of the query. */
        const Relation &take(int value) const;
        /*! Set the "offset" value of the query. */
        const Relation &offset(int value) const;
        /*! Alias to set the "offset" value of the query. */
        const Relation &skip(int value) const;
        /*! Set the limit and offset for a given page. */
        const Relation &forPage(int page, int perPage = 30) const;

        /*! Increment a column's value by a given amount. */
        template<typename T> requires std::is_arithmetic_v<T>
        std::tuple<int, QSqlQuery>
        increment(const QString &column, T amount = 1,
                  const QVector<UpdateItem> &extra = {}) const;
        /*! Decrement a column's value by a given amount. */
        template<typename T> requires std::is_arithmetic_v<T>
        std::tuple<int, QSqlQuery>
        decrement(const QString &column, T amount = 1,
                  const QVector<UpdateItem> &extra = {}) const;

        /* Pessimistic Locking */
        /*! Lock the selected rows in the table for updating. */
        const Relation &lockForUpdate() const;
        /*! Share lock the selected rows in the table. */
        const Relation &sharedLock() const;
        /*! Lock the selected rows in the table. */
        const Relation &lock(bool value = true) const;
        /*! Lock the selected rows in the table. */
        const Relation &lock(const char *value) const;
        /*! Lock the selected rows in the table. */
        const Relation &lock(const QString &value) const;
        /*! Lock the selected rows in the table. */
        const Relation &lock(QString &&value) const;

        /* Others */
        /*! The textual representation of the Relation type. */
        virtual QString relationTypeName() const = 0;

    protected:
        /*! Initialize a Relation instance. */
        inline void init() const
        { addConstraints(); }

        /*! Get all of the primary keys for the vector of models. */
        QVector<QVariant>
        getKeys(const QVector<Model> &models, const QString &key = "") const;

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
        // FEATURE scopes silverqx
        return m_query->update(values);
    }

    template<class Model, class Related>
    QVariant Relation<Model, Related>::value(const Column &column) const
    {
        return m_query->value(column);
    }

    template<class Model, class Related>
    std::optional<Related>
    Relation<Model, Related>::find(const QVariant &id,
                                   const QVector<Column> &columns) const
    {
        return m_query->find(id, columns);
    }

    template<class Model, class Related>
    Related
    Relation<Model, Related>::findOrNew(const QVariant &id,
                                        const QVector<Column> &columns) const
    {
        return m_query->findOrNew(id, columns);
    }

    template<class Model, class Related>
    Related
    Relation<Model, Related>::findOrFail(const QVariant &id,
                                         const QVector<Column> &columns) const
    {
        return m_query->findOrFail(id, columns);
    }

    template<class Model, class Related>
    std::optional<Related>
    Relation<Model, Related>::first(const QVector<Column> &columns) const
    {
        return m_query->first(columns);
    }

    template<class Model, class Related>
    Related
    Relation<Model, Related>::firstOrNew(const QVector<WhereItem> &attributes,
                                         const QVector<AttributeItem> &values) const
    {
        return m_query->firstOrNew(attributes, values);
    }

    template<class Model, class Related>
    Related
    Relation<Model, Related>::firstOrCreate(const QVector<WhereItem> &attributes,
                                            const QVector<AttributeItem> &values) const
    {
        return m_query->firstOrCreate(attributes, values);
    }

    template<class Model, class Related>
    Related Relation<Model, Related>::firstOrFail(const QVector<Column> &columns) const
    {
        return m_query->firstOrFail(columns);
    }

    template<class Model, class Related>
    std::optional<Related>
    Relation<Model, Related>::firstWhere(
            const Column &column, const QString &comparison,
            const QVariant &value, const QString &condition) const
    {
        return m_query->firstWhere(column, comparison, value, condition);
    }

    template<class Model, class Related>
    std::optional<Related>
    Relation<Model, Related>::firstWhereEq(const Column &column, const QVariant &value,
                                           const QString &condition) const
    {
        return m_query->firstWhereEq(column, value, condition);
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::whereKey(const QVariant &id) const
    {
        m_query->whereKey(id);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::whereKey(const QVector<QVariant> &ids) const
    {
        m_query->whereKey(ids);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::whereKeyNot(const QVariant &id) const
    {
        m_query->whereKeyNot(id);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::whereKeyNot(const QVector<QVariant> &ids) const
    {
        m_query->whereKeyNot(ids);

        return *this;
    }

    template<class Model, class Related>
    template<typename>
    const Relation<Model, Related> &
    Relation<Model, Related>::with(const QVector<WithItem> &relations) const
    {
        m_query->with(relations);

        return *this;
    }

    template<class Model, class Related>
    template<typename>
    const Relation<Model, Related> &
    Relation<Model, Related>::with(const QString &relation) const
    {
        m_query->with(relation);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::with(const QVector<QString> &relations) const
    {
        m_query->with(relations);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::with(QVector<QString> &&relations) const
    {
        m_query->with(std::move(relations));

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::without(const QVector<QString> &relations) const
    {
        m_query->without(relations);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::without(const QString &relation) const
    {
        m_query->without(relation);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::withOnly(const QVector<WithItem> &relations) const
    {
        m_query->withOnly(relations);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::withOnly(const QString &relation) const
    {
        m_query->withOnly(relation);

        return *this;
    }

    template<class Model, class Related>
    Related
    Relation<Model, Related>::updateOrCreate(
            const QVector<WhereItem> &attributes,
            const QVector<AttributeItem> &values) const
    {
        return m_query->updateOrCreate(attributes, values);
    }

    template<class Model, class Related>
    inline QString Relation<Model, Related>::toSql() const
    {
        return getBaseQuery().toSql();
    }

    template<class Model, class Related>
    inline QVector<QVariant> Relation<Model, Related>::getBindings() const
    {
        return getBaseQuery().getBindings();
    }

    template<class Model, class Related>
    std::optional<QSqlQuery>
    Relation<Model, Related>::insert(const QVector<AttributeItem> &values) const
    {
        return m_query->insert(values);
    }

    template<class Model, class Related>
    std::optional<QSqlQuery>
    Relation<Model, Related>::insert(const QVector<QVector<AttributeItem>> &values) const
    {
        return m_query->insert(values);
    }

    // FEATURE dilemma primarykey, Model::KeyType vs QVariant silverqx
    template<class Model, class Related>
    quint64
    Relation<Model, Related>::insertGetId(const QVector<AttributeItem> &attributes,
                                          const QString &sequence) const
    {
        return m_query->insertGetId(attributes, sequence);
    }

    template<class Model, class Related>
    std::tuple<int, std::optional<QSqlQuery>>
    Relation<Model, Related>::insertOrIgnore(const QVector<AttributeItem> &values) const
    {
        return m_query->insertOrIgnore(values);
    }

    template<class Model, class Related>
    std::tuple<int, std::optional<QSqlQuery>>
    Relation<Model, Related>::insertOrIgnore(
            const QVector<QVector<AttributeItem>> &values) const
    {
        return m_query->insertOrIgnore(values);
    }

    template<class Model, class Related>
    std::tuple<int, QSqlQuery>
    Relation<Model, Related>::update(const QVector<UpdateItem> &values) const
    {
        return m_query->update(values);
    }

    template<class Model, class Related>
    std::tuple<int, QSqlQuery>
    Relation<Model, Related>::remove() const
    {
        return m_query->remove();
    }

    template<class Model, class Related>
    std::tuple<int, QSqlQuery>
    Relation<Model, Related>::deleteModels() const
    {
        return m_query->deleteModels();
    }

    template<class Model, class Related>
    void Relation<Model, Related>::truncate() const
    {
        m_query->truncate();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::select(const QVector<Column> &columns) const
    {
        m_query->select(columns);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::select(const Column &column) const
    {
        m_query->select(column);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::addSelect(const QVector<Column> &columns) const
    {
        m_query->addSelect(columns);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::addSelect(const Column &column) const
    {
        m_query->addSelect(column);

        return *this;
    }

    template<class Model, class Related>
    template<SubQuery T>
    const Relation<Model, Related> &
    Relation<Model, Related>::selectSub(T &&query, const QString &as) const
    {
        m_query->selectSub(std::forward<T>(query), as);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::selectRaw(
            const QString &expression, const QVector<QVariant> &bindings) const
    {
        m_query->selectRaw(expression, bindings);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::distinct() const
    {
        m_query->distinct();

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::distinct(const QStringList &columns) const
    {
        m_query->distinct(columns);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::distinct(QStringList &&columns) const
    {
        m_query->distinct(std::move(columns));

        return *this;
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    Relation<Model, Related>::join(
            T &&table, const QString &first, const QString &comparison,
            const QString &second, const QString &type, const bool where) const
    {
        m_query->join(std::forward<T>(table), first, comparison, second, type, where);

        return *this;
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    Relation<Model, Related>::join(
            T &&table, const std::function<void(JoinClause &)> &callback,
            const QString &type) const
    {
        m_query->join(std::forward<T>(table), callback, type);

        return *this;
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    Relation<Model, Related>::joinWhere(
            T &&table, const QString &first, const QString &comparison,
            const QVariant &second, const QString &type) const
    {
        m_query->joinWhere(std::forward<T>(table), first, comparison, second, type);

        return *this;
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    Relation<Model, Related>::leftJoin(
            T &&table, const QString &first, const QString &comparison,
            const QString &second) const
    {
        m_query->leftJoin(std::forward<T>(table), first, comparison, second);

        return *this;
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    Relation<Model, Related>::leftJoin(
            T &&table, const std::function<void(JoinClause &)> &callback) const
    {
        m_query->leftJoin(std::forward<T>(table), callback);

        return *this;
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    Relation<Model, Related>::leftJoinWhere(
            T &&table, const QString &first, const QString &comparison,
            const QVariant &second) const
    {
        m_query->leftJoinWhere(std::forward<T>(table), first, comparison, second);

        return *this;
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    Relation<Model, Related>::rightJoin(
            T &&table, const QString &first, const QString &comparison,
            const QString &second) const
    {
        m_query->rightJoin(std::forward<T>(table), first, comparison, second);

        return *this;
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    Relation<Model, Related>::rightJoin(
            T &&table, const std::function<void(JoinClause &)> &callback) const
    {
        m_query->rightJoin(std::forward<T>(table), callback);

        return *this;
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    Relation<Model, Related>::rightJoinWhere(
            T &&table, const QString &first, const QString &comparison,
            const QVariant &second) const
    {
        m_query->rightJoinWhere(std::forward<T>(table), first, comparison, second);

        return *this;
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    Relation<Model, Related>::crossJoin(
            T &&table, const QString &first, const QString &comparison,
            const QString &second) const
    {
        m_query->crossJoin(std::forward<T>(table), first, comparison, second);

        return *this;
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    Relation<Model, Related>::crossJoin(
            T &&table, const std::function<void(JoinClause &)> &callback) const
    {
        m_query->crossJoin(std::forward<T>(table), callback);

        return *this;
    }

    template<class Model, class Related>
    template<SubQuery T>
    const Relation<Model, Related> &
    Relation<Model, Related>::joinSub(
            T &&query, const QString &as, const QString &first,
            const QString &comparison, const QVariant &second,
            const QString &type, const bool where) const
    {
        m_query->joinSub(std::forward<T>(query), as, first, comparison, second, type,
                         where);

        return *this;
    }

    template<class Model, class Related>
    template<SubQuery T>
    const Relation<Model, Related> &
    Relation<Model, Related>::leftJoinSub(
            T &&query, const QString &as, const QString &first,
            const QString &comparison, const QVariant &second) const
    {
        m_query->leftJoinSub(std::forward<T>(query), as, first, comparison, second);

        return *this;
    }

    template<class Model, class Related>
    template<SubQuery T>
    const Relation<Model, Related> &
    Relation<Model, Related>::rightJoinSub(
            T &&query, const QString &as, const QString &first,
            const QString &comparison, const QVariant &second) const
    {
        m_query->rightJoinSub(std::forward<T>(query), as, first, comparison, second);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::where(const Column &column, const QString &comparison,
                                    const QVariant &value, const QString &condition) const
    {
        m_query->where(column, comparison, value, condition);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::orWhere(const Column &column, const QString &comparison,
                                      const QVariant &value) const
    {
        m_query->orWhere(column, comparison, value);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::whereEq(const Column &column, const QVariant &value,
                                      const QString &condition) const
    {
        m_query->whereEq(column, value, condition);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::orWhereEq(const Column &column,
                                        const QVariant &value) const
    {
        m_query->orWhereEq(column, value);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::where(
            const std::function<void(Builder<Related> &)> &callback,
            const QString &condition) const
    {
        m_query->where(callback, condition);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::orWhere(
            const std::function<void(Builder<Related> &)> &callback) const
    {
        m_query->orWhere(callback);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::where(const QVector<WhereItem> &values,
                                    const QString &condition) const
    {
        m_query->where(values, condition);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::orWhere(const QVector<WhereItem> &values) const
    {
        m_query->orWhere(values);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::whereColumn(
            const QVector<WhereColumnItem> &values, const QString &condition) const
    {
        m_query->whereColumn(values, condition);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::orWhereColumn(const QVector<WhereColumnItem> &values) const
    {
        m_query->orWhereColumn(values);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::whereColumn(
            const Column &first, const QString &comparison,
            const Column &second, const QString &condition) const
    {
        m_query->whereColumn(first, comparison, second, condition);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::orWhereColumn(
            const Column &first, const QString &comparison, const Column &second) const
    {
        m_query->orWhereColumn(first, comparison, second);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::whereColumnEq(
            const Column &first, const Column &second, const QString &condition) const
    {
        m_query->whereColumnEq(first, second, condition);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::orWhereColumnEq(const Column &first,
                                              const Column &second) const
    {
        m_query->orWhereColumnEq(first, second);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::whereIn(
            const Column &column, const QVector<QVariant> &values,
            const QString &condition, const bool nope) const
    {
        m_query->whereIn(column, values, condition, nope);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::orWhereIn(const Column &column,
                                        const QVector<QVariant> &values) const
    {
        m_query->orWhereIn(column, values);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::whereNotIn(
            const Column &column, const QVector<QVariant> &values,
            const QString &condition) const
    {
        m_query->whereNotIn(column, values, condition);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::orWhereNotIn(const Column &column,
                                           const QVector<QVariant> &values) const
    {
        m_query->orWhereNotIn(column, values);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::whereNull(
            const QVector<Column> &columns, const QString &condition,
            const bool nope) const
    {
        m_query->whereNull(columns, condition, nope);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::orWhereNull(const QVector<Column> &columns) const
    {
        m_query->orWhereNull(columns);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::whereNotNull(const QVector<Column> &columns,
                                           const QString &condition) const
    {
        m_query->whereNotNull(columns, condition);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::orWhereNotNull(const QVector<Column> &columns) const
    {
        m_query->orWhereNotNull(columns);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::whereNull(
            const Column &column, const QString &condition, const bool nope) const
    {
        m_query->whereNull(column, condition, nope);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::orWhereNull(const Column &column) const
    {
        m_query->orWhereNull(column);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::whereNotNull(const Column &column,
                                           const QString &condition) const
    {
        m_query->whereNotNull(column, condition);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::orWhereNotNull(const Column &column) const
    {
        m_query->orWhereNotNull(column);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::groupBy(const QStringList &groups) const
    {
        m_query->groupBy(groups);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::groupBy(const QString &group) const
    {
        m_query->groupBy(group);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::having(
            const QString &column, const QString &comparison,
            const QVariant &value, const QString &condition) const
    {
        m_query->having(column, comparison, value, condition);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::orHaving(const QString &column, const QString &comparison,
                                       const QVariant &value) const
    {
        m_query->orHaving(column, comparison, value);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::orderBy(const QString &column,
                                      const QString &direction) const
    {
        m_query->orderBy(column, direction);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::orderByDesc(const QString &column) const
    {
        m_query->orderByDesc(column);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::latest(const QString &column) const
    {
        m_query->latest(column);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::oldest(const QString &column) const
    {
        m_query->oldest(column);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::reorder() const
    {
        m_query->reorder();

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::reorder(const QString &column,
                                      const QString &direction) const
    {
        m_query->reorder(column, direction);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::limit(const int value) const
    {
        m_query->limit(value);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::take(const int value) const
    {
        m_query->take(value);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::offset(const int value) const
    {
        m_query->offset(value);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::skip(const int value) const
    {
        m_query->skip(value);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::forPage(const int page, const int perPage) const
    {
        m_query->forPage(page, perPage);

        return *this;
    }

    template<class Model, class Related>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, QSqlQuery>
    Relation<Model, Related>::increment(const QString &column, const T amount,
                                        const QVector<UpdateItem> &extra) const
    {
        return m_query->increment(column, amount, extra);
    }

    template<class Model, class Related>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, QSqlQuery>
    Relation<Model, Related>::decrement(const QString &column, const T amount,
                                        const QVector<UpdateItem> &extra) const
    {
        return m_query->decrement(column, amount, extra);
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::lockForUpdate() const
    {
        m_query->lock(true);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::sharedLock() const
    {
        m_query->lock(false);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::lock(const bool value) const
    {
        m_query->lock(value);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::lock(const char *value) const
    {
        m_query->lock(value);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::lock(const QString &value) const
    {
        m_query->lock(value);

        return *this;
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    Relation<Model, Related>::lock(QString &&value) const
    {
        m_query->lock(std::move(value));

        return *this;
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
