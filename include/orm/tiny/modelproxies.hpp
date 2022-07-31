#pragma once
#ifndef ORM_TINY_MODELPROXIES_HPP
#define ORM_TINY_MODELPROXIES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtSql/QSqlQuery>

#include "orm/ormconcepts.hpp"
#include "orm/tiny/tinytypes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

namespace Query
{
    class JoinClause;
}

namespace Tiny
{
namespace Relations
{
    template<class Model, class Related>
    class Relation;
}

    /*! Contains proxy methods to the TinyBuilder. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class ModelProxies
    {
        /*! JoinClause alias. */
        using JoinClause = Orm::Query::JoinClause;
        /*! Alias for the QueriesRelationships callback type. */
        template<typename Related>
        using CallbackType = Concerns::QueriesRelationshipsCallback<Related>;
        /*! Alias for the Relations::Relation. */
        template<typename Related>
        using RelationAlias = Orm::Tiny::Relations::Relation<Derived, Related>;

    public:
        /* Retrieving results */
        /*! Get a single column's value from the first result of a query. */
        static QVariant value(const Column &column);
        /*! Get a single column's value from the first result of a query if it's
            the sole matching record. */
        static QVariant soleValue(const Column &column);

        /*! Get the vector with the values of a given column. */
        static QVector<QVariant> pluck(const QString &column);
        /*! Get the vector with the values of a given column. */
        template<typename T>
        static std::map<T, QVariant> pluck(const QString &column, const QString &key);

        /*! Concatenate values of a given column as a string. */
        static QString implode(const QString &column, const QString &glue = "");

        /*! Find a model by its primary key. */
        static std::optional<Derived>
        find(const QVariant &id, const QVector<Column> &columns = {ASTERISK});
        /*! Find a model by its primary key or return fresh model instance. */
        static Derived
        findOrNew(const QVariant &id, const QVector<Column> &columns = {ASTERISK});
        /*! Find a model by its primary key or throw an exception. */
        static Derived
        findOrFail(const QVariant &id, const QVector<Column> &columns = {ASTERISK});
        /*! Find multiple models by their primary keys. */
        static QVector<Derived>
        findMany(const QVector<QVariant> &ids,
                 const QVector<Column> &columns = {ASTERISK});

        /*! Execute a query for a single record by ID or call a callback. */
        static std::optional<Derived>
        findOr(const QVariant &id, const QVector<Column> &columns,
               const std::function<void()> &callback);
        /*! Execute a query for a single record by ID or call a callback. */
        static std::optional<Derived>
        findOr(const QVariant &id, const std::function<void()> &callback);

        /*! Execute a query for a single record by ID or call a callback. */
        template<typename R>
        static std::pair<std::optional<Derived>, R>
        findOr(const QVariant &id, const QVector<Column> &columns,
               const std::function<R()> &callback);
        /*! Execute a query for a single record by ID or call a callback. */
        template<typename R>
        static std::pair<std::optional<Derived>, R>
        findOr(const QVariant &id, const std::function<R()> &callback);

        /*! Execute the query and get the first result. */
        static std::optional<Derived>
        first(const QVector<Column> &columns = {ASTERISK});
        /*! Get the first record matching the attributes or instantiate it. */
        static Derived
        firstOrNew(const QVector<WhereItem> &attributes = {},
                   const QVector<AttributeItem> &values = {});
        /*! Get the first record matching the attributes or create it. */
        static Derived
        firstOrCreate(const QVector<WhereItem> &attributes = {},
                      const QVector<AttributeItem> &values = {});
        /*! Execute the query and get the first result or throw an exception. */
        static Derived firstOrFail(const QVector<Column> &columns = {ASTERISK});

        /*! Execute the query and get the first result or call a callback. */
        static std::optional<Derived>
        firstOr(const QVector<Column> &columns,
                const std::function<void()> &callback = nullptr);
        /*! Execute the query and get the first result or call a callback. */
        static std::optional<Derived>
        firstOr(const std::function<void()> &callback = nullptr);

        /*! Execute the query and get the first result or call a callback. */
        template<typename R>
        static std::pair<std::optional<Derived>, R>
        firstOr(const QVector<Column> &columns, const std::function<R()> &callback);
        /*! Execute the query and get the first result or call a callback. */
        template<typename R>
        static std::pair<std::optional<Derived>, R>
        firstOr(const std::function<R()> &callback);

        /*! Add a basic where clause to the query, and return the first result. */
        static std::optional<Derived>
        firstWhere(const Column &column, const QString &comparison,
                   const QVariant &value, const QString &condition = AND);
        /*! Add a basic equal where clause to the query, and return the first result. */
        static std::optional<Derived>
        firstWhereEq(const Column &column, const QVariant &value,
                     const QString &condition = AND);

        /*! Add a where clause on the primary key to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereKey(const QVariant &id);
        /*! Add a where clause on the primary key to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereKey(const QVector<QVariant> &ids);
        /*! Add a where clause on the primary key to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereKeyNot(const QVariant &id);
        /*! Add a where clause on the primary key to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereKeyNot(const QVector<QVariant> &ids);

        /*! Begin querying a model with eager loading. */
        template<typename = void>
        static std::unique_ptr<TinyBuilder<Derived>>
        with(const QVector<WithItem> &relations);
        /*! Begin querying a model with eager loading. */
        template<typename = void>
        static std::unique_ptr<TinyBuilder<Derived>>
        with(const QString &relation);
        /*! Begin querying a model with eager loading. */
        static std::unique_ptr<TinyBuilder<Derived>>
        with(const QVector<QString> &relations);
        /*! Begin querying a model with eager loading. */
        static std::unique_ptr<TinyBuilder<Derived>>
        with(QVector<QString> &&relations);

        /*! Prevent the specified relations from being eager loaded. */
        static std::unique_ptr<TinyBuilder<Derived>>
        without(const QVector<QString> &relations);
        /*! Prevent the specified relations from being eager loaded. */
        static std::unique_ptr<TinyBuilder<Derived>>
        without(const QString &relation);

        /*! Set the relationships that should be eager loaded while removing
            any previously added eager loading specifications. */
        static std::unique_ptr<TinyBuilder<Derived>>
        withOnly(const QVector<WithItem> &relations);
        /*! Set the relationship that should be eager loaded while removing
            any previously added eager loading specifications. */
        static std::unique_ptr<TinyBuilder<Derived>>
        withOnly(const QString &relation);

        /* Insert, Update, Delete */
        /*! Save a new model and return the instance. */
        static Derived create(const QVector<AttributeItem> &attributes = {});
        /*! Save a new model and return the instance. */
        static Derived create(QVector<AttributeItem> &&attributes = {});

        /*! Create or update a record matching the attributes, and fill it with values. */
        Derived updateOrCreate(const QVector<WhereItem> &attributes,
                               const QVector<AttributeItem> &values = {});

        /* Proxies to TinyBuilder -> QueryBuilder */
        /* Insert, Update, Delete */
        /*! Insert a new record into the database. */
        static std::optional<QSqlQuery>
        insert(const QVector<AttributeItem> &values);
        /*! Insert new records into the database. */
        static std::optional<QSqlQuery>
        insert(const QVector<QVector<AttributeItem>> &values);
        /*! Insert new records into the database (multi insert). */
        static std::optional<QSqlQuery>
        insert(const QVector<QString> &columns, QVector<QVector<QVariant>> values);

        /*! Insert a new record and get the value of the primary key. */
        static quint64
        insertGetId(const QVector<AttributeItem> &values,
                    const QString &sequence = "");

        /*! Insert a new record into the database while ignoring errors. */
        static std::tuple<int, std::optional<QSqlQuery>>
        insertOrIgnore(const QVector<AttributeItem> &values);
        /*! Insert new records into the database while ignoring errors. */
        static std::tuple<int, std::optional<QSqlQuery>>
        insertOrIgnore(const QVector<QVector<AttributeItem>> &values);
        /*! Insert new records into the database while ignoring errors (multi insert). */
        static std::tuple<int, std::optional<QSqlQuery>>
        insertOrIgnore(const QVector<QString> &columns,
                       QVector<QVector<QVariant>> values);

        /*! Destroy the models for the given IDs. */
        static std::size_t destroy(const QVector<QVariant> &ids);
        /*! Destroy the model by the given ID. */
        static std::size_t destroy(const QVariant &id);

        /*! Run a truncate statement on the table. */
        static void truncate();

