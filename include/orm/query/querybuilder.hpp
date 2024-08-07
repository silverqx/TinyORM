#pragma once
#ifndef ORM_QUERY_QUERYBUILDER_HPP
#define ORM_QUERY_QUERYBUILDER_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/query/concerns/buildsqueries.hpp"
#include "orm/query/grammars/grammar.hpp"
#include "orm/utils/query.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

#ifndef TINYORM_DISABLE_ORM
namespace Orm::Tiny
{
    template<typename Model>
    class Builder;
}
#endif

namespace Orm::Query
{
    class JoinClause;

    /*! Concept for the remove() method parameter. */
    template<typename T>
    concept Remove = std::convertible_to<T, quint64> ||
                     std::convertible_to<T, Query::Expression>;

    // TODO querybuilder, whereFullText, whereBitwise silverqx
    // FUTURE querybuilder, paginator silverqx
    // FUTURE querybuilder, index hint silverqx
    /*! Database query builder. */
    class TINYORM_EXPORT Builder : public Concerns::BuildsQueries // clazy:exclude=copyable-polymorphic
    {
        // To access enforceOrderBy()
        friend Concerns::BuildsQueries;
#ifndef TINYORM_DISABLE_ORM
        // To access stripTableForPluck()
        template<typename Model>
        friend class Tiny::Builder;
#endif

        /*! Alias for the query grammar. */
        using QueryGrammar = Query::Grammars::Grammar;
        /*! Alias for query utils. */
        using QueryUtils = Orm::Utils::Query;

    public:
        /*! Constructor. */
        Builder(std::shared_ptr<DatabaseConnection> connection,
                std::shared_ptr<QueryGrammar> grammar);
        /* Need to be the polymorphic type because of dynamic_cast<>
           in the Grammar::concatenateWhereClauses(). */
        /*! Virtual destructor. */
        ~Builder() override = default;

        /*! Copy constructor. */
        Builder(const Builder &) = default;
        /*! Deleted copy assignment operator (class contains reference and const). */
        Builder &operator=(const Builder &) = delete;

        /*! Move constructor. */
        Builder(Builder &&) noexcept = default;
        /*! Deleted move assignment operator (class contains reference and const). */
        Builder &operator=(Builder &&) = delete;

        /* Retrieving results */
        /*! Execute the query as a "select" statement. */
        SqlQuery get(const QList<Column> &columns = {ASTERISK});
        /*! Execute a query for a single record by ID. */
        SqlQuery find(const QVariant &id, const QList<Column> &columns = {ASTERISK});

        /*! Execute a query for a single record by ID or call a callback. */
        SqlQuery findOr(const QVariant &id, const QList<Column> &columns,
                        const std::function<void()> &callback);
        /*! Execute a query for a single record by ID or call a callback. */
        inline SqlQuery findOr(const QVariant &id,
                               const std::function<void()> &callback);

        /*! Execute a query for a single record by ID or call a callback. */
        template<typename R>
        std::pair<SqlQuery, R>
        findOr(const QVariant &id, const QList<Column> &columns,
               const std::function<R()> &callback);
        /*! Execute a query for a single record by ID or call a callback. */
        template<typename R>
        std::pair<SqlQuery, R>
        findOr(const QVariant &id, const std::function<R()> &callback);

        /*! Execute the query and get the first result. */
        SqlQuery first(const QList<Column> &columns = {ASTERISK});
        /*! Get a single column's value from the first result of a query. */
        QVariant value(const Column &column);
        /*! Get a single column's value from the first result of a query if it's
            the sole matching record. */
        QVariant soleValue(const Column &column);

        /*! Get a vector with values in the given column. */
        QList<QVariant> pluck(const Column &column);
        /*! Get a map with values in the given column and keyed by values in the key
            column. */
        template<typename T>
        std::map<T, QVariant> pluck(const Column &column, const Column &key);

        /*! Concatenate values of the given column as a string. */
        QString implode(const QString &column, const QString &glue = "");

        /*! Get the SQL representation of the query. */
        QString toSql();

        /* Insert, Update, Delete */
        /*! Insert new records into the database (multi-rows insert). */
        std::optional<SqlQuery>
        insert(const QList<QVariantMap> &values);
        /*! Insert a new record into the database. */
        std::optional<SqlQuery>
        insert(const QVariantMap &values);
        /*! Insert new records into the database (multi insert with separated columns). */
        std::optional<SqlQuery>
        insert(const QList<QString> &columns, const QList<QList<QVariant>> &values);

        /*! Insert a new record and get the value of the primary key. */
        quint64 insertGetId(const QVariantMap &values, const QString &sequence = "");

        /*! Insert new records into the database while ignoring errors. */
        std::tuple<int, std::optional<TSqlQuery>>
        insertOrIgnore(const QList<QVariantMap> &values);
        /*! Insert a new record into the database while ignoring errors. */
        std::tuple<int, std::optional<TSqlQuery>>
        insertOrIgnore(const QVariantMap &values);
        /*! Insert new records into the database while ignoring errors (multi insert). */
        std::tuple<int, std::optional<TSqlQuery>>
        insertOrIgnore(const QList<QString> &columns,
                       const QList<QList<QVariant>> &values);

        /*! Update records in the database. */
        std::tuple<int, TSqlQuery>
        update(const QList<UpdateItem> &values);
        /*! Insert or update a record matching the attributes, and fill it with values. */
        std::tuple<int, std::optional<TSqlQuery>>
        updateOrInsert(const QList<WhereItem> &attributes,
                       const QList<UpdateItem> &values);

        /*! Insert new records or update the existing ones. */
        std::tuple<int, std::optional<TSqlQuery>>
        upsert(const QList<QVariantMap> &values, const QStringList &uniqueBy,
               const QStringList &update);
        /*! Insert new records or update the existing ones (update all columns). */
        std::tuple<int, std::optional<TSqlQuery>>
        upsert(const QList<QVariantMap> &values, const QStringList &uniqueBy);

        /*! Delete records from the database. */
        std::tuple<int, TSqlQuery> deleteRow();
        /*! Delete records from the database. */
        std::tuple<int, TSqlQuery> remove();
        /*! Delete records from the database. */
        template<Remove T>
        inline std::tuple<int, TSqlQuery> deleteRow(T &&id);
        /*! Delete records from the database. */
        template<Remove T>
        std::tuple<int, TSqlQuery> remove(T &&id);

        /*! Run a truncate statement on the table. */
        void truncate();

        /* Select */
        /*! Retrieve the "count" result of the query. */
        inline quint64 count(const QList<Column> &columns = {ASTERISK}) const;
        /*! Retrieve the "count" result of the query. */
        template<typename = void>
        inline quint64 count(const Column &column);
        /*! Retrieve the minimum value of a given column. */
        inline QVariant min(const Column &column) const;
        /*! Retrieve the maximum value of a given column. */
        inline QVariant max(const Column &column) const;
        /*! Retrieve the sum of the values of a given column. */
        inline QVariant sum(const Column &column) const;
        /*! Retrieve the average of the values of a given column. */
        inline QVariant avg(const Column &column) const;
        /*! Alias for the "avg" method. */
        inline QVariant average(const Column &column) const;

        /*! Execute an aggregate function on the database. */
        QVariant aggregate(const QString &function,
                           const QList<Column> &columns = {ASTERISK}) const;

        /*! Determine if any rows exist for the current query. */
        bool exists();
        /*! Determine if no rows exist for the current query. */
        inline bool doesntExist();

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

        /*! Set the columns to be selected. */
        Builder &select(const QList<Column> &columns = {ASTERISK});
        /*! Set the column to be selected. */
        Builder &select(const Column &column);
        /*! Add new select columns to the query. */
        Builder &addSelect(const QList<Column> &columns);
        /*! Add a new select column to the query. */
        Builder &addSelect(const Column &column);

