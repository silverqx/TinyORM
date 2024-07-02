#pragma once
#ifndef ORM_TINY_TINYBUILDERPROXIES_HPP
#define ORM_TINY_TINYBUILDERPROXIES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/ormconcepts.hpp"
#include "orm/tiny/utils/attribute.hpp"
#include "orm/types/sqlquery.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

namespace Query
{
    class JoinClause;
}

namespace Tiny
{

    /*! Contains proxy methods to the QueryBuilder. */
    template<typename Model>
    class BuilderProxies
    {
        /*! Alias for the attribute utils. */
        using AttributeUtils = Orm::Tiny::Utils::Attribute;
        /*! Alias for the JoinClause. */
        using JoinClause = Orm::Query::JoinClause;

    public:
        /*! Default constructor. */
        BuilderProxies() = default;
        /*! Default destructor. */
        ~BuilderProxies() = default;

        /*! Copy constructor. */
        BuilderProxies(const BuilderProxies &) = default;
        /*! Deleted copy assignment operator (not needed). */
        BuilderProxies &operator=(const BuilderProxies &) = delete;

        /*! Move constructor. */
        BuilderProxies(BuilderProxies &&) noexcept = default;
        /*! Deleted move assignment operator (not needed). */
        BuilderProxies &operator=(BuilderProxies &&) = delete;

        /*! The table which the query is targeting. */
        const FromClause &from();

        /* Proxy methods that internally call the toBase() (applySoftDeletes) */
        /* Retrieving results */
        /*! Concatenate values of the given column as a string. */
        QString implode(const QString &column, const QString &glue = "");

        /* Aggregates */
        /*! Retrieve the "count" result of the query. */
        quint64 count(const QList<Column> &columns = {ASTERISK});
        /*! Retrieve the "count" result of the query. */
        template<typename = void>
        quint64 count(const Column &column);
        /*! Retrieve the minimum value of a given column. */
        QVariant min(const Column &column);
        /*! Retrieve the maximum value of a given column. */
        QVariant max(const Column &column);
        /*! Retrieve the sum of the values of a given column. */
        QVariant sum(const Column &column);
        /*! Retrieve the average of the values of a given column. */
        QVariant avg(const Column &column);
        /*! Alias for the "avg" method. */
        QVariant average(const Column &column);

        /*! Execute an aggregate function on the database. */
        QVariant aggregate(const QString &function,
                           const QList<Column> &columns = {ASTERISK});

        /* Records exist */
        /*! Determine if any rows exist for the current query. */
        bool exists();
        /*! Determine if no rows exist for the current query. */
        bool doesntExist();

        /*! Execute the given callback if no rows exist for the current query. */
        bool existsOr(const std::function<void()> &callback);
        /*! Execute the given callback if rows exist for the current query. */
        bool doesntExistOr(const std::function<void()> &callback);

        /*! Execute the given callback if no rows exist for the current query. */
        template<typename R>
        std::pair<bool, R> existsOr(const std::function<R()> &callback);
        template<typename R>
        /*! Execute the given callback if rows exist for the current query. */
        std::pair<bool, R> doesntExistOr(const std::function<R()> &callback);

        /* Debugging */
        /*! Dump the current SQL and bindings. */
        void dump(bool replaceBindings = true, bool simpleBindings = false);
        /*! Die and dump the current SQL and bindings. */
        void dd(bool replaceBindings = true, bool simpleBindings = false);

        /* Others */
        /*! Increment a column's value by a given amount. */
        template<typename T = std::size_t> requires std::is_arithmetic_v<T>
        std::tuple<int, TSqlQuery>
        increment(const QString &column, T amount = 1,
                  const QList<UpdateItem> &extra = {});
        /*! Decrement a column's value by a given amount. */
        template<typename T = std::size_t> requires std::is_arithmetic_v<T>
        std::tuple<int, TSqlQuery>
        decrement(const QString &column, T amount = 1,
                  const QList<UpdateItem> &extra = {});

        /* Proxy methods that internally call the getQuery() (don't applySoftDeletes) */
        /* Insert, Update, Delete */
        /*! Insert a new record into the database. */
        std::optional<SqlQuery>
        insert(const QList<AttributeItem> &values) const;
        /*! Insert new records into the database. */
        std::optional<SqlQuery>
        insert(const QList<QList<AttributeItem>> &values) const;
        /*! Insert new records into the database (multi insert). */
        std::optional<SqlQuery>
        insert(const QList<QString> &columns, QList<QList<QVariant>> values) const;

        /*! Insert a new record and get the value of the primary key. */
        quint64 insertGetId(const QList<AttributeItem> &values,
                            const QString &sequence = "") const;

        /*! Insert a new record into the database while ignoring errors. */
        std::tuple<int, std::optional<TSqlQuery>>
        insertOrIgnore(const QList<AttributeItem> &values) const;
        /*! Insert new records into the database while ignoring errors. */
        std::tuple<int, std::optional<TSqlQuery>>
        insertOrIgnore(const QList<QList<AttributeItem>> &values) const;
        /*! Insert new records into the database while ignoring errors (multi insert). */
        std::tuple<int, std::optional<TSqlQuery>>
        insertOrIgnore(const QList<QString> &columns,
                       QList<QList<QVariant>> values) const;

        /*! Run the default delete function on the builder (sidestep soft deleting). */
        std::tuple<int, TSqlQuery> forceDelete() const;
        /*! Run the default delete function on the builder (sidestep soft deleting),
            alias. */
        std::tuple<int, TSqlQuery> forceRemove() const;

        /*! Run a truncate statement on the table. */
        void truncate() const;

        /* Select */
        /*! Set the columns to be selected. */
        TinyBuilder<Model> &select(const QList<Column> &columns = {ASTERISK});
        /*! Set the column to be selected. */
        TinyBuilder<Model> &select(const Column &column);
        /*! Add new select columns to the query. */
        TinyBuilder<Model> &addSelect(const QList<Column> &columns);
        /*! Add a new select column to the query. */
        TinyBuilder<Model> &addSelect(const Column &column);

        /*! Set the columns to be selected. */
        TinyBuilder<Model> &select(QList<Column> &&columns);
        /*! Set the column to be selected. */
        TinyBuilder<Model> &select(Column &&column);
        /*! Add new select columns to the query. */
        TinyBuilder<Model> &addSelect(QList<Column> &&columns);
        /*! Add a new select column to the query. */
        TinyBuilder<Model> &addSelect(Column &&column);

        /*! Set a select subquery on the query. */
        template<Queryable T>
        TinyBuilder<Model> &select(T &&query, const QString &as);
        /*! Add a select subquery to the query. */
        template<Queryable T>
        TinyBuilder<Model> &addSelect(T &&query, const QString &as);

        /*! Add a subselect expression to the query. */
        template<SubQuery T>
        TinyBuilder<Model> &selectSub(T &&query, const QString &as);
        /*! Add a new "raw" select expression to the query. */
        TinyBuilder<Model> &selectRaw(const QString &expression,
                                      const QList<QVariant> &bindings = {});

        /*! Force the query to only return distinct results. */
        TinyBuilder<Model> &distinct();
        /*! Force the query to only return distinct results. */
        TinyBuilder<Model> &distinct(const QStringList &columns);
        /*! Force the query to only return distinct results. */
        TinyBuilder<Model> &distinct(QStringList &&columns);