        /* Select */
        /*! Retrieve the "count" result of the query. */
        static quint64 count(const QVector<Column> &columns = {ASTERISK});
        /*! Retrieve the "count" result of the query. */
        template<typename = void>
        static quint64 count(const Column &column);
        /*! Retrieve the minimum value of a given column. */
        static QVariant min(const Column &column);
        /*! Retrieve the maximum value of a given column. */
        static QVariant max(const Column &column);
        /*! Retrieve the sum of the values of a given column. */
        static QVariant sum(const Column &column);
        /*! Retrieve the average of the values of a given column. */
        static QVariant avg(const Column &column);
        /*! Alias for the "avg" method. */
        static QVariant average(const Column &column);

        /*! Execute an aggregate function on the database. */
        static QVariant aggregate(const QString &function,
                                  const QVector<Column> &columns = {ASTERISK});

        /*! Determine if any rows exist for the current query. */
        static bool exists();
        /*! Determine if no rows exist for the current query. */
        static bool doesntExist();

        /*! Execute the given callback if no rows exist for the current query. */
        static bool existsOr(const std::function<void()> &callback);
        /*! Execute the given callback if rows exist for the current query. */
        static bool doesntExistOr(const std::function<void()> &callback);

        /*! Execute the given callback if no rows exist for the current query. */
        template<typename R>
        static std::pair<bool, R> existsOr(const std::function<R()> &callback);
        template<typename R>
        /*! Execute the given callback if rows exist for the current query. */
        static std::pair<bool, R> doesntExistOr(const std::function<R()> &callback);

        /*! Set the columns to be selected. */
        static std::unique_ptr<TinyBuilder<Derived>>
        select(const QVector<Column> &columns = {ASTERISK});
        /*! Set the column to be selected. */
        static std::unique_ptr<TinyBuilder<Derived>>
        select(const Column &column);
        /*! Add new select columns to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        addSelect(const QVector<Column> &columns);
        /*! Add a new select column to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        addSelect(const Column &column);

        /*! Set the columns to be selected. */
        static std::unique_ptr<TinyBuilder<Derived>>
        select(QVector<Column> &&columns);
        /*! Set the column to be selected. */
        static std::unique_ptr<TinyBuilder<Derived>>
        select(Column &&column);
        /*! Add new select columns to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        addSelect(QVector<Column> &&columns);
        /*! Add a new select column to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        addSelect(Column &&column);

        /*! Set a select subquery on the query. */
        template<Queryable T>
        static std::unique_ptr<TinyBuilder<Derived>>
        select(T &&query, const QString &as);
        /*! Add a select subquery to the query. */
        template<Queryable T>
        static std::unique_ptr<TinyBuilder<Derived>>
        addSelect(T &&query, const QString &as);

        /*! Add a subselect expression to the query. */
        template<SubQuery T>
        static std::unique_ptr<TinyBuilder<Derived>>
        selectSub(T &&query, const QString &as);
        /*! Add a new "raw" select expression to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        selectRaw(const QString &expression, const QVector<QVariant> &bindings = {});

        /*! Force the query to only return distinct results. */
        static std::unique_ptr<TinyBuilder<Derived>>
        distinct();
        /*! Force the query to only return distinct results. */
        static std::unique_ptr<TinyBuilder<Derived>>
        distinct(const QStringList &columns);
        /*! Force the query to only return distinct results. */
        static std::unique_ptr<TinyBuilder<Derived>>
        distinct(QStringList &&columns);

        /* Joins */
        /*! Add a join clause to the query. */
        template<JoinTable T>
        static std::unique_ptr<TinyBuilder<Derived>>
        join(T &&table, const QString &first, const QString &comparison,
             const QString &second, const QString &type = INNER, bool where = false);
        /*! Add an advanced join clause to the query. */
        template<JoinTable T>
        static std::unique_ptr<TinyBuilder<Derived>>
        join(T &&table, const std::function<void(JoinClause &)> &callback,
             const QString &type = INNER);
        /*! Add a "join where" clause to the query. */
        template<JoinTable T>
        static std::unique_ptr<TinyBuilder<Derived>>
        joinWhere(T &&table, const QString &first, const QString &comparison,
                  const QVariant &second, const QString &type = INNER);

        /*! Add a left join to the query. */
        template<JoinTable T>
        static std::unique_ptr<TinyBuilder<Derived>>
        leftJoin(T &&table, const QString &first, const QString &comparison,
                 const QString &second);
        /*! Add an advanced left join to the query. */
        template<JoinTable T>
        static std::unique_ptr<TinyBuilder<Derived>>
        leftJoin(T &&table, const std::function<void(JoinClause &)> &callback);
        /*! Add a "join where" clause to the query. */
        template<JoinTable T>
        static std::unique_ptr<TinyBuilder<Derived>>
        leftJoinWhere(T &&table, const QString &first, const QString &comparison,
                      const QVariant &second);

        /*! Add a right join to the query. */
        template<JoinTable T>
        static std::unique_ptr<TinyBuilder<Derived>>
        rightJoin(T &&table, const QString &first, const QString &comparison,
                  const QString &second);
        /*! Add an advanced right join to the query. */
        template<JoinTable T>
        static std::unique_ptr<TinyBuilder<Derived>>
        rightJoin(T &&table, const std::function<void(JoinClause &)> &callback);
        /*! Add a "right join where" clause to the query. */
        template<JoinTable T>
        static std::unique_ptr<TinyBuilder<Derived>>
        rightJoinWhere(T &&table, const QString &first, const QString &comparison,
                       const QVariant &second);

        /*! Add a "cross join" clause to the query. */
        template<JoinTable T>
        static std::unique_ptr<TinyBuilder<Derived>>
        crossJoin(T &&table, const QString &first, const QString &comparison,
                  const QString &second);
        /*! Add an advanced "cross join" clause to the query. */
        template<JoinTable T>
        static std::unique_ptr<TinyBuilder<Derived>>
        crossJoin(T &&table, const std::function<void(JoinClause &)> &callback);
        /*! Add a "cross join" clause to the query. */
        template<JoinTable T>
        static std::unique_ptr<TinyBuilder<Derived>>
        crossJoin(T &&table);

        /*! Add a subquery join clause to the query. */
        template<SubQuery T>
        static std::unique_ptr<TinyBuilder<Derived>>
        joinSub(T &&query, const QString &as, const QString &first,
                const QString &comparison, const QVariant &second,
                const QString &type = INNER, bool where = false);
        /*! Add a subquery join clause to the query. */
        template<SubQuery T>
        static std::unique_ptr<TinyBuilder<Derived>>
        joinSub(T &&query, const QString &as,
                const std::function<void(JoinClause &)> &callback,
                const QString &type = INNER);

        /*! Add a subquery left join to the query. */
        template<SubQuery T>
        static std::unique_ptr<TinyBuilder<Derived>>
        leftJoinSub(T &&query, const QString &as, const QString &first,
                    const QString &comparison, const QVariant &second);
        /*! Add a subquery left join to the query. */
        template<SubQuery T>
        static std::unique_ptr<TinyBuilder<Derived>>
        leftJoinSub(T &&query, const QString &as,
                    const std::function<void(JoinClause &)> &callback);

        /*! Add a subquery right join to the query. */
        template<SubQuery T>
        static std::unique_ptr<TinyBuilder<Derived>>
        rightJoinSub(T &&query, const QString &as, const QString &first,
                     const QString &comparison, const QVariant &second);
        /*! Add a subquery right join to the query. */
        template<SubQuery T>
        static std::unique_ptr<TinyBuilder<Derived>>
        rightJoinSub(T &&query, const QString &as,
                     const std::function<void(JoinClause &)> &callback);

        /* General where */
        /*! Add a basic where clause to the query. */
        template<WhereValue T>
        static std::unique_ptr<TinyBuilder<Derived>>
        where(const Column &column, const QString &comparison, T &&value,
              const QString &condition = AND);
        /*! Add an "or where" clause to the query. */
        template<WhereValue T>
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhere(const Column &column, const QString &comparison, T &&value);
        /*! Add a basic equal where clause to the query. */
        template<WhereValue T>
        static std::unique_ptr<TinyBuilder<Derived>>
        whereEq(const Column &column, T &&value, const QString &condition = AND);
        /*! Add an equal "or where" clause to the query. */
        template<WhereValue T>
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereEq(const Column &column, T &&value);

        /* General where not */
        /*! Add a basic "where not" clause to the query. */
        template<WhereValue T>
        static std::unique_ptr<TinyBuilder<Derived>>
        whereNot(const Column &column, const QString &comparison, T &&value,
                 const QString &condition = AND);
        /*! Add an "or where not" clause to the query. */
        template<WhereValue T>
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereNot(const Column &column, const QString &comparison, T &&value);
        /*! Add a basic equal "where not" clause to the query. */
        template<WhereValue T>
        static std::unique_ptr<TinyBuilder<Derived>>
        whereNotEq(const Column &column, T &&value, const QString &condition = AND);
        /*! Add an equal "or where not" clause to the query. */
        template<WhereValue T>
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereNotEq(const Column &column, T &&value);