        /*! Set the columns to be selected. */
        Builder &select(QList<Column> &&columns);
        /*! Set the column to be selected. */
        Builder &select(Column &&column);
        /*! Add new select columns to the query. */
        Builder &addSelect(QList<Column> &&columns);
        /*! Add a new select column to the query. */
        Builder &addSelect(Column &&column);

        /*! Set a select subquery on the query. */
        template<Queryable T>
        inline Builder &select(T &&query, const QString &as);
        /*! Add a select subquery to the query. */
        template<Queryable T>
        Builder &addSelect(T &&query, const QString &as);

        /*! Add a subselect expression to the query. */
        template<SubQuery T>
        Builder &selectSub(T &&query, const QString &as);
        /*! Add a new "raw" select expression to the query. */
        Builder &selectRaw(const QString &expression,
                           const QList<QVariant> &bindings = {});

        /*! Force the query to only return distinct results. */
        Builder &distinct();
        /*! Force the query to only return distinct results. */
        Builder &distinct(const QStringList &columns);
        /*! Force the query to only return distinct results. */
        Builder &distinct(QStringList &&columns);

        /*! Set the table which the query is targeting. */
        Builder &from(const QString &table, const QString &as = "");
        /*! Set the table which the query is targeting. */
        Builder &from(const Expression &table);
        /*! Set the table which the query is targeting. */
        Builder &from(Expression &&table);

        /*! Makes "from" fetch from a subquery. */
        template<SubQuery T>
        Builder &fromSub(T &&query, const QString &as);
        /*! Set the table which the query is targeting. */
        Builder &fromRaw(const QString &expression,
                         const QList<QVariant> &bindings = {});

        /* Joins */
        /*! Add a join clause to the query. */
        template<JoinTable T>
        inline Builder &
        join(T &&table, const QString &first, const QString &comparison,
             const QVariant &second, const QString &type = INNER, bool where = false);
        /*! Add an advanced join clause to the query. */
        template<JoinTable T>
        inline Builder &
        join(T &&table, const std::function<void(JoinClause &)> &callback,
             const QString &type = INNER);
        /*! Add a "join where" clause to the query. */
        template<JoinTable T>
        inline Builder &
        joinWhere(T &&table, const QString &first, const QString &comparison,
                  const QVariant &second, const QString &type = INNER);

        /*! Add a left join to the query. */
        template<JoinTable T>
        inline Builder &
        leftJoin(T &&table, const QString &first, const QString &comparison,
                 const QVariant &second);
        /*! Add an advanced left join to the query. */
        template<JoinTable T>
        inline Builder &
        leftJoin(T &&table, const std::function<void(JoinClause &)> &callback);
        /*! Add a "join where" clause to the query. */
        template<JoinTable T>
        inline Builder &
        leftJoinWhere(T &&table, const QString &first, const QString &comparison,
                      const QVariant &second);

        /*! Add a right join to the query. */
        template<JoinTable T>
        inline Builder &
        rightJoin(T &&table, const QString &first, const QString &comparison,
                  const QVariant &second);
        /*! Add an advanced right join to the query. */
        template<JoinTable T>
        inline Builder &
        rightJoin(T &&table, const std::function<void(JoinClause &)> &callback);
        /*! Add a "right join where" clause to the query. */
        template<JoinTable T>
        inline Builder &
        rightJoinWhere(T &&table, const QString &first, const QString &comparison,
                       const QVariant &second);

        /*! Add a "cross join" clause to the query. */
        template<JoinTable T>
        inline Builder &
        crossJoin(T &&table, const QString &first, const QString &comparison,
                  const QVariant &second);
        /*! Add an advanced "cross join" clause to the query. */
        template<JoinTable T>
        inline Builder &
        crossJoin(T &&table, const std::function<void(JoinClause &)> &callback);
        /*! Add a "cross join" clause to the query. */
        template<JoinTable T>
        inline Builder &crossJoin(T &&table);

        /*! Add a subquery join clause to the query. */
        template<SubQuery T>
        inline Builder &
        joinSub(T &&query, const QString &as, const QString &first,
                const QString &comparison, const QVariant &second,
                const QString &type = INNER, bool where = false);
        /*! Add a subquery join clause to the query. */
        template<SubQuery T>
        inline Builder &
        joinSub(T &&query, const QString &as,
                const std::function<void(JoinClause &)> &callback,
                const QString &type = INNER);

        /*! Add a subquery left join to the query. */
        template<SubQuery T>
        inline Builder &
        leftJoinSub(T &&query, const QString &as, const QString &first,
                    const QString &comparison, const QVariant &second);
        /*! Add a subquery left join to the query. */
        template<SubQuery T>
        inline Builder &
        leftJoinSub(T &&query, const QString &as,
                    const std::function<void(JoinClause &)> &callback);

        /*! Add a subquery right join to the query. */
        template<SubQuery T>
        inline Builder &
        rightJoinSub(T &&query, const QString &as, const QString &first,
                     const QString &comparison, const QVariant &second);
        /*! Add a subquery right join to the query. */
        template<SubQuery T>
        inline Builder &
        rightJoinSub(T &&query, const QString &as,
                     const std::function<void(JoinClause &)> &callback);

        /* General where */
        /*! Add a basic where clause to the query. */
        template<WhereValue T>
        Builder &where(const Column &column, const QString &comparison,
                       T &&value, const QString &condition = AND);
        /*! Add an "or where" clause to the query. */
        template<WhereValue T>
        Builder &orWhere(const Column &column, const QString &comparison, T &&value);
        /*! Add a basic equal where clause to the query. */
        template<WhereValue T>
        Builder &whereEq(const Column &column, T &&value,
                         const QString &condition = AND);
        /*! Add an equal "or where" clause to the query. */
        template<WhereValue T>
        Builder &orWhereEq(const Column &column, T &&value);

        /* General where not */
        /*! Add a basic "where not" clause to the query. */
        template<WhereValue T>
        Builder &whereNot(const Column &column, const QString &comparison,
                          T &&value, const QString &condition = AND);
        /*! Add an "or where not" clause to the query. */
        template<WhereValue T>
        Builder &orWhereNot(const Column &column, const QString &comparison, T &&value);
        /*! Add a basic equal "where not" clause to the query. */
        template<WhereValue T>
        Builder &whereNotEq(const Column &column, T &&value,
                            const QString &condition = AND);
        /*! Add an equal "or where not" clause to the query. */
        template<WhereValue T>
        Builder &orWhereNotEq(const Column &column, T &&value);

        /* Nested where */
        /*! Add a nested where clause to the query. */
        Builder &where(const std::function<void(Builder &)> &callback,
                       const QString &condition = AND);
        /*! Add a nested "or where" clause to the query. */
        Builder &orWhere(const std::function<void(Builder &)> &callback);
        /*! Add a nested "where not" clause to the query. */
        Builder &whereNot(const std::function<void(Builder &)> &callback,
                          const QString &condition = AND);
        /*! Add a nested "or where not" clause to the query. */
        Builder &orWhereNot(const std::function<void(Builder &)> &callback);

        /* Array where */
        /*! Add a vector of basic where clauses to the query. */
        Builder &where(const QList<WhereItem> &values,
                       const QString &condition = AND,
                       const QString &defaultCondition = "");
        /*! Add a vector of basic "or where" clauses to the query. */
        Builder &orWhere(const QList<WhereItem> &values,
                         const QString &defaultCondition = "");
        /*! Add a vector of basic "where not" clauses to the query. */
        Builder &whereNot(const QList<WhereItem> &values,
                          const QString &condition = AND,
                          const QString &defaultCondition = "");
        /*! Add a vector of basic "or where not" clauses to the query. */
        Builder &orWhereNot(const QList<WhereItem> &values,
                            const QString &defaultCondition = "");