        /* Joins */
        /*! Add a join clause to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &join(T &&table, const QString &first,
                                 const QString &comparison, const QString &second,
                                 const QString &type = INNER, bool where = false);
        /*! Add an advanced join clause to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &join(T &&table,
                                 const std::function<void(JoinClause &)> &callback,
                                 const QString &type = INNER);
        /*! Add a "join where" clause to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &joinWhere(T &&table, const QString &first,
                                      const QString &comparison,
                                      const QVariant &second,
                                      const QString &type = INNER);

        /*! Add a left join to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &leftJoin(T &&table, const QString &first,
                                     const QString &comparison, const QString &second);
        /*! Add an advanced left join to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &leftJoin(T &&table,
                                     const std::function<void(JoinClause &)> &callback);
        /*! Add a "join where" clause to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &leftJoinWhere(T &&table, const QString &first,
                                          const QString &comparison,
                                          const QVariant &second);

        /*! Add a right join to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &rightJoin(T &&table, const QString &first,
                                      const QString &comparison, const QString &second);
        /*! Add an advanced right join to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &rightJoin(T &&table,
                                      const std::function<void(JoinClause &)> &callback);
        /*! Add a "right join where" clause to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &rightJoinWhere(T &&table, const QString &first,
                                           const QString &comparison,
                                           const QVariant &second);

        /*! Add a "cross join" clause to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &crossJoin(T &&table, const QString &first,
                                      const QString &comparison, const QString &second);
        /*! Add an advanced "cross join" clause to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &crossJoin(T &&table,
                                      const std::function<void(JoinClause &)> &callback);
        /*! Add a "cross join" clause to the query. */
        template<JoinTable T>
        TinyBuilder<Model> &crossJoin(T &&table);

        /*! Add a subquery join clause to the query. */
        template<SubQuery T>
        TinyBuilder<Model> &joinSub(T &&query, const QString &as, const QString &first,
                                    const QString &comparison, const QVariant &second,
                                    const QString &type = INNER, bool where = false);
        /*! Add a subquery join clause to the query. */
        template<SubQuery T>
        TinyBuilder<Model> &joinSub(T &&query, const QString &as,
                                    const std::function<void(JoinClause &)> &callback,
                                    const QString &type = INNER);

        /*! Add a subquery left join to the query. */
        template<SubQuery T>
        TinyBuilder<Model> &
        leftJoinSub(T &&query, const QString &as, const QString &first,
                    const QString &comparison, const QVariant &second);
        /*! Add a subquery left join to the query. */
        template<SubQuery T>
        TinyBuilder<Model> &
        leftJoinSub(T &&query, const QString &as,
                    const std::function<void(JoinClause &)> &callback);

        /*! Add a subquery right join to the query. */
        template<SubQuery T>
        TinyBuilder<Model> &
        rightJoinSub(T &&query, const QString &as, const QString &first,
                     const QString &comparison, const QVariant &second);
        /*! Add a subquery right join to the query. */
        template<SubQuery T>
        TinyBuilder<Model> &
        rightJoinSub(T &&query, const QString &as,
                     const std::function<void(JoinClause &)> &callback);

        /* General where */
        /*! Add a basic where clause to the query. */
        template<WhereValue T>
        TinyBuilder<Model> &where(const Column &column, const QString &comparison,
                                  T &&value, const QString &condition = AND);
        /*! Add an "or where" clause to the query. */
        template<WhereValue T>
        TinyBuilder<Model> &orWhere(const Column &column, const QString &comparison,
                                    T &&value);
        /*! Add a basic equal where clause to the query. */
        template<WhereValue T>
        TinyBuilder<Model> &whereEq(const Column &column, T &&value,
                                    const QString &condition = AND);
        /*! Add an equal "or where" clause to the query. */
        template<WhereValue T>
        TinyBuilder<Model> &orWhereEq(const Column &column, T &&value);

        /* General where not */
        /*! Add a basic "where not" clause to the query. */
        template<WhereValue T>
        TinyBuilder<Model> &whereNot(const Column &column, const QString &comparison,
                                     T &&value, const QString &condition = AND);
        /*! Add an "or where not" clause to the query. */
        template<WhereValue T>
        TinyBuilder<Model> &orWhereNot(const Column &column, const QString &comparison,
                                       T &&value);
        /*! Add a basic equal "where not" clause to the query. */
        template<WhereValue T>
        TinyBuilder<Model> &whereNotEq(const Column &column, T &&value,
                                       const QString &condition = AND);
        /*! Add an equal "or where not" clause to the query. */
        template<WhereValue T>
        TinyBuilder<Model> &orWhereNotEq(const Column &column, T &&value);

        /* Nested where */
        /*! Add a nested where clause to the query. */
        TinyBuilder<Model> &
        where(const std::function<void(TinyBuilder<Model> &)> &callback,
              const QString &condition = AND);
        /*! Add a nested "or where" clause to the query. */
        TinyBuilder<Model> &
        orWhere(const std::function<void(TinyBuilder<Model> &)> &callback);
        /*! Add a nested "where not" clause to the query. */
        TinyBuilder<Model> &
        whereNot(const std::function<void(TinyBuilder<Model> &)> &callback,
                 const QString &condition = AND);
        /*! Add a nested "or where not" clause to the query. */
        TinyBuilder<Model> &
        orWhereNot(const std::function<void(TinyBuilder<Model> &)> &callback);

        /* Array where */
        /*! Add a vector of basic where clauses to the query. */
        TinyBuilder<Model> &where(const QList<WhereItem> &values,
                                  const QString &condition = AND);
        /*! Add a vector of basic "or where" clauses to the query. */
        TinyBuilder<Model> &orWhere(const QList<WhereItem> &values);
        /*! Add a vector of basic "where not" clauses to the query. */
        TinyBuilder<Model> &whereNot(const QList<WhereItem> &values,
                                     const QString &condition = AND,
                                     const QString &defaultCondition = "");
        /*! Add a vector of basic "or where not" clauses to the query. */
        TinyBuilder<Model> &orWhereNot(const QList<WhereItem> &values,
                                       const QString &defaultCondition = "");

        /* where column */
        /*! Add a vector of where clauses comparing two columns to the query. */
        TinyBuilder<Model> &whereColumn(const QList<WhereColumnItem> &values,
                                        const QString &condition = AND);
        /*! Add a vector of "or where" clauses comparing two columns to the query. */
        TinyBuilder<Model> &orWhereColumn(const QList<WhereColumnItem> &values);

        /*! Add a "where" clause comparing two columns to the query. */
        TinyBuilder<Model> &whereColumn(const Column &first, const QString &comparison,
                                        const Column &second,
                                        const QString &condition = AND);
        /*! Add a "or where" clause comparing two columns to the query. */
        TinyBuilder<Model> &orWhereColumn(const Column &first, const QString &comparison,
                                          const Column &second);
        /*! Add an equal "where" clause comparing two columns to the query. */
        TinyBuilder<Model> &whereColumnEq(const Column &first, const Column &second,
                                          const QString &condition = AND);
        /*! Add an equal "or where" clause comparing two columns to the query. */
        TinyBuilder<Model> &orWhereColumnEq(const Column &first, const Column &second);

        /* where IN */
        /*! Add a "where in" clause to the query. */
        TinyBuilder<Model> &whereIn(const Column &column,
                                    const QList<QVariant> &values,
                                    const QString &condition = AND, bool nope = false);
        /*! Add an "or where in" clause to the query. */
        TinyBuilder<Model> &orWhereIn(const Column &column,
                                      const QList<QVariant> &values);
        /*! Add a "where not in" clause to the query. */
        TinyBuilder<Model> &whereNotIn(const Column &column,
                                       const QList<QVariant> &values,
                                       const QString &condition = AND);
        /*! Add an "or where not in" clause to the query. */
        TinyBuilder<Model> &orWhereNotIn(const Column &column,
                                         const QList<QVariant> &values);