        /* Nested where */
        /*! Add a nested where clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        where(const std::function<void(TinyBuilder<Derived> &)> &callback,
              const QString &condition = AND);
        /*! Add a nested "or where" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhere(const std::function<void(TinyBuilder<Derived> &)> &callback);
        /*! Add a nested "where not" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereNot(const std::function<void(TinyBuilder<Derived> &)> &callback,
                 const QString &condition = AND);
        /*! Add a nested "or where not" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereNot(const std::function<void(TinyBuilder<Derived> &)> &callback);

        /* Array where */
        /*! Add a vector of basic where clauses to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        where(const QVector<WhereItem> &values, const QString &condition = AND);
        /*! Add a vector of basic "or where" clauses to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhere(const QVector<WhereItem> &values);
        /*! Add a vector of basic "where not" clauses to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereNot(const QVector<WhereItem> &values, const QString &condition = AND,
                 const QString &defaultCondition = "");
        /*! Add a vector of basic "or where not" clauses to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereNot(const QVector<WhereItem> &values,
                   const QString &defaultCondition = "");

        /* where column */
        /*! Add a vector of where clauses comparing two columns to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereColumn(const QVector<WhereColumnItem> &values,
                    const QString &condition = AND);
        /*! Add a vector of "or where" clauses comparing two columns to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereColumn(const QVector<WhereColumnItem> &values);

        /*! Add a "where" clause comparing two columns to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereColumn(const Column &first, const QString &comparison,
                    const Column &second, const QString &condition = AND);
        /*! Add a "or where" clause comparing two columns to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereColumn(const Column &first, const QString &comparison,
                      const Column &second);
        /*! Add an equal "where" clause comparing two columns to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereColumnEq(const Column &first, const Column &second,
                      const QString &condition = AND);
        /*! Add an equal "or where" clause comparing two columns to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereColumnEq(const Column &first, const Column &second);

        /* where IN */
        /*! Add a "where in" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereIn(const Column &column, const QVector<QVariant> &values,
                const QString &condition = AND, bool nope = false);
        /*! Add an "or where in" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereIn(const Column &column, const QVector<QVariant> &values);
        /*! Add a "where not in" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereNotIn(const Column &column, const QVector<QVariant> &values,
                   const QString &condition = AND);
        /*! Add an "or where not in" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereNotIn(const Column &column, const QVector<QVariant> &values);

        /* where null */
        /*! Add a "where null" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereNull(const QVector<Column> &columns = {ASTERISK},
                  const QString &condition = AND, bool nope = false);
        /*! Add an "or where null" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereNull(const QVector<Column> &columns = {ASTERISK});
        /*! Add a "where not null" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereNotNull(const QVector<Column> &columns = {ASTERISK},
                     const QString &condition = AND);
        /*! Add an "or where not null" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereNotNull(const QVector<Column> &columns = {ASTERISK});

        /*! Add a "where null" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereNull(const Column &column, const QString &condition = AND,
                  bool nope = false);
        /*! Add an "or where null" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereNull(const Column &column);
        /*! Add a "where not null" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereNotNull(const Column &column, const QString &condition = AND);
        /*! Add an "or where not null" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereNotNull(const Column &column);

        /* where sub-queries */
        /*! Add a basic where clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        static std::unique_ptr<TinyBuilder<Derived>>
        where(C &&column, const QString &comparison, V &&value,
              const QString &condition = AND);
        /*! Add an "or where" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhere(C &&column, const QString &comparison, V &&value);
        /*! Add a basic equal where clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        static std::unique_ptr<TinyBuilder<Derived>>
        whereEq(C &&column, V &&value, const QString &condition = AND);
        /*! Add an equal "or where" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereEq(C &&column, V &&value);

        /* where not sub-queries */
        /*! Add a basic "where not" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        static std::unique_ptr<TinyBuilder<Derived>>
        whereNot(C &&column, const QString &comparison, V &&value,
                 const QString &condition = AND);
        /*! Add an "or where not" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereNot(C &&column, const QString &comparison, V &&value);
        /*! Add a basic equal "where not" clause to the query with a full sub-select
            column. */
        template<Queryable C, WhereValue V>
        static std::unique_ptr<TinyBuilder<Derived>>
        whereNotEq(C &&column, V &&value, const QString &condition = AND);
        /*! Add an equal "or where not" clause to the query with a full sub-select
            column. */
        template<Queryable C, WhereValue V>
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereNotEq(C &&column, V &&value);

        /*! Add a full sub-select to the "where" clause. */
        template<WhereValueSubQuery T>
        static std::unique_ptr<TinyBuilder<Derived>>
        whereSub(const Column &column, const QString &comparison, T &&query,
                 const QString &condition = AND);

        /* where exists */
        /*! Add an exists clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereExists(const std::function<void(QueryBuilder &)> &callback,
                    const QString &condition = AND, bool nope = false);
        /*! Add an or exists clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereExists(const std::function<void(QueryBuilder &)> &callback,
                      bool nope = false);
        /*! Add a where not exists clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereNotExists(const std::function<void(QueryBuilder &)> &callback,
                       const QString &condition = AND);
        /*! Add a where not exists clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereNotExists(const std::function<void(QueryBuilder &)> &callback);

        /* where row values */
        /*! Adds a where condition using row values. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereRowValues(const QVector<Column> &columns, const QString &comparison,
                       const QVector<QVariant> &values, const QString &condition = AND);
        /*! Adds an or where condition using row values. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereRowValues(const QVector<Column> &columns, const QString &comparison,
                         const QVector<QVariant> &values);
        static std::unique_ptr<TinyBuilder<Derived>>
        whereRowValuesEq(const QVector<Column> &columns, const QVector<QVariant> &values,
                         const QString &condition = AND);
        /*! Adds an or where condition using row values. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereRowValuesEq(const QVector<Column> &columns,
                           const QVector<QVariant> &values);

        /* where raw */
        /*! Add a raw "where" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        whereRaw(const QString &sql, const QVector<QVariant> &bindings = {},
                 const QString &condition = AND);
        /*! Add a raw "or where" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereRaw(const QString &sql, const QVector<QVariant> &bindings = {});

        /* Group by and having */
        /*! Add a "group by" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        groupBy(const QVector<Column> &groups);
        /*! Add a "group by" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        groupBy(const Column &group);
        /*! Add a "group by" clause to the query. */
        template<ColumnConcept ...Args>
        static std::unique_ptr<TinyBuilder<Derived>>
        groupBy(Args &&...groups);

        /*! Add a raw "groupBy" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        groupByRaw(const QString &sql, const QVector<QVariant> &bindings = {});

        /*! Add a "having" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        having(const Column &column, const QString &comparison,
               const QVariant &value, const QString &condition = AND);
        /*! Add an "or having" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orHaving(const Column &column, const QString &comparison,
                 const QVariant &value);

        /*! Add a raw "having" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        havingRaw(const QString &sql, const QVector<QVariant> &bindings = {},
                  const QString &condition = AND);
        /*! Add a raw "or having" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orHavingRaw(const QString &sql, const QVector<QVariant> &bindings = {});

        /* Ordering */
        /*! Add an "order by" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orderBy(const Column &column, const QString &direction = ASC);
        /*! Add a descending "order by" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orderByDesc(const Column &column);

        /*! Add an "order by" clause to the query with a subquery ordering. */
        template<Queryable T>
        static std::unique_ptr<TinyBuilder<Derived>>
        orderBy(T &&query, const QString &direction = ASC);
        /*! Add a descending "order by" clause to the query with a subquery ordering. */
        template<Queryable T>
        static std::unique_ptr<TinyBuilder<Derived>>
        orderByDesc(T &&query);

        /*! Put the query's results in random order. */
        static std::unique_ptr<TinyBuilder<Derived>>
        inRandomOrder(const QString &seed = "");
        /*! Add a raw "order by" clause to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        orderByRaw(const QString &sql, const QVector<QVariant> &bindings = {});

        /*! Add an "order by" clause for a timestamp to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        latest(const Column &column = "");
        /*! Add an "order by" clause for a timestamp to the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        oldest(const Column &column = "");
        /*! Remove all existing orders. */
        static std::unique_ptr<TinyBuilder<Derived>>
        reorder();
        /*! Remove all existing orders and optionally add a new order. */
        static std::unique_ptr<TinyBuilder<Derived>>
        reorder(const Column &column, const QString &direction = ASC);

