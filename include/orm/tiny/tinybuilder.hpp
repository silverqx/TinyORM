#ifndef TINYBUILDER_H
#define TINYBUILDER_H

#include <QtSql/QSqlRecord>

#include <any>

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/move.hpp>
#include <range/v3/iterator/insert_iterators.hpp>
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

    template<typename Model, typename ...AllRelations>
    class BaseModel;

namespace Relations
{
    template<class Model, class Related>
    class Relation;
}

    /*! The type returned by Model's relation methods. */
    template<typename Model, typename Related>
    using RelationType = std::unique_ptr<Relations::Relation<Model, Related>>(Model::*)();

    using JoinClause = Orm::Query::JoinClause;

    template<typename Model>
    class Builder
    {
    public:
        Builder(const QSharedPointer<QueryBuilder> query, Model &model);

        /*! Execute the query as a "select" statement. */
        QVector<Model> get(const QStringList &columns = {"*"});

        // TODO dilmma primarykey, Model::KeyType for id silverqx
        /*! Find a model by its primary key. */
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
        /*! Add a basic where clause to the query, and return the first result. */
        std::optional<Model>
        firstWhereEq(const QString &column, const QVariant &value,
                     const QString &condition = "and");

        /*! Get a single column's value from the first result of a query. */
        QVariant value(const QString &column);

        /*! Set the relationships that should be eager loaded. */
        Builder &with(const QVector<WithItem> &relations);
        /*! Set the relationships that should be eager loaded. */
        Builder &with(const QString &relation);
        /*! Prevent the specified relations from being eager loaded. */
        Builder &without(const QVector<QString> &relations);
        /*! Prevent the specified relations from being eager loaded. */
        Builder &without(const QString &relation);

        /*! Save a new model and return the instance. */
        Model create(const QVector<AttributeItem> &attributes);
        /*! Save a new model and return the instance. */
        Model create(QVector<AttributeItem> &&attributes);

        /* BuildsQueries */
        // TODO BuildsQueries contains duplicit methods in TinyBuilder and QueryBuilder, make it by multi inheritance, I discovered now, that TinyBuilder will return different types than QueryBuilder, look eg at first() or get(), but investigate if there are cases, when API is same and use multi inheritance pattern for this methods silver
        /*! Execute the query and get the first result. */
        std::optional<Model> first(const QStringList &columns = {"*"});

        /* Proxy methods to the QueryBuilder */
        /* Insert, Update, Delete */
        /*! Insert new records into the database. */
        std::tuple<bool, std::optional<QSqlQuery>>
        insert(const QVector<AttributeItem> &attributes) const;
        /*! Insert a new record and get the value of the primary key. */
        quint64 insertGetId(const QVector<AttributeItem> &attributes) const;

        /*! Update records in the database. */
        std::tuple<int, QSqlQuery>
        update(const QVector<UpdateItem> &values) const;
        /*! Create or update a record matching the attributes, and fill it with values. */
        Model updateOrCreate(const QVector<WhereItem> &attributes,
                             const QVector<AttributeItem> &values = {});

        /*! Delete records from the database. */
        std::tuple<int, QSqlQuery> remove();
        /*! Delete records from the database. */
        std::tuple<int, QSqlQuery> deleteModels();

        /*! Run a truncate statement on the table. */
        std::tuple<bool, QSqlQuery> truncate();

        /* Select */
        /*! Set the columns to be selected. */
        Builder &select(const QStringList columns = {"*"});
        /*! Set the column to be selected. */
        Builder &select(const QString column);
        /*! Add new select columns to the query. */
        Builder &addSelect(const QStringList &columns);
        /*! Add a new select column to the query. */
        Builder &addSelect(const QString &column);

        /*! Force the query to only return distinct results. */
        Builder &distinct();

        /*! Add a join clause to the query. */
        Builder &join(const QString &table, const QString &first,
                      const QString &comparison, const QString &second,
                      const QString &type = "inner", bool where = false);
        /*! Add an advanced join clause to the query. */
        Builder &join(const QString &table,
                      const std::function<void(JoinClause &)> &callback,
                      const QString &type = "inner");
        /*! Add a "join where" clause to the query. */
        Builder &joinWhere(const QString &table, const QString &first,
                           const QString &comparison, const QString &second,
                           const QString &type = "inner");
        /*! Add a left join to the query. */
        Builder &leftJoin(const QString &table, const QString &first,
                          const QString &comparison, const QString &second);
        /*! Add an advanced left join to the query. */
        Builder &leftJoin(const QString &table,
                          const std::function<void(JoinClause &)> &callback);
        /*! Add a "join where" clause to the query. */
        Builder &leftJoinWhere(const QString &table, const QString &first,
                               const QString &comparison, const QString &second);
        /*! Add a right join to the query. */
        Builder &rightJoin(const QString &table, const QString &first,
                           const QString &comparison, const QString &second);
        /*! Add an advanced right join to the query. */
        Builder &rightJoin(const QString &table,
                           const std::function<void(JoinClause &)> &callback);
        /*! Add a "right join where" clause to the query. */
        Builder &rightJoinWhere(const QString &table, const QString &first,
                                const QString &comparison, const QString &second);
        /*! Add a "cross join" clause to the query. */
        Builder &crossJoin(const QString &table, const QString &first,
                           const QString &comparison, const QString &second);
        /*! Add an advanced "cross join" clause to the query. */
        Builder &crossJoin(const QString &table,
                           const std::function<void(JoinClause &)> &callback);

        /*! Add a basic where clause to the query. */
        Builder &where(const QString &column, const QString &comparison,
                       const QVariant &value, const QString &condition = "and");
        /*! Add an "or where" clause to the query. */
        Builder &orWhere(const QString &column, const QString &comparison,
                         const QVariant &value);
        /*! Add a basic equal where clause to the query. */
        Builder &whereEq(const QString &column, const QVariant &value,
                         const QString &condition = "and");
        /*! Add an equal "or where" clause to the query. */
        Builder &orWhereEq(const QString &column, const QVariant &value);
        /*! Add a nested where clause to the query. */
        Builder &where(const std::function<void(Builder &)> &callback,
                       const QString &condition = "and");
        /*! Add a nested "or where" clause to the query. */
        Builder &orWhere(const std::function<void(Builder &)> &callback);

        /*! Add an array of basic where clauses to the query. */
        Builder &where(const QVector<WhereItem> &values,
                       const QString &condition = "and");
        /*! Add an array of basic "or where" clauses to the query. */
        Builder &orWhere(const QVector<WhereItem> &values);

        /*! Add an array of where clauses comparing two columns to the query. */
        Builder &whereColumn(const QVector<WhereColumnItem> &values,
                             const QString &condition = "and");
        /*! Add an array of "or where" clauses comparing two columns to the query. */
        Builder &orWhereColumn(const QVector<WhereColumnItem> &values);

        /*! Add a "where" clause comparing two columns to the query. */
        Builder &whereColumn(const QString &first, const QString &comparison,
                             const QString &second, const QString &condition = "and");
        /*! Add a "or where" clause comparing two columns to the query. */
        Builder &orWhereColumn(const QString &first, const QString &comparison,
                               const QString &second);
        /*! Add an equal "where" clause comparing two columns to the query. */
        Builder &whereColumnEq(const QString &first, const QString &second,
                               const QString &condition = "and");
        /*! Add an equal "or where" clause comparing two columns to the query. */
        Builder &orWhereColumnEq(const QString &first, const QString &second);

        /*! Add a "where in" clause to the query. */
        Builder &whereIn(const QString &column, const QVector<QVariant> &values,
                         const QString &condition = "and", bool nope = false);
        /*! Add an "or where in" clause to the query. */
        Builder &orWhereIn(const QString &column, const QVector<QVariant> &values);
        /*! Add a "where not in" clause to the query. */
        Builder &whereNotIn(const QString &column, const QVector<QVariant> &values,
                            const QString &condition = "and");
        /*! Add an "or where not in" clause to the query. */
        Builder &orWhereNotIn(const QString &column, const QVector<QVariant> &values);

        /*! Add a "where null" clause to the query. */
        Builder &whereNull(const QStringList &columns = {"*"},
                           const QString &condition = "and", bool nope = false);
        /*! Add a "where null" clause to the query. */
        Builder &whereNull(const QString &column, const QString &condition = "and",
                           bool nope = false);
        /*! Add an "or where null" clause to the query. */
        Builder &orWhereNull(const QStringList &columns = {"*"});
        /*! Add an "or where null" clause to the query. */
        Builder &orWhereNull(const QString &column);
        /*! Add a "where not null" clause to the query. */
        Builder &whereNotNull(const QStringList &columns = {"*"},
                              const QString &condition = "and");
        /*! Add a "where not null" clause to the query. */
        Builder &whereNotNull(const QString &column, const QString &condition = "and");
        /*! Add an "or where not null" clause to the query. */
        Builder &orWhereNotNull(const QStringList &columns = {"*"});
        /*! Add an "or where not null" clause to the query. */
        Builder &orWhereNotNull(const QString &column);

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
                  const QVector<UpdateItem> &extra = {});
        /*! Decrement a column's value by a given amount. */
        template<typename T> requires std::is_arithmetic_v<T>
        std::tuple<int, QSqlQuery>
        decrement(const QString &column, T amount = 1,
                  const QVector<UpdateItem> &extra = {});

        /* TinyBuilder methods */
        /*! Add a where clause on the primary key to the query. */
        Builder &whereKey(const QVariant &id);
        /*! Add a where clause on the primary key to the query. */
        Builder &whereKeyNot(const QVariant &id);

        /*! Create a new instance of the model being queried. */
        Model newModelInstance(const QVector<AttributeItem> &attributes = {});

        /*! Get the hydrated models without eager loading. */
        QVector<Model> getModels(const QStringList &columns = {"*"});
        /*! Eager load the relationships for the models. */
        void eagerLoadRelations(QVector<Model> &models);
        /*! Eagerly load the relationship on a set of models. */
        template<typename Related>
        void eagerLoadRelation(QVector<Model> &models, const WithItem &relationItem);
        /*! Get the relation instance for the given relation name. */
        template<typename Related>
        auto getRelation(const QString &name, Model &dummyModel);
        /*! Create a collection of models from QSqlQuery. */
        QVector<Model> hydrate(QSqlQuery result);

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
        // TODO future add Query Scopes feature silverqx