        /* where column */
        /*! Add a vector of where clauses comparing two columns to the query. */
        Builder &whereColumn(const QList<WhereColumnItem> &values,
                             const QString &condition = AND);
        /*! Add a vector of "or where" clauses comparing two columns to the query. */
        Builder &orWhereColumn(const QList<WhereColumnItem> &values);

        /*! Add a "where" clause comparing two columns to the query. */
        Builder &whereColumn(const Column &first, const QString &comparison,
                             const Column &second, const QString &condition = AND);
        /*! Add a "or where" clause comparing two columns to the query. */
        Builder &orWhereColumn(const Column &first, const QString &comparison,
                               const Column &second);
        /*! Add an equal "where" clause comparing two columns to the query. */
        Builder &whereColumnEq(const Column &first, const Column &second,
                               const QString &condition = AND);
        /*! Add an equal "or where" clause comparing two columns to the query. */
        Builder &orWhereColumnEq(const Column &first, const Column &second);

        /* where IN */
        /*! Add a "where in" clause to the query. */
        Builder &whereIn(const Column &column, const QList<QVariant> &values,
                         const QString &condition = AND, bool nope = false);
        /*! Add an "or where in" clause to the query. */
        Builder &orWhereIn(const Column &column, const QList<QVariant> &values);
        /*! Add a "where not in" clause to the query. */
        Builder &whereNotIn(const Column &column, const QList<QVariant> &values,
                            const QString &condition = AND);
        /*! Add an "or where not in" clause to the query. */
        Builder &orWhereNotIn(const Column &column, const QList<QVariant> &values);

        /* where null */
        /*! Add a "where null" clause to the query. */
        Builder &whereNull(const QList<Column> &columns = {ASTERISK},
                           const QString &condition = AND, bool nope = false);
        /*! Add an "or where null" clause to the query. */
        Builder &orWhereNull(const QList<Column> &columns = {ASTERISK});
        /*! Add a "where not null" clause to the query. */
        Builder &whereNotNull(const QList<Column> &columns = {ASTERISK},
                              const QString &condition = AND);
        /*! Add an "or where not null" clause to the query. */
        Builder &orWhereNotNull(const QList<Column> &columns = {ASTERISK});

        /*! Add a "where null" clause to the query. */
        Builder &whereNull(const Column &column, const QString &condition = AND,
                           bool nope = false);
        /*! Add an "or where null" clause to the query. */
        Builder &orWhereNull(const Column &column);
        /*! Add a "where not null" clause to the query. */
        Builder &whereNotNull(const Column &column, const QString &condition = AND);
        /*! Add an "or where not null" clause to the query. */
        Builder &orWhereNotNull(const Column &column);

        /* where between */
        /*! Add a "where between" statement to the query. */
        Builder &whereBetween(const Column &column, const WhereBetweenItem &values,
                              const QString &condition = AND, bool nope = false);
        /*! Add an "or where between" statement to the query. */
        Builder &orWhereBetween(const Column &column, const WhereBetweenItem &values);
        /*! Add a "where not between" statement to the query. */
        Builder &whereNotBetween(const Column &column, const WhereBetweenItem &values,
                                 const QString &condition = AND);
        /*! Add an "or where not between" statement to the query. */
        Builder &orWhereNotBetween(const Column &column, const WhereBetweenItem &values);

        /* where between columns */
        /*! Add a "where between" statement using columns to the query. */
        Builder &whereBetweenColumns(
                const Column &column, const WhereBetweenColumnsItem &betweenColumns,
                const QString &condition = AND, bool nope = false);
        /*! Add an "or where between" statement using columns to the query. */
        Builder &orWhereBetweenColumns(
                const Column &column, const WhereBetweenColumnsItem &betweenColumns);
        /*! Add a "where not between" statement using columns to the query. */
        Builder &whereNotBetweenColumns(
                const Column &column, const WhereBetweenColumnsItem &betweenColumns,
                const QString &condition = AND);
        /*! Add an "or where not between" statement using columns to the query. */
        Builder &orWhereNotBetweenColumns(
                const Column &column, const WhereBetweenColumnsItem &betweenColumns);

        /* where sub-queries */
        /*! Add a basic where clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        Builder &where(C &&column, const QString &comparison, V &&value,
                       const QString &condition = AND);
        /*! Add an "or where" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        inline Builder &orWhere(C &&column, const QString &comparison, V &&value);
        /*! Add a basic equal where clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        inline Builder &whereEq(C &&column, V &&value, const QString &condition = AND);
        /*! Add an equal "or where" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        inline Builder &orWhereEq(C &&column, V &&value);

        /* where not sub-queries */
        /*! Add a basic "where not" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        Builder &whereNot(C &&column, const QString &comparison, V &&value,
                          const QString &condition = AND);
        /*! Add an "or where not" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        inline Builder &orWhereNot(C &&column, const QString &comparison, V &&value);
        /*! Add a basic equal "where not" clause to the query with a full sub-select
            column. */
        template<Queryable C, WhereValue V>
        inline Builder &whereNotEq(C &&column, V &&value, const QString &condition = AND);
        /*! Add an equal "or where not" clause to the query with a full sub-select
            column. */
        template<Queryable C, WhereValue V>
        inline Builder &orWhereNotEq(C &&column, V &&value);

        /*! Add a full sub-select to the "where" clause. */
        template<WhereValueSubQuery T>
        Builder &whereSub(const Column &column, const QString &comparison, T &&query,
                          const QString &condition = AND);

        /* where exists */
        /*! Add an exists clause to the query. */
        template<QueryableShared C>
        Builder &whereExists(C &&callback, const QString &condition = AND,
                             bool nope = false);
        /*! Add an or exists clause to the query. */
        template<QueryableShared C>
        Builder &orWhereExists(C &&callback, bool nope = false);
        /*! Add a where not exists clause to the query. */
        template<QueryableShared C>
        Builder &whereNotExists(C &&callback, const QString &condition = AND);
        /*! Add a where not exists clause to the query. */
        template<QueryableShared C>
        Builder &orWhereNotExists(C &&callback);

        /* where row values */
        /*! Adds a where condition using row values. */
        Builder &
        whereRowValues(const QList<Column> &columns, const QString &comparison,
                       const QList<QVariant> &values, const QString &condition = AND);
        /*! Adds an or where condition using row values. */
        Builder &
        orWhereRowValues(const QList<Column> &columns, const QString &comparison,
                         const QList<QVariant> &values);
        Builder &
        whereRowValuesEq(const QList<Column> &columns, const QList<QVariant> &values,
                         const QString &condition = AND);
        /*! Adds an or where condition using row values. */
        Builder &
        orWhereRowValuesEq(const QList<Column> &columns,
                           const QList<QVariant> &values);

        /* where dates */
        /*! Add a "where date" statement to the query. */
        Builder &whereDate(const Column &column, const QString &comparison,
                           QVariant value, const QString &condition = AND);
        /*! Add a "where time" statement to the query. */
        Builder &whereTime(const Column &column, const QString &comparison,
                           QVariant value, const QString &condition = AND);
        /*! Add a "where day" statement to the query. */
        Builder &whereDay(const Column &column, const QString &comparison,
                          QVariant value, const QString &condition = AND);
        /*! Add a "where month" statement to the query. */
        Builder &whereMonth(const Column &column, const QString &comparison,
                            QVariant value, const QString &condition = AND);
        /*! Add a "where year" statement to the query. */
        Builder &whereYear(const Column &column, const QString &comparison,
                           QVariant value, const QString &condition = AND);