        /*! Set the "limit" value of the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        limit(int value);
        /*! Alias to set the "limit" value of the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        take(int value);
        /*! Set the "offset" value of the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        offset(int value);
        /*! Alias to set the "offset" value of the query. */
        static std::unique_ptr<TinyBuilder<Derived>>
        skip(int value);
        /*! Set the limit and offset for a given page. */
        static std::unique_ptr<TinyBuilder<Derived>>
        forPage(int page, int perPage = 30);

        /*! Constrain the query to the previous "page" of results before a given ID. */
        static std::unique_ptr<TinyBuilder<Derived>>
        forPageBeforeId(int perPage = 30, const QVariant &lastId = {},
                        const QString &column = Orm::Constants::ID,
                        bool prependOrder = false);
        /*! Constrain the query to the next "page" of results after a given ID. */
        static std::unique_ptr<TinyBuilder<Derived>>
        forPageAfterId(int perPage = 30, const QVariant &lastId = {},
                       const QString &column = Orm::Constants::ID,
                       bool prependOrder = false);

        /* Pessimistic Locking */
        /*! Lock the selected rows in the table for updating. */
        static std::unique_ptr<TinyBuilder<Derived>>
        lockForUpdate();
        /*! Share lock the selected rows in the table. */
        static std::unique_ptr<TinyBuilder<Derived>>
        sharedLock();
        /*! Lock the selected rows in the table. */
        static std::unique_ptr<TinyBuilder<Derived>>
        lock(bool value = true);
        /*! Lock the selected rows in the table. */
        static std::unique_ptr<TinyBuilder<Derived>>
        lock(const char *value);
        /*! Lock the selected rows in the table. */
        static std::unique_ptr<TinyBuilder<Derived>>
        lock(const QString &value);
        /*! Lock the selected rows in the table. */
        static std::unique_ptr<TinyBuilder<Derived>>
        lock(QString &&value);

        /* Debugging */
        /*! Dump the current SQL and bindings. */
        static void dump(bool replaceBindings = true, bool simpleBindings = false);
        /*! Die and dump the current SQL and bindings. */
        static void dd(bool replaceBindings = true, bool simpleBindings = false);

        /* Builds Queries */
        /*! Chunk the results of the query. */
        static bool
        chunk(int count,
              const std::function<bool(QVector<Derived> &&models, int page)> &callback);
        /*! Execute a callback over each item while chunking. */
        static bool
        each(const std::function<bool(Derived &&model, int index)> &callback,
             int count = 1000);

        /*! Run a map over each item while chunking. */
        static QVector<Derived>
        chunkMap(const std::function<Derived(Derived &&model)> &callback,
                 int count = 1000);
        /*! Run a map over each item while chunking. */
        template<typename T>
        static QVector<T>
        chunkMap(const std::function<T(Derived &&model)> &callback, int count = 1000);

        /*! Chunk the results of a query by comparing IDs. */
        static bool
        chunkById(int count,
                  const std::function<
                      bool(QVector<Derived> &&models, int page)> &callback,
                  const QString &column = "", const QString &alias = "");
        /*! Execute a callback over each item while chunking by ID. */
        static bool
        eachById(const std::function<bool(Derived &&model, int index)> &callback,
                 int count = 1000, const QString &column = "",
                 const QString &alias = "");

        /*! Execute the query and get the first result if it's the sole matching
            record. */
        static Derived sole(const QVector<Column> &columns = {ASTERISK});

        /*! Pass the query to a given callback. */
        static Builder<Derived> &
        tap(const std::function<void(Builder<Derived> &query)> &callback);

        /* Querying Relationship Existence/Absence */
        /*! Add a relationship count / exists condition to the query. */
        template<typename Related = void>
        static std::unique_ptr<TinyBuilder<Derived>>
        has(const QString &relation, const QString &comparison = GE, qint64 count = 1,
            const QString &condition = AND,
            const std::function<void(CallbackType<Related> &)> &callback = nullptr);

        /*! Add a relationship count / exists condition to the query with an "or". */
        template<typename Related = void>
        static std::unique_ptr<TinyBuilder<Derived>>
        orHas(const QString &relation, const QString &comparison = GE, qint64 count = 1);
        /*! Add a relationship count / exists condition to the query. */
        template<typename Related = void>
        static std::unique_ptr<TinyBuilder<Derived>>
        doesntHave(const QString &relation, const QString &condition = AND,
                   const std::function<void(
                       CallbackType<Related> &)> &callback = nullptr);
        /*! Add a relationship count / exists condition to the query with an "or". */
        template<typename Related = void>
        static std::unique_ptr<TinyBuilder<Derived>>
        orDoesntHave(const QString &relation);

        /*! Add a relationship count / exists condition to the query. */
        template<typename Related>
        static std::unique_ptr<TinyBuilder<Derived>>
        has(std::unique_ptr<RelationAlias<Related>> &&relation,
            const QString &comparison = GE, qint64 count = 1,
            const QString &condition = AND,
            const std::function<void(QueryBuilder &)> &callback = nullptr);
        /*! Add a relationship count / exists condition to the query, prefer this over
            above overload, void type to avoid ambiguity. */
        template<typename Related, typename = void>
        static std::unique_ptr<TinyBuilder<Derived>>
        has(std::unique_ptr<RelationAlias<Related>> &&relation,
            const QString &comparison = GE, qint64 count = 1,
            const QString &condition = AND,
            const std::function<void(TinyBuilder<Related> &)> &callback = nullptr);

        /*! Add a relationship count / exists condition to the query. */
#ifdef __clang__
        template<typename Related, typename Method,
                 std::enable_if_t<std::is_member_function_pointer_v<Method>, bool> = true>
#else
        template<typename Related, typename Method>
        requires std::is_member_function_pointer_v<Method>
#endif
        static std::unique_ptr<TinyBuilder<Derived>>
        has(Method relation, const QString &comparison = GE, qint64 count = 1,
            const QString &condition = AND,
            const std::function<void(TinyBuilder<Related> &)> &callback = nullptr);

        /*! Add a relationship count / exists condition to the query with where
            clauses. */
        template<typename Related = void>
        static std::unique_ptr<TinyBuilder<Derived>>
        whereHas(const QString &relation,
                 const std::function<void(CallbackType<Related> &)> &callback = nullptr,
                 const QString &comparison = GE, qint64 count = 1);

        /*! Add a relationship count / exists condition to the query with where
            clauses and an "or". */
        template<typename Related = void>
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereHas(const QString &relation,
                   const std::function<void(
                       CallbackType<Related> &)> &callback = nullptr,
                   const QString &comparison = GE, qint64 count = 1);
        /*! Add a relationship count / exists condition to the query with where
            clauses. */
        template<typename Related = void>
        static std::unique_ptr<TinyBuilder<Derived>>
        whereDoesntHave(const QString &relation,
                        const std::function<void(
                            CallbackType<Related> &)> &callback = nullptr);
        /*! Add a relationship count / exists condition to the query with where
            clauses and an "or". */
        template<typename Related = void>
        static std::unique_ptr<TinyBuilder<Derived>>
        orWhereDoesntHave(const QString &relation,
                          const std::function<void(
                              CallbackType<Related> &)> &callback = nullptr);

        /*! Add a relationship count / exists condition to the query with where
            clauses. */
#ifdef __clang__
        template<typename Related, typename Method,
                 std::enable_if_t<std::is_member_function_pointer_v<Method>, bool> = true>
#else
        template<typename Related, typename Method>
        requires std::is_member_function_pointer_v<Method>
#endif
        static std::unique_ptr<TinyBuilder<Derived>>
        whereHas(Method relation,
                 const std::function<void(TinyBuilder<Related> &)> &callback = nullptr,
                 const QString &comparison = GE, qint64 count = 1);