        /* where null */
        /*! Add a "where null" clause to the query. */
        TinyBuilder<Model> &whereNull(const QList<Column> &columns = {ASTERISK},
                                      const QString &condition = AND, bool nope = false);
        /*! Add an "or where null" clause to the query. */
        TinyBuilder<Model> &orWhereNull(const QList<Column> &columns = {ASTERISK});
        /*! Add a "where not null" clause to the query. */
        TinyBuilder<Model> &whereNotNull(const QList<Column> &columns = {ASTERISK},
                                         const QString &condition = AND);
        /*! Add an "or where not null" clause to the query. */
        TinyBuilder<Model> &orWhereNotNull(const QList<Column> &columns = {ASTERISK});

        /*! Add a "where null" clause to the query. */
        TinyBuilder<Model> &whereNull(const Column &column,
                                      const QString &condition = AND, bool nope = false);
        /*! Add an "or where null" clause to the query. */
        TinyBuilder<Model> &orWhereNull(const Column &column);
        /*! Add a "where not null" clause to the query. */
        TinyBuilder<Model> &whereNotNull(const Column &column,
                                         const QString &condition = AND);
        /*! Add an "or where not null" clause to the query. */
        TinyBuilder<Model> &orWhereNotNull(const Column &column);

        /* where between */
        /*! Add a "where between" statement to the query. */
        TinyBuilder<Model> &whereBetween(
                const Column &column, const WhereBetweenItem &values,
                const QString &condition = AND, bool nope = false);
        /*! Add an "or where between" statement to the query. */
        TinyBuilder<Model> &orWhereBetween(
                const Column &column, const WhereBetweenItem &values);
        /*! Add a "where not between" statement to the query. */
        TinyBuilder<Model> &whereNotBetween(
                const Column &column, const WhereBetweenItem &values,
                const QString &condition = AND);
        /*! Add an "or where not between" statement to the query. */
        TinyBuilder<Model> &orWhereNotBetween(
                const Column &column, const WhereBetweenItem &values);

        /* where between columns */
        /*! Add a "where between" statement using columns to the query. */
        TinyBuilder<Model> &whereBetweenColumns(
                const Column &column, const WhereBetweenColumnsItem &betweenColumns,
                const QString &condition = AND, bool nope = false);
        /*! Add an "or where between" statement using columns to the query. */
        TinyBuilder<Model> &orWhereBetweenColumns(
                const Column &column, const WhereBetweenColumnsItem &betweenColumns);
        /*! Add a "where not between" statement using columns to the query. */
        TinyBuilder<Model> &whereNotBetweenColumns(
                const Column &column, const WhereBetweenColumnsItem &betweenColumns,
                const QString &condition = AND);
        /*! Add an "or where not between" statement using columns to the query. */
        TinyBuilder<Model> &orWhereNotBetweenColumns(
                const Column &column, const WhereBetweenColumnsItem &betweenColumns);

        /* where sub-queries */
        /*! Add a basic where clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        TinyBuilder<Model> &where(C &&column, const QString &comparison, V &&value,
                                  const QString &condition = AND);
        /*! Add an "or where" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        TinyBuilder<Model> &orWhere(C &&column, const QString &comparison, V &&value);
        /*! Add a basic equal where clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        TinyBuilder<Model> &whereEq(C &&column, V &&value,
                                    const QString &condition = AND);
        /*! Add an equal "or where" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        TinyBuilder<Model> &orWhereEq(C &&column, V &&value);

        /* where not sub-queries */
        /*! Add a basic "where not" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        TinyBuilder<Model> &whereNot(C &&column, const QString &comparison, V &&value,
                                     const QString &condition = AND);
        /*! Add an "or where not" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        TinyBuilder<Model> &orWhereNot(C &&column, const QString &comparison, V &&value);
        /*! Add a basic equal "where not" clause to the query with a full sub-select
            column. */
        template<Queryable C, WhereValue V>
        TinyBuilder<Model> &whereNotEq(C &&column, V &&value,
                                       const QString &condition = AND);
        /*! Add an equal "or where not" clause to the query with a full sub-select
            column. */
        template<Queryable C, WhereValue V>
        TinyBuilder<Model> &orWhereNotEq(C &&column, V &&value);

        /*! Add a full sub-select to the "where" clause. */
        template<WhereValueSubQuery T>
        TinyBuilder<Model> &whereSub(const Column &column, const QString &comparison,
                                     T &&query, const QString &condition = AND);

        /* where exists */
        /*! Add an exists clause to the query. */
        template<QueryableShared C>
        TinyBuilder<Model> &
        whereExists(C &&callback, const QString &condition = AND, bool nope = false);
        /*! Add an or exists clause to the query. */
        template<QueryableShared C>
        TinyBuilder<Model> &
        orWhereExists(C &&callback, bool nope = false);
        /*! Add a where not exists clause to the query. */
        template<QueryableShared C>
        TinyBuilder<Model> &
        whereNotExists(C &&callback, const QString &condition = AND);
        /*! Add a where not exists clause to the query. */
        template<QueryableShared C>
        TinyBuilder<Model> &
        orWhereNotExists(C &&callback);

        /* where row values */
        /*! Adds a where condition using row values. */
        TinyBuilder<Model> &
        whereRowValues(const QList<Column> &columns, const QString &comparison,
                       const QList<QVariant> &values, const QString &condition = AND);
        /*! Adds an or where condition using row values. */
        TinyBuilder<Model> &
        orWhereRowValues(const QList<Column> &columns, const QString &comparison,
                         const QList<QVariant> &values);
        TinyBuilder<Model> &
        whereRowValuesEq(const QList<Column> &columns, const QList<QVariant> &values,
                         const QString &condition = AND);
        /*! Adds an or where condition using row values. */
        TinyBuilder<Model> &
        orWhereRowValuesEq(const QList<Column> &columns,
                           const QList<QVariant> &values);

        /* where dates */
        /*! Add a "where date" statement to the query. */
        TinyBuilder<Model> &
        whereDate(const Column &column, const QString &comparison, QVariant value,
                  const QString &condition = AND);
        /*! Add a "where time" statement to the query. */
        TinyBuilder<Model> &
        whereTime(const Column &column, const QString &comparison, QVariant value,
                  const QString &condition = AND);
        /*! Add a "where day" statement to the query. */
        TinyBuilder<Model> &
        whereDay(const Column &column, const QString &comparison, QVariant value,
                 const QString &condition = AND);
        /*! Add a "where month" statement to the query. */
        TinyBuilder<Model> &
        whereMonth(const Column &column, const QString &comparison, QVariant value,
                   const QString &condition = AND);
        /*! Add a "where year" statement to the query. */
        TinyBuilder<Model> &
        whereYear(const Column &column, const QString &comparison, QVariant value,
                  const QString &condition = AND);

        /*! Add an equal "where date" statement to the query. */
        TinyBuilder<Model> &
        whereEqDate(const Column &column, QVariant value,
                    const QString &condition = AND);
        /*! Add an equal "where time" statement to the query. */
        TinyBuilder<Model> &
        whereEqTime(const Column &column, QVariant value,
                    const QString &condition = AND);
        /*! Add an equal "where day" statement to the query. */
        TinyBuilder<Model> &
        whereEqDay(const Column &column, QVariant value,
                   const QString &condition = AND);
        /*! Add an equal "where month" statement to the query. */
        TinyBuilder<Model> &
        whereEqMonth(const Column &column, QVariant value,
                     const QString &condition = AND);
        /*! Add an equal "where year" statement to the query. */
        TinyBuilder<Model> &
        whereEqYear(const Column &column, QVariant value,
                    const QString &condition = AND);