        /*! Add an equal "where date" statement to the query. */
        inline Builder &whereEqDate(const Column &column, QVariant value,
                                    const QString &condition = AND);
        /*! Add an equal "where time" statement to the query. */
        inline Builder &whereEqTime(const Column &column, QVariant value,
                                    const QString &condition = AND);
        /*! Add an equal "where day" statement to the query. */
        inline Builder &whereEqDay(const Column &column, QVariant value,
                                   const QString &condition = AND);
        /*! Add an equal "where month" statement to the query. */
        inline Builder &whereEqMonth(const Column &column, QVariant value,
                                     const QString &condition = AND);
        /*! Add an equal "where year" statement to the query. */
        inline Builder &whereEqYear(const Column &column, QVariant value,
                                    const QString &condition = AND);

        /*! Add a "or where date" statement to the query. */
        inline Builder &orWhereDate(const Column &column, const QString &comparison,
                                    QVariant value);
        /*! Add a "or where time" statement to the query. */
        inline Builder &orWhereTime(const Column &column, const QString &comparison,
                                    QVariant value);
        /*! Add a "or where day" statement to the query. */
        inline Builder &orWhereDay(const Column &column, const QString &comparison,
                                   QVariant value);
        /*! Add a "or where month" statement to the query. */
        inline Builder &orWhereMonth(const Column &column, const QString &comparison,
                                     QVariant value);
        /*! Add a "or where year" statement to the query. */
        inline Builder &orWhereYear(const Column &column, const QString &comparison,
                                    QVariant value);

        /* where raw */
        /*! Add a raw "where" clause to the query. */
        Builder &whereRaw(const QString &sql, const QList<QVariant> &bindings = {},
                          const QString &condition = AND);
        /*! Add a raw "or where" clause to the query. */
        Builder &orWhereRaw(const QString &sql, const QList<QVariant> &bindings = {});

        /* Group by and having */
        /*! Add a "group by" clause to the query. */
        Builder &groupBy(const QList<Column> &groups);
        /*! Add a "group by" clause to the query. */
        Builder &groupBy(const Column &group);
        /*! Add a "group by" clause to the query. */
        template<ColumnConcept ...Args>
        inline Builder &groupBy(Args &&...groups);

        /*! Add a raw "groupBy" clause to the query. */
        Builder &groupByRaw(const QString &sql, const QList<QVariant> &bindings = {});

        /*! Add a "having" clause to the query. */
        Builder &having(const Column &column, const QString &comparison,
                        const QVariant &value, const QString &condition = AND);
        /*! Add an "or having" clause to the query. */
        Builder &orHaving(const Column &column, const QString &comparison,
                          const QVariant &value);

        /*! Add a raw "having" clause to the query. */
        Builder &havingRaw(const QString &sql, const QList<QVariant> &bindings = {},
                           const QString &condition = AND);
        /*! Add a raw "or having" clause to the query. */
        Builder &orHavingRaw(const QString &sql, const QList<QVariant> &bindings = {});

        /* Ordering */
        /*! Add an "order by" clause to the query. */
        Builder &orderBy(const Column &column, const QString &direction = ASC);
        /*! Add a descending "order by" clause to the query. */
        Builder &orderByDesc(const Column &column);

        /*! Add an "order by" clause to the query with a subquery ordering. */
        template<Queryable T>
        Builder &orderBy(T &&query, const QString &direction = ASC);
        /*! Add a descending "order by" clause to the query with a subquery ordering. */
        template<Queryable T>
        inline Builder &orderByDesc(T &&query);

        /*! Put the query's results in random order. */
        Builder &inRandomOrder(const QString &seed = "");
        /*! Add a raw "order by" clause to the query. */
        Builder &orderByRaw(const QString &sql, const QList<QVariant> &bindings = {});

        /*! Add an "order by" clause for a timestamp to the query. */
        Builder &latest(const Column &column = CREATED_AT);
        /*! Add an "order by" clause for a timestamp to the query. */
        Builder &oldest(const Column &column = CREATED_AT);
        /*! Remove all existing orders. */
        Builder &reorder();
        /*! Remove all existing orders and optionally add a new order. */
        Builder &reorder(const Column &column, const QString &direction = ASC);

        /*! Set the "limit" value of the query. */
        Builder &limit(qint64 value);
        /*! Alias to set the "limit" value of the query. */
        Builder &take(qint64 value);
        /*! Set the "offset" value of the query. */
        Builder &offset(qint64 value);
        /*! Alias to set the "offset" value of the query. */
        Builder &skip(qint64 value);
        /*! Set the limit and offset for a given page. */
        Builder &forPage(qint64 page, qint64 perPage = 30);

        /*! Constrain the query to the previous "page" of results before a given ID. */
        Builder &forPageBeforeId(qint64 perPage = 30, const QVariant &lastId = {},
                                 const QString &column = Orm::Constants::ID,
                                 bool prependOrder = false);
        /*! Constrain the query to the next "page" of results after a given ID. */
        Builder &forPageAfterId(qint64 perPage = 30, const QVariant &lastId = {},
                                const QString &column = Orm::Constants::ID,
                                bool prependOrder = false);

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

        /* Debugging */
        /*! Dump the current SQL and bindings. */
        void dump(bool replaceBindings = true, bool simpleBindings = false);
        /*! Die and dump the current SQL and bindings. */
        void dd(bool replaceBindings = true, bool simpleBindings = false);

        /* Getters / Setters */
        /*! Get the default key name of the table. */
        const QString &defaultKeyName() const noexcept;
        /*! Get a database connection. */
        inline DatabaseConnection &getConnection() const noexcept;
        /*! Get the query grammar reference. */
        inline const QueryGrammar &getGrammar() const noexcept;

        /*! Get a database connection as a std::shared_ptr. */
        inline std::shared_ptr<DatabaseConnection> getConnectionShared() const noexcept;
        /*! Get the query grammar instance as a std::shared_ptr. */
        inline std::shared_ptr<QueryGrammar> getGrammarShared() const noexcept;

        /*! Get the current query value bindings as flattened QList. */
        QList<QVariant> getBindings() const;
        /*! Get the raw map of bindings. */
        inline const BindingsMap &getRawBindings() const noexcept;
        /*! Add a binding to the query. */
        Builder &addBinding(const QVariant &binding,
                            BindingType type = BindingType::WHERE);
        /*! Add a binding to the query. */
        Builder &addBinding(QVariant &&binding,
                            BindingType type = BindingType::WHERE);
        /*! Add bindings to the query. */
        Builder &addBinding(const QList<QVariant> &bindings,
                            BindingType type = BindingType::WHERE);
        /*! Add bindings to the query. */
        Builder &addBinding(QList<QVariant> &&bindings,
                            BindingType type = BindingType::WHERE);
        /*! Set the bindings on the query builder. */
        Builder &setBindings(QList<QVariant> &&bindings,
                             BindingType type = BindingType::WHERE);

        /*! Get an aggregate function and column to be run. */
        inline const std::optional<AggregateItem> &getAggregate() const noexcept;
        /*! Check if the query returns distinct results. */
        inline const std::variant<bool, QStringList> &getDistinct() const noexcept;
        /*! Check if the query returns distinct results. */
        template<typename T> requires std::same_as<T, bool>
        inline bool getDistinct() const;
        /*! Check if the query returns distinct results. */
        template<typename T> requires std::same_as<T, QStringList>
        inline const QStringList &getDistinct() const;
        // TODO check up all code and return references when appropriate silverqx
        /*! Get the columns that should be returned. */
        inline const QList<Column> &getColumns() const noexcept;
        /*! Set the columns that should be returned. */
        inline Builder &setColumns(const QList<Column> &columns) noexcept;
        /*! Set the columns that should be returned. */
        inline Builder &setColumns(QList<Column> &&columns) noexcept;
        /*! Get the table associated with the query builder. */
        inline const FromClause &getFrom() const noexcept;
        /*! Get the table joins for the query. */
        inline const QList<std::shared_ptr<JoinClause>> &getJoins() const noexcept;
        /*! Get the where constraints for the query. */
        inline const QList<WhereConditionItem> &getWheres() const noexcept;
        /*! Get the groupings for the query. */
        inline const QList<Column> &getGroups() const noexcept;
        /*! Get the having constraints for the query. */
        inline const QList<HavingConditionItem> &getHavings() const noexcept;
        /*! Get the orderings for the query. */
        inline const QList<OrderByItem> &getOrders() const noexcept;
        /*! Get the maximum number of records to return. */
        inline qint64 getLimit() const noexcept;
        /*! Get the number of records to skip. */
        inline qint64 getOffset() const noexcept;
        /*! Get the row locking. */
        inline const std::variant<std::monostate, bool, QString> &
        getLock() const noexcept;

