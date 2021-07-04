#pragma once
#ifndef TINYBUILDER_H
#define TINYBUILDER_H

#include <QtSql/QSqlRecord>

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/remove_if.hpp>

#include "orm/databaseconnection.hpp"
#include "orm/query/querybuilder.hpp"
#include "orm/tiny/modelnotfounderror.hpp"
#include "orm/utils/attribute.hpp"
#include "orm/utils/type.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny
{

namespace Relations
{
    template<class Model, class Related>
    class Relation;
}

    template<typename Model>
    class Builder
    {
        using JoinClause = Orm::Query::JoinClause;

    public:
        Builder(const QSharedPointer<QueryBuilder> query, Model &model);

        /*! Get the SQL representation of the query. */
        QString toSql() const;
        /*! Get the current query value bindings as flattened QVector. */
        QVector<QVariant> getBindings() const;

        /*! Execute the query as a "select" statement. */
        QVector<Model> get(const QVector<Column> &columns = {"*"});

        /*! Get a single column's value from the first result of a query. */
        QVariant value(const Column &column);

        /*! Find a model by its primary key. */
        std::optional<Model>
        find(const QVariant &id, const QVector<Column> &columns = {"*"});
        /*! Find a model by its primary key or return fresh model instance. */
        Model findOrNew(const QVariant &id, const QVector<Column> &columns = {"*"});
        /*! Find a model by its primary key or throw an exception. */
        Model findOrFail(const QVariant &id, const QVector<Column> &columns = {"*"});
        /*! Find multiple models by their primary keys. */
        QVector<Model>
        findMany(const QVector<QVariant> &ids, const QVector<Column> &columns = {"*"});

        /*! Execute the query and get the first result. */
        std::optional<Model> first(const QVector<Column> &columns = {"*"});
        /*! Get the first record matching the attributes or instantiate it. */
        Model firstOrNew(const QVector<WhereItem> &attributes = {},
                         const QVector<AttributeItem> &values = {});
        /*! Get the first record matching the attributes or create it. */
        Model firstOrCreate(const QVector<WhereItem> &attributes = {},
                            const QVector<AttributeItem> &values = {});
        /*! Execute the query and get the first result or throw an exception. */
        Model firstOrFail(const QVector<Column> &columns = {"*"});

        /*! Add a basic where clause to the query, and return the first result. */
        std::optional<Model>
        firstWhere(const Column &column, const QString &comparison,
                   const QVariant &value, const QString &condition = "and");
        /*! Add a basic equal where clause to the query, and return the first result. */
        std::optional<Model>
        firstWhereEq(const Column &column, const QVariant &value,
                     const QString &condition = "and");

        /*! Add a where clause on the primary key to the query. */
        Builder &whereKey(const QVariant &id);
        /*! Add a where clause on the primary key to the query. */
        Builder &whereKey(const QVector<QVariant> &ids);
        /*! Add a where clause on the primary key to the query. */
        Builder &whereKeyNot(const QVariant &id);
        /*! Add a where clause on the primary key to the query. */
        Builder &whereKeyNot(const QVector<QVariant> &ids);

        /*! Set the relationships that should be eager loaded. */
        template<typename = void>
        Builder &with(const QVector<WithItem> &relations);
        /*! Set the relationships that should be eager loaded. */
        template<typename = void>
        Builder &with(const QString &relation);
        /*! Set the relationships that should be eager loaded. */
        Builder &with(const QVector<QString> &relations);
        /*! Set the relationships that should be eager loaded. */
        Builder &with(QVector<QString> &&relations);

        /*! Prevent the specified relations from being eager loaded. */
        Builder &without(const QVector<QString> &relations);
        /*! Prevent the specified relations from being eager loaded. */
        Builder &without(const QString &relation);

        /*! Set the relationships that should be eager loaded while removing
            any previously added eager loading specifications. */
        Builder &withOnly(const QVector<WithItem> &relations);
        /*! Set the relationship that should be eager loaded while removing
            any previously added eager loading specifications. */
        Builder &withOnly(const QString &relation);

        /* Insert, Update, Delete */
        /*! Save a new model and return the instance. */
        Model create(const QVector<AttributeItem> &attributes = {});
        /*! Save a new model and return the instance. */
        Model create(QVector<AttributeItem> &&attributes = {});

        /*! Create or update a record matching the attributes, and fill it with
            values. */
        Model updateOrCreate(const QVector<WhereItem> &attributes,
                             const QVector<AttributeItem> &values = {});

        /* Proxy methods to the QueryBuilder */
        /* Insert, Update, Delete */
        /*! Insert a new record into the database. */
        std::optional<QSqlQuery>
        insert(const QVector<AttributeItem> &values) const;
        /*! Insert new records into the database. */
        std::optional<QSqlQuery>
        insert(const QVector<QVector<AttributeItem>> &values) const;
        /*! Insert a new record and get the value of the primary key. */
        quint64 insertGetId(const QVector<AttributeItem> &values,
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
        Builder &select(const QVector<Column> &columns = {"*"});
        /*! Set the column to be selected. */
        Builder &select(const Column &column);
        /*! Add new select columns to the query. */
        Builder &addSelect(const QVector<Column> &columns);
        /*! Add a new select column to the query. */
        Builder &addSelect(const Column &column);

        /*! Add a subselect expression to the query. */
        template<SubQuery T>
        Builder &selectSub(T &&query, const QString &as);
        /*! Add a new "raw" select expression to the query. */
        Builder &selectRaw(const QString &expression,
                           const QVector<QVariant> &bindings = {});

        /*! Force the query to only return distinct results. */
        Builder &distinct();
        /*! Force the query to only return distinct results. */
        Builder &distinct(const QStringList &columns);
        /*! Force the query to only return distinct results. */
        Builder &distinct(QStringList &&columns);

        /*! Add a join clause to the query. */
        template<JoinTable T>
        Builder &join(T &&table, const QString &first, const QString &comparison,
                      const QString &second, const QString &type = "inner",
                      bool where = false);
        /*! Add an advanced join clause to the query. */
        template<JoinTable T>
        Builder &join(T &&table, const std::function<void(JoinClause &)> &callback,
                      const QString &type = "inner");
        /*! Add a "join where" clause to the query. */
        template<JoinTable T>
        Builder &joinWhere(T &&table, const QString &first, const QString &comparison,
                           const QVariant &second, const QString &type = "inner");

        /*! Add a left join to the query. */
        template<JoinTable T>
        Builder &leftJoin(T &&table, const QString &first, const QString &comparison,
                          const QString &second);
        /*! Add an advanced left join to the query. */
        template<JoinTable T>
        Builder &leftJoin(T &&table, const std::function<void(JoinClause &)> &callback);
        /*! Add a "join where" clause to the query. */
        template<JoinTable T>
        Builder &leftJoinWhere(T &&table, const QString &first,
                               const QString &comparison, const QVariant &second);

        /*! Add a right join to the query. */
        template<JoinTable T>
        Builder &rightJoin(T &&table, const QString &first, const QString &comparison,
                           const QString &second);
        /*! Add an advanced right join to the query. */
        template<JoinTable T>
        Builder &rightJoin(T &&table, const std::function<void(JoinClause &)> &callback);
        /*! Add a "right join where" clause to the query. */
        template<JoinTable T>
        Builder &rightJoinWhere(T &&table, const QString &first,
                                const QString &comparison, const QVariant &second);

        /*! Add a "cross join" clause to the query. */
        template<JoinTable T>
        Builder &crossJoin(T &&table, const QString &first, const QString &comparison,
                           const QString &second);
        /*! Add an advanced "cross join" clause to the query. */
        template<JoinTable T>
        Builder &crossJoin(T &&table, const std::function<void(JoinClause &)> &callback);

        /*! Add a subquery join clause to the query. */
        template<SubQuery T>
        Builder &joinSub(T &&query, const QString &as, const QString &first,
                         const QString &comparison, const QVariant &second,
                         const QString &type = "inner", bool where = false);
        /*! Add a subquery left join to the query. */
        template<SubQuery T>
        Builder &leftJoinSub(T &&query, const QString &as, const QString &first,
                             const QString &comparison, const QVariant &second);
        /*! Add a subquery right join to the query. */
        template<SubQuery T>
        Builder &rightJoinSub(T &&query, const QString &as, const QString &first,
                              const QString &comparison, const QVariant &second);

        /*! Add a basic where clause to the query. */
        Builder &where(const Column &column, const QString &comparison,
                       const QVariant &value, const QString &condition = "and");
        /*! Add an "or where" clause to the query. */
        Builder &orWhere(const Column &column, const QString &comparison,
                         const QVariant &value);
        /*! Add a basic equal where clause to the query. */
        Builder &whereEq(const Column &column, const QVariant &value,
                         const QString &condition = "and");
        /*! Add an equal "or where" clause to the query. */
        Builder &orWhereEq(const Column &column, const QVariant &value);

        /*! Add a nested where clause to the query. */
        Builder &where(const std::function<void(Builder &)> &callback,
                       const QString &condition = "and");
        /*! Add a nested "or where" clause to the query. */
        Builder &orWhere(const std::function<void(Builder &)> &callback);

        /*! Add a vector of basic where clauses to the query. */
        Builder &where(const QVector<WhereItem> &values,
                       const QString &condition = "and");
        /*! Add a vector of basic "or where" clauses to the query. */
        Builder &orWhere(const QVector<WhereItem> &values);

        /*! Add a vector of where clauses comparing two columns to the query. */
        Builder &whereColumn(const QVector<WhereColumnItem> &values,
                             const QString &condition = "and");
        /*! Add a vector of "or where" clauses comparing two columns to the query. */
        Builder &orWhereColumn(const QVector<WhereColumnItem> &values);

        /*! Add a "where" clause comparing two columns to the query. */
        Builder &whereColumn(const Column &first, const QString &comparison,
                             const Column &second, const QString &condition = "and");
        /*! Add a "or where" clause comparing two columns to the query. */
        Builder &orWhereColumn(const Column &first, const QString &comparison,
                               const Column &second);
        /*! Add an equal "where" clause comparing two columns to the query. */
        Builder &whereColumnEq(const Column &first, const Column &second,
                               const QString &condition = "and");
        /*! Add an equal "or where" clause comparing two columns to the query. */
        Builder &orWhereColumnEq(const Column &first, const Column &second);

        /*! Add a "where in" clause to the query. */
        Builder &whereIn(const Column &column, const QVector<QVariant> &values,
                         const QString &condition = "and", bool nope = false);
        /*! Add an "or where in" clause to the query. */
        Builder &orWhereIn(const Column &column, const QVector<QVariant> &values);
        /*! Add a "where not in" clause to the query. */
        Builder &whereNotIn(const Column &column, const QVector<QVariant> &values,
                            const QString &condition = "and");
        /*! Add an "or where not in" clause to the query. */
        Builder &orWhereNotIn(const Column &column, const QVector<QVariant> &values);

        /*! Add a "where null" clause to the query. */
        Builder &whereNull(const QVector<Column> &columns = {"*"},
                           const QString &condition = "and", bool nope = false);
        /*! Add an "or where null" clause to the query. */
        Builder &orWhereNull(const QVector<Column> &columns = {"*"});
        /*! Add a "where not null" clause to the query. */
        Builder &whereNotNull(const QVector<Column> &columns = {"*"},
                              const QString &condition = "and");
        /*! Add an "or where not null" clause to the query. */
        Builder &orWhereNotNull(const QVector<Column> &columns = {"*"});

        /*! Add a "where null" clause to the query. */
        Builder &whereNull(const Column &column, const QString &condition = "and",
                           bool nope = false);
        /*! Add an "or where null" clause to the query. */
        Builder &orWhereNull(const Column &column);
        /*! Add a "where not null" clause to the query. */
        Builder &whereNotNull(const Column &column, const QString &condition = "and");
        /*! Add an "or where not null" clause to the query. */
        Builder &orWhereNotNull(const Column &column);

        /*! Add a "group by" clause to the query. */
        Builder &groupBy(const QStringList &groups);
        /*! Add a "group by" clause to the query. */
        Builder &groupBy(const QString &group);

        /*! Add a "having" clause to the query. */
        Builder &having(const QString &column, const QString &comparison,
                        const QVariant &value, const QString &condition = "and");
        /*! Add an "or having" clause to the query. */
        Builder &orHaving(const QString &column, const QString &comparison,
                          const QVariant &value);

        /*! Add an "order by" clause to the query. */
        Builder &orderBy(const QString &column, const QString &direction = "asc");
        /*! Add a descending "order by" clause to the query. */
        Builder &orderByDesc(const QString &column);
        /*! Add an "order by" clause for a timestamp to the query. */
        Builder &latest(const QString &column = "");
        /*! Add an "order by" clause for a timestamp to the query. */
        Builder &oldest(const QString &column = "");
        /*! Remove all existing orders. */
        Builder &reorder();
        /*! Remove all existing orders and optionally add a new order. */
        Builder &reorder(const QString &column, const QString &direction = "asc");

        /*! Set the "limit" value of the query. */
        Builder &limit(int value);
        /*! Alias to set the "limit" value of the query. */
        Builder &take(int value);
        /*! Set the "offset" value of the query. */
        Builder &offset(int value);
        /*! Alias to set the "offset" value of the query. */
        Builder &skip(int value);
        /*! Set the limit and offset for a given page. */
        Builder &forPage(int page, int perPage = 30);

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
        Builder &lockForUpdate();
        /*! Share lock the selected rows in the table. */
        Builder &sharedLock();
        /*! Lock the selected rows in the table. */
        Builder &lock(bool value = true);
        /*! Lock the selected rows in the table. */
        Builder &lock(const char *value);
        /*! Lock the selected rows in the table. */
        Builder &lock(const QString &value);
        /*! Lock the selected rows in the table. */
        Builder &lock(QString &&value);

        /* TinyBuilder methods */
        /*! Create a new instance of the model being queried. */
        Model newModelInstance(const QVector<AttributeItem> &attributes = {});

        /*! Get the hydrated models without eager loading. */
        QVector<Model> getModels(const QVector<Column> &columns = {"*"});

        /*! Eager load the relationships for the models. */
        void eagerLoadRelations(QVector<Model> &models);
        /*! Eagerly load the relationship on a set of models. */
        template<typename Relation>
        void eagerLoadRelationVisited(Relation &&relation, QVector<Model> &models,
                                      const WithItem &relationItem) const;
        /*! Create a vector of models from the QSqlQuery. */
        QVector<Model> hydrate(QSqlQuery &&result);

        /*! Get the model instance being queried. */
        inline Model &getModel()
        { return m_model; }
        /*! Get the underlying query builder instance. */
        inline QueryBuilder &getQuery() const
        { return *m_query; }
        // TODO now fix revisit silverqx
        /*! Get the underlying query builder instance as a QSharedPointer. */
        inline const QSharedPointer<QueryBuilder> &
        getQuerySharedPointer() const
        { return m_query; }

        /*! Get a database connection. */
        inline const ConnectionInterface &getConnection() const
        { return m_query->getConnection(); }

        /*! Get a base query builder instance. */
        inline QueryBuilder &toBase() const
        { return getQuery(); }
        // FUTURE add Query Scopes feature silverqx
//        { return $this->applyScopes()->getQuery(); }

    protected:
        /*! Expression alias. */
        using Expression = Orm::Query::Expression;

        /*! Parse a list of relations into individuals. */
        QVector<WithItem> parseWithRelations(const QVector<WithItem> &relations);
        /*! Create a constraint to select the given columns for the relation. */
        WithItem createSelectWithConstraint(const QString &name);
        /*! Parse the nested relationships in a relation. */
        void addNestedWiths(const QString &name, QVector<WithItem> &results) const;

        /*! Get the deeply nested relations for a given top-level relation. */
        QVector<WithItem>
        relationsNestedUnder(const QString &topRelationName) const;
        /*! Determine if the relationship is nested. */
        bool isNestedUnder(const QString &topRelation,
                           const QString &nestedRelation) const;

        /*! Add the "updated at" column to the vector of values. */
        QVector<UpdateItem>
        addUpdatedAtColumn(QVector<UpdateItem> values) const;

        /*! Get the name of the "created at" column. */
        QString getCreatedAtColumnForLatestOldest(QString column) const;

        /*! The base query builder instance. */
        const QSharedPointer<QueryBuilder> m_query;
        /* This can't be a reference because the model is created on the stack
           in Model::query(), then copied here and the original is destroyed
           immediately. */
        /*! The model being queried. */
        Model m_model;
        /*! The relationships that should be eager loaded. */
        QVector<WithItem> m_eagerLoad;
    };

    template<typename Model>
    Builder<Model>::Builder(const QSharedPointer<QueryBuilder> query,
                            Model &model)
        : m_query(query)
        , m_model(model)
    {
        m_query->from(m_model.getTable());
    }

    template<typename Model>
    inline QString Builder<Model>::toSql() const
    {
        return toBase().toSql();
    }

    template<typename Model>
    inline QVector<QVariant> Builder<Model>::getBindings() const
    {
        return toBase().getBindings();
    }

    // TODO now name QVector<Model> model collections by using, eg CollectionType silverqx
    template<typename Model>
    QVector<Model>
    Builder<Model>::get(const QVector<Column> &columns)
    {
        auto models = getModels(columns);

        /* If we actually found models we will also eager load any relationships that
           have been specified as needing to be eager loaded, which will solve the
           n+1 query issue for the developers to avoid running a lot of queries. */
        if (models.size() > 0)
            /* 'models' are passed down as the reference and relations are set on models
               at the end of the call tree, no need to return models. */
            eagerLoadRelations(models);

        return models;
        // Laravel does it this way
//        return $builder->getModel()->newCollection($models);
    }

    // FEATURE expressions, fuckup🤔 silverqx
    template<typename Model>
    QVariant Builder<Model>::value(const Column &column)
    {
        auto model = first({column});

        if (!model)
            return {};

        // Expression support
        QString column_;

        if (std::holds_alternative<Expression>(column))
            column_ = std::get<Expression>(column).getValue().value<QString>();
        else
            column_ = std::get<QString>(column);

        return model->getAttribute(column_.mid(column_.lastIndexOf(QChar('.')) + 1));
    }

    // FEATURE dilemma primarykey, Model::KeyType for id silverqx
    template<typename Model>
    std::optional<Model>
    Builder<Model>::find(const QVariant &id, const QVector<Column> &columns)
    {
        return whereKey(id).first(columns);
    }

    template<typename Model>
    Model Builder<Model>::findOrNew(const QVariant &id, const QVector<Column> &columns)
    {
        auto model = find(id, columns);

        // Found
        if (model)
            return *model;

        return newModelInstance();
    }

    template<typename Model>
    Model Builder<Model>::findOrFail(const QVariant &id, const QVector<Column> &columns)
    {
        auto model = find(id, columns);

        // Found
        if (model)
            return *model;

        throw ModelNotFoundError(Utils::Type::classPureBasename<Model>(), {id});
    }

    template<typename Model>
    QVector<Model>
    Builder<Model>::findMany(const QVector<QVariant> &ids,
                             const QVector<Column> &columns)
    {
        if (ids.isEmpty())
            return {};

        return whereKey(ids).get(columns);
    }

    template<typename Model>
    std::optional<Model>
    Builder<Model>::first(const QVector<Column> &columns)
    {
        auto models = take(1).get(columns);

        if (models.isEmpty())
            return std::nullopt;

        return std::move(models.first());
    }

    template<typename Model>
    Model
    Builder<Model>::firstOrNew(const QVector<WhereItem> &attributes,
                               const QVector<AttributeItem> &values)
    {
        auto instance = where(attributes).first();

        // Model found in db
        if (instance)
            return *instance;

        return newModelInstance(
                    Utils::Attribute::joinAttributesForFirstOr(attributes, values,
                                                               m_model.getKeyName()));
    }

    template<typename Model>
    Model
    Builder<Model>::firstOrCreate(const QVector<WhereItem> &attributes,
                                  const QVector<AttributeItem> &values)
    {
        // Model found in db
        if (auto instance = where(attributes).first(); instance)
            return *instance;

        auto newInstance =
                newModelInstance(
                    Utils::Attribute::joinAttributesForFirstOr(attributes, values,
                                                               m_model.getKeyName()));

        newInstance.save();

        return newInstance;
    }

    template<typename Model>
    Model Builder<Model>::firstOrFail(const QVector<Column> &columns)
    {
        auto model = first(columns);

        // Found
        if (model)
            return *model;

        throw ModelNotFoundError(Utils::Type::classPureBasename<Model>());
    }

    template<typename Model>
    std::optional<Model>
    Builder<Model>::firstWhere(const Column &column, const QString &comparison,
                               const QVariant &value, const QString &condition)
    {
        return where(column, comparison, value, condition).first();
    }

    template<typename Model>
    std::optional<Model>
    Builder<Model>::firstWhereEq(const Column &column, const QVariant &value,
                                 const QString &condition)
    {
        return where(column, QStringLiteral("="), value, condition).first();
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereKey(const QVariant &id)
    {
        return where(m_model.getQualifiedKeyName(), QStringLiteral("="), id);
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereKey(const QVector<QVariant> &ids)
    {
        m_query->whereIn(m_model.getQualifiedKeyName(), ids);

        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereKeyNot(const QVariant &id)
    {
        return where(m_model.getQualifiedKeyName(), QStringLiteral("!="), id);
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereKeyNot(const QVector<QVariant> &ids)
    {
        m_query->whereNotIn(m_model.getQualifiedKeyName(), ids);

        return *this;
    }

    template<typename Model>
    template<typename>
    Builder<Model> &
    Builder<Model>::with(const QVector<WithItem> &relations)
    {
        auto eagerLoad = parseWithRelations(relations);

        std::ranges::move(eagerLoad, std::back_inserter(m_eagerLoad));

        return *this;
    }

    template<typename Model>
    template<typename>
    Builder<Model> &
    Builder<Model>::with(const QString &relation)
    {
        return with(QVector<WithItem> {{relation}});
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::with(const QVector<QString> &relations)
    {
        QVector<WithItem> relationsConverted;
        relationsConverted.reserve(relations.size());

        for (const auto &relation : relations)
            relationsConverted.append({relation});

        return with(relationsConverted);
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::with(QVector<QString> &&relations)
    {
        QVector<WithItem> relationsConverted;
        relationsConverted.reserve(relations.size());

        for (auto &relation : relations)
            relationsConverted.append({std::move(relation)});

        return with(relationsConverted);
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::without(const QVector<QString> &relations)
    {
        // Remove relations in the "relations" vector from m_eagerLoad vector
        using namespace ranges;
        m_eagerLoad = m_eagerLoad | views::remove_if(
                          [&relations](const WithItem &with)
        {
            return relations.contains(with.name);
        })
                | ranges::to<QVector<WithItem>>();

        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::without(const QString &relation)
    {
        return without(QVector<QString> {relation});
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::withOnly(const QVector<WithItem> &relations)
    {
        m_eagerLoad.clear();

        return with(relations);
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::withOnly(const QString &relation)
    {
        return withOnly(QVector<WithItem> {{relation}});
    }

    template<typename Model>
    Model Builder<Model>::create(const QVector<AttributeItem> &attributes)
    {
        auto model = newModelInstance(attributes);

        model.save();

        return model;
    }

    template<typename Model>
    Model Builder<Model>::create(QVector<AttributeItem> &&attributes)
    {
        auto model = newModelInstance(std::move(attributes));

        model.save();

        return model;
    }

    template<typename Model>
    Model Builder<Model>::updateOrCreate(const QVector<WhereItem> &attributes,
                                         const QVector<AttributeItem> &values)
    {
        auto instance = firstOrNew(attributes);

        instance.fill(values).save();

        return instance;
    }

    template<typename Model>
    std::optional<QSqlQuery>
    Builder<Model>::insert(const QVector<AttributeItem> &values) const
    {
        return toBase().insert(Utils::Attribute::convertVectorToMap(values));
    }

    template<typename Model>
    std::optional<QSqlQuery>
    Builder<Model>::insert(const QVector<QVector<AttributeItem>> &values) const
    {
        return toBase().insert(Utils::Attribute::convertVectorsToMaps(values));
    }

    // FEATURE dilemma primarykey, Model::KeyType vs QVariant silverqx
    template<typename Model>
    quint64
    Builder<Model>::insertGetId(const QVector<AttributeItem> &values,
                                const QString &sequence) const
    {
        return toBase().insertGetId(Utils::Attribute::convertVectorToMap(values),
                                    sequence);
    }

    template<typename Model>
    std::tuple<int, std::optional<QSqlQuery>>
    Builder<Model>::insertOrIgnore(const QVector<AttributeItem> &values) const
    {
        return toBase().insertOrIgnore(Utils::Attribute::convertVectorToMap(values));
    }

    template<typename Model>
    std::tuple<int, std::optional<QSqlQuery>>
    Builder<Model>::insertOrIgnore(const QVector<QVector<AttributeItem>> &values) const
    {
        return toBase().insertOrIgnore(Utils::Attribute::convertVectorsToMaps(values));
    }

    template<typename Model>
    std::tuple<int, QSqlQuery>
    Builder<Model>::update(const QVector<UpdateItem> &values) const
    {
        return toBase().update(addUpdatedAtColumn(values));
    }

    // FUTURE add onDelete (and similar) callback feature silverqx
    template<typename Model>
    std::tuple<int, QSqlQuery> Builder<Model>::remove() const
    {
        return toBase().deleteRow();
    }

    template<typename Model>
    std::tuple<int, QSqlQuery> Builder<Model>::deleteModels() const
    {
        return remove();
    }

    template<typename Model>
    void Builder<Model>::truncate() const
    {
        toBase().truncate();
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::select(const QVector<Column> &columns)
    {
        toBase().select(columns);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::select(const Column &column)
    {
        toBase().select(column);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::addSelect(const QVector<Column> &columns)
    {
        toBase().addSelect(columns);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::addSelect(const Column &column)
    {
        toBase().addSelect(column);
        return *this;
    }

    template<typename Model>
    template<SubQuery T>
    Builder<Model> &Builder<Model>::selectSub(T &&query, const QString &as)
    {
        toBase().selectSub(std::forward<T>(query), as);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::selectRaw(const QString &expression,
                                              const QVector<QVariant> &bindings)
    {
        toBase().selectRaw(expression, bindings);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::distinct()
    {
        toBase().distinct();
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::distinct(const QStringList &columns)
    {
        toBase().distinct(columns);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::distinct(QStringList &&columns)
    {
        toBase().distinct(std::move(columns));
        return *this;
    }

    template<typename Model>
    template<JoinTable T>
    Builder<Model> &
    Builder<Model>::join(T &&table, const QString &first, const QString &comparison,
                         const QString &second, const QString &type, const bool where)
    {
        toBase().join(std::forward<T>(table), first, comparison, second, type, where);
        return *this;
    }

    template<typename Model>
    template<JoinTable T>
    Builder<Model> &
    Builder<Model>::join(T &&table, const std::function<void(JoinClause &)> &callback,
                         const QString &type)
    {
        toBase().join(std::forward<T>(table), callback, type);
        return *this;
    }

    template<typename Model>
    template<JoinTable T>
    Builder<Model> &
    Builder<Model>::joinWhere(T &&table, const QString &first, const QString &comparison,
                              const QVariant &second, const QString &type)
    {
        toBase().joinWhere(std::forward<T>(table), first, comparison, second, type);
        return *this;
    }

    template<typename Model>
    template<JoinTable T>
    Builder<Model> &
    Builder<Model>::leftJoin(T &&table, const QString &first, const QString &comparison,
                             const QString &second)
    {
        toBase().leftJoin(std::forward<T>(table), first, comparison, second);
        return *this;
    }

    template<typename Model>
    template<JoinTable T>
    Builder<Model> &
    Builder<Model>::leftJoin(T &&table,
                             const std::function<void(JoinClause &)> &callback)
    {
        toBase().leftJoin(std::forward<T>(table), callback);
        return *this;
    }

    template<typename Model>
    template<JoinTable T>
    Builder<Model> &
    Builder<Model>::leftJoinWhere(T &&table, const QString &first,
                                  const QString &comparison, const QVariant &second)
    {
        toBase().leftJoinWhere(std::forward<T>(table), first, comparison, second);
        return *this;
    }

    template<typename Model>
    template<JoinTable T>
    Builder<Model> &
    Builder<Model>::rightJoin(T &&table, const QString &first, const QString &comparison,
                              const QString &second)
    {
        toBase().rightJoin(std::forward<T>(table), first, comparison, second);
        return *this;
    }

    template<typename Model>
    template<JoinTable T>
    Builder<Model> &
    Builder<Model>::rightJoin(T &&table,
                              const std::function<void(JoinClause &)> &callback)
    {
        toBase().rightJoin(std::forward<T>(table), callback);
        return *this;
    }

    template<typename Model>
    template<JoinTable T>
    Builder<Model> &
    Builder<Model>::rightJoinWhere(T &&table, const QString &first,
                                   const QString &comparison, const QVariant &second)
    {
        toBase().rightJoinWhere(std::forward<T>(table), first, comparison, second);
        return *this;
    }

    template<typename Model>
    template<JoinTable T>
    Builder<Model> &
    Builder<Model>::crossJoin(T &&table, const QString &first, const QString &comparison,
                              const QString &second)
    {
        toBase().crossJoin(std::forward<T>(table), first, comparison, second);
        return *this;
    }

    template<typename Model>
    template<JoinTable T>
    Builder<Model> &
    Builder<Model>::crossJoin(T &&table,
                              const std::function<void(JoinClause &)> &callback)
    {
        toBase().crossJoin(std::forward<T>(table), callback);
        return *this;
    }

    template<typename Model>
    template<SubQuery T>
    Builder<Model> &
    Builder<Model>::joinSub(T &&query, const QString &as, const QString &first,
                            const QString &comparison, const QVariant &second,
                            const QString &type, const bool where)
    {
        toBase().joinSub(std::forward<T>(query), as, first, comparison, second, type,
                         where);
        return *this;
    }

    template<typename Model>
    template<SubQuery T>
    Builder<Model> &
    Builder<Model>::leftJoinSub(T &&query, const QString &as, const QString &first,
                                const QString &comparison, const QVariant &second)
    {
        toBase().leftJoinSub(std::forward<T>(query), as, first, comparison, second);
        return *this;
    }

    template<typename Model>
    template<SubQuery T>
    Builder<Model> &
    Builder<Model>::rightJoinSub(T &&query, const QString &as, const QString &first,
                                 const QString &comparison, const QVariant &second)
    {
        toBase().rightJoinSub(std::forward<T>(query), as, first, comparison, second);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::where(const Column &column, const QString &comparison,
                          const QVariant &value, const QString &condition)
    {
        toBase().where(column, comparison, value, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhere(const Column &column, const QString &comparison,
                            const QVariant &value)
    {
        toBase().orWhere(column, comparison, value);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereEq(const Column &column, const QVariant &value,
                            const QString &condition)
    {
        toBase().whereEq(column, value, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhereEq(const Column &column, const QVariant &value)
    {
        toBase().orWhereEq(column, value);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::where(const std::function<void(Builder &)> &callback,
                          const QString &condition)
    {
        // Ownership of a unique_ptr()
        auto query = m_model.newQueryWithoutRelationships();

        std::invoke(callback, *query);

        toBase().addNestedWhereQuery(query->getQuerySharedPointer(), condition);

        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhere(const std::function<void(Builder &)> &callback)
    {
        return where(callback, "or");
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::where(const QVector<WhereItem> &values, const QString &condition)
    {
        toBase().where(values, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::orWhere(const QVector<WhereItem> &values)
    {
        toBase().orWhere(values);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereColumn(const QVector<WhereColumnItem> &values,
                                const QString &condition)
    {
        toBase().whereColumn(values, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhereColumn(const QVector<WhereColumnItem> &values)
    {
        toBase().orWhereColumn(values);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereColumn(const Column &first, const QString &comparison,
                                const Column &second, const QString &condition)
    {
        toBase().whereColumn(first, comparison, second, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhereColumn(const Column &first, const QString &comparison,
                                  const Column &second)
    {
        toBase().orWhereColumn(first, comparison, second);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereColumnEq(const Column &first, const Column &second,
                                  const QString &condition)
    {
        toBase().whereColumnEq(first, second, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhereColumnEq(const Column &first, const Column &second)
    {
        toBase().orWhereColumnEq(first, second);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereIn(const Column &column, const QVector<QVariant> &values,
                            const QString &condition, const bool nope)
    {
        toBase().whereIn(column, values, condition, nope);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhereIn(const Column &column, const QVector<QVariant> &values)
    {
        toBase().orWhereIn(column, values);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereNotIn(const Column &column, const QVector<QVariant> &values,
                               const QString &condition)
    {
        toBase().whereNotIn(column, values, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhereNotIn(const Column &column, const QVector<QVariant> &values)
    {
        toBase().orWhereNotIn(column, values);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereNull(const QVector<Column> &columns, const QString &condition,
                              bool nope)
    {
        toBase().whereNull(columns, condition, nope);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::orWhereNull(const QVector<Column> &columns)
    {
        toBase().orWhereNull(columns);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereNotNull(const QVector<Column> &columns,
                                 const QString &condition)
    {
        toBase().whereNotNull(columns, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::orWhereNotNull(const QVector<Column> &columns)
    {
        toBase().orWhereNotNull(columns);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereNull(const Column &column, const QString &condition,
                              const bool nope)
    {
        toBase().whereNull(column, condition, nope);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::orWhereNull(const Column &column)
    {
        toBase().orWhereNull(column);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereNotNull(const Column &column, const QString &condition)
    {
        toBase().whereNotNull(column, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::orWhereNotNull(const Column &column)
    {
        toBase().orWhereNotNull(column);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::groupBy(const QStringList &groups)
    {
        toBase().groupBy(groups);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::groupBy(const QString &group)
    {
        toBase().groupBy(group);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::having(const QString &column, const QString &comparison,
                           const QVariant &value, const QString &condition)
    {
        toBase().having(column, comparison, value, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orHaving(const QString &column, const QString &comparison,
                             const QVariant &value)
    {
        toBase().orHaving(column, comparison, value);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orderBy(const QString &column, const QString &direction)
    {
        toBase().orderBy(column, direction);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::orderByDesc(const QString &column)
    {
        toBase().orderByDesc(column);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::latest(const QString &column)
    {
        toBase().latest(getCreatedAtColumnForLatestOldest(column));
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::oldest(const QString &column)
    {
        toBase().oldest(getCreatedAtColumnForLatestOldest(column));
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::reorder()
    {
        toBase().reorder();
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::reorder(const QString &column, const QString &direction)
    {
        toBase().reorder(column, direction);
        return *this;
    }

    template<typename Model>
    QString
    Builder<Model>::getCreatedAtColumnForLatestOldest(QString column) const
    {
        if (column.isEmpty()) {
            if (const auto &createdAtColumn = m_model.getCreatedAtColumn();
                createdAtColumn.isEmpty()
            )
                column = "created_at";
            else
                column = createdAtColumn;
        }

        return column;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::limit(const int value)
    {
        toBase().limit(value);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::take(const int value)
    {
        return limit(value);
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::offset(const int value)
    {
        toBase().offset(value);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::skip(const int value)
    {
        return offset(value);
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::forPage(const int page, const int perPage)
    {
        toBase().forPage(page, perPage);
        return *this;
    }

    template<typename Model>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, QSqlQuery>
    Builder<Model>::increment(const QString &column, const T amount,
                              const QVector<UpdateItem> &extra) const
    {
        return toBase().template increment<T>(column, amount,
                                              addUpdatedAtColumn(extra));
    }

    template<typename Model>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, QSqlQuery>
    Builder<Model>::decrement(const QString &column, const T amount,
                              const QVector<UpdateItem> &extra) const
    {
        return toBase().template decrement<T>(column, amount,
                                              addUpdatedAtColumn(extra));
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::lockForUpdate()
    {
        toBase().lock(true);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::sharedLock()
    {
        toBase().lock(false);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::lock(const bool value)
    {
        toBase().lock(value);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::lock(const char *value)
    {
        toBase().lock(value);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::lock(const QString &value)
    {
        toBase().lock(value);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::lock(QString &&value)
    {
        toBase().lock(std::move(value));
        return *this;
    }

    template<typename Model>
    Model Builder<Model>::newModelInstance(const QVector<AttributeItem> &attributes)
    {
        return m_model.newInstance(attributes)
                .setConnection(m_query->getConnection().getName());
        // TODO study, or stackoverflow move or not move? its a question 🤔 silverqx
//        return std::move(m_model.newInstance(attributes)
//                         .setConnection(m_query->getConnection().getName()));
    }

    template<typename Model>
    QVector<Model>
    Builder<Model>::getModels(const QVector<Column> &columns)
    {
        return hydrate(m_query->get(columns));
    }

    template<typename Model>
    void Builder<Model>::eagerLoadRelations(QVector<Model> &models)
    {
        if (m_eagerLoad.isEmpty())
            return;

        for (const auto &relation : std::as_const(m_eagerLoad))
            /* For nested eager loads we'll skip loading them here and they will be set
               as an eager load on the query to retrieve the relation so that they will
               be eager loaded on that query, because that is where they get hydrated
               as models. */
            if (!relation.name.contains(QChar('.')))
                /* Get the relation instance for the given relation name, have to be done
                   through the visitor pattern. */
                m_model.eagerLoadRelationWithVisitor(relation, *this, models);
    }

    template<typename Model>
    template<typename Relation>
    void Builder<Model>::eagerLoadRelationVisited(
            Relation &&relation, QVector<Model> &models,
            const WithItem &relationItem) const
    {
        // TODO docs add similar note for lazy load silverqx
        /* Look also at EagerRelationStore::visited(), where the whole flow begins.
           How this relation flow works:
           EagerRelationStore::visited() obtains a reference by the relation name
           to the relation method, these relation methods are defined on models
           as member functions.
           A reference to the relation methods are defined in the Model::u_relations
           hash as lambda expressions. These lambda expressions will be visited/invoked
           by EagerRelationStore::visited() to obtain references to the relation methods.
           Relation constraints will be disabled for eager relations by
           Relation::noConstraints() method, these default constraints are only used
           for lazy loading, for eager constraints are used constraints, which are
           defined by Relation::addEagerConstraints() virtual methods.
           To the Relation::noConstraints() method is passed lambda, which invokes
           obtained reference to the relation method defined on the model and invokes it
           on the 'new' model instance refered as 'dummyModel'.
           The Relation instance is created by this relation method, this relation
           method calls factory method, which creates the Relation instance.
           Every Relation has it's own Relation::create() factory method, to which
           the following parameters are passed, newly created Related model instance,
           current/parent model instance, database column names of the relationship, and
           for a BelongsTo relation also the name of the relation.
           The Relation instance saves a non-const reference to the current/parent model
           instance, a copy of the related model instance because it is created
           on the stack.
           The Relation instance creates a new TinyBuilder instance from the Related
           model instance by TinyBuilder::newQuery() and saves ownership as
           the unique pointer.
           Then eager constraints are applied to this newly created TinyBuilder and
           the result is returned back to the initial model.
           The result is transformed into models and these models are hydrated.
           Hydrated models are saved to the Model::m_relations data member. */

        /* First we will "back up" the existing where conditions on the query so we can
           add our eager constraints, this is done in the EagerRelationStore::visited()
           by help of the Relations::Relation::noConstraints().
           Folowing is not implemented for now, it is true for relationItem.constraints:
           Then we will merge the wheres that were on the query back to it in order
           that any where conditions might be specified. */
        const auto nested = relationsNestedUnder(relationItem.name);

        /* If there are nested relationships set on the query, we will put those onto
           the query instances so that they can be handled after this relationship
           is loaded. In this way they will all trickle down as they are loaded. */
        if (nested.size() > 0)
            relation->getQuery().with(nested);

        relation->addEagerConstraints(models);

        // Add relation contraints defined in a user callback
        // NOTE api different, Eloquent is passing the Relation reference into the lambda, it would be almost impossible to do it silverqx
        if (relationItem.constraints)
            std::invoke(relationItem.constraints, relation->getBaseQuery());

        /* Once we have the results, we just match those back up to their parent models
           using the relationship instance. Then we just return the finished vectors
           of models which have been eagerly hydrated and are readied for return. */
        relation->match(relation->initRelation(models, relationItem.name),
                        relation->getEager(), relationItem.name);
    }

    template<typename Model>
    QVector<Model>
    Builder<Model>::hydrate(QSqlQuery &&result)
    {
        auto instance = newModelInstance();

        QVector<Model> models;

        // Table row, instantiate the QVector once and then re-use
        QVector<AttributeItem> row;
        row.reserve(result.record().count());

        while (result.next()) {
            row.clear();

            // Populate table row with data from the database
            const auto record = result.record();
            for (int i = 0; i < record.count(); ++i)
                row.append({record.fieldName(i), result.value(i)});

            // Create a new model instance from the table row
            models.append(instance.newFromBuilder(row));
        }

        return models;
    }

    template<typename Model>
    QVector<WithItem>
    Builder<Model>::parseWithRelations(const QVector<WithItem> &relations)
    {
        QVector<WithItem> results;
        // Can contain nested relations
        results.reserve(relations.size() * 2);

        for (auto relation : relations) {
            const auto isSelectConstraint = relation.name.contains(QChar(':'));

            if (isSelectConstraint && relation.constraints)
                throw RuntimeError(
                        "Passing both 'Select constraint' and 'Lambda expression "
                        "constraint' to the Model::with() method is not allowed, use "
                        "only one of them.");

            if (isSelectConstraint)
                relation = createSelectWithConstraint(relation.name);

            /* We need to separate out any nested includes, which allows the developers
               to load deep relationships using "dots" without stating each level of
               the relationship with its own key in the vector of eager-load names. */
            addNestedWiths(relation.name, results);

            results.append(std::move(relation));
        }

        return results;
    }

    template<typename Model>
    WithItem Builder<Model>::createSelectWithConstraint(const QString &name)
    {
        auto splitted = name.split(QChar(':'));
        auto relation = splitted.at(0).trimmed();
        auto &columns = splitted[1];

        auto belongsToManyRelatedTable =
                m_model.getRelatedTableForBelongsToManyWithVisitor(relation);

        return {
            std::move(relation),
            [columns = std::move(columns),
                    belongsToManyRelatedTable = std::move(belongsToManyRelatedTable)]
                    (auto &query)
            {
                QVector<Column> columnsList;
                columnsList.reserve(columns.count(QChar(',')) + 1);

                // Avoid 'clazy might detach' warning
                for (const auto columns_ = columns.split(QChar(','));
                     auto column : columns_)
                {
                    column = column.trimmed();

                    // Fully qualified column passed, not needed to process
                    if (column.contains(QChar('.'))) {
                        columnsList << std::move(column);
                        continue;
                    }

                    /* Generate fully qualified column name for the BelongsToMany
                       relation. */
                    if (belongsToManyRelatedTable) {
#ifdef __GNUG__
                        columnsList << QString("%1.%2")
                                       .arg(*belongsToManyRelatedTable, column);
#else
                        columnsList << QStringLiteral("%1.%2")
                                       .arg(*belongsToManyRelatedTable, column);
#endif
                        continue;
                    }

                    columnsList << std::move(column);
                }

                // TODO move, query.select() silverqx
                query.select(std::move(columnsList));
            }
        };
    }

    template<typename Model>
    void Builder<Model>::addNestedWiths(const QString &name,
                                        QVector<WithItem> &results) const
    {
        QStringList progress;

        /* If the relation has already been set on the result vector, we will not set it
           again, since that would override any constraints that were already placed
           on the relationships. We will only set the ones that are not specified. */
        // Prevent container detach
        const auto names = name.split(QChar('.'));

        progress.reserve(names.size());

        for (const auto &segment : names) {
            progress << segment;

            auto last = progress.join(QChar('.'));
            const auto containsRelation = [&last](const auto &relation)
            {
                return relation.name == last;
            };
            const auto contains = ranges::contains(results, true, containsRelation);

            // Don't add a relation in the 'name' variable
            if (!contains && (last != name))
                results.append({std::move(last)});
        }
    }

    template<typename Model>
    QVector<WithItem>
    Builder<Model>::relationsNestedUnder(const QString &topRelationName) const
    {
        QVector<WithItem> nested;

        /* We are basically looking for any relationships that are nested deeper than
           the given top-level relationship. We will just check for any relations
           that start with the given top relations and adds them to our vectors. */
        for (const auto &relation : m_eagerLoad)
            if (isNestedUnder(topRelationName, relation.name))
                nested.append({relation.name.mid(topRelationName.size() + 1),
                               relation.constraints});

        return nested;
    }

    template<typename Model>
    bool Builder<Model>::isNestedUnder(const QString &topRelation,
                                       const QString &nestedRelation) const
    {
        return nestedRelation.contains(QChar('.'))
                && nestedRelation.startsWith(QStringLiteral("%1.").arg(topRelation));
    }

    template<typename Model>
    QVector<UpdateItem>
    Builder<Model>::addUpdatedAtColumn(QVector<UpdateItem> values) const
    {
        const auto &updatedAtColumn = m_model.getUpdatedAtColumn();
        const auto &qualifiedUpdatedAtColumn = m_model.getQualifiedUpdatedAtColumn();

        if (!m_model.usesTimestamps() || updatedAtColumn.isEmpty())
            return values;

        const auto valuesUpdatedAtColumn =
                std::ranges::find_if(values,
                                     [&updatedAtColumn](const auto &updateItem)
        {
            return updateItem.column == updatedAtColumn;
        });

        // Not found
        if (valuesUpdatedAtColumn == std::ranges::cend(values))
            values.append({qualifiedUpdatedAtColumn,
                           m_model.freshTimestampString()});
        else
            // Rename updated_at column to the qualified column
            valuesUpdatedAtColumn->column = qualifiedUpdatedAtColumn;

        return values;
    }

} // namespace Orm::Tiny
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // TINYBUILDER_H
