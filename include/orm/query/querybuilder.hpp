#pragma once
#ifndef QUERYBUILDER_H
#define QUERYBUILDER_H

#include <QtSql/QSqlQuery>

#include <optional>
#include <unordered_set>

#include "orm/concepts.hpp"
#include "orm/ormtypes.hpp"
#include "orm/query/grammars/grammar.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif

namespace Orm
{

    class ConnectionInterface;
namespace Query
{
    class JoinClause;

    /*! Concept for the remove() method parameter. */
    template<typename T>
    concept Remove = std::convertible_to<T, quint64> ||
                     std::same_as<T, Query::Expression>;

    // FEATURE subqueries, add support for subqueries, first in where() silverqx
    // TODO add inRandomOrder() silverqx
    /*! Database query builder. */
    class SHAREDLIB_EXPORT Builder
    {
        using QueryGrammar = Query::Grammars::Grammar;

    public:
        /*! Constructor. */
        Builder(ConnectionInterface &connection, const QueryGrammar &grammar);
        // WARNING solve pure virtual dtor vs default silverqx
        /* Need to be the polymorphic type because of dynamic_cast<>
           in Grammar::concatenateWhereClauses(). */
        /*! Default virtual destructor. */
        inline virtual ~Builder() = default;

        /*! Execute the query as a "select" statement. */
        QSqlQuery get(const QVector<Column> &columns = {ASTERISK});
        /*! Execute a query for a single record by ID. */
        QSqlQuery find(const QVariant &id, const QVector<Column> &columns = {ASTERISK});
        /*! Execute the query and get the first result. */
        QSqlQuery first(const QVector<Column> &columns = {ASTERISK});
        /*! Get a single column's value from the first result of a query. */
        QVariant value(const Column &column);
        /*! Get the vector with the values of a given column. */
        QVector<QVariant> pluck(const QString &column);
        /*! Get the vector with the values of a given column. */
        template<typename T>
        std::map<T, QVariant> pluck(const QString &column, const QString &key);

        /*! Get the SQL representation of the query. */
        QString toSql();
        // TODO next implement dd silverqx
        /*! Die and dump the current SQL and bindings. */
//        void dd() const
//        { dd($this->toSql(), $this->getBindings()); }

        /* Insert, Update, Delete */
        /*! Insert a new record into the database. */
        std::optional<QSqlQuery>
        insert(const QVariantMap &values);
        /*! Insert new records into the database. */
        std::optional<QSqlQuery>
        insert(const QVector<QVariantMap> &values);
        /*! Insert a new record and get the value of the primary key. */
        quint64 insertGetId(const QVariantMap &values, const QString &sequence = "");

        /*! Insert a new record into the database while ignoring errors. */
        std::tuple<int, std::optional<QSqlQuery>>
        insertOrIgnore(const QVariantMap &values);
        /*! Insert new records into the database while ignoring errors. */
        std::tuple<int, std::optional<QSqlQuery>>
        insertOrIgnore(const QVector<QVariantMap> &values);

        /*! Update records in the database. */
        std::tuple<int, QSqlQuery>
        update(const QVector<UpdateItem> &values);

        /*! Delete records from the database. */
        std::tuple<int, QSqlQuery> deleteRow();
        /*! Delete records from the database. */
        std::tuple<int, QSqlQuery> remove();
        /*! Delete records from the database. */
        template<Remove T>
        std::tuple<int, QSqlQuery> deleteRow(T &&id);
        /*! Delete records from the database. */
        template<Remove T>
        std::tuple<int, QSqlQuery> remove(T &&id);

        /*! Run a truncate statement on the table. */
        void truncate();

        /* Select */
        /*! Retrieve the "count" result of the query. */
        quint64 count(const QVector<Column> &columns = {ASTERISK});
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
                           const QVector<Column> &columns = {ASTERISK});

        /*! Set the columns to be selected. */
        Builder &select(const QVector<Column> &columns = {ASTERISK});
        /*! Set the column to be selected. */
        Builder &select(const Column &column);
        /*! Add new select columns to the query. */
        Builder &addSelect(const QVector<Column> &columns);
        /*! Add a new select column to the query. */
        Builder &addSelect(const Column &column);

        /*! Set a select subquery on the query. */
        template<Queryable T>
        Builder &select(T &&query, const QString &as);
        /*! Add a select subquery to the query. */
        template<Queryable T>
        Builder &addSelect(T &&query, const QString &as);

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
                         const QVector<QVariant> &bindings = {});