    private:
        /*! Begin querying the model, proxy to Model::query(). */
        static std::unique_ptr<TinyBuilder<Derived>> query();
    };

    /* public */

    /* Retrieving results */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant ModelProxies<Derived, AllRelations...>::value(const Column &column)
    {
        return query()->value(column);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant ModelProxies<Derived, AllRelations...>::soleValue(const Column &column)
    {
        return query()->soleValue(column);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVector<QVariant>
    ModelProxies<Derived, AllRelations...>::pluck(const QString &column)
    {
        return query()->pluck(column);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename T>
    std::map<T, QVariant>
    ModelProxies<Derived, AllRelations...>::pluck(const QString &column,
                                                  const QString &key)
    {
        return query()->template pluck<T>(column, key);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString ModelProxies<Derived, AllRelations...>::implode(const QString &column,
                                                            const QString &glue)
    {
        return query()->implode(column, glue);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<Derived>
    ModelProxies<Derived, AllRelations...>::find(const QVariant &id,
                                                 const QVector<Column> &columns)
    {
        return query()->find(id, columns);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    ModelProxies<Derived, AllRelations...>::findOrNew(const QVariant &id,
                                                      const QVector<Column> &columns)
    {
        return query()->findOrNew(id, columns);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    ModelProxies<Derived, AllRelations...>::findOrFail(const QVariant &id,
                                                       const QVector<Column> &columns)
    {
        return query()->findOrFail(id, columns);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVector<Derived>
    ModelProxies<Derived, AllRelations...>::findMany(const QVector<QVariant> &ids,
                                                     const QVector<Column> &columns)
    {
        return query()->findMany(ids, columns);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<Derived>
    ModelProxies<Derived, AllRelations...>::findOr(
            const QVariant &id, const QVector<Column> &columns,
            const std::function<void()> &callback)
    {
        return query()->findOr(id, columns, callback);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<Derived>
    ModelProxies<Derived, AllRelations...>::findOr(
            const QVariant &id, const std::function<void()> &callback)
    {
        return query()->findOr(id, {ASTERISK}, callback);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename R>
    std::pair<std::optional<Derived>, R>
    ModelProxies<Derived, AllRelations...>::findOr(
            const QVariant &id, const QVector<Column> &columns,
            const std::function<R()> &callback)
    {
        return query()->template findOr<R>(id, columns, callback);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename R>
    std::pair<std::optional<Derived>, R>
    ModelProxies<Derived, AllRelations...>::findOr(
            const QVariant &id, const std::function<R()> &callback)
    {
        return query()->template findOr<R>(id, {ASTERISK}, callback);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<Derived>
    ModelProxies<Derived, AllRelations...>::first(const QVector<Column> &columns)
    {
        return query()->first(columns);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    ModelProxies<Derived, AllRelations...>::firstOrNew(
            const QVector<WhereItem> &attributes,
            const QVector<AttributeItem> &values)
    {
        return query()->firstOrNew(attributes, values);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    ModelProxies<Derived, AllRelations...>::firstOrCreate(
            const QVector<WhereItem> &attributes,
            const QVector<AttributeItem> &values)
    {
        return query()->firstOrCreate(attributes, values);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    ModelProxies<Derived, AllRelations...>::firstOrFail(const QVector<Column> &columns)
    {
        return query()->firstOrFail(columns);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<Derived>
    ModelProxies<Derived, AllRelations...>::firstOr(
            const QVector<Column> &columns, const std::function<void()> &callback)
    {
        return query()->firstOr(columns, callback);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<Derived>
    ModelProxies<Derived, AllRelations...>::firstOr(const std::function<void()> &callback)
    {
        return query()->firstOr({ASTERISK}, callback);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename R>
    std::pair<std::optional<Derived>, R>
    ModelProxies<Derived, AllRelations...>::firstOr(
            const QVector<Column> &columns, const std::function<R()> &callback)
    {
        return query()->template firstOr<R>(columns, callback);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename R>
    std::pair<std::optional<Derived>, R>
    ModelProxies<Derived, AllRelations...>::firstOr(const std::function<R()> &callback)
    {
        return query()->template firstOr<R>({ASTERISK}, callback);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<Derived>
    ModelProxies<Derived, AllRelations...>::firstWhere(
            const Column &column, const QString &comparison,
            const QVariant &value, const QString &condition)
    {
        return where(column, comparison, value, condition)->first();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<Derived>
    ModelProxies<Derived, AllRelations...>::firstWhereEq(
            const Column &column, const QVariant &value, const QString &condition)
    {
        return where(column, EQ, value, condition)->first();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereKey(const QVariant &id)
    {
        auto builder = query();

        builder->whereKey(id);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereKey(const QVector<QVariant> &ids)
    {
        auto builder = query();

        builder->whereKey(ids);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereKeyNot(const QVariant &id)
    {
        auto builder = query();

        builder->whereKeyNot(id);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereKeyNot(const QVector<QVariant> &ids)
    {
        auto builder = query();

        builder->whereKeyNot(ids);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::with(const QVector<WithItem> &relations)
    {
        auto builder = query();

        builder->with(relations);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::with(const QString &relation)
    {
        return with(QVector<WithItem> {{relation}});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::with(const QVector<QString> &relations)
    {
        auto builder = query();

        builder->with(relations);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::with(QVector<QString> &&relations)
    {
        auto builder = query();

        builder->with(std::move(relations));

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::without(const QVector<QString> &relations)
    {
        auto builder = query();

        builder->without(relations);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::without(const QString &relation)
    {
        return without(QVector<QString> {relation});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::withOnly(const QVector<WithItem> &relations)
    {
        auto builder = query();

        builder->withOnly(relations);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::withOnly(const QString &relation)
    {
        return withOnly(QVector<WithItem> {{relation}});
    }

    /* Insert, Update, Delete */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    ModelProxies<Derived, AllRelations...>::create(
            const QVector<AttributeItem> &attributes)
    {
        return query()->create(attributes);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    ModelProxies<Derived, AllRelations...>::create(QVector<AttributeItem> &&attributes)
    {
        return query()->create(std::move(attributes));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    ModelProxies<Derived, AllRelations...>::updateOrCreate(
            const QVector<WhereItem> &attributes, const QVector<AttributeItem> &values)
    {
        return query()->updateOrCreate(attributes, values);
    }

    /* Proxies to TinyBuilder -> QueryBuilder */

    /* Insert, Update, Delete */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<QSqlQuery>
    ModelProxies<Derived, AllRelations...>::insert(
            const QVector<AttributeItem> &values)
    {
        return query()->insert(values);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<QSqlQuery>
    ModelProxies<Derived, AllRelations...>::insert(
            const QVector<QVector<AttributeItem>> &values)
    {
        return query()->insert(values);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<QSqlQuery>
    ModelProxies<Derived, AllRelations...>::insert(
            const QVector<QString> &columns, QVector<QVector<QVariant>> values)
    {
        return query()->insert(columns, std::move(values));
    }

    // FEATURE dilemma primarykey, Derived::KeyType vs QVariant silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    quint64
    ModelProxies<Derived, AllRelations...>::insertGetId(
            const QVector<AttributeItem> &values, const QString &sequence)
    {
        return query()->insertGetId(values, sequence);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::tuple<int, std::optional<QSqlQuery>>
    ModelProxies<Derived, AllRelations...>::insertOrIgnore(
            const QVector<AttributeItem> &values)
    {
        return query()->insertOrIgnore(values);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::tuple<int, std::optional<QSqlQuery>>
    ModelProxies<Derived, AllRelations...>::insertOrIgnore(
            const QVector<QVector<AttributeItem>> &values)
    {
        return query()->insertOrIgnore(values);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::tuple<int, std::optional<QSqlQuery>>
    ModelProxies<Derived, AllRelations...>::insertOrIgnore(
            const QVector<QString> &columns, QVector<QVector<QVariant>> values)
    {
        return query()->insertOrIgnore(columns, std::move(values));
    }

    // TODO cpp check all int types and use std::size_t where appropriate silverqx
    // FEATURE dilemma primarykey, id should be Derived::KeyType, if I don't solve this problem, do runtime type check, QVariant type has to be the same type like KeyType and throw exception silverqx
    // TODO next test all this remove()/destroy() methods, when deletion fails silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::size_t
    ModelProxies<Derived, AllRelations...>::destroy(const QVector<QVariant> &ids)
    {
        if (ids.isEmpty())
            return 0;

        /* We will actually pull the models from the database table and call delete on
           each of them individually so that their events get fired properly with a
           correct set of attributes in case the developers wants to check these. */
        Derived instance;

        std::size_t count = 0;

        // Ownership of a unique_ptr()
        for (auto &model : instance.whereIn(instance.getKeyName(), ids)->get())
            if (model.remove())
                ++count;

        return count;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::size_t
    ModelProxies<Derived, AllRelations...>::destroy(const QVariant &id)
    {
        return destroy(QVector<QVariant> {id});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void ModelProxies<Derived, AllRelations...>::truncate()
    {
        query()->truncate();
    }

    /* Select */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    quint64 ModelProxies<Derived, AllRelations...>::count(const QVector<Column> &columns)
    {
        return query()->count(columns);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename>
    quint64 ModelProxies<Derived, AllRelations...>::count(const Column &column)
    {
        return query()->count(QVector<Column> {column});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant ModelProxies<Derived, AllRelations...>::min(const Column &column)
    {
        return query()->min(column);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant ModelProxies<Derived, AllRelations...>::max(const Column &column)
    {
        return query()->max(column);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant ModelProxies<Derived, AllRelations...>::sum(const Column &column)
    {
        return query()->sum(column);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant ModelProxies<Derived, AllRelations...>::avg(const Column &column)
    {
        return query()->avg(column);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant ModelProxies<Derived, AllRelations...>::average(const Column &column)
    {
        return query()->avg(column);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    ModelProxies<Derived, AllRelations...>::aggregate(const QString &function,
                                               const QVector<Column> &columns)
    {
        return query()->aggregate(function, columns);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool ModelProxies<Derived, AllRelations...>::exists()
    {
        return query()->exists();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool ModelProxies<Derived, AllRelations...>::doesntExist()
    {
        return query()->doesntExist();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool ModelProxies<Derived, AllRelations...>::existsOr(
            const std::function<void()> &callback)
    {
        return query()->existsOr(callback);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool ModelProxies<Derived, AllRelations...>::doesntExistOr(
            const std::function<void()> &callback)
    {
        return query()->doesntExistOr(callback);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename R>
    std::pair<bool, R> ModelProxies<Derived, AllRelations...>::existsOr(
            const std::function<R()> &callback)
    {
        return query()->template existsOr<R>(callback);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename R>
    std::pair<bool, R> ModelProxies<Derived, AllRelations...>::doesntExistOr(
            const std::function<R()> &callback)
    {
        return query()->template doesntExistOr<R>(callback);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::select(const QVector<Column> &columns)
    {
        auto builder = query();

        builder->select(columns);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::select(const Column &column)
    {
        auto builder = query();

        builder->select(column);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::addSelect(const QVector<Column> &columns)
    {
        auto builder = query();

        builder->addSelect(columns);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::addSelect(const Column &column)
    {
        auto builder = query();

        builder->addSelect(column);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::select(QVector<Column> &&columns)
    {
        auto builder = query();

        builder->select(std::move(columns));

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::select(Column &&column)
    {
        auto builder = query();

        builder->select(std::move(column));

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::addSelect(QVector<Column> &&columns)
    {
        auto builder = query();

        builder->addSelect(std::move(columns));

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::addSelect(Column &&column)
    {
        auto builder = query();

        builder->addSelect(std::move(column));

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<Queryable T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::select(T &&query, const QString &as)
    {
        auto builder = Derived::query();

        builder->select(std::forward<T>(query), as);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<Queryable T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::addSelect(T &&query, const QString &as)
    {
        auto builder = Derived::query();

        builder->addSelect(std::forward<T>(query), as);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<SubQuery T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::selectSub(T &&query, const QString &as)
    {
        auto builder = Derived::query();

        builder->selectSub(std::forward<T>(query), as);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::selectRaw(
            const QString &expression, const QVector<QVariant> &bindings)
    {
        auto builder = query();

        builder->selectRaw(expression, bindings);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::distinct()
    {
        auto builder = query();

        builder->distinct();

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::distinct(const QStringList &columns)
    {
        auto builder = query();

        builder->distinct(columns);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::distinct(QStringList &&columns)
    {
        auto builder = query();

        builder->distinct(std::move(columns));

        return builder;
    }

    /* Joins */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<JoinTable T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::join(
            T &&table, const QString &first,  const QString &comparison,
            const QString &second, const QString &type, const bool where)
    {
        auto builder = query();

        builder->join(std::forward<T>(table), first, comparison, second, type, where);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<JoinTable T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::join(
            T &&table, const std::function<void(JoinClause &)> &callback,
            const QString &type)
    {
        auto builder = query();

        builder->join(std::forward<T>(table), callback, type);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<JoinTable T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::joinWhere(
            T &&table, const QString &first, const QString &comparison,
            const QVariant &second, const QString &type)
    {
        auto builder = query();

        builder->joinWhere(std::forward<T>(table), first, comparison, second, type);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<JoinTable T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::leftJoin(
            T &&table, const QString &first, const QString &comparison,
            const QString &second)
    {
        auto builder = query();

        builder->leftJoin(std::forward<T>(table), first, comparison, second);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<JoinTable T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::leftJoin(
            T &&table, const std::function<void(JoinClause &)> &callback)
    {
        auto builder = query();

        builder->leftJoin(std::forward<T>(table), callback);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<JoinTable T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::leftJoinWhere(
            T &&table, const QString &first, const QString &comparison,
            const QVariant &second)
    {
        auto builder = query();

        builder->leftJoinWhere(std::forward<T>(table), first, comparison, second);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<JoinTable T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::rightJoin(
            T &&table, const QString &first, const QString &comparison,
            const QString &second)
    {
        auto builder = query();

        builder->rightJoin(std::forward<T>(table), first, comparison, second);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<JoinTable T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::rightJoin(
            T &&table, const std::function<void(JoinClause &)> &callback)
    {
        auto builder = query();

        builder->rightJoin(std::forward<T>(table), callback);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<JoinTable T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::rightJoinWhere(
            T &&table, const QString &first, const QString &comparison,
            const QVariant &second)
    {
        auto builder = query();

        builder->rightJoinWhere(std::forward<T>(table), first, comparison, second);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<JoinTable T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::crossJoin(
            T &&table, const QString &first, const QString &comparison,
            const QString &second)
    {
        auto builder = query();

        builder->crossJoin(std::forward<T>(table), first, comparison, second);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<JoinTable T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::crossJoin(
            T &&table, const std::function<void(JoinClause &)> &callback)
    {
        auto builder = query();

        builder->crossJoin(std::forward<T>(table), callback);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<JoinTable T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::crossJoin(T &&table)
    {
        auto builder = query();

        builder->crossJoin(std::forward<T>(table));

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<SubQuery T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::joinSub(
            T &&query, const QString &as, const QString &first,
            const QString &comparison, const QVariant &second,
            const QString &type, const bool where)
    {
        auto builder = Derived::query();

        builder->joinSub(std::forward<T>(query), as, first, comparison, second, type,
                         where);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<SubQuery T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::joinSub(
            T &&query, const QString &as,
            const std::function<void(JoinClause &)> &callback,
            const QString &type)
    {
        auto builder = Derived::query();

        builder->joinSub(std::forward<T>(query), as, callback, type);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<SubQuery T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::leftJoinSub(
            T &&query, const QString &as, const QString &first,
            const QString &comparison, const QVariant &second)
    {
        auto builder = Derived::query();

        builder->leftJoinSub(std::forward<T>(query), as, first, comparison, second);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<SubQuery T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::leftJoinSub(
            T &&query, const QString &as,
            const std::function<void(JoinClause &)> &callback)
    {
        auto builder = Derived::query();

        builder->joinSub(std::forward<T>(query), as, callback, LEFT);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<SubQuery T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::rightJoinSub(
            T &&query, const QString &as, const QString &first,
            const QString &comparison, const QVariant &second)
    {
        auto builder = Derived::query();

        builder->rightJoinSub(std::forward<T>(query), as, first, comparison, second);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<SubQuery T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::rightJoinSub(
            T &&query, const QString &as,
            const std::function<void(JoinClause &)> &callback)
    {
        auto builder = Derived::query();

        builder->joinSub(std::forward<T>(query), as, callback, RIGHT);

        return builder;
    }

    /* General where */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<WhereValue T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::where(
            const Column &column, const QString &comparison, T &&value,
            const QString &condition)
    {
        auto builder = query();

        builder->where(column, comparison, std::forward<T>(value), condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<WhereValue T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhere(
            const Column &column, const QString &comparison, T &&value)
    {
        auto builder = query();

        builder->orWhere(column, comparison, std::forward<T>(value));

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<WhereValue T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereEq(
            const Column &column, T &&value, const QString &condition)
    {
        auto builder = query();

        builder->whereEq(column, std::forward<T>(value), condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<WhereValue T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereEq(
            const Column &column, T &&value)
    {
        auto builder = query();

        builder->orWhereEq(column, std::forward<T>(value));

        return builder;
    }

    /* General where not */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<WhereValue T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereNot(
            const Column &column, const QString &comparison, T &&value,
            const QString &condition)
    {
        auto builder = query();

        builder->whereNot(column, comparison, std::forward<T>(value), condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<WhereValue T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereNot(
            const Column &column, const QString &comparison, T &&value)
    {
        auto builder = query();

        builder->orWhereNot(column, comparison, std::forward<T>(value));

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<WhereValue T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereNotEq(
            const Column &column, T &&value, const QString &condition)
    {
        auto builder = query();

        builder->whereNotEq(column, std::forward<T>(value), condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<WhereValue T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereNotEq(const Column &column, T &&value)
    {
        auto builder = query();

        builder->orWhereNotEq(column, std::forward<T>(value));

        return builder;
    }

    /* Nested where */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::where(
            const std::function<void(TinyBuilder<Derived> &)> &callback,
            const QString &condition)
    {
        auto builder = query();

        builder->where(callback, condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhere(
            const std::function<void(TinyBuilder<Derived> &)> &callback)
    {
        auto builder = query();

        builder->orWhere(callback);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereNot(
            const std::function<void(TinyBuilder<Derived> &)> &callback,
            const QString &condition)
    {
        auto builder = query();

        builder->whereNot(callback, condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereNot(
            const std::function<void(TinyBuilder<Derived> &)> &callback)
    {
        auto builder = query();

        builder->orWhereNot(callback);

        return builder;
    }

    /* Array where */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::where(const QVector<WhereItem> &values,
                                                  const QString &condition)
    {
        /* The parentheses in this query are ok:
           select * from xyz where (id = ?) */
        auto builder = query();

        builder->where(values, condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhere(const QVector<WhereItem> &values)
    {
        auto builder = query();

        builder->orWhere(values);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereNot(
            const QVector<WhereItem> &values, const QString &condition,
            const QString &defaultCondition)
    {
        auto builder = query();

        builder->whereNot(values, condition, defaultCondition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereNot(
            const QVector<WhereItem> &values, const QString &defaultCondition)
    {
        auto builder = query();

        builder->orWhereNot(values, defaultCondition);

        return builder;
    }

    /* where column */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereColumn(
            const QVector<WhereColumnItem> &values, const QString &condition)
    {
        auto builder = query();

        builder->whereColumn(values, condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereColumn(
            const QVector<WhereColumnItem> &values)
    {
        auto builder = query();

        builder->orWhereColumn(values);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereColumn(
            const Column &first, const QString &comparison,
            const Column &second, const QString &condition)
    {
        auto builder = query();

        builder->whereColumn(first, comparison, second, condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereColumn(
            const Column &first, const QString &comparison, const Column &second)
    {
        auto builder = query();

        builder->orWhereColumn(first, comparison, second);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereColumnEq(
            const Column &first, const Column &second, const QString &condition)
    {
        auto builder = query();

        builder->whereColumnEq(first, second, condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereColumnEq(
            const Column &first, const Column &second)
    {
        auto builder = query();

        builder->orWhereColumnEq(first, second);

        return builder;
    }

    /* where IN */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereIn(
            const Column &column, const QVector<QVariant> &values,
            const QString &condition, const bool nope)
    {
        auto builder = query();

        builder->whereIn(column, values, condition, nope);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereIn(
            const Column &column, const QVector<QVariant> &values)
    {
        auto builder = query();

        builder->orWhereIn(column, values);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereNotIn(
            const Column &column, const QVector<QVariant> &values,
            const QString &condition)
    {
        auto builder = query();

        builder->whereNotIn(column, values, condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereNotIn(
            const Column &column, const QVector<QVariant> &values)
    {
        auto builder = query();

        builder->orWhereNotIn(column, values);

        return builder;
    }

    /* where null */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereNull(
            const QVector<Column> &columns, const QString &condition, const bool nope)
    {
        auto builder = query();

        builder->whereNull(columns, condition, nope);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereNull(const QVector<Column> &columns)
    {
        auto builder = query();

        builder->orWhereNull(columns);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereNotNull(
            const QVector<Column> &columns, const QString &condition)
    {
        auto builder = query();

        builder->whereNotNull(columns, condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereNotNull(
            const QVector<Column> &columns)
    {
        auto builder = query();

        builder->orWhereNotNull(columns);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereNull(
            const Column &column, const QString &condition, const bool nope)
    {
        auto builder = query();

        builder->whereNull(column, condition, nope);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereNull(const Column &column)
    {
        auto builder = query();

        builder->orWhereNull(column);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereNotNull(
            const Column &column, const QString &condition)
    {
        auto builder = query();

        builder->whereNotNull(column, condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereNotNull(const Column &column)
    {
        auto builder = query();

        builder->orWhereNotNull(column);

        return builder;
    }

    /* where sub-queries */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<Queryable C, WhereValue V>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::where(
            C &&column, const QString &comparison, V &&value,
            const QString &condition)
    {
        auto builder = query();

        builder->where(std::forward<C>(column), comparison, std::forward<V>(value),
                       condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<Queryable C, WhereValue V>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhere(
            C &&column, const QString &comparison, V &&value)
    {
        auto builder = query();

        builder->where(std::forward<C>(column), comparison, std::forward<V>(value), OR);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<Queryable C, WhereValue V>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereEq(
            C &&column, V &&value, const QString &condition)
    {
        auto builder = query();

        builder->where(std::forward<C>(column), EQ, std::forward<V>(value), condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<Queryable C, WhereValue V>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereEq(C &&column, V &&value)
    {
        auto builder = query();

        builder->where(std::forward<C>(column), EQ, std::forward<V>(value), OR);

        return builder;
    }

    /* where not sub-queries */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<Queryable C, WhereValue V>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereNot(
            C &&column, const QString &comparison, V &&value, const QString &condition)
    {
        auto builder = query();

        builder->whereNot(std::forward<C>(column), comparison, std::forward<V>(value),
                          condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<Queryable C, WhereValue V>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereNot(
            C &&column, const QString &comparison, V &&value)
    {
        auto builder = query();

        builder->orWhereNot(std::forward<C>(column), comparison, std::forward<V>(value));

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<Queryable C, WhereValue V>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereNotEq(
            C &&column, V &&value, const QString &condition)
    {
        auto builder = query();

        builder->whereNotEq(std::forward<C>(column), std::forward<V>(value), condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<Queryable C, WhereValue V>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereNotEq(C &&column, V &&value)
    {
        auto builder = query();

        builder->orWhereNotEq(std::forward<C>(column), std::forward<V>(value));

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<WhereValueSubQuery T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereSub(
            const Column &column, const QString &comparison, T &&query,
            const QString &condition)
    {
        auto builder = Derived::query();

        builder->whereSub(column, comparison, std::forward<T>(query), condition);

        return builder;
    }

    /* where exists */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereExists(
            const std::function<void(QueryBuilder &)> &callback,
            const QString &condition, const bool nope)
    {
        auto builder = query();

        builder->whereExists(callback, condition, nope);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereExists(
            const std::function<void(QueryBuilder &)> &callback, const bool nope)
    {
        auto builder = query();

        builder->whereExists(callback, OR, nope);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereNotExists(
            const std::function<void(QueryBuilder &)> &callback,
            const QString &condition)
    {
        auto builder = query();

        builder->whereExists(callback, condition, true);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereNotExists(
            const std::function<void(QueryBuilder &)> &callback)
    {
        auto builder = query();

        builder->whereExists(callback, OR, true);

        return builder;
    }

    /* where row values */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereRowValues(
            const QVector<Column> &columns, const QString &comparison,
            const QVector<QVariant> &values, const QString &condition)
    {
        auto builder = query();

        builder->whereRowValues(columns, comparison, values, condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereRowValues(
            const QVector<Column> &columns, const QString &comparison,
            const QVector<QVariant> &values)
    {
        auto builder = query();

        builder->whereRowValues(columns, comparison, values, OR);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereRowValuesEq(
            const QVector<Column> &columns, const QVector<QVariant> &values,
            const QString &condition)
    {
        auto builder = query();

        builder->whereRowValues(columns, EQ, values, condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereRowValuesEq(
            const QVector<Column> &columns, const QVector<QVariant> &values)
    {
        auto builder = query();

        builder->whereRowValues(columns, EQ, values, OR);

        return builder;
    }

    /* where raw */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereRaw(
            const QString &sql, const QVector<QVariant> &bindings,
            const QString &condition)
    {
        auto builder = query();

        builder->whereRaw(sql, bindings, condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereRaw(
            const QString &sql, const QVector<QVariant> &bindings)
    {
        auto builder = query();

        builder->whereRaw(sql, bindings, OR);

        return builder;
    }

    /* Group by and having */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::groupBy(const QVector<Column> &groups)
    {
        auto builder = query();

        builder->groupBy(groups);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::groupBy(const Column &group)
    {
        auto builder = query();

        builder->groupBy(group);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::groupByRaw(
            const QString &sql, const QVector<QVariant> &bindings)
    {
        auto builder = query();

        builder->groupByRaw(sql, bindings);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<ColumnConcept ...Args>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::groupBy(Args &&...groups)
    {
        auto builder = query();

        builder->groupBy(QVector<Column> {std::forward<Args>(groups)...});

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::having(
            const Column &column, const QString &comparison,
            const QVariant &value, const QString &condition)
    {
        auto builder = query();

        builder->having(column, comparison, value, condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orHaving(
            const Column &column, const QString &comparison, const QVariant &value)
    {
        auto builder = query();

        builder->orHaving(column, comparison, value);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::havingRaw(
            const QString &sql, const QVector<QVariant> &bindings,
            const QString &condition)
    {
        auto builder = query();

        builder->havingRaw(sql, bindings, condition);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orHavingRaw(
            const QString &sql, const QVector<QVariant> &bindings)
    {
        auto builder = query();

        builder->havingRaw(sql, bindings, OR);

        return builder;
    }

    /* Ordering */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orderBy(const Column &column,
                                                    const QString &direction)
    {
        auto builder = query();

        builder->orderBy(column, direction);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orderByDesc(const Column &column)
    {
        auto builder = query();

        builder->orderByDesc(column);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<Queryable T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orderBy(T &&query, const QString &direction)
    {
        auto builder = Derived::query();

        builder->orderBy(std::forward<T>(query), direction);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<Queryable T>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orderByDesc(T &&query)
    {
        auto builder = Derived::query();

        builder->orderBy(std::forward<T>(query), DESC);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::inRandomOrder(const QString &seed)
    {
        auto builder = query();

        builder->inRandomOrder(seed);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orderByRaw(
            const QString &sql, const QVector<QVariant> &bindings)
    {
        auto builder = query();

        builder->orderByRaw(sql, bindings);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::latest(const Column &column)
    {
        auto builder = query();

        builder->latest(column);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::oldest(const Column &column)
    {
        auto builder = query();

        builder->oldest(column);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::reorder()
    {
        auto builder = query();

        builder->reorder();

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::reorder(const Column &column,
                                                    const QString &direction)
    {
        auto builder = query();

        builder->reorder(column, direction);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::limit(const int value)
    {
        auto builder = query();

        builder->limit(value);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::take(const int value)
    {
        auto builder = query();

        builder->take(value);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::offset(const int value)
    {
        auto builder = query();

        builder->offset(value);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::skip(const int value)
    {
        auto builder = query();

        builder->skip(value);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::forPage(const int page, const int perPage)
    {
        auto builder = query();

        builder->forPage(page, perPage);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::forPageBeforeId(
            const int perPage, const QVariant &lastId, const QString &column,
            const bool prependOrder)
    {
        auto builder = query();

        builder->forPageBeforeId(perPage, lastId, column, prependOrder);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::forPageAfterId(
            const int perPage, const QVariant &lastId, const QString &column,
            const bool prependOrder)
    {
        auto builder = query();

        builder->forPageAfterId(perPage, lastId, column, prependOrder);

        return builder;
    }

    /* Pessimistic Locking */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::lockForUpdate()
    {
        auto builder = query();

        builder->lock(true);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::sharedLock()
    {
        auto builder = query();

        builder->lock(false);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::lock(const bool value)
    {
        auto builder = query();

        builder->lock(value);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::lock(const char *value)
    {
        auto builder = query();

        builder->lock(value);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::lock(const QString &value)
    {
        auto builder = query();

        builder->lock(value);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::lock(QString &&value)
    {
        auto builder = query();

        builder->lock(std::move(value));

        return builder;
    }

    /* Debugging */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void ModelProxies<Derived, AllRelations...>::dump(const bool replaceBindings,
                                                      const bool simpleBindings)
    {
        query()->dump(replaceBindings, simpleBindings);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void ModelProxies<Derived, AllRelations...>::dd(const bool replaceBindings,
                                                    const bool simpleBindings)
    {
        query()->dd(replaceBindings, simpleBindings);
    }

    /* Builds Queries */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool ModelProxies<Derived, AllRelations...>::chunk(
            const int count,
            const std::function<bool(QVector<Derived> &&, int)> &callback)
    {
        return query()->chunk(count, callback);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool ModelProxies<Derived, AllRelations...>::each(
            const std::function<bool(Derived &&, int)> &callback, const int count)
    {
        return query()->each(callback, count);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVector<Derived>
    ModelProxies<Derived, AllRelations...>::chunkMap(
            const std::function<Derived(Derived &&)> &callback, const int count)
    {
        return query()->chunkMap(callback, count);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename T>
    QVector<T>
    ModelProxies<Derived, AllRelations...>::chunkMap(
            const std::function<T(Derived &&)> &callback, const int count)
    {
        return query()->template chunkMap<T>(callback, count);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool ModelProxies<Derived, AllRelations...>::chunkById(
            const int count,
            const std::function<bool(QVector<Derived> &&, int)> &callback,
            const QString &column, const QString &alias)
    {
        return query()->chunkById(count, callback, column, alias);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool ModelProxies<Derived, AllRelations...>::eachById(
            const std::function<bool(Derived &&, int)> &callback,
            const int count, const QString &column, const QString &alias)
    {
        return query()->eachById(callback, count, column, alias);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived ModelProxies<Derived, AllRelations...>::sole(const QVector<Column> &columns)
    {
        return query()->sole(columns);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Builder<Derived> &
    ModelProxies<Derived, AllRelations...>::tap(
            const std::function<void(Builder<Derived> &)> &callback)
    {
        return query()->tap(callback);
    }

    /* Querying Relationship Existence/Absence */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::has(
            const QString &relation, const QString &comparison, const qint64 count,
            const QString &condition,
            const std::function<void(CallbackType<Related> &)> &callback)
    {
        auto builder = query();

        builder->template has<Related>(relation, comparison, count, condition, callback);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orHas(
            const QString &relation, const QString &comparison, const qint64 count)
    {
        auto builder = query();

        builder->template orHas<Related>(relation, comparison, count);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::doesntHave(
            const QString &relation, const QString &condition,
            const std::function<void(CallbackType<Related> &)> &callback)
    {
        auto builder = query();

        builder->template doesntHave<Related>(relation, condition, callback);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orDoesntHave(const QString &relation)
    {
        auto builder = query();

        builder->template orDoesntHave<Related>(relation);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::has(
            std::unique_ptr<RelationAlias<Related>> &&relation,
            const QString &comparison, const qint64 count, const QString &condition,
            const std::function<void(QueryBuilder &)> &callback)
    {
        /* I will not unify this two has(unique_ptr) overloads because it would not be
           possible to call them like it is possible now, I mean exactly this api
           for TinyBuilder overload:
           has<Xyz, void>(..., [](auto &q)) */
        auto builder = query();

        builder->template has<Related>(std::move(relation), comparison, count,
                                       condition, callback);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::has(
            std::unique_ptr<RelationAlias<Related>> &&relation,
            const QString &comparison, const qint64 count, const QString &condition,
            const std::function<void(TinyBuilder<Related> &)> &callback)
    {
        auto builder = query();

        builder->template has<Related, void>(std::move(relation), comparison, count,
                                             condition, callback);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
#ifdef __clang__
    template<typename Related, typename Method,
             std::enable_if_t<std::is_member_function_pointer_v<Method>, bool>>
#else
    template<typename Related, typename Method>
    requires std::is_member_function_pointer_v<Method>
#endif
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::has(
            const Method relation, const QString &comparison, const qint64 count,
            const QString &condition,
            const std::function<void(TinyBuilder<Related> &)> &callback)
    {
        auto builder = query();

        builder->template has<Related>(relation, comparison, count, condition, callback);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereHas(
            const QString &relation,
            const std::function<void(CallbackType<Related> &)> &callback,
            const QString &comparison, const qint64 count)
    {
        auto builder = query();

        builder->template whereHas<Related>(relation, callback, comparison, count);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereHas(
            const QString &relation,
            const std::function<void(CallbackType<Related> &)> &callback,
            const QString &comparison, const qint64 count)
    {
        auto builder = query();

        builder->template orWhereHas<Related>(relation, callback, comparison, count);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereDoesntHave(
            const QString &relation,
            const std::function<void(CallbackType<Related> &)> &callback)
    {
        auto builder = query();

        builder->template whereDoesntHave<Related>(relation, callback);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::orWhereDoesntHave(
            const QString &relation,
            const std::function<void(CallbackType<Related> &)> &callback)
    {
        auto builder = query();

        builder->template orWhereDoesntHave<Related>(relation, callback);

        return builder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
#ifdef __clang__
    template<typename Related, typename Method,
             std::enable_if_t<std::is_member_function_pointer_v<Method>, bool>>
#else
    template<typename Related, typename Method>
    requires std::is_member_function_pointer_v<Method>
#endif
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::whereHas(
            const Method relation,
            const std::function<void(TinyBuilder<Related> &)> &callback,
            const QString &comparison, const qint64 count)
    {
        auto builder = query();

        builder->template whereHas<Related>(relation, callback, comparison, count);

        return builder;
    }

    /* private */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    ModelProxies<Derived, AllRelations...>::query()
    {
        return Model<Derived, AllRelations...>::query();
    }

} // namespace Tiny
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_MODELPROXIES_HPP