        /* Other methods */
        /*! Get a new instance of the query builder. */
        virtual std::shared_ptr<Builder> newQuery() const;
        /*! Create a new query instance for nested where condition. */
        std::shared_ptr<Builder> forNestedWhere() const;

        /*! Create a raw database expression. */
        Expression raw(const QVariant &value) const;
        /*! Create a raw database expression. */
        Expression raw(QVariant &&value) const noexcept;

        /*! Add another query builder as a nested where to the query builder. */
        Builder &addNestedWhereQuery(const std::shared_ptr<Builder> &query,
                                     const QString &condition);
        /*! Add an "exists" clause to the query. */
        Builder &addWhereExistsQuery(const std::shared_ptr<Builder> &query,
                                     const QString &condition = AND, bool nope = false);
        /*! Add an "exists" clause to the query. */
        Builder &addWhereExistsQuery(Builder &query, const QString &condition = AND,
                                     bool nope = false);

        /*! Merge an array of where clauses and bindings. */
        Builder &mergeWheres(const QList<WhereConditionItem> &wheres,
                             const QList<QVariant> &bindings);
        /*! Merge an array of where clauses and bindings. */
        Builder &mergeWheres(QList<WhereConditionItem> &&wheres,
                             QList<QVariant> &&bindings);

        /*! Builder property types. */
        enum struct PropertyType : quint8
        {
            /*! Columns property type (used to clone query without column names). */
            COLUMNS,
        };

        /*! Clone the query. */
        inline Builder clone() const;
        /*! Clone the query without the given properties. */
        Builder cloneWithout(const std::unordered_set<PropertyType> &properties) const;
        /*! Clone the query without the given bindings. */
        Builder cloneWithoutBindings(
                const std::unordered_set<BindingType> &except) const;

    protected:
        /*! Throw if the given operator is not valid for the current DB connection. */
        void throwIfInvalidOperator(const QString &comparison) const;

        /*! Remove all of the expressions from a list of bindings. */
        static QList<QVariant> cleanBindings(const QList<QVariant> &bindings);
        /*! Remove all of the expressions from a list of bindings. */
        static QList<QVariant> cleanBindings(QList<QVariant> &&bindings);
        /*! Remove all of the expressions from the WhereBetweenItem bindings. */
        static QList<QVariant> cleanBindings(const WhereBetweenItem &bindings);

        /*! Add a vector of basic where clauses to the query. */
        Builder &
        addArrayOfWheres(const QList<WhereItem> &values,
                         const QString &condition = AND,
                         const QString &defaultCondition = "");
        /*! Add a vector of where clauses comparing two columns to the query. */
        Builder &
        addArrayOfWheres(const QList<WhereColumnItem> &values,
                         const QString &condition = AND);

        /*! Get a new join clause. */
        static std::shared_ptr<JoinClause>
        newJoinClause(const Builder &query, const QString &type, const QString &table);
        /*! Get a new join clause. */
        static std::shared_ptr<JoinClause>
        newJoinClause(const Builder &query, const QString &type, Expression &&table);

        /*! Remove all existing columns and column bindings. */
        Builder &clearColumns();
        /*! Execute the given callback while selecting the given columns. */
        SqlQuery
        onceWithColumns(const QList<Column> &columns,
                        const std::function<SqlQuery()> &callback);

        /*! Creates a subquery and parse it. */
        std::pair<QString, QList<QVariant>>
        createSub(const std::function<void(Builder &)> &callback) const;
        /*! Creates a subquery and parse it. */
        std::pair<QString, QList<QVariant>>
        createSub(Builder &query) const;
        /*! Creates a subquery and parse it. */
        static std::pair<QString, QList<QVariant>>
        createSub(const QString &query) noexcept;
        /*! Creates a subquery and parse it. */
        static std::pair<QString, QList<QVariant>>
        createSub(QString &&query) noexcept;

        /*! Determine whether the T type is a query builder instance or a lambda expr. */
        template<typename T>
        constexpr static bool isQueryable =
                std::is_convertible_v<T, Orm::QueryBuilder &> ||
                std::is_invocable_v<T, Orm::QueryBuilder &>;

        /*! Create a new query instance for a sub-query. */
        inline virtual std::shared_ptr<Builder> forSubQuery() const;

        /*! Prepend the database name if the given query is on another database. */
        Builder &prependDatabaseNameIfCrossDatabaseQuery(Builder &query) const;

        /*! Throw an exception if the query doesn't have an orderBy clause. */
        void enforceOrderBy() const;
        /*! Get an array with all orders with a given column removed. */
        QList<OrderByItem> removeExistingOrdersFor(const QString &column) const;

        /*! Strip off the table name or alias from a column identifier. */
        static QString stripTableForPluck(const Column &column);

        /* Getters / Setters */
        /*! Set the aggregate property without running the query. */
        Builder &setAggregate(const QString &function,
                              const QList<Column> &columns = {ASTERISK});

    private:
        /*! Run the query as a "select" statement against the connection. */
        SqlQuery runSelect();

        /*! Set the table which the query is targeting. */
        inline Builder &setFrom(const FromClause &from);

        /*! Add a join clause to the query, common code. */
        Builder &joinInternal(
                std::shared_ptr<JoinClause> &&join, const QString &first,
                const QString &comparison, const QVariant &second, bool where);
        /*! Add an advanced join clause to the query, common code. */
        Builder &joinInternal(
                std::shared_ptr<JoinClause> &&join,
                const std::function<void(JoinClause &)> &callback);
        /*! Add a join clause to the query, common code for the above two methods. */
        Builder &joinInternal(std::shared_ptr<JoinClause> &&join);

        /*! Add a subquery join clause to the query, common code. */
        Builder &joinSubInternal(
                std::pair<QString, QList<QVariant>> &&subQuery, const QString &as,
                const QString &first, const QString &comparison, const QVariant &second,
                const QString &type, bool where);
        /*! Add a subquery join clause to the query, common code. */
        Builder &joinSubInternal(
                std::pair<QString, QList<QVariant>> &&subQuery, const QString &as,
                const std::function<void(JoinClause &)> &callback,
                const QString &type);

        /*! Add a basic where clause to the query, common code. */
        Builder &whereInternal(
                const Column &column, const QString &comparison, QVariant value,
                const QString &condition, WhereType type = WhereType::BASIC);

        /*! Throw an exception when m_bindings doesn't contain a passed type. */
        void checkBindingType(BindingType type) const;

        /*! All of the available clause operators. */
        static const std::unordered_set<QString> &getOperators();

        /*! The database connection instance. */
        std::shared_ptr<DatabaseConnection> m_connection;
        /*! The database query grammar instance. */
        std::shared_ptr<QueryGrammar> m_grammar;

        /*! The current query value bindings.
            Order is crucial here because of that QMap with an enum struct is used. */
        BindingsMap m_bindings {
            {BindingType::SELECT,     {}},
            {BindingType::FROM,       {}},
            {BindingType::JOIN,       {}},
            {BindingType::WHERE,      {}},
            {BindingType::GROUPBY,    {}},
            {BindingType::HAVING,     {}},
            {BindingType::ORDER,      {}},
            {BindingType::UNION,      {}},
            {BindingType::UNIONORDER, {}},
        };