        /*! Add a "or where date" statement to the query. */
        TinyBuilder<Model> &
        orWhereDate(const Column &column, const QString &comparison, QVariant value);
        /*! Add a "or where time" statement to the query. */
        TinyBuilder<Model> &
        orWhereTime(const Column &column, const QString &comparison, QVariant value);
        /*! Add a "or where day" statement to the query. */
        TinyBuilder<Model> &
        orWhereDay(const Column &column, const QString &comparison, QVariant value);
        /*! Add a "or where month" statement to the query. */
        TinyBuilder<Model> &
        orWhereMonth(const Column &column, const QString &comparison, QVariant value);
        /*! Add a "or where year" statement to the query. */
        TinyBuilder<Model> &
        orWhereYear(const Column &column, const QString &comparison, QVariant value);

        /* where raw */
        /*! Add a raw "where" clause to the query. */
        TinyBuilder<Model> &whereRaw(const QString &sql,
                                     const QList<QVariant> &bindings = {},
                                     const QString &condition = AND);
        /*! Add a raw "or where" clause to the query. */
        TinyBuilder<Model> &orWhereRaw(const QString &sql,
                                       const QList<QVariant> &bindings = {});

        /* Group by and having */
        /*! Add a "group by" clause to the query. */
        TinyBuilder<Model> &groupBy(const QList<Column> &groups);
        /*! Add a "group by" clause to the query. */
        TinyBuilder<Model> &groupBy(const Column &group);
        /*! Add a "group by" clause to the query. */
        template<ColumnConcept ...Args>
        TinyBuilder<Model> &groupBy(Args &&...groups);

        /*! Add a raw "groupBy" clause to the query. */
        TinyBuilder<Model> &groupByRaw(const QString &sql,
                                       const QList<QVariant> &bindings = {});

        /*! Add a "having" clause to the query. */
        TinyBuilder<Model> &having(const Column &column, const QString &comparison,
                                   const QVariant &value,
                                   const QString &condition = AND);
        /*! Add an "or having" clause to the query. */
        TinyBuilder<Model> &orHaving(const Column &column, const QString &comparison,
                                     const QVariant &value);

        /*! Add a raw "having" clause to the query. */
        TinyBuilder<Model> &havingRaw(const QString &sql,
                                      const QList<QVariant> &bindings = {},
                                      const QString &condition = AND);
        /*! Add a raw "or having" clause to the query. */
        TinyBuilder<Model> &orHavingRaw(const QString &sql,
                                        const QList<QVariant> &bindings = {});

        /* Ordering */
        /*! Add an "order by" clause to the query. */
        TinyBuilder<Model> &orderBy(const Column &column,
                                    const QString &direction = ASC);
        /*! Add a descending "order by" clause to the query. */
        TinyBuilder<Model> &orderByDesc(const Column &column);

        /*! Add an "order by" clause to the query with a subquery ordering. */
        template<Queryable T>
        TinyBuilder<Model> &orderBy(T &&query, const QString &direction = ASC);
        /*! Add a descending "order by" clause to the query with a subquery ordering. */
        template<Queryable T>
        TinyBuilder<Model> &orderByDesc(T &&query);

        /*! Put the query's results in random order. */
        TinyBuilder<Model> &inRandomOrder(const QString &seed = "");
        /*! Add a raw "order by" clause to the query. */
        TinyBuilder<Model> &orderByRaw(const QString &sql,
                                       const QList<QVariant> &bindings = {});

        /*! Add an "order by" clause for a timestamp to the query. */
        TinyBuilder<Model> &latest(const Column &column = "");
        /*! Add an "order by" clause for a timestamp to the query. */
        TinyBuilder<Model> &oldest(const Column &column = "");
        /*! Remove all existing orders. */
        TinyBuilder<Model> &reorder();
        /*! Remove all existing orders and optionally add a new order. */
        TinyBuilder<Model> &reorder(const Column &column,
                                    const QString &direction = ASC);

        /*! Set the "limit" value of the query. */
        TinyBuilder<Model> &limit(qint64 value);
        /*! Alias to set the "limit" value of the query. */
        TinyBuilder<Model> &take(qint64 value);
        /*! Set the "offset" value of the query. */
        TinyBuilder<Model> &offset(qint64 value);
        /*! Alias to set the "offset" value of the query. */
        TinyBuilder<Model> &skip(qint64 value);
        /*! Set the limit and offset for a given page. */
        TinyBuilder<Model> &forPage(qint64 page, qint64 perPage = 30);

        /*! Constrain the query to the previous "page" of results before a given ID. */
        TinyBuilder<Model> &
        forPageBeforeId(qint64 perPage = 30, const QVariant &lastId = {},
                        const QString &column = Orm::Constants::ID,
                        bool prependOrder = false);
        /*! Constrain the query to the next "page" of results after a given ID. */
        TinyBuilder<Model> &
        forPageAfterId(qint64 perPage = 30, const QVariant &lastId = {},
                       const QString &column = Orm::Constants::ID,
                       bool prependOrder = false);

        /* Pessimistic Locking */
        /*! Lock the selected rows in the table for updating. */
        TinyBuilder<Model> &lockForUpdate();
        /*! Share lock the selected rows in the table. */
        TinyBuilder<Model> &sharedLock();
        /*! Lock the selected rows in the table. */
        TinyBuilder<Model> &lock(bool value = true);
        /*! Lock the selected rows in the table. */
        TinyBuilder<Model> &lock(const char *value);
        /*! Lock the selected rows in the table. */
        TinyBuilder<Model> &lock(const QString &value);
        /*! Lock the selected rows in the table. */
        TinyBuilder<Model> &lock(QString &&value);

        /* Others proxy methods, not added to the Model and Relation */
        /*! Add an "exists" clause to the query. */
        TinyBuilder<Model> &
        addWhereExistsQuery(const std::shared_ptr<QueryBuilder> &query,
                            const QString &condition = AND, bool nope = false);

        /*! Merge an array of where clauses and bindings. */
        TinyBuilder<Model> &mergeWheres(const QList<WhereConditionItem> &wheres,
                                        const QList<QVariant> &bindings);
        /*! Merge an array of where clauses and bindings. */
        TinyBuilder<Model> &mergeWheres(QList<WhereConditionItem> &&wheres,
                                        QList<QVariant> &&bindings);

    private:
        /*! Static cast this to a child's instance TinyBuilder type. */
        inline const TinyBuilder<Model> &builder() const noexcept;
        /*! Static cast this to a child's instance TinyBuilder type, const version. */
        inline TinyBuilder<Model> &builder() noexcept;
        /*! Get a base query builder instance with applied SoftDeletes. */
        QueryBuilder &toBase();
        /*! Get a base query builder instance with applied SoftDeletes. */
        QueryBuilder &getQuery() const noexcept;
    };

    /* public */

    template<typename Model>
    const FromClause &BuilderProxies<Model>::from()
    {
        return toBase().getFrom();
    }

    /* Proxy methods that internally call the toBase() (applySoftDeletes) */

    /* Retrieving results */

    template<typename Model>
    QString
    BuilderProxies<Model>::implode(const QString &column, const QString &glue)
    {
        return toBase().implode(column, glue);
    }

    /* Aggregates */