//        { return $this->applyScopes()->getQuery(); }

    protected:
        /*! Parse a list of relations into individuals. */
        QVector<WithItem> parseWithRelations(const QVector<WithItem> &relations) const;
        /*! Create a constraint to select the given columns for the relation. */
        WithItem createSelectWithConstraint(const QString &name) const;
        /*! Parse the nested relationships in a relation. */
        void addNestedWiths(const QString &name, QVector<WithItem> &results) const;

        /*! Get the deeply nested relations for a given top-level relation. */
        QVector<WithItem>
        relationsNestedUnder(const QString &topRelationName) const;
        /*! Determine if the relationship is nested. */
        bool isNestedUnder(const QString &topRelation,
                           const QString &nestedRelation) const;

        /*! Add the "updated at" column to an array of values. */
        QVector<UpdateItem>
        addUpdatedAtColumn(QVector<UpdateItem> values) const;

        /*! Get the name of the "created at" column. */
        QString getCreatedAtColumnForLatestOldest(QString column) const;

        /*! The base query builder instance. */
        const QSharedPointer<QueryBuilder> m_query;
        /*! The model being queried. */
        Model m_model;
        /*! The relationships that should be eager loaded. */
        QVector<WithItem> m_eagerLoad;

    private:
        /*! Join attributes for firstOrXx methods. */
        QVector<AttributeItem>
        joinAttributesForFirstOr(const QVector<WhereItem> &attributes,
                                 const QVector<AttributeItem> &values);
    };

    template<typename Model>
    Builder<Model>::Builder(const QSharedPointer<QueryBuilder> query,
                            Model &model)
        : m_query(query)
        , m_model(model)
    {
        m_query->from(m_model.getTable());
    }

    // TODO now name QVector<Model> model collections by using, eg CollectionType silverqx
    template<typename Model>
    QVector<Model>
    Builder<Model>::get(const QStringList &columns)
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

    template<typename Model>
    std::optional<Model>
    Builder<Model>::find(const QVariant &id, const QStringList &columns)
    {
//        if (is_array($id) || $id instanceof Arrayable) {
//            return $this->findMany($id, $columns);
//        }

        return whereKey(id).first(columns);
    }

    template<typename Model>
    Model Builder<Model>::findOrNew(const QVariant &id, const QStringList &columns)
    {
        auto model = find(id, columns);

        // Found
        if (model)
            return *model;

        return newModelInstance();
    }

    template<typename Model>
    Model Builder<Model>::findOrFail(const QVariant &id, const QStringList &columns)
    {
        auto model = find(id, columns);

        // Found
        if (model)
            return *model;

        throw ModelNotFoundError(Utils::Type::classPureBasename<Model>(), {id});
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

        return newModelInstance(joinAttributesForFirstOr(attributes, values));
    }

    template<typename Model>
    Model
    Builder<Model>::firstOrCreate(const QVector<WhereItem> &attributes,
                                  const QVector<AttributeItem> &values)
    {
        auto instance = where(attributes).first();

        // Model found in db
        if (instance)
            return *instance;

        auto newInstance = newModelInstance(
                               joinAttributesForFirstOr(attributes, values));
        newInstance.save();

        return newInstance;
    }

    template<typename Model>
    Model Builder<Model>::firstOrFail(const QStringList &columns)
    {
        auto model = first(columns);

        // Found
        if (model)
            return *model;

        throw ModelNotFoundError(Utils::Type::classPureBasename<Model>());
    }

    template<typename Model>
    std::optional<Model>
    Builder<Model>::firstWhere(const QString &column, const QString &comparison,
                               const QVariant &value, const QString &condition)
    {
        return where(column, comparison, value, condition).first();
    }

    template<typename Model>
    std::optional<Model>
    Builder<Model>::firstWhereEq(const QString &column, const QVariant &value,
                                 const QString &condition)
    {
        return where(column, QStringLiteral("="), value, condition).first();
    }

    // TODO dilemma raw expression silverqx
    template<typename Model>
    QVariant Builder<Model>::value(const QString &column)
    {
        auto model = first({column});

        if (!model)
            return {};

        return model->getAttribute(column.mid(column.lastIndexOf(QChar('.')) + 1));
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::with(const QVector<WithItem> &relations)
    {
        auto eagerLoad = parseWithRelations(relations);

        ranges::move(eagerLoad, ranges::back_inserter(m_eagerLoad));

        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::with(const QString &relation)
    {
        return with(QVector<WithItem> {{relation}});
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
    std::optional<Model>
    Builder<Model>::first(const QStringList &columns)
    {
        const auto models = take(1).get(columns);

        if (models.isEmpty())
            return std::nullopt;

        return models.first();
    }

    template<typename Model>
    std::tuple<bool, std::optional<QSqlQuery>>
    Builder<Model>::insert(const QVector<AttributeItem> &attributes) const
    {
        return toBase().insert(Utils::Attribute::convertVectorToMap(attributes));
    }

    // TODO dilemma primarykey, Model::KeyType vs QVariant silverqx
    template<typename Model>
    quint64
    Builder<Model>::insertGetId(const QVector<AttributeItem> &attributes) const
    {
        return toBase().insertGetId(Utils::Attribute::convertVectorToMap(attributes));
    }

    template<typename Model>
    std::tuple<int, QSqlQuery>
    Builder<Model>::update(const QVector<UpdateItem> &values) const
    {
        return toBase().update(addUpdatedAtColumn(values));
    }

    template<typename Model>
    Model Builder<Model>::updateOrCreate(const QVector<WhereItem> &attributes,
                                         const QVector<AttributeItem> &values)
    {
        auto instance = firstOrNew(attributes);

        instance.fill(values).save();

        return instance;
    }

    // TODO future add onDelete (and similar) callback feature silverqx
    template<typename Model>
    std::tuple<int, QSqlQuery> Builder<Model>::remove()
    {
        return toBase().deleteRow();
    }

    template<typename Model>
    std::tuple<int, QSqlQuery> Builder<Model>::deleteModels()
    {
        return remove();
    }

    template<typename Model>
    std::tuple<bool, QSqlQuery> Builder<Model>::truncate()
    {
        return toBase().truncate();
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::select(const QStringList columns)
    {
        toBase().select(columns);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::select(const QString column)
    {
        toBase().select(column);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::addSelect(const QStringList &columns)
    {
        toBase().addSelect(columns);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::addSelect(const QString &column)
    {
        toBase().addSelect(column);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::distinct()
    {
        toBase().distinct();
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::join(const QString &table, const QString &first,
                         const QString &comparison, const QString &second,
                         const QString &type, const bool where)
    {
        toBase().join(table, first, comparison, second, type, where);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::join(const QString &table,
                         const std::function<void(JoinClause &)> &callback,
                         const QString &type)
    {
        toBase().join(table, callback, type);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::joinWhere(const QString &table, const QString &first,
                              const QString &comparison, const QString &second,
                              const QString &type)
    {
        toBase().joinWhere(table, first, comparison, second, type);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::leftJoin(const QString &table, const QString &first,
                             const QString &comparison, const QString &second)
    {
        toBase().leftJoin(table, first, comparison, second);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::leftJoin(const QString &table,
                             const std::function<void(JoinClause &)> &callback)
    {
        toBase().leftJoin(table, callback);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::leftJoinWhere(const QString &table, const QString &first,
                                  const QString &comparison, const QString &second)
    {
        toBase().leftJoinWhere(table, first, comparison, second);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::rightJoin(const QString &table, const QString &first,
                              const QString &comparison, const QString &second)
    {
        toBase().rightJoin(table, first, comparison, second);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::rightJoin(const QString &table,
                              const std::function<void(JoinClause &)> &callback)
    {
        toBase().rightJoin(table, callback);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::rightJoinWhere(const QString &table, const QString &first,
                                   const QString &comparison, const QString &second)
    {
        toBase().rightJoinWhere(table, first, comparison, second);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::crossJoin(const QString &table, const QString &first,
                              const QString &comparison, const QString &second)
    {
        toBase().crossJoin(table, first, comparison, second);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::crossJoin(const QString &table,
                              const std::function<void(JoinClause &)> &callback)
    {
        toBase().crossJoin(table, callback);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::where(const QString &column, const QString &comparison,
                          const QVariant &value, const QString &condition)
    {
        toBase().where(column, comparison, value, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhere(const QString &column, const QString &comparison,
                            const QVariant &value)
    {
        toBase().orWhere(column, comparison, value);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereEq(const QString &column, const QVariant &value,
                            const QString &condition)
    {
        toBase().whereEq(column, value, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhereEq(const QString &column, const QVariant &value)
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
    Builder<Model>::whereColumn(const QString &first, const QString &comparison,
                                const QString &second, const QString &condition)
    {
        toBase().whereColumn(first, comparison, second, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhereColumn(const QString &first, const QString &comparison,
                                  const QString &second)
    {
        toBase().orWhereColumn(first, comparison, second);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereColumnEq(const QString &first, const QString &second,
                                  const QString &condition)
    {
        toBase().whereColumnEq(first, second, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhereColumnEq(const QString &first, const QString &second)
    {
        toBase().orWhereColumnEq(first, second);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereIn(const QString &column, const QVector<QVariant> &values,
                            const QString &condition, const bool nope)
    {
        toBase().whereIn(column, values, condition, nope);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhereIn(const QString &column, const QVector<QVariant> &values)
    {
        toBase().orWhereIn(column, values);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereNotIn(const QString &column, const QVector<QVariant> &values,
                               const QString &condition)
    {
        toBase().whereNotIn(column, values, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::orWhereNotIn(const QString &column, const QVector<QVariant> &values)
    {
        toBase().orWhereNotIn(column, values);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereNull(const QStringList &columns, const QString &condition,
                              bool nope)
    {
        toBase().whereNull(columns, condition, nope);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereNull(const QString &column, const QString &condition,
                              const bool nope)
    {
        toBase().whereNull(column, condition, nope);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::orWhereNull(const QStringList &columns)
    {
        toBase().orWhereNull(columns);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::orWhereNull(const QString &column)
    {
        toBase().orWhereNull(column);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereNotNull(const QStringList &columns, const QString &condition)
    {
        toBase().whereNotNull(columns, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereNotNull(const QString &column, const QString &condition)
    {
        toBase().whereNotNull(column, condition);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::orWhereNotNull(const QStringList &columns)
    {
        toBase().orWhereNotNull(columns);
        return *this;
    }

    template<typename Model>
    Builder<Model> &Builder<Model>::orWhereNotNull(const QString &column)
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
                              const QVector<UpdateItem> &extra)
    {
        return toBase().template increment<T>(column, amount,
                                              addUpdatedAtColumn(extra));
    }

    template<typename Model>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, QSqlQuery>
    Builder<Model>::decrement(const QString &column, const T amount,
                              const QVector<UpdateItem> &extra)
    {
        return toBase().template decrement<T>(column, amount,
                                              addUpdatedAtColumn(extra));
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereKey(const QVariant &id)
    {
        return where(m_model.getQualifiedKeyName(), QStringLiteral("="), id);
    }

    template<typename Model>
    Builder<Model> &
    Builder<Model>::whereKeyNot(const QVariant &id)
    {
        return where(m_model.getQualifiedKeyName(), QStringLiteral("!="), id);
    }

    template<typename Model>
    Model Builder<Model>::newModelInstance(const QVector<AttributeItem> &attributes)
    {
        return m_model.newInstance(attributes)
                .setConnection(m_query->getConnection().getName());
    }

    template<typename Model>
    QVector<Model>
    Builder<Model>::getModels(const QStringList &columns)
    {
        return hydrate(std::get<1>(m_query->get(columns)));
    }

    template<typename Model>
    void Builder<Model>::eagerLoadRelations(QVector<Model> &models)
    {
        if (m_eagerLoad.isEmpty())
            return;

        for (const auto &relation : qAsConst(m_eagerLoad))
            /* For nested eager loads we'll skip loading them here and they will be set
               as an eager load on the query to retrieve the relation so that they will
               be eager loaded on that query, because that is where they get hydrated
               as models. */
            if (!relation.name.contains(QChar('.')))
                m_model.eagerLoadRelationVisitor(relation, *this, models);
    }

    template<typename Model>
    template<typename Related>
    void Builder<Model>::eagerLoadRelation(QVector<Model> &models,
                                           const WithItem &relationItem)
    {
        /*! Helping model for eager loads, because Relation::m_parent has to be
            reference, this dummy model prevents dangling reference, we have to secure
            that the model passed to the relation method called inside getRelation()
            will live long enough, to not become a dangling reference.
            Have to exists, until the 'relation->match()' is processed in this method.
            Look at Relation::m_parent for additional info. */
        auto dummyModel = getModel().newInstance();

        /* First we will "back up" the existing where conditions on the query so we can
           add our eager constraints. Then we will merge the wheres that were on the
           query back to it in order that any where conditions might be specified. */
        auto relation = getRelation<Related>(relationItem.name, dummyModel);

        relation->addEagerConstraints(models);

        // TODO next add support for std::invoke(relationItem.constraints); silverqx
        // Add relation contraints defined in a user callback
//        std::invoke(relationItem.constraints);

        /* Once we have the results, we just match those back up to their parent models
           using the relationship instance. Then we just return the finished arrays
           of models which have been eagerly hydrated and are readied for return. */
        relation->match(relation->initRelation(models, relationItem.name),
                        relation->getEager(), relationItem.name);
    }

    template<typename Model>
    template<typename Related>
    auto Builder<Model>::getRelation(const QString &name, Model &dummyModel)
    {
        // TODO docs add similar note for lazy load silverqx
        /* How this relation flow works:
           m_model.getRelationMethod<Related>() obtains relation method by relation
           name, these relation methods are defined on models, relation constraints
           will be disabled for eager relations by Relation::noConstraints() method,
           these default constraints are only used for lazy loading, for eager
           constraints are used constraints, which are defined
           by Relation::addEagerConstraints() virtual methods.
           To the Relation::noConstraints() method is passed lambda, which invokes
           obtained relation method and invokes it on the new model instance.
           The Relation instance is created by this relation method, this relation
           method calls factory method, which creates the Relation instance.
           Every Relation has it's own Relation::create() factory method, to which
           the following parameters are passed, newly created Related model instance,
           current/parent model instance, database column names of relationship, and
           for BelongsTo relation also the name of the relation.
           The Relation instance creates a copy of the current/parent model instance,
           a copy of the related model instance because they are created on the stack.
           The Relation instance creates a new TinyBuilder instance from the Related
           model instance by TinyBuilder::newQuery() and saves ownership as
           a shared pointer.
           Then eager constraints are applied to this newly created TinyBuilder and
           the result is returned back to the initial model.
           The result is transformed into models and these models are hydrated.
           Hydrated models are saved to the BaseModel::m_relations data member. */

        auto method = m_model.template getRelationMethod<Related>(name);

        /* We want to run a relationship query without any constrains so that we will
           not have to remove these where clauses manually which gets really hacky
           and error prone. We don't want constraints because we add eager ones. */
        auto relation = Relations::Relation<Model, Related>::noConstraints(
                    [this, &method, &dummyModel]
        {
            return std::invoke(method, dummyModel);
        });

        const auto nested = relationsNestedUnder(name);

        /* If there are nested relationships set on the query, we will put those onto
           the query instances so that they can be handled after this relationship
           is loaded. In this way they will all trickle down as they are loaded. */
        if (nested.size() > 0)
            relation->getQuery().with(nested);

        return relation;
    }

    template<typename Model>
    QVector<Model>
    Builder<Model>::hydrate(QSqlQuery result)
    {
        auto instance = newModelInstance();

        QVector<Model> models;

        while (result.next()) {
            // Table row
            QVector<AttributeItem> row;

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
    Builder<Model>::parseWithRelations(const QVector<WithItem> &relations) const
    {
        QVector<WithItem> results;

        for (auto relation : relations) {
            const auto emptyConstraints = !relation.constraints;
            const auto isSelectConstraint = relation.name.contains(QChar(':'));

            // TODO next Eager Loading Specific Columns silverqx
            /* Select columns constraints are only allowed, when relation.constraints
               is nullptr. */
            if (isSelectConstraint)
                Q_ASSERT(!emptyConstraints);

            if (emptyConstraints && isSelectConstraint)
                relation = createSelectWithConstraint(relation.name);

            /* We need to separate out any nested includes, which allows the developers
               to load deep relationships using "dots" without stating each level of
               the relationship with its own key in the array of eager-load names. */
            addNestedWiths(relation.name, results);

            results.append(relation);
        }

        return results;
    }

    template<typename Model>
    WithItem Builder<Model>::createSelectWithConstraint(const QString &name) const
    {
        return {
            name.split(QChar(':')).first(), [name](/*auto &query*/)
            {
//                query->select(name.split(QChar(':')).last().split(QChar(',')));
            }
        };
    }

    template<typename Model>
    void Builder<Model>::addNestedWiths(const QString &name,
                                        QVector<WithItem> &results) const
    {
        QStringList progress;

        /* If the relation has already been set on the result array, we will not set it
           again, since that would override any constraints that were already placed
           on the relationships. We will only set the ones that are not specified. */
        // Prevent container detach
        const auto names = name.split(QChar('.'));
        for (const auto &segment : names) {
            progress << segment;

            const auto last = progress.join(QChar('.'));
            const auto containsRelation = [&last](const auto &relation)
            {
                return relation.name == last;
            };
            const auto contains = ranges::contains(results, true, containsRelation);

            // Don't add a relation in the 'name' variable
            if (!contains && (last != name))
                results.append({last});
        }
    }

    template<typename Model>
    QVector<WithItem>
    Builder<Model>::relationsNestedUnder(const QString &topRelationName) const
    {
        QVector<WithItem> nested;

        /* We are basically looking for any relationships that are nested deeper than
           the given top-level relationship. We will just check for any relations
           that start with the given top relations and adds them to our arrays. */
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
                && nestedRelation.startsWith(topRelation + QChar('.'));
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
                ranges::find_if(values,
                                [&updatedAtColumn](const auto &updateItem)
        {
            return updateItem.column == updatedAtColumn;
        });

        // Not found
        if (valuesUpdatedAtColumn == ranges::end(values))
            values.append({qualifiedUpdatedAtColumn,
                           m_model.freshTimestampString()});
        else
            // Rename updated_at column to the qualified column
            valuesUpdatedAtColumn->column = qualifiedUpdatedAtColumn;

        return values;
    }

    template<typename Model>
    QVector<AttributeItem>
    Builder<Model>::joinAttributesForFirstOr(const QVector<WhereItem> &attributes,
                                             const QVector<AttributeItem> &values)
    {
        // Convert attributes to the QVector<AttributeItem>, so they can be
        QVector<AttributeItem> attributesConverted;
        ranges::copy(attributes, ranges::back_inserter(attributesConverted));

        // Attributes which already exist in 'attributes' will be removed from 'values'
        using namespace ranges;
        auto valuesFiltered =
                values | views::remove_if(
                    [&attributesConverted](const AttributeItem &value)
        {
            return ranges::contains(attributesConverted, true,
                                    [&value](const AttributeItem &attribute)
            {
                return attribute.key == value.key;
            });
        })
                | ranges::to<QVector<AttributeItem>>();

        return attributesConverted + valuesFiltered;
    }

} // namespace Orm::Tiny
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // TINYBUILDER_H