        /*! An aggregate function and column to be run. */
        std::optional<AggregateItem> m_aggregate = std::nullopt;
        /*! Indicates if the query returns distinct results. */
        std::variant<bool, QStringList> m_distinct = false;
        /*! The columns that should be returned. */
        QList<Column> m_columns;
        /*! The table which the query is targeting. */
        FromClause m_from;
        /*! The table joins for the query. */
        QList<std::shared_ptr<JoinClause>> m_joins;
        /*! The where constraints for the query. */
        QList<WhereConditionItem> m_wheres;
        /*! The groupings for the query. */
        QList<Column> m_groups;
        /*! The having constraints for the query. */
        QList<HavingConditionItem> m_havings;
        /*! The orderings for the query. */
        QList<OrderByItem> m_orders;
        /*! The maximum number of records to return. */
        qint64 m_limit = -1;
        /*! The number of records to skip. */
        qint64 m_offset = -1;
        /*! Indicates whether row locking is being used. */
        std::variant<std::monostate, bool, QString> m_lock;
    };

    /* public */

    /* Retrieving results */

    SqlQuery Builder::findOr(const QVariant &id,
                             const std::function<void()> &callback)
    {
        return findOr(id, {ASTERISK}, callback);
    }

    template<typename R>
    std::pair<SqlQuery, R>
    Builder::findOr(const QVariant &id, const QList<Column> &columns,
                    const std::function<R()> &callback)
    {
        auto query = find(id, columns);

        if (query.isValid())
            return {std::move(query), R {}};

        /* Return invalid SqlQuery if a record was not found. Don't return
           the SqlQuery() as a user can still obtain some info from the invalid
           SqlQuery. */

        // Optionally invoke the callback
        if (callback)
            return {std::move(query), std::invoke(callback)};

        return {std::move(query), R {}};
    }

    template<typename R>
    std::pair<SqlQuery, R>
    Builder::findOr(const QVariant &id, const std::function<R()> &callback)
    {
        return findOr<R>(id, {ASTERISK}, callback);
    }

    template<typename T>
    std::map<T, QVariant>
    Builder::pluck(const Column &column, const Column &key)
    {
        /* First, we will need to select the results of the query accounting for the
           given column / key. Once we have the results, we will be able to take
           the results and get the exact data that was requested for the query. */
        auto query = get({column, key});

        // Empty result
        if (QueryUtils::queryResultSize(query) <= 0)
            return {};

        /* If the column is qualified with a table or have an alias, we cannot use
           those directly in the "pluck" operations, we have to strip the table out or
           use the alias name instead. */
        const auto unqualifiedColumn = stripTableForPluck(column);

        const auto unqualifiedKey = stripTableForPluck(key);

        std::map<T, QVariant> result;

        while (query.next())
            result.try_emplace(query.value(unqualifiedKey).value<T>(),
                               query.value(unqualifiedColumn));

        return result;
    }

    /* Insert, Update, Delete */

    template<Remove T>
    std::tuple<int, TSqlQuery> Builder::deleteRow(T &&id)
    {
        return remove(std::forward<T>(id));
    }

    template<Remove T>
    std::tuple<int, TSqlQuery> Builder::remove(T &&id)
    {
        /* If an ID is passed to the method, we will set the where clause to check the
           ID to let developers to simply and quickly remove a single row from this
           database without manually specifying the "where" clauses on the query.
           m_from will be wrapped in the Grammar. */
        where(QStringLiteral("%1.id").arg(std::get<QString>(m_from)), EQ,
              std::forward<T>(id), AND);

        return remove();
    }

    /* Select */

    quint64 Builder::count(const QList<Column> &columns) const
    {
        return aggregate(QStringLiteral("count"), columns).template value<quint64>();
    }

    template<typename>
    quint64 Builder::count(const Column &column)
    {
        return aggregate(QStringLiteral("count"), {column}).template value<quint64>();
    }

    QVariant Builder::min(const Column &column) const
    {
        return aggregate(QStringLiteral("min"), {column});
    }

    QVariant Builder::max(const Column &column) const
    {
        return aggregate(QStringLiteral("max"), {column});
    }

    QVariant Builder::sum(const Column &column) const
    {
        auto result = aggregate(QStringLiteral("sum"), {column});

        if (!result.isValid() || result.isNull())
            result = 0;

        return result;
    }

    QVariant Builder::avg(const Column &column) const
    {
        return aggregate(QStringLiteral("avg"), {column});
    }

    QVariant Builder::average(const Column &column) const
    {
        return avg(column);
    }

    bool Builder::doesntExist()
    {
        return !exists();
    }

    template<typename R>
    std::pair<bool, R> Builder::existsOr(const std::function<R()> &callback)
    {
        if (exists())
            return {true, R {}};

        return {false, std::invoke(callback)};
    }

    template<typename R>
    std::pair<bool, R> Builder::doesntExistOr(const std::function<R()> &callback)
    {
        if (doesntExist())
            return {true, R {}};

        return {false, std::invoke(callback)};
    }

    template<Queryable T>
    Builder &Builder::select(T &&query, const QString &as)
    {
        return selectSub(std::forward<T>(query), as);
    }

    template<Queryable T>
    Builder &Builder::addSelect(T &&query, const QString &as)
    {
        if (m_columns.isEmpty())
            select(QList<Column> {QStringLiteral("%1.*")
                                  .arg(std::get<QString>(m_from))});

        return selectSub(std::forward<T>(query), as);
    }