    template<typename Model>
    quint64 BuilderProxies<Model>::count(const QList<Column> &columns)
    {
        return toBase().count(columns);
    }

    template<typename Model>
    template<typename>
    quint64 BuilderProxies<Model>::count(const Column &column)
    {
        return toBase().count(QList<Column> {column});
    }

    template<typename Model>
    QVariant BuilderProxies<Model>::min(const Column &column)
    {
        return toBase().min(column);
    }

    template<typename Model>
    QVariant BuilderProxies<Model>::max(const Column &column)
    {
        return toBase().max(column);
    }

    template<typename Model>
    QVariant BuilderProxies<Model>::sum(const Column &column)
    {
        return toBase().sum(column);
    }

    template<typename Model>
    QVariant BuilderProxies<Model>::avg(const Column &column)
    {
        return toBase().avg(column);
    }

    template<typename Model>
    QVariant BuilderProxies<Model>::average(const Column &column)
    {
        return toBase().avg(column);
    }

    template<typename Model>
    QVariant
    BuilderProxies<Model>::aggregate(const QString &function,
                                     const QList<Column> &columns)
    {
        return toBase().aggregate(function, columns);
    }

    /* Records exist */

    template<typename Model>
    bool BuilderProxies<Model>::exists()
    {
        return toBase().exists();
    }

    template<typename Model>
    bool BuilderProxies<Model>::doesntExist()
    {
        return toBase().doesntExist();
    }

    template<typename Model>
    bool BuilderProxies<Model>::existsOr(const std::function<void()> &callback)
    {
        return toBase().existsOr(callback);
    }

    template<typename Model>
    bool
    BuilderProxies<Model>::doesntExistOr(const std::function<void()> &callback)
    {
        return toBase().doesntExistOr(callback);
    }

    template<typename Model>
    template<typename R>
    std::pair<bool, R>
    BuilderProxies<Model>::existsOr(const std::function<R()> &callback)
    {
        return toBase().template existsOr<R>(callback);
    }

    template<typename Model>
    template<typename R>
    std::pair<bool, R>
    BuilderProxies<Model>::doesntExistOr(const std::function<R()> &callback)
    {
        return toBase().template doesntExistOr<R>(callback);
    }

    /* Debugging */

    template<typename Model>
    void BuilderProxies<Model>::dump(const bool replaceBindings,
                                     const bool simpleBindings)
    {
        toBase().dump(replaceBindings, simpleBindings);
    }

    template<typename Model>
    void BuilderProxies<Model>::dd(const bool replaceBindings, const bool simpleBindings)
    {
        toBase().dd(replaceBindings, simpleBindings);
    }

    /* Others */

    template<typename Model>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, TSqlQuery>
    BuilderProxies<Model>::increment(
            const QString &column, const T amount, const QList<UpdateItem> &extra)
    {
        return toBase().increment(column, amount, builder().addUpdatedAtColumn(extra));
    }

    template<typename Model>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, TSqlQuery>
    BuilderProxies<Model>::decrement(
            const QString &column, const T amount, const QList<UpdateItem> &extra)
    {
        return toBase().decrement(column, amount, builder().addUpdatedAtColumn(extra));
    }

    /* Insert, Update, Delete */

    /* Proxy methods that internally call the getQuery() (don't applySoftDeletes) */

    template<typename Model>
    std::optional<SqlQuery>
    BuilderProxies<Model>::insert(const QList<AttributeItem> &values) const
    {
        return getQuery().insert(AttributeUtils::convertVectorToMap(values));
    }

    template<typename Model>
    std::optional<SqlQuery>
    BuilderProxies<Model>::insert(const QList<QList<AttributeItem>> &values) const
    {
        return getQuery().insert(AttributeUtils::convertVectorsToMaps(values));
    }

    template<typename Model>
    std::optional<SqlQuery>
    BuilderProxies<Model>::insert(
            const QList<QString> &columns, QList<QList<QVariant>> values) const
    {
        return getQuery().insert(columns, std::move(values));
    }

    // FEATURE dilemma primarykey, Model::KeyType vs QVariant silverqx
    template<typename Model>
    quint64
    BuilderProxies<Model>::insertGetId(const QList<AttributeItem> &values,
                                       const QString &sequence) const
    {
        return getQuery().insertGetId(AttributeUtils::convertVectorToMap(values),
                                      sequence);
    }

    template<typename Model>
    std::tuple<int, std::optional<TSqlQuery>>
    BuilderProxies<Model>::insertOrIgnore(const QList<AttributeItem> &values) const
    {
        return getQuery().insertOrIgnore(AttributeUtils::convertVectorToMap(values));
    }

    template<typename Model>
    std::tuple<int, std::optional<TSqlQuery>>
    BuilderProxies<Model>::insertOrIgnore(
            const QList<QList<AttributeItem>> &values) const
    {
        return getQuery().insertOrIgnore(AttributeUtils::convertVectorsToMaps(values));
    }

    template<typename Model>
    std::tuple<int, std::optional<TSqlQuery>>
    BuilderProxies<Model>::insertOrIgnore(
            const QList<QString> &columns, QList<QList<QVariant>> values) const
    {
        return getQuery().insertOrIgnore(columns, std::move(values));
    }

    template<typename Model>
    std::tuple<int, TSqlQuery> BuilderProxies<Model>::forceDelete() const
    {
        // Skip applying SoftDeletes (getQuery()) to actually delete
        return getQuery().remove();
    }

    template<typename Model>
    std::tuple<int, TSqlQuery> BuilderProxies<Model>::forceRemove() const
    {
        // Skip applying SoftDeletes (getQuery()) to actually delete
        return getQuery().remove();
    }

    template<typename Model>
    void BuilderProxies<Model>::truncate() const
    {
        getQuery().truncate();
    }