        /*! Add a join clause to the query. */
        template<JoinTable T>
        Builder &join(T &&table, const QString &first, const QString &comparison,
                      const QVariant &second, const QString &type = INNER,
                      bool where = false);
        /*! Add an advanced join clause to the query. */
        template<JoinTable T>
        Builder &join(T &&table, const std::function<void(JoinClause &)> &callback,
                      const QString &type = INNER);
        /*! Add a "join where" clause to the query. */
        template<JoinTable T>
        Builder &joinWhere(T &&table, const QString &first, const QString &comparison,
                           const QVariant &second, const QString &type = INNER);

        /*! Add a left join to the query. */
        template<JoinTable T>
        Builder &leftJoin(T &&table, const QString &first, const QString &comparison,
                          const QVariant &second);
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
                           const QVariant &second);
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
                           const QVariant &second);
        /*! Add an advanced "cross join" clause to the query. */
        template<JoinTable T>
        Builder &crossJoin(T &&table, const std::function<void(JoinClause &)> &callback);

        /*! Add a subquery join clause to the query. */
        template<SubQuery T>
        Builder &joinSub(T &&query, const QString &as, const QString &first,
                         const QString &comparison, const QVariant &second,
                         const QString &type = INNER, bool where = false);
        /*! Add a subquery join clause to the query. */
        template<SubQuery T>
        Builder &joinSub(T &&query, const QString &as,
                         const std::function<void(JoinClause &)> &callback,
                         const QString &type = INNER);
        /*! Add a subquery left join to the query. */
        template<SubQuery T>
        Builder &leftJoinSub(T &&query, const QString &as, const QString &first,
                             const QString &comparison, const QVariant &second);
        /*! Add a subquery left join to the query. */
        template<SubQuery T>
        Builder &leftJoinSub(T &&query, const QString &as,
                             const std::function<void(JoinClause &)> &callback);
        /*! Add a subquery right join to the query. */
        template<SubQuery T>
        Builder &rightJoinSub(T &&query, const QString &as, const QString &first,
                              const QString &comparison, const QVariant &second);
        /*! Add a subquery right join to the query. */
        template<SubQuery T>
        Builder &rightJoinSub(T &&query, const QString &as,
                              const std::function<void(JoinClause &)> &callback);

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

        /*! Add a nested where clause to the query. */
        Builder &where(const std::function<void(Builder &)> &callback,
                       const QString &condition = AND);
        /*! Add a nested "or where" clause to the query. */
        Builder &orWhere(const std::function<void(Builder &)> &callback);

        /*! Add a vector of basic where clauses to the query. */
        Builder &where(const QVector<WhereItem> &values,
                       const QString &condition = AND);
        /*! Add a vector of basic "or where" clauses to the query. */
        Builder &orWhere(const QVector<WhereItem> &values);

        /*! Add a vector of where clauses comparing two columns to the query. */
        Builder &whereColumn(const QVector<WhereColumnItem> &values,
                             const QString &condition = AND);
        /*! Add a vector of "or where" clauses comparing two columns to the query. */
        Builder &orWhereColumn(const QVector<WhereColumnItem> &values);

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

        /*! Add a "where in" clause to the query. */
        Builder &whereIn(const Column &column, const QVector<QVariant> &values,
                         const QString &condition = AND, bool nope = false);
        /*! Add an "or where in" clause to the query. */
        Builder &orWhereIn(const Column &column, const QVector<QVariant> &values);
        /*! Add a "where not in" clause to the query. */
        Builder &whereNotIn(const Column &column, const QVector<QVariant> &values,
                            const QString &condition = AND);
        /*! Add an "or where not in" clause to the query. */
        Builder &orWhereNotIn(const Column &column, const QVector<QVariant> &values);

        /*! Add a "where null" clause to the query. */
        Builder &whereNull(const QVector<Column> &columns = {ASTERISK},
                           const QString &condition = AND, bool nope = false);
        /*! Add an "or where null" clause to the query. */
        Builder &orWhereNull(const QVector<Column> &columns = {ASTERISK});
        /*! Add a "where not null" clause to the query. */
        Builder &whereNotNull(const QVector<Column> &columns = {ASTERISK},
                              const QString &condition = AND);
        /*! Add an "or where not null" clause to the query. */
        Builder &orWhereNotNull(const QVector<Column> &columns = {ASTERISK});

        /*! Add a "where null" clause to the query. */
        Builder &whereNull(const Column &column, const QString &condition = AND,
                           bool nope = false);
        /*! Add an "or where null" clause to the query. */
        Builder &orWhereNull(const Column &column);
        /*! Add a "where not null" clause to the query. */
        Builder &whereNotNull(const Column &column, const QString &condition = AND);
        /*! Add an "or where not null" clause to the query. */
        Builder &orWhereNotNull(const Column &column);

        /*! Add a basic where clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        Builder &where(C &&column, const QString &comparison, V &&value,
                       const QString &condition = AND);
        /*! Add an "or where" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        Builder &orWhere(C &&column, const QString &comparison, V &&value);
        /*! Add a basic equal where clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        Builder &whereEq(C &&column, V &&value, const QString &condition = AND);
        /*! Add an equal "or where" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        Builder &orWhereEq(C &&column, V &&value);

        /*! Add a full sub-select to the "where" clause. */
        template<WhereValueSubQuery T>
        Builder &whereSub(const Column &column, const QString &comparison, T &&query,
                          const QString &condition = AND);

        /*! Add a raw "where" clause to the query. */
        Builder &whereRaw(const QString &sql, const QVector<QVariant> &bindings = {},
                          const QString &condition = AND);
        /*! Add a raw "or where" clause to the query. */
        Builder &orWhereRaw(const QString &sql, const QVector<QVariant> &bindings = {});

        /*! Add a "group by" clause to the query. */
        Builder &groupBy(const QVector<Column> &groups);
        /*! Add a "group by" clause to the query. */
        Builder &groupBy(const Column &group);
        /*! Add a "group by" clause to the query. */
        template<ColumnConcept ...Args>
        Builder &groupBy(Args &&...groups);

        /*! Add a raw "groupBy" clause to the query. */
        Builder &groupByRaw(const QString &sql, const QVector<QVariant> &bindings = {});

        /*! Add a "having" clause to the query. */
        Builder &having(const Column &column, const QString &comparison,
                        const QVariant &value, const QString &condition = AND);
        /*! Add an "or having" clause to the query. */
        Builder &orHaving(const Column &column, const QString &comparison,
                          const QVariant &value);

        /*! Add a raw "having" clause to the query. */
        Builder &havingRaw(const QString &sql, const QVector<QVariant> &bindings = {},
                           const QString &condition = AND);
        /*! Add a raw "or having" clause to the query. */
        Builder &orHavingRaw(const QString &sql, const QVector<QVariant> &bindings = {});

        /*! Add an "order by" clause to the query. */
        Builder &orderBy(const Column &column, const QString &direction = ASC);
        /*! Add a descending "order by" clause to the query. */
        Builder &orderByDesc(const Column &column);

        /*! Add an "order by" clause to the query with a subquery ordering. */
        template<Queryable T>
        Builder &orderBy(T &&query, const QString &direction = ASC);
        /*! Add a descending "order by" clause to the query with a subquery ordering. */
        template<Queryable T>
        Builder &orderByDesc(T &&query);

        /*! Add a raw "order by" clause to the query. */
        Builder &orderByRaw(const QString &sql, const QVector<QVariant> &bindings = {});

        /*! Add an "order by" clause for a timestamp to the query. */
        Builder &latest(const Column &column = CREATED_AT);
        /*! Add an "order by" clause for a timestamp to the query. */
        Builder &oldest(const Column &column = CREATED_AT);
        /*! Remove all existing orders. */
        Builder &reorder();
        /*! Remove all existing orders and optionally add a new order. */
        Builder &reorder(const Column &column, const QString &direction = ASC);

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

        /* Getters / Setters */
        /*! Get a database connection. */
        ConnectionInterface &getConnection() const;
        /*! Get the query grammar instance. */
        const QueryGrammar &getGrammar() const;

        /*! Get the current query value bindings as flattened QVector. */
        QVector<QVariant> getBindings() const;
        /*! Get the raw map of bindings. */
        const BindingsMap &getRawBindings() const;
        /*! Add a binding to the query. */
        Builder &addBinding(const QVariant &binding,
                            BindingType type = BindingType::WHERE);
        /*! Add bindings to the query. */
        Builder &addBinding(const QVector<QVariant> &bindings,
                            BindingType type = BindingType::WHERE);
        /*! Add bindings to the query. */
        Builder &addBinding(QVector<QVariant> &&bindings,
                            BindingType type = BindingType::WHERE);
        /*! Set the bindings on the query builder. */
        Builder &setBindings(QVector<QVariant> &&bindings,
                             BindingType type = BindingType::WHERE);

        /*! Get an aggregate function and column to be run. */
        const std::optional<AggregateItem> &getAggregate() const;
        /*! Check if the query returns distinct results. */
        const std::variant<bool, QStringList> &
        getDistinct() const;
        /*! Check if the query returns distinct results. */
        template<typename T> requires std::same_as<T, bool>
        bool getDistinct() const;
        /*! Check if the query returns distinct results. */
        template<typename T> requires std::same_as<T, QStringList>
        const QStringList &
        getDistinct() const;
        // TODO check up all code and return references when appropriate silverqx
        /*! Get the columns that should be returned. */
        const QVector<Column> &getColumns() const;
        /*! Set the columns that should be returned. */
        Builder &setColumns(const QVector<Column> &columns);
        /*! Get the table associated with the query builder. */
        const std::variant<std::monostate, QString, Expression> &getFrom() const;
        /*! Get the table joins for the query. */
        const QVector<QSharedPointer<JoinClause>> &getJoins() const;
        /*! Get the where constraints for the query. */
        const QVector<WhereConditionItem> &getWheres() const;
        /*! Get the groupings for the query. */
        const QVector<Column> &getGroups() const;
        /*! Get the having constraints for the query. */
        const QVector<HavingConditionItem> &getHavings() const;
        /*! Get the orderings for the query. */
        const QVector<OrderByItem> &getOrders() const;
        /*! Get the maximum number of records to return. */
        int getLimit() const;
        /*! Get the number of records to skip. */
        int getOffset() const;
        /*! Get the row locking. */
        const std::variant<std::monostate, bool, QString> &getLock() const;

        /* Other methods */
        /*! Get a new instance of the query builder. */
        virtual QSharedPointer<Builder> newQuery() const;
        /*! Create a new query instance for nested where condition. */
        QSharedPointer<Builder> forNestedWhere() const;

        /*! Create a raw database expression. */
        Expression raw(const QVariant &value) const;

        /*! Add another query builder as a nested where to the query builder. */
        Builder &addNestedWhereQuery(const QSharedPointer<Builder> &query,
                                     const QString &condition);
        /*! Add an "exists" clause to the query. */
        Builder &addWhereExistsQuery(const QSharedPointer<Builder> &query,
                                     const QString &condition = AND, bool nope = false);

        /*! Merge an array of where clauses and bindings. */
        Builder &mergeWheres(const QVector<WhereConditionItem> &wheres,
                             const QVector<QVariant> &bindings);
        /*! Merge an array of where clauses and bindings. */
        Builder &mergeWheres(QVector<WhereConditionItem> &&wheres,
                             QVector<QVariant> &&bindings);

        /*! Builder property types. */
        enum struct PropertyType
        {
            COLUMNS,
        };

        /*! Clone the query without the given properties. */
        Builder cloneWithout(const std::unordered_set<PropertyType> &properties) const;
        /*! Clone the query without the given bindings. */
        Builder cloneWithoutBindings(
                const std::unordered_set<BindingType> &except) const;

    protected:
        /*! Determine if the given operator is supported. */
        bool invalidOperator(const QString &comparison) const;

        /*! Remove all of the expressions from a list of bindings. */
        QVector<QVariant> cleanBindings(const QVector<QVariant> &bindings) const;

        /*! Add a vector of basic where clauses to the query. */
        Builder &
        addArrayOfWheres(const QVector<WhereItem> &values,
                         const QString &condition = AND);
        /*! Add a vector of where clauses comparing two columns to the query. */
        Builder &
        addArrayOfWheres(const QVector<WhereColumnItem> &values,
                         const QString &condition = AND);

        /*! Get a new join clause. */
        QSharedPointer<JoinClause>
        newJoinClause(const Builder &query, const QString &type,
                      const QString &table) const;
        /*! Get a new join clause. */
        QSharedPointer<JoinClause>
        newJoinClause(const Builder &query, const QString &type,
                      Expression &&table) const;

        /*! Remove all existing columns and column bindings. */
        Builder &clearColumns();
        /*! Execute the given callback while selecting the given columns. */
        QSqlQuery
        onceWithColumns(const QVector<Column> &columns,
                        const std::function<QSqlQuery()> &callback);

        /*! Creates a subquery and parse it. */
        std::pair<QString, QVector<QVariant>>
        createSub(const std::function<void(Builder &)> &callback) const;
        /*! Creates a subquery and parse it. */
        std::pair<QString, QVector<QVariant>>
        createSub(Builder &query) const;
        /*! Creates a subquery and parse it. */
        std::pair<QString, QVector<QVariant>>
        createSub(const QString &query) const;
        /*! Creates a subquery and parse it. */
        std::pair<QString, QVector<QVariant>>
        createSub(QString &&query) const;

        /*! Determine whether the T type is a query builder instance or a lambda expr. */
        template<typename T>
        static constexpr bool isQueryable =
                std::is_convertible_v<T, Orm::QueryBuilder &> ||
                std::is_invocable_v<T, Orm::QueryBuilder &>;

        /*! Create a new query instance for a sub-query. */
        virtual QSharedPointer<Builder> forSubQuery() const;

        /*! Prepend the database name if the given query is on another database. */
        Builder &prependDatabaseNameIfCrossDatabaseQuery(Builder &query) const;

        /*! Strip off the table name or alias from a column identifier. */
        QString stripTableForPluck(const QString &column) const;

        /* Getters / Setters */
        /*! Set the aggregate property without running the query. */
        Builder &setAggregate(const QString &function,
                              const QVector<Column> &columns = {ASTERISK});

    private:
        /*! Run the query as a "select" statement against the connection. */
        QSqlQuery runSelect();

        /*! Set the table which the query is targeting. */
        Builder &setFrom(const FromClause &from);

        /*! Add a join clause to the query, common code. */
        Builder &joinInternal(
                QSharedPointer<JoinClause> &&join, const QString &first,
                const QString &comparison, const QVariant &second, bool where);
        /*! Add an advanced join clause to the query, common code. */
        Builder &joinInternal(
                QSharedPointer<JoinClause> &&join,
                const std::function<void(JoinClause &)> &callback);
        /*! Add a join clause to the query, common code for the above two methods. */
        Builder &joinInternal(QSharedPointer<JoinClause> &&join);

        /*! Add a subquery join clause to the query, common code. */
        Builder &joinSubInternal(
                std::pair<QString, QVector<QVariant>> &&subQuery, const QString &as,
                const QString &first, const QString &comparison, const QVariant &second,
                const QString &type, bool where);
        /*! Add a subquery join clause to the query, common code. */
        Builder &joinSubInternal(
                std::pair<QString, QVector<QVariant>> &&subQuery, const QString &as,
                const std::function<void(JoinClause &)> &callback,
                const QString &type);

        /*! Add a basic where clause to the query, common code. */
        Builder &whereInternal(const Column &column, const QString &comparison,
                               const QVariant &value, const QString &condition);

        /*! Throw exception when m_bindings doesn't contain a passed type. */
        void checkBindingType(BindingType type) const;

        /*! All of the available clause operators. */
        const QVector<QString> m_operators {
            EQ, LT, GT, LE, GE, NE_, NE, "<=>",
            LIKE, "like binary", NLIKE, ILIKE,
            B_AND, B_OR, "^", "<<", ">>",
            "rlike", "not rlike", "regexp", "not regexp",
            "~", "~*", "!~", "!~*", "similar to",
            "not similar to", "not ilike", "~~*", "!~~*",
        };

        /*! The database connection instance. */
        ConnectionInterface &m_connection;
        /*! The database query grammar instance. */
        const QueryGrammar &m_grammar;

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
        QVector<Column> m_columns;
        /*! The table which the query is targeting. */
        FromClause m_from;
        /*! The table joins for the query. */
        QVector<QSharedPointer<JoinClause>> m_joins;
        /*! The where constraints for the query. */
        QVector<WhereConditionItem> m_wheres;
        /*! The groupings for the query. */
        QVector<Column> m_groups;
        /*! The having constraints for the query. */
        QVector<HavingConditionItem> m_havings;
        /*! The orderings for the query. */
        QVector<OrderByItem> m_orders;
        /*! The maximum number of records to return. */
        int m_limit = -1;
        /*! The number of records to skip. */
        int m_offset = -1;
        /*! Indicates whether row locking is being used. */
        std::variant<std::monostate, bool, QString> m_lock;
    };

    template<typename T>
    std::map<T, QVariant>
    Builder::pluck(const QString &column, const QString &key)
    {
        /* First, we will need to select the results of the query accounting for the
           given column / key. Once we have the results, we will be able to take
           the results and get the exact data that was requested for the query. */
        auto query = get({column, key});

        const auto size = query.size();

        // Empty result
        if (size == 0)
            return {};

        /* If the columns are qualified with a table or have an alias, we cannot use
           those directly in the "pluck" operations since the results from the DB
           are only keyed by the column itself. We'll strip the table out here. */
        const auto unqualifiedColumn = stripTableForPluck(column);

        const auto unqualifiedKey = stripTableForPluck(key);

        std::map<T, QVariant> result;

        while (query.next())
            result.emplace(std::make_pair(query.value(unqualifiedKey).value<T>(),
                                          query.value(unqualifiedColumn)));

        return result;
    }

    inline quint64 Builder::count(const QVector<Column> &columns)
    {
        return aggregate(QStringLiteral("count"), columns).template value<quint64>();
    }

    template<typename>
    inline quint64 Builder::count(const Column &column)
    {
        return aggregate(QStringLiteral("count"), {column}).template value<quint64>();
    }

    inline QVariant Builder::min(const Column &column)
    {
        return aggregate(QStringLiteral("min"), {column});
    }

    inline QVariant Builder::max(const Column &column)
    {
        return aggregate(QStringLiteral("max"), {column});
    }

    inline QVariant Builder::sum(const Column &column)
    {
        auto result = aggregate(QStringLiteral("sum"), {column});

        if (!result.isValid() || result.isNull())
            result = 0;

        return result;
    }

    inline QVariant Builder::avg(const Column &column)
    {
        return aggregate(QStringLiteral("avg"), {column});
    }

    inline QVariant Builder::average(const Column &column)
    {
        return avg(column);
    }

    template<Queryable T>
    inline Builder &Builder::select(T &&query, const QString &as)
    {
        return selectSub(std::forward<T>(query), as);
    }

    template<Queryable T>
    Builder &Builder::addSelect(T &&query, const QString &as)
    {
        if (m_columns.isEmpty())
            select(QVector<Column> {QStringLiteral("%1.*")
                                    .arg(std::get<QString>(m_from))});

        return selectSub(std::forward<T>(query), as);
    }

    // TODO docs silverqx
    template<SubQuery T>
    Builder &Builder::selectSub(T &&query, const QString &as)
    {
        auto [queryString, bindings] = createSub(std::forward<T>(query));

        return selectRaw(QStringLiteral("(%1) as %2").arg(queryString,
                                                          m_grammar.wrap(as)),
                         bindings);
    }

    template<Remove T>
    inline std::tuple<int, QSqlQuery> Builder::deleteRow(T &&id)
    {
        return remove(std::forward<T>(id));
    }

    template<Remove T>
    std::tuple<int, QSqlQuery> Builder::remove(T &&id)
    {
        /* If an ID is passed to the method, we will set the where clause to check the
           ID to let developers to simply and quickly remove a single row from this
           database without manually specifying the "where" clauses on the query.
           m_from will be wrapped in the Grammar. */
        where(QStringLiteral("%1.id").arg(std::get<QString>(m_from)), EQ,
              std::forward<T>(id), AND);

        return remove();
    }

    template<SubQuery T>
    Builder &
    Builder::fromSub(T &&query, const QString &as)
    {
        auto [queryString, bindings] = createSub(std::forward<T>(query));

        return fromRaw(QStringLiteral("(%1) as %2").arg(queryString,
                                                        m_grammar.wrapTable(as)),
                       bindings);
    }

    template<JoinTable T>
    inline Builder &
    Builder::join(T &&table, const QString &first, const QString &comparison,
                  const QVariant &second, const QString &type, const bool where)
    {
        // Ownership of the QSharedPointer<JoinClause>
        return joinInternal(newJoinClause(*this, type, std::forward<T>(table)),
                            first, comparison, second, where);
    }

    // FUTURE joinSub, missing where param, also in joinSub silverqx
    template<JoinTable T>
    inline Builder &
    Builder::join(T &&table, const std::function<void(JoinClause &)> &callback,
                  const QString &type)
    {
        // Ownership of the QSharedPointer<JoinClause>
        return joinInternal(newJoinClause(*this, type, std::forward<T>(table)),
                            callback);
    }

    template<JoinTable T>
    inline Builder &
    Builder::joinWhere(T &&table, const QString &first, const QString &comparison,
                       const QVariant &second, const QString &type)
    {
        return join(std::forward<T>(table), first, comparison, second, type, true);
    }

    template<JoinTable T>
    inline Builder &
    Builder::leftJoin(T &&table, const QString &first, const QString &comparison,
                      const QVariant &second)
    {
        return join(std::forward<T>(table), first, comparison, second, LEFT);
    }

    template<JoinTable T>
    inline Builder &
    Builder::leftJoin(T &&table, const std::function<void(JoinClause &)> &callback)
    {
        return join(std::forward<T>(table), callback, LEFT);
    }

    template<JoinTable T>
    inline Builder &
    Builder::leftJoinWhere(T &&table, const QString &first, const QString &comparison,
                           const QVariant &second)
    {
        return joinWhere(std::forward<T>(table), first, comparison, second, LEFT);
    }

    template<JoinTable T>
    inline Builder &
    Builder::rightJoin(T &&table, const QString &first, const QString &comparison,
                       const QVariant &second)
    {
        return join(table, first, comparison, second, RIGHT);
    }

    template<JoinTable T>
    inline Builder &
    Builder::rightJoin(T &&table, const std::function<void(JoinClause &)> &callback)
    {
        return join(table, callback, RIGHT);
    }

    template<JoinTable T>
    inline Builder &
    Builder::rightJoinWhere(T &&table, const QString &first, const QString &comparison,
                            const QVariant &second)
    {
        return joinWhere(table, first, comparison, second, RIGHT);
    }

    // TODO docs missing example, because of different api silverqx
    // NOTE api different silverqx
    template<JoinTable T>
    inline Builder &
    Builder::crossJoin(T &&table, const QString &first, const QString &comparison,
                       const QVariant &second)
    {
        return join(table, first, comparison, second, CROSS);
    }

    template<JoinTable T>
    inline Builder &
    Builder::crossJoin(T &&table, const std::function<void(JoinClause &)> &callback)
    {
        return join(table, callback, CROSS);
    }

    template<SubQuery T>
    inline Builder &
    Builder::joinSub(T &&query, const QString &as, const QString &first,
                     const QString &comparison, const QVariant &second,
                     const QString &type, const bool where)
    {
        return joinSubInternal(createSub(std::forward<T>(query)),
                               as, first, comparison, second, type, where);
    }

    template<SubQuery T>
    inline Builder &
    Builder::joinSub(T &&query, const QString &as,
                     const std::function<void(JoinClause &)> &callback,
                     const QString &type)
    {
        return joinSubInternal(createSub(std::forward<T>(query)), as, callback, type);
    }

    template<SubQuery T>
    inline Builder &
    Builder::leftJoinSub(T &&query, const QString &as, const QString &first,
                         const QString &comparison, const QVariant &second)
    {
        return joinSub(std::forward<T>(query), as, first, comparison, second, LEFT);
    }

    template<SubQuery T>
    inline Builder &
    Builder::leftJoinSub(T &&query, const QString &as,
                         const std::function<void(JoinClause &)> &callback)
    {
        return joinSub(std::forward<T>(query), as, callback, LEFT);
    }

    template<SubQuery T>
    inline Builder &
    Builder::rightJoinSub(T &&query, const QString &as, const QString &first,
                          const QString &comparison, const QVariant &second)
    {
        return joinSub(std::forward<T>(query), as, first, comparison, second, RIGHT);
    }

    template<SubQuery T>
    inline Builder &
    Builder::rightJoinSub(T &&query, const QString &as,
                          const std::function<void(JoinClause &)> &callback)
    {
        return joinSub(std::forward<T>(query), as, callback, RIGHT);
    }

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
            return whereInternal(column, comparison, std::forward<T>(value), condition);
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
    inline Builder &Builder::orWhere(C &&column, const QString &comparison, V &&value)
    {
        return where(std::forward<C>(column), comparison, std::forward<V>(value), OR);
    }

    template<Queryable C, WhereValue V>
    inline Builder &Builder::whereEq(C &&column, V &&value, const QString &condition)
    {
        return where(std::forward<C>(column), EQ, std::forward<V>(value), condition);
    }

    template<Queryable C, WhereValue V>
    inline Builder &Builder::orWhereEq(C &&column, V &&value)
    {
        return where(std::forward<C>(column), EQ, std::forward<V>(value), OR);
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

    template<ColumnConcept ...Args>
    inline Builder &Builder::groupBy(Args &&...groups)
    {
        return groupBy(QVector<Column> {std::forward<Args>(groups)...});
    }

    template<Queryable T>
    Builder &Builder::orderBy(T &&query, const QString &direction)
    {
        auto [queryString, bindings] = createSub(std::forward<T>(query));

        addBinding(std::move(bindings), BindingType::ORDER);

        return orderBy(Expression(PARENTH_ONE.arg(queryString)), direction);
    }

    template<Queryable T>
    inline Builder &Builder::orderByDesc(T &&query)
    {
        return orderBy(std::forward<T>(query), DESC);
    }

    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, QSqlQuery>
    Builder::increment(const QString &column, const T amount,
                       const QVector<UpdateItem> &extra)
    {
        const auto expression = QStringLiteral("%1 + %2").arg(m_grammar.wrap(column))
                                .arg(amount);

        QVector<UpdateItem> columns {{column, raw(expression)}};
        std::copy(extra.cbegin(), extra.cend(), std::back_inserter(columns));

        return update(columns);
    }

    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, QSqlQuery>
    Builder::decrement(const QString &column, const T amount,
                       const QVector<UpdateItem> &extra)
    {
        const auto expression = QStringLiteral("%1 - %2").arg(m_grammar.wrap(column))
                                .arg(amount);

        QVector<UpdateItem> columns {{column, raw(expression)}};
        std::copy(extra.cbegin(), extra.cend(), std::back_inserter(columns));

        return update(columns);
    }

    inline ConnectionInterface &Builder::getConnection() const
    {
        return m_connection;
    }

    inline const Builder::QueryGrammar &Builder::getGrammar() const
    {
        return m_grammar;
    }

    inline const BindingsMap &Builder::getRawBindings() const
    {
        return m_bindings;
    }

    inline const std::optional<AggregateItem> &Builder::getAggregate() const
    {
        return m_aggregate;
    }

    inline const std::variant<bool, QStringList> &
    Builder::getDistinct() const
    {
        return m_distinct;
    }

    template<typename T> requires std::same_as<T, bool>
    inline bool Builder::getDistinct() const
    {
        return std::get<bool>(m_distinct);
    }

    template<typename T> requires std::same_as<T, QStringList>
    inline const QStringList &
    Builder::getDistinct() const
    {
        return std::get<QStringList>(m_distinct);
    }

    inline const QVector<Column> &
    Builder::getColumns() const
    {
        return m_columns;
    }

    inline Builder &
    Builder::setColumns(const QVector<Column> &columns)
    {
        m_columns = columns;

        return *this;
    }

    inline const std::variant<std::monostate, QString, Expression> &
    Builder::getFrom() const
    {
        return m_from;
    }

    inline const QVector<QSharedPointer<JoinClause>> &
    Builder::getJoins() const
    {
        return m_joins;
    }

    inline const QVector<WhereConditionItem> &
    Builder::getWheres() const
    {
        return m_wheres;
    }

    inline const QVector<Column> &
    Builder::getGroups() const
    {
        return m_groups;
    }

    inline const QVector<HavingConditionItem> &
    Builder::getHavings() const
    {
        return m_havings;
    }

    inline const QVector<OrderByItem> &
    Builder::getOrders() const
    {
        return m_orders;
    }

    inline int Builder::getLimit() const
    {
        return m_limit;
    }

    inline int Builder::getOffset() const
    {
        return m_offset;
    }

    inline const std::variant<std::monostate, bool, QString> &
    Builder::getLock() const
    {
        return m_lock;
    }

    inline QSharedPointer<Builder>
    Builder::forSubQuery() const
    {
        return newQuery();
    }

    inline Builder &
    Builder::setFrom(const FromClause &from)
    {
        m_from = from;

        return *this;
    }

} // namespace Orm::Query
} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // QUERYBUILDER_H