    template<SubQuery T>
    Builder &Builder::selectSub(T &&query, const QString &as)
    {
        const auto [queryString, bindings] = createSub(std::forward<T>(query)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

        return selectRaw(QStringLiteral("(%1) as %2").arg(queryString,
                                                          m_grammar->wrap(as)),
                         bindings);
    }

    template<SubQuery T>
    Builder &
    Builder::fromSub(T &&query, const QString &as)
    {
        const auto [queryString, bindings] = createSub(std::forward<T>(query)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

        return fromRaw(QStringLiteral("(%1) as %2").arg(queryString,
                                                        m_grammar->wrapTable(as)),
                       bindings);
    }

    /* Joins */

    template<JoinTable T>
    Builder &
    Builder::join(T &&table, const QString &first, const QString &comparison,
                  const QVariant &second, const QString &type, const bool where)
    {
        // Ownership of the std::shared_ptr<JoinClause>
        return joinInternal(newJoinClause(*this, type, std::forward<T>(table)), // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                            first, comparison, second, where);
    }

    // FUTURE joinSub, missing where param, also in joinSub silverqx
    template<JoinTable T>
    Builder &
    Builder::join(T &&table, const std::function<void(JoinClause &)> &callback,
                  const QString &type)
    {
        // Ownership of the std::shared_ptr<JoinClause>
        return joinInternal(newJoinClause(*this, type, std::forward<T>(table)),
                            callback);
    }

    template<JoinTable T>
    Builder &
    Builder::joinWhere(T &&table, const QString &first, const QString &comparison,
                       const QVariant &second, const QString &type)
    {
        return join(std::forward<T>(table), first, comparison, second, type, true);
    }

    template<JoinTable T>
    Builder &
    Builder::leftJoin(T &&table, const QString &first, const QString &comparison,
                      const QVariant &second)
    {
        return join(std::forward<T>(table), first, comparison, second, LEFT);
    }

    template<JoinTable T>
    Builder &
    Builder::leftJoin(T &&table, const std::function<void(JoinClause &)> &callback)
    {
        return join(std::forward<T>(table), callback, LEFT);
    }

    template<JoinTable T>
    Builder &
    Builder::leftJoinWhere(T &&table, const QString &first, const QString &comparison,
                           const QVariant &second)
    {
        return joinWhere(std::forward<T>(table), first, comparison, second, LEFT);
    }

    template<JoinTable T>
    Builder &
    Builder::rightJoin(T &&table, const QString &first, const QString &comparison,
                       const QVariant &second)
    {
        return join(std::forward<T>(table), first, comparison, second, RIGHT);
    }

    template<JoinTable T>
    Builder &
    Builder::rightJoin(T &&table, const std::function<void(JoinClause &)> &callback)
    {
        return join(std::forward<T>(table), callback, RIGHT);
    }

    template<JoinTable T>
    Builder &
    Builder::rightJoinWhere(T &&table, const QString &first, const QString &comparison,
                            const QVariant &second)
    {
        return joinWhere(std::forward<T>(table), first, comparison, second, RIGHT);
    }

    // TODO docs missing example, because of different api silverqx
    // NOTE api different silverqx
    template<JoinTable T>
    Builder &
    Builder::crossJoin(T &&table, const QString &first, const QString &comparison,
                       const QVariant &second)
    {
        return join(std::forward<T>(table), first, comparison, second, CROSS);
    }

    template<JoinTable T>
    Builder &
    Builder::crossJoin(T &&table, const std::function<void(JoinClause &)> &callback)
    {
        return join(std::forward<T>(table), callback, CROSS);
    }

    template<JoinTable T>
    Builder &
    Builder::crossJoin(T &&table)
    {
        // No need to call joinInternal() because no bindings
        m_joins << newJoinClause(*this, CROSS, std::forward<T>(table));

        return *this;
    }

    template<SubQuery T>
    Builder &
    Builder::joinSub(T &&query, const QString &as, const QString &first,
                     const QString &comparison, const QVariant &second,
                     const QString &type, const bool where)
    {
        return joinSubInternal(createSub(std::forward<T>(query)), // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                               as, first, comparison, second, type, where);
    }

    template<SubQuery T>
    Builder &
    Builder::joinSub(T &&query, const QString &as,
                     const std::function<void(JoinClause &)> &callback,
                     const QString &type)
    {
        return joinSubInternal(createSub(std::forward<T>(query)), as, callback, type);
    }

    template<SubQuery T>
    Builder &
    Builder::leftJoinSub(T &&query, const QString &as, const QString &first,
                         const QString &comparison, const QVariant &second)
    {
        return joinSub(std::forward<T>(query), as, first, comparison, second, LEFT);
    }

    template<SubQuery T>
    Builder &
    Builder::leftJoinSub(T &&query, const QString &as,
                         const std::function<void(JoinClause &)> &callback)
    {
        return joinSub(std::forward<T>(query), as, callback, LEFT);
    }

    template<SubQuery T>
    Builder &
    Builder::rightJoinSub(T &&query, const QString &as, const QString &first,
                          const QString &comparison, const QVariant &second)
    {
        return joinSub(std::forward<T>(query), as, first, comparison, second, RIGHT);
    }

    template<SubQuery T>
    Builder &
    Builder::rightJoinSub(T &&query, const QString &as,
                          const std::function<void(JoinClause &)> &callback)
    {
        return joinSub(std::forward<T>(query), as, callback, RIGHT);
    }

    /* Basic where */

    template<WhereValue T>
    Builder &
    Builder::where(const Column &column, const QString &comparison, T &&value,
                   const QString &condition)
    {
        /* If the value is queryable, it means the developer is performing an entire
           sub-select within the query and we will need to compile the sub-select
           within the where clause to get the appropriate query record results. */
        if constexpr (isQueryable<T>)
            return whereSub(column, comparison, std::forward<T>(value), condition);
        else
            return whereInternal(column, comparison, std::forward<T>(value), condition); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    }

    template<WhereValue T>
    Builder &
    Builder::orWhere(const Column &column, const QString &comparison, T &&value)
    {
        return where(column, comparison, std::forward<T>(value), OR);
    }

    template<WhereValue T>
    Builder &
    Builder::whereEq(const Column &column, T &&value, const QString &condition)
    {
        return where(column, EQ, std::forward<T>(value), condition);
    }

    template<WhereValue T>
    Builder &Builder::orWhereEq(const Column &column, T &&value)
    {
        return where(column, EQ, std::forward<T>(value), OR);
    }

    /* Genral where not */

    template<WhereValue T>
    Builder &
    Builder::whereNot(const Column &column, const QString &comparison, T &&value,
                      const QString &condition)
    {
        return where(column, comparison, std::forward<T>(value),
                     SPACE_IN.arg(condition, NOT));
    }

    template<WhereValue T>
    Builder &
    Builder::orWhereNot(const Column &column, const QString &comparison, T &&value)
    {
        return where(column, comparison, std::forward<T>(value),
                     SPACE_IN.arg(OR, NOT));
    }

    template<WhereValue T>
    Builder &
    Builder::whereNotEq(const Column &column, T &&value, const QString &condition)
    {
        return where(column, EQ, std::forward<T>(value), SPACE_IN.arg(condition, NOT));
    }

    template<WhereValue T>
    Builder &Builder::orWhereNotEq(const Column &column, T &&value)
    {
        return where(column, EQ, std::forward<T>(value), SPACE_IN.arg(OR, NOT));
    }

    /* where sub-queries */

    template<Queryable C, WhereValue V>
    Builder &
    Builder::where(C &&column, const QString &comparison, V &&value,
                   const QString &condition)
    {
        /* If the column is a Closure instance and there is an operator value, we will
           assume the developer wants to run a subquery and then compare the result
           of that subquery with the given value that was provided to the method. */

        // comparison operator check will be done in the where() method

        auto [queryString, bindings] = createSub(std::forward<C>(column));

        addBinding(std::move(bindings), BindingType::WHERE);

        return where(Expression(PARENTH_ONE.arg(queryString)), comparison,
                     std::forward<V>(value), condition);
    }

    template<Queryable C, WhereValue V>
    Builder &Builder::orWhere(C &&column, const QString &comparison, V &&value)
    {
        return where(std::forward<C>(column), comparison, std::forward<V>(value), OR);
    }

    template<Queryable C, WhereValue V>
    Builder &Builder::whereEq(C &&column, V &&value, const QString &condition)
    {
        return where(std::forward<C>(column), EQ, std::forward<V>(value), condition);
    }

    template<Queryable C, WhereValue V>
    Builder &Builder::orWhereEq(C &&column, V &&value)
    {
        return where(std::forward<C>(column), EQ, std::forward<V>(value), OR);
    }

    /* where not sub-queries */

    template<Queryable C, WhereValue V>
    Builder &
    Builder::whereNot(C &&column, const QString &comparison, V &&value,
                      const QString &condition)
    {
        return where(std::forward<C>(column), comparison, std::forward<V>(value),
                     SPACE_IN.arg(condition, NOT));
    }

    template<Queryable C, WhereValue V>
    Builder &Builder::orWhereNot(C &&column, const QString &comparison, V &&value)
    {
        return where(std::forward<C>(column), comparison, std::forward<V>(value),
                     SPACE_IN.arg(OR, NOT));
    }

    template<Queryable C, WhereValue V>
    Builder &Builder::whereNotEq(C &&column, V &&value, const QString &condition)
    {
        return where(std::forward<C>(column), EQ, std::forward<V>(value),
                     SPACE_IN.arg(condition, NOT));
    }

    template<Queryable C, WhereValue V>
    Builder &Builder::orWhereNotEq(C &&column, V &&value)
    {
        return where(std::forward<C>(column), EQ, std::forward<V>(value),
                     SPACE_IN.arg(OR, NOT));
    }

    template<WhereValueSubQuery T>
    Builder &
    Builder::whereSub(const Column &column, const QString &comparison,
                      T &&query, const QString &condition)
    {
        // comparison operator check will be done in the where() method

        auto [queryString, bindings] = createSub(std::forward<T>(query));

        addBinding(std::move(bindings), BindingType::WHERE);

        return where(column, comparison, Expression(PARENTH_ONE.arg(queryString)),
                     condition);
    }

    /* where exists */

    template<QueryableShared C>
    Builder &Builder::whereExists(C &&callback, const QString &condition, const bool nope)
    {
        if constexpr (std::invocable<C, Builder &>) {
            // Ownership of the std::shared_ptr<QueryBuilder>
            const auto query = forSubQuery();

            /* Similar to the sub-select clause, we will create a new query instance so
               the developer may cleanly specify the entire exists query and we will
               compile the whole thing in the grammar and insert it into the SQL. */
            std::invoke(std::forward<C>(callback), *query);

            return addWhereExistsQuery(query, condition, nope);
        }

        // For the QueryBuilder & or std::shared_ptr<QueryBuilder>
        else
            return addWhereExistsQuery(std::forward<C>(callback), condition, nope);
    }

    template<QueryableShared C>
    Builder &Builder::orWhereExists(C &&callback, const bool nope)
    {
        return whereExists(std::forward<C>(callback), OR, nope);
    }

    template<QueryableShared C>
    Builder &Builder::whereNotExists(C &&callback, const QString &condition)
    {
        return whereExists(std::forward<C>(callback), condition, true);
    }

    template<QueryableShared C>
    Builder &Builder::orWhereNotExists(C &&callback)
    {
        return whereExists(std::forward<C>(callback), OR, true);
    }

    /* where dates */

    Builder &
    Builder::whereEqDate(const Column &column, QVariant value, const QString &condition)
    {
        return whereDate(column, EQ, std::move(value), condition);
    }

    Builder &
    Builder::whereEqTime(const Column &column, QVariant value, const QString &condition)
    {
        return whereTime(column, EQ, std::move(value), condition);
    }

    Builder &
    Builder::whereEqDay(const Column &column, QVariant value, const QString &condition)
    {
        return whereDay(column, EQ, std::move(value), condition);
    }

    Builder &
    Builder::whereEqMonth(const Column &column, QVariant value, const QString &condition)
    {
        return whereMonth(column, EQ, std::move(value), condition);
    }

    Builder &
    Builder::whereEqYear(const Column &column, QVariant value, const QString &condition)
    {
        return whereYear(column, EQ, std::move(value), condition);
    }

    Builder &
    Builder::orWhereDate(const Column &column, const QString &comparison, QVariant value)
    {
        return whereDate(column, comparison, std::move(value), OR);
    }

    Builder &
    Builder::orWhereTime(const Column &column, const QString &comparison, QVariant value)
    {
        return whereTime(column, comparison, std::move(value), OR);
    }

    Builder &
    Builder::orWhereDay(const Column &column, const QString &comparison, QVariant value)
    {
        return whereDay(column, comparison, std::move(value), OR);
    }

    Builder &
    Builder::orWhereMonth(const Column &column, const QString &comparison, QVariant value)
    {
        return whereMonth(column, comparison, std::move(value), OR);
    }

    Builder &
    Builder::orWhereYear(const Column &column, const QString &comparison, QVariant value)
    {
        return whereYear(column, comparison, std::move(value), OR);
    }

    template<ColumnConcept ...Args>
    Builder &Builder::groupBy(Args &&...groups)
    {
        return groupBy(QList<Column> {std::forward<Args>(groups)...});
    }

    template<Queryable T>
    Builder &Builder::orderBy(T &&query, const QString &direction)
    {
        auto [queryString, bindings] = createSub(std::forward<T>(query));

        addBinding(std::move(bindings), BindingType::ORDER);

        return orderBy(Expression(PARENTH_ONE.arg(queryString)), direction);
    }

    template<Queryable T>
    Builder &Builder::orderByDesc(T &&query)
    {
        return orderBy(std::forward<T>(query), DESC);
    }

    /* Others */

    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, TSqlQuery>
    Builder::increment(const QString &column, const T amount,
                       const QList<UpdateItem> &extra)
    {
        const auto expression = QStringLiteral("%1 + %2").arg(m_grammar->wrap(column))
                                .arg(amount);

        QList<UpdateItem> columns {{column, raw(expression)}};
        std::copy(extra.cbegin(), extra.cend(), std::back_inserter(columns));

        return update(columns);
    }

    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, TSqlQuery>
    Builder::decrement(const QString &column, const T amount,
                       const QList<UpdateItem> &extra)
    {
        const auto expression = QStringLiteral("%1 - %2").arg(m_grammar->wrap(column))
                                .arg(amount);

        QList<UpdateItem> columns {{column, raw(expression)}};
        std::copy(extra.cbegin(), extra.cend(), std::back_inserter(columns));

        return update(columns);
    }

    /* Getters / Setters */

    DatabaseConnection &Builder::getConnection() const noexcept
    {
        return *m_connection;
    }

    const Builder::QueryGrammar &Builder::getGrammar() const noexcept
    {
        return *m_grammar;
    }

    std::shared_ptr<DatabaseConnection> Builder::getConnectionShared() const noexcept
    {
        return m_connection;
    }

    std::shared_ptr<Builder::QueryGrammar> Builder::getGrammarShared() const noexcept
    {
        return m_grammar;
    }

    const BindingsMap &Builder::getRawBindings() const noexcept
    {
        return m_bindings;
    }

    const std::optional<AggregateItem> &Builder::getAggregate() const noexcept
    {
        return m_aggregate;
    }

    const std::variant<bool, QStringList> &Builder::getDistinct() const noexcept
    {
        return m_distinct;
    }

    template<typename T> requires std::same_as<T, bool>
    bool Builder::getDistinct() const
    {
        return std::get<bool>(m_distinct);
    }

    template<typename T> requires std::same_as<T, QStringList>
    const QStringList &
    Builder::getDistinct() const
    {
        return std::get<QStringList>(m_distinct);
    }

    const QList<Column> &
    Builder::getColumns() const noexcept
    {
        return m_columns;
    }

    Builder &
    Builder::setColumns(const QList<Column> &columns) noexcept
    {
        m_columns = columns;

        return *this;
    }

    Builder &
    Builder::setColumns(QList<Column> &&columns) noexcept
    {
        m_columns = std::move(columns);

        return *this;
    }

    const FromClause &
    Builder::getFrom() const noexcept
    {
        return m_from;
    }

    const QList<std::shared_ptr<JoinClause>> &
    Builder::getJoins() const noexcept
    {
        return m_joins;
    }

    const QList<WhereConditionItem> &
    Builder::getWheres() const noexcept
    {
        return m_wheres;
    }

    const QList<Column> &
    Builder::getGroups() const noexcept
    {
        return m_groups;
    }

    const QList<HavingConditionItem> &
    Builder::getHavings() const noexcept
    {
        return m_havings;
    }

    const QList<OrderByItem> &
    Builder::getOrders() const noexcept
    {
        return m_orders;
    }

    qint64 Builder::getLimit() const noexcept
    {
        return m_limit;
    }

    qint64 Builder::getOffset() const noexcept
    {
        return m_offset;
    }

    const std::variant<std::monostate, bool, QString> &
    Builder::getLock() const noexcept
    {
        return m_lock;
    }

    Builder Builder::clone() const
    {
        return *this;
    }

    /* protected */

    std::shared_ptr<Builder>
    Builder::forSubQuery() const
    {
        return newQuery();
    }

    /* private */

    Builder &
    Builder::setFrom(const FromClause &from)
    {
        m_from = from;

        return *this;
    }

} // namespace Orm::Query

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_QUERY_QUERYBUILDER_HPP