    /* Select */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::select(const QList<Column> &columns)
    {
        getQuery().select(columns);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::select(const Column &column)
    {
        getQuery().select(column);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::addSelect(const QList<Column> &columns)
    {
        getQuery().addSelect(columns);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::addSelect(const Column &column)
    {
        getQuery().addSelect(column);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::select(QList<Column> &&columns)
    {
        getQuery().select(std::move(columns));
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::select(Column &&column)
    {
        getQuery().select(std::move(column));
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::addSelect(QList<Column> &&columns)
    {
        getQuery().addSelect(std::move(columns));
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::addSelect(Column &&column)
    {
        getQuery().addSelect(std::move(column));
        return builder();
    }

    template<typename Model>
    template<Queryable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::select(T &&query, const QString &as)
    {
        getQuery().select(std::forward<T>(query), as);
        return builder();
    }

    template<typename Model>
    template<Queryable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::addSelect(T &&query, const QString &as)
    {
        getQuery().addSelect(std::forward<T>(query), as);
        return builder();
    }

    template<typename Model>
    template<SubQuery T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::selectSub(T &&query, const QString &as)
    {
        getQuery().selectSub(std::forward<T>(query), as);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::selectRaw(const QString &expression,
                                     const QList<QVariant> &bindings)
    {
        getQuery().selectRaw(expression, bindings);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &BuilderProxies<Model>::distinct()
    {
        getQuery().distinct();
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::distinct(const QStringList &columns)
    {
        getQuery().distinct(columns);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::distinct(QStringList &&columns)
    {
        getQuery().distinct(std::move(columns));
        return builder();
    }

    /* Joins */

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::join(
            T &&table, const QString &first, const QString &comparison,
            const QString &second, const QString &type, const bool where)
    {
        getQuery().join(std::forward<T>(table), first, comparison, second, type, where);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::join(
            T &&table, const std::function<void(JoinClause &)> &callback,
            const QString &type)
    {
        getQuery().join(std::forward<T>(table), callback, type);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::joinWhere(
            T &&table, const QString &first, const QString &comparison,
            const QVariant &second, const QString &type)
    {
        getQuery().joinWhere(std::forward<T>(table), first, comparison, second, type);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::leftJoin(
            T &&table, const QString &first, const QString &comparison,
            const QString &second)
    {
        getQuery().leftJoin(std::forward<T>(table), first, comparison, second);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::leftJoin(
            T &&table, const std::function<void(JoinClause &)> &callback)
    {
        getQuery().leftJoin(std::forward<T>(table), callback);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::leftJoinWhere(
            T &&table, const QString &first, const QString &comparison,
            const QVariant &second)
    {
        getQuery().leftJoinWhere(std::forward<T>(table), first, comparison, second);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::rightJoin(
            T &&table, const QString &first, const QString &comparison,
            const QString &second)
    {
        getQuery().rightJoin(std::forward<T>(table), first, comparison, second);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::rightJoin(
            T &&table, const std::function<void(JoinClause &)> &callback)
    {
        getQuery().rightJoin(std::forward<T>(table), callback);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::rightJoinWhere(
            T &&table, const QString &first, const QString &comparison,
            const QVariant &second)
    {
        getQuery().rightJoinWhere(std::forward<T>(table), first, comparison, second);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::crossJoin(
            T &&table, const QString &first, const QString &comparison,
            const QString &second)
    {
        getQuery().crossJoin(std::forward<T>(table), first, comparison, second);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::crossJoin(
            T &&table, const std::function<void(JoinClause &)> &callback)
    {
        getQuery().crossJoin(std::forward<T>(table), callback);
        return builder();
    }

    template<typename Model>
    template<JoinTable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::crossJoin(T &&table)
    {
        getQuery().crossJoin(std::forward<T>(table));
        return builder();
    }

    template<typename Model>
    template<SubQuery T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::joinSub(
            T &&query, const QString &as, const QString &first,
            const QString &comparison, const QVariant &second, const QString &type,
            const bool where)
    {
        getQuery().joinSub(std::forward<T>(query), as, first, comparison, second, type,
                           where);
        return builder();
    }

    template<typename Model>
    template<SubQuery T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::joinSub(
            T &&query, const QString &as,
            const std::function<void(JoinClause &)> &callback, const QString &type)
    {
        getQuery().joinSub(std::forward<T>(query), as, callback, type);
        return builder();
    }

    template<typename Model>
    template<SubQuery T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::leftJoinSub(
            T &&query, const QString &as, const QString &first,
            const QString &comparison, const QVariant &second)
    {
        getQuery().leftJoinSub(std::forward<T>(query), as, first, comparison, second);
        return builder();
    }

    template<typename Model>
    template<SubQuery T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::leftJoinSub(
            T &&query, const QString &as,
            const std::function<void(JoinClause &)> &callback)
    {
        getQuery().joinSub(std::forward<T>(query), as, callback, LEFT);
        return builder();
    }

    template<typename Model>
    template<SubQuery T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::rightJoinSub(
            T &&query, const QString &as, const QString &first,
            const QString &comparison, const QVariant &second)
    {
        getQuery().rightJoinSub(std::forward<T>(query), as, first, comparison, second);
        return builder();
    }

    template<typename Model>
    template<SubQuery T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::rightJoinSub(
            T &&query, const QString &as,
            const std::function<void(JoinClause &)> &callback)
    {
        getQuery().joinSub(std::forward<T>(query), as, callback, RIGHT);
        return builder();
    }

    /* General where */

    template<typename Model>
    template<WhereValue T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::where(
            const Column &column, const QString &comparison, T &&value,
            const QString &condition)
    {
        getQuery().where(column, comparison, std::forward<T>(value), condition);
        return builder();
    }

    template<typename Model>
    template<WhereValue T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhere(
            const Column &column, const QString &comparison, T &&value)
    {
        getQuery().orWhere(column, comparison, std::forward<T>(value));
        return builder();
    }

    template<typename Model>
    template<WhereValue T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereEq(
            const Column &column, T &&value, const QString &condition)
    {
        getQuery().whereEq(column, std::forward<T>(value), condition);
        return builder();
    }

    template<typename Model>
    template<WhereValue T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereEq(const Column &column, T &&value)
    {
        getQuery().orWhereEq(column, std::forward<T>(value));
        return builder();
    }

    /* General where not */

    template<typename Model>
    template<WhereValue T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNot(
            const Column &column, const QString &comparison, T &&value,
            const QString &condition)
    {
        getQuery().whereNot(column, comparison, std::forward<T>(value), condition);
        return builder();
    }

    template<typename Model>
    template<WhereValue T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNot(
            const Column &column, const QString &comparison, T &&value)
    {
        getQuery().orWhereNot(column, comparison, std::forward<T>(value));
        return builder();
    }

    template<typename Model>
    template<WhereValue T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNotEq(
            const Column &column, T &&value, const QString &condition)
    {
        getQuery().whereNotEq(column, std::forward<T>(value), condition);
        return builder();
    }

    template<typename Model>
    template<WhereValue T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNotEq(const Column &column, T &&value)
    {
        getQuery().orWhereNotEq(column, std::forward<T>(value));
        return builder();
    }

    /* Nested where */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::where(
            const std::function<void(TinyBuilder<Model> &)> &callback,
            const QString &condition)
    {
        // Ownership of a unique_ptr()
        auto query = builder().m_model.newQueryWithoutRelationships();

        std::invoke(callback, *query);

        getQuery().addNestedWhereQuery(query->getQueryShared(), condition);

        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhere(
            const std::function<void(TinyBuilder<Model> &)> &callback)
    {
        return where(callback, OR);
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNot(
            const std::function<void(TinyBuilder<Model> &)> &callback,
            const QString &condition)
    {
        return where(callback, SPACE_IN.arg(condition, NOT));
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNot(
            const std::function<void(TinyBuilder<Model> &)> &callback)
    {
        return where(callback, SPACE_IN.arg(OR, NOT));
    }

    /* Array where */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::where(
            const QList<WhereItem> &values, const QString &condition)
    {
        getQuery().where(values, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhere(const QList<WhereItem> &values)
    {
        getQuery().orWhere(values);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNot(
            const QList<WhereItem> &values, const QString &condition,
            const QString &defaultCondition)
    {
        getQuery().whereNot(values, condition, defaultCondition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNot(
            const QList<WhereItem> &values, const QString &defaultCondition)
    {
        getQuery().orWhereNot(values, defaultCondition);
        return builder();
    }

    /* where column */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereColumn(
            const QList<WhereColumnItem> &values, const QString &condition)
    {
        getQuery().whereColumn(values, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereColumn(const QList<WhereColumnItem> &values)
    {
        getQuery().orWhereColumn(values);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereColumn(
            const Column &first, const QString &comparison, const Column &second,
            const QString &condition)
    {
        getQuery().whereColumn(first, comparison, second, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereColumn(
            const Column &first, const QString &comparison, const Column &second)
    {
        getQuery().orWhereColumn(first, comparison, second);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereColumnEq(
            const Column &first, const Column &second, const QString &condition)
    {
        getQuery().whereColumnEq(first, second, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereColumnEq(const Column &first, const Column &second)
    {
        getQuery().orWhereColumnEq(first, second);
        return builder();
    }

    /* where IN */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereIn(
            const Column &column, const QList<QVariant> &values,
            const QString &condition, const bool nope)
    {
        getQuery().whereIn(column, values, condition, nope);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereIn(
            const Column &column, const QList<QVariant> &values)
    {
        getQuery().orWhereIn(column, values);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNotIn(
            const Column &column, const QList<QVariant> &values,
            const QString &condition)
    {
        getQuery().whereNotIn(column, values, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNotIn(
            const Column &column, const QList<QVariant> &values)
    {
        getQuery().orWhereNotIn(column, values);
        return builder();
    }

    /* where null */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNull(
            const QList<Column> &columns, const QString &condition, const bool nope)
    {
        getQuery().whereNull(columns, condition, nope);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNull(const QList<Column> &columns)
    {
        getQuery().orWhereNull(columns);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNotNull(
            const QList<Column> &columns, const QString &condition)
    {
        getQuery().whereNotNull(columns, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNotNull(const QList<Column> &columns)
    {
        getQuery().orWhereNotNull(columns);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNull(
            const Column &column, const QString &condition, const bool nope)
    {
        getQuery().whereNull(column, condition, nope);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNull(const Column &column)
    {
        getQuery().orWhereNull(column);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNotNull(const Column &column, const QString &condition)
    {
        getQuery().whereNotNull(column, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNotNull(const Column &column)
    {
        getQuery().orWhereNotNull(column);
        return builder();
    }

    /* where between columns */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereBetween(
            const Column &column, const WhereBetweenItem &values,
            const QString &condition, const bool nope)
    {
        getQuery().whereBetween(column, values, condition, nope);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereBetween(
            const Column &column, const WhereBetweenItem &values)
    {
        getQuery().whereBetween(column, values, OR);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNotBetween(
            const Column &column, const WhereBetweenItem &values,
            const QString &condition)
    {
        getQuery().whereBetween(column, values, condition, true);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNotBetween(
            const Column &column, const WhereBetweenItem &values)
    {
        getQuery().whereBetween(column, values, OR, true);
        return builder();
    }

    /* where between columns */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereBetweenColumns(
            const Column &column, const WhereBetweenColumnsItem &betweenColumns,
            const QString &condition, const bool nope)
    {
        getQuery().whereBetweenColumns(column, betweenColumns, condition, nope);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereBetweenColumns(
            const Column &column, const WhereBetweenColumnsItem &betweenColumns)
    {
        getQuery().whereBetweenColumns(column, betweenColumns, OR);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNotBetweenColumns(
            const Column &column, const WhereBetweenColumnsItem &betweenColumns,
            const QString &condition)
    {
        getQuery().whereBetweenColumns(column, betweenColumns, condition, true);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNotBetweenColumns(
            const Column &column, const WhereBetweenColumnsItem &betweenColumns)
    {
        getQuery().whereBetweenColumns(column, betweenColumns, OR, true);
        return builder();
    }

    /* where sub-queries */

    template<typename Model>
    template<Queryable C, WhereValue V>
    TinyBuilder<Model> &
    BuilderProxies<Model>::where(
            C &&column, const QString &comparison, V &&value, const QString &condition)
    {
        getQuery().where(std::forward<C>(column), comparison, std::forward<V>(value),
                         condition);
        return builder();
    }

    template<typename Model>
    template<Queryable C, WhereValue V>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhere(C &&column, const QString &comparison, V &&value)
    {
        getQuery().where(std::forward<C>(column), comparison, std::forward<V>(value), OR);
        return builder();
    }

    template<typename Model>
    template<Queryable C, WhereValue V>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereEq(C &&column, V &&value, const QString &condition)
    {
        getQuery().where(std::forward<C>(column), EQ, std::forward<V>(value), condition);
        return builder();
    }

    template<typename Model>
    template<Queryable C, WhereValue V>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereEq(C &&column, V &&value)
    {
        getQuery().where(std::forward<C>(column), EQ, std::forward<V>(value), OR);
        return builder();
    }

    /* where not sub-queries */

    template<typename Model>
    template<Queryable C, WhereValue V>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNot(
            C &&column, const QString &comparison, V &&value, const QString &condition)
    {
        getQuery().whereNot(std::forward<C>(column), comparison, std::forward<V>(value),
                            condition);
        return builder();
    }

    template<typename Model>
    template<Queryable C, WhereValue V>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNot(C &&column, const QString &comparison, V &&value)
    {
        getQuery().orWhereNot(std::forward<C>(column), comparison,
                              std::forward<V>(value));
        return builder();
    }

    template<typename Model>
    template<Queryable C, WhereValue V>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNotEq(C &&column, V &&value, const QString &condition)
    {
        getQuery().whereNotEq(std::forward<C>(column), std::forward<V>(value), condition);
        return builder();
    }

    template<typename Model>
    template<Queryable C, WhereValue V>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNotEq(C &&column, V &&value)
    {
        getQuery().orWhereNotEq(std::forward<C>(column), std::forward<V>(value));
        return builder();
    }

    template<typename Model>
    template<WhereValueSubQuery T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereSub(
            const Column &column, const QString &comparison, T &&query,
            const QString &condition)
    {
        getQuery().whereSub(column, comparison, std::forward<T>(query), condition);
        return builder();
    }

    /* where exists */

    template<typename Model>
    template<QueryableShared C>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereExists(C &&callback, const QString &condition,
                                       const bool nope)
    {
        getQuery().whereExists(std::forward<C>(callback), condition, nope);
        return builder();
    }

    template<typename Model>
    template<QueryableShared C>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereExists(C &&callback, const bool nope)
    {
        getQuery().whereExists(std::forward<C>(callback), OR, nope);
        return builder();
    }

    template<typename Model>
    template<QueryableShared C>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereNotExists(C &&callback, const QString &condition)
    {
        getQuery().whereExists(std::forward<C>(callback), condition, true);
        return builder();
    }

    template<typename Model>
    template<QueryableShared C>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereNotExists(C &&callback)
    {
        getQuery().whereExists(std::forward<C>(callback), OR, true);
        return builder();
    }

    /* where row values */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereRowValues(
            const QList<Column> &columns, const QString &comparison,
            const QList<QVariant> &values, const QString &condition)
    {
        getQuery().whereRowValues(columns, comparison, values, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereRowValues(
            const QList<Column> &columns, const QString &comparison,
            const QList<QVariant> &values)
    {
        getQuery().whereRowValues(columns, comparison, values, OR);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereRowValuesEq(
            const QList<Column> &columns, const QList<QVariant> &values,
            const QString &condition)
    {
        getQuery().whereRowValues(columns, EQ, values, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereRowValuesEq(const QList<Column> &columns,
                                              const QList<QVariant> &values)
    {
        getQuery().whereRowValues(columns, EQ, values, OR);
        return builder();
    }

    /* where dates */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereDate(const Column &column, const QString &comparison,
                                     QVariant value, const QString &condition)
    {
        getQuery().whereDate(column, comparison, std::move(value), condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereTime(const Column &column, const QString &comparison,
                                     QVariant value, const QString &condition)
    {
        getQuery().whereTime(column, comparison, std::move(value), condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereDay(const Column &column, const QString &comparison,
                                    QVariant value, const QString &condition)
    {
        getQuery().whereDay(column, comparison, std::move(value), condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereMonth(const Column &column, const QString &comparison,
                                      QVariant value, const QString &condition)
    {
        getQuery().whereMonth(column, comparison, std::move(value), condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereYear(const Column &column, const QString &comparison,
                                     QVariant value, const QString &condition)
    {
        getQuery().whereYear(column, comparison, std::move(value), condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereEqDate(const Column &column, QVariant value,
                                       const QString &condition)
    {
        getQuery().whereDate(column, EQ, std::move(value), condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereEqTime(const Column &column, QVariant value,
                                       const QString &condition)
    {
        getQuery().whereTime(column, EQ, std::move(value), condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereEqDay(const Column &column, QVariant value,
                                      const QString &condition)
    {
        getQuery().whereDay(column, EQ, std::move(value), condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereEqMonth(const Column &column, QVariant value,
                                        const QString &condition)
    {
        getQuery().whereMonth(column, EQ, std::move(value), condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereEqYear(const Column &column, QVariant value,
                                       const QString &condition)
    {
        getQuery().whereYear(column, EQ, std::move(value), condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereDate(const Column &column, const QString &comparison,
                                     QVariant value)
    {
        getQuery().whereDate(column, comparison, std::move(value), OR);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereTime(const Column &column, const QString &comparison,
                                     QVariant value)
    {
        getQuery().whereTime(column, comparison, std::move(value), OR);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereDay(const Column &column, const QString &comparison,
                                    QVariant value)
    {
        getQuery().whereDay(column, comparison, std::move(value), OR);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereMonth(const Column &column, const QString &comparison,
                                      QVariant value)
    {
        getQuery().whereMonth(column, comparison, std::move(value), OR);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereYear(const Column &column, const QString &comparison,
                                       QVariant value)
    {
        getQuery().whereYear(column, comparison, std::move(value), OR);
        return builder();
    }

    /* where raw */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::whereRaw(
            const QString &sql, const QList<QVariant> &bindings,
            const QString &condition)
    {
        getQuery().whereRaw(sql, bindings, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orWhereRaw(
            const QString &sql, const QList<QVariant> &bindings)
    {
        getQuery().whereRaw(sql, bindings, OR);
        return builder();
    }

    /* Group by and having */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::groupBy(const QList<Column> &groups)
    {
        getQuery().groupBy(groups);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::groupBy(const Column &group)
    {
        getQuery().groupBy(group);
        return builder();
    }

    template<typename Model>
    template<ColumnConcept ...Args>
    TinyBuilder<Model> &
    BuilderProxies<Model>::groupBy(Args &&...groups)
    {
        getQuery().groupBy(QList<Column> {std::forward<Args>(groups)...});
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::groupByRaw(
            const QString &sql, const QList<QVariant> &bindings)
    {
        getQuery().groupByRaw(sql, bindings);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::having(
            const Column &column, const QString &comparison, const QVariant &value,
            const QString &condition)
    {
        getQuery().having(column, comparison, value, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orHaving(
            const Column &column, const QString &comparison, const QVariant &value)
    {
        getQuery().orHaving(column, comparison, value);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::havingRaw(
            const QString &sql, const QList<QVariant> &bindings,
            const QString &condition)
    {
        getQuery().havingRaw(sql, bindings, condition);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orHavingRaw(
            const QString &sql, const QList<QVariant> &bindings)
    {
        getQuery().havingRaw(sql, bindings, OR);
        return builder();
    }

    /* Ordering */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orderBy(const Column &column, const QString &direction)
    {
        getQuery().orderBy(column, direction);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orderByDesc(const Column &column)
    {
        getQuery().orderByDesc(column);
        return builder();
    }

    template<typename Model>
    template<Queryable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orderBy(T &&query, const QString &direction)
    {
        getQuery().orderBy(std::forward<T>(query), direction);
        return builder();
    }

    template<typename Model>
    template<Queryable T>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orderByDesc(T &&query)
    {
        getQuery().orderBy(std::forward<T>(query), DESC);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::inRandomOrder(const QString &seed)
    {
        getQuery().inRandomOrder(seed);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::orderByRaw(
            const QString &sql, const QList<QVariant> &bindings)
    {
        getQuery().orderByRaw(sql, bindings);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::latest(const Column &column)
    {
        getQuery().latest(builder().getCreatedAtColumnForLatestOldest(column));
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::oldest(const Column &column)
    {
        getQuery().oldest(builder().getCreatedAtColumnForLatestOldest(column));
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::reorder()
    {
        getQuery().reorder();
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::reorder(const Column &column, const QString &direction)
    {
        getQuery().reorder(column, direction);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::limit(const qint64 value)
    {
        getQuery().limit(value);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::take(const qint64 value)
    {
        return limit(value);
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::offset(const qint64 value)
    {
        getQuery().offset(value);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::skip(const qint64 value)
    {
        return offset(value);
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::forPage(const qint64 page, const qint64 perPage)
    {
        getQuery().forPage(page, perPage);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::forPageBeforeId(const qint64 perPage, const QVariant &lastId,
                                           const QString &column, const bool prependOrder)
    {
        getQuery().forPageBeforeId(perPage, lastId, column, prependOrder);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::forPageAfterId(const qint64 perPage, const QVariant &lastId,
                                          const QString &column, const bool prependOrder)
    {
        getQuery().forPageAfterId(perPage, lastId, column, prependOrder);
        return builder();
    }

    /* Pessimistic Locking */

    template<typename Model>
    TinyBuilder<Model> &BuilderProxies<Model>::lockForUpdate()
    {
        getQuery().lock(true);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &BuilderProxies<Model>::sharedLock()
    {
        getQuery().lock(false);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &BuilderProxies<Model>::lock(const bool value)
    {
        getQuery().lock(value);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &BuilderProxies<Model>::lock(const char *value)
    {
        getQuery().lock(value);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &BuilderProxies<Model>::lock(const QString &value)
    {
        getQuery().lock(value);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &BuilderProxies<Model>::lock(QString &&value)
    {
        getQuery().lock(std::move(value));
        return builder();
    }

    /* Others proxy methods, not added to the Model and Relation */

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::addWhereExistsQuery(
            const std::shared_ptr<QueryBuilder> &query, const QString &condition,
            const bool nope)
    {
        getQuery().addWhereExistsQuery(query, condition, nope);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::mergeWheres(
            const QList<WhereConditionItem> &wheres, const QList<QVariant> &bindings)
    {
        getQuery().mergeWheres(wheres, bindings);
        return builder();
    }

    template<typename Model>
    TinyBuilder<Model> &
    BuilderProxies<Model>::mergeWheres(
            QList<WhereConditionItem> &&wheres, QList<QVariant> &&bindings)
    {
        getQuery().mergeWheres(std::move(wheres), std::move(bindings));
        return builder();
    }

    /* private */

    /* This builder() const vs non-const is correct, only the update() calls
       the non-const version, all other proxy methods call the const version.
       All TinyBuilderProxy methods can not be made const, I have verified this.
       It would be possible to make all const, but it would not be correct. */

    template<typename Model>
    const TinyBuilder<Model> &BuilderProxies<Model>::builder() const noexcept
    {
        return static_cast<const TinyBuilder<Model> &>(*this);
    }

    template<typename Model>
    TinyBuilder<Model> &BuilderProxies<Model>::builder() noexcept
    {
        return static_cast<TinyBuilder<Model> &>(*this);
    }

    template<typename Model>
    QueryBuilder &BuilderProxies<Model>::toBase()
    {
        return builder().toBase();
    }

    template<typename Model>
    QueryBuilder &BuilderProxies<Model>::getQuery() const noexcept
    {
        return builder().getQuery();
    }

} // namespace Tiny
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_TINYBUILDERPROXIES_HPP
