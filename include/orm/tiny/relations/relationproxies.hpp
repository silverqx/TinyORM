#pragma once
#ifndef ORM_TINY_RELATIONS_RELATIONPROXIES_HPP
#define ORM_TINY_RELATIONS_RELATIONPROXIES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/ormconcepts.hpp"
#include "orm/tiny/types/modelscollection.hpp"
#include "orm/types/sqlquery.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
namespace Query
{
    class JoinClause;
}

namespace Tiny::Relations
{

    /*! Contains proxy methods to the TinyBuilder. */
    template<class Model, class Related>
    class RelationProxies
    {
        /*! Alias for the JoinClause. */
        using JoinClause = Orm::Query::JoinClause;
        /*! Alias for the QueriesRelationships callback type. */
        template<typename HasRelated>
        using CallbackType = Orm::Tiny::Concerns
                                      ::QueriesRelationshipsCallback<HasRelated>;

    protected:
        /*! RelationProxies's copy constructor (used by BelongsToMany::clone()). */
        RelationProxies(const RelationProxies &) = default;

    public:
        /*! Parent Model type. */
        using ModelType = Model;
        /*! Related type. */
        using RelatedType = Related;

        /*! Default constructor. */
        RelationProxies() = default;
        /*! Pure virtual destructor. */
        inline virtual ~RelationProxies() = 0;

        /*! RelationProxies's copy assignment operator. */
        RelationProxies &operator=(const RelationProxies &) = delete;
        /*! RelationProxies's move constructor. */
        RelationProxies(RelationProxies &&) = delete;
        /*! RelationProxies's move assignment operator. */
        RelationProxies &operator=(RelationProxies &&) = delete;

        /* Retrieving results */
        /*! Get a single column's value from the first result of a query. */
        QVariant value(const Column &column) const;
        /*! Get a single column's value from the first result of a query if it's
            the sole matching record. */
        QVariant soleValue(const Column &column) const;

        /*! Get a vector with values in the given column. */
        QList<QVariant> pluck(const Column &column) const;
        /*! Get a map with values in the given column and keyed by values in the key
            column. */
        template<typename T>
        std::map<T, QVariant> pluck(const Column &column, const Column &key) const;

        /*! Concatenate values of the given column as a string. */
        QString implode(const QString &column, const QString &glue = "") const;

        /*! Find a model by its primary key. */
        virtual std::optional<Related>
        find(const QVariant &id, const QList<Column> &columns = {ASTERISK}) const; // NOLINT(google-default-arguments)
        /*! Find a model by its primary key or return fresh model instance. */
        virtual Related
        findOrNew(const QVariant &id, const QList<Column> &columns = {ASTERISK}) const; // NOLINT(google-default-arguments)
        /*! Find a model by its primary key or throw an exception. */
        virtual Related
        findOrFail(const QVariant &id, // NOLINT(google-default-arguments)
                   const QList<Column> &columns = {ASTERISK}) const;
        // findMany() is missing intentionally doesn't make sense for one type relations

        /*! Execute a query for a single record by ID or call a callback. */
        virtual std::optional<Related>
        findOr(const QVariant &id, const QList<Column> &columns,
               const std::function<void()> &callback) const;
        /*! Execute a query for a single record by ID or call a callback. */
        virtual std::optional<Related>
        findOr(const QVariant &id, const std::function<void()> &callback) const;

        /*! Execute a query for a single record by ID or call a callback. */
        template<typename R>
        std::pair<std::optional<Related>, R>
        findOr(const QVariant &id, const QList<Column> &columns,
               const std::function<R()> &callback) const;
        /*! Execute a query for a single record by ID or call a callback. */
        template<typename R>
        std::pair<std::optional<Related>, R>
        findOr(const QVariant &id, const std::function<R()> &callback) const;

        /*! Execute the query and get the first result. */
        virtual std::optional<Related>
        first(const QList<Column> &columns = {ASTERISK}) const; // NOLINT(google-default-arguments)
        /*! Get the first record matching the attributes or instantiate it. */
        virtual Related
        firstOrNew(const QList<WhereItem> &attributes = {}, // NOLINT(google-default-arguments)
                   const QList<AttributeItem> &values = {}) const;
        /*! Get the first record matching the attributes or create it. */
        Related
        firstOrCreate(const QList<WhereItem> &attributes = {},
                      const QList<AttributeItem> &values = {}) const;
        /*! Execute the query and get the first result or throw an exception. */
        virtual Related firstOrFail(const QList<Column> &columns = {ASTERISK}) const; // NOLINT(google-default-arguments)

        /*! Execute the query and get the first result or call a callback. */
        virtual std::optional<Related>
        firstOr(const QList<Column> &columns, // NOLINT(google-default-arguments)
                const std::function<void()> &callback = nullptr) const;
        /*! Execute the query and get the first result or call a callback. */
        virtual std::optional<Related>
        firstOr(const std::function<void()> &callback = nullptr) const; // NOLINT(google-default-arguments)

        /*! Execute the query and get the first result or call a callback. */
        template<typename R>
        std::pair<std::optional<Related>, R>
        firstOr(const QList<Column> &columns, const std::function<R()> &callback) const;
        /*! Execute the query and get the first result or call a callback. */
        template<typename R>
        std::pair<std::optional<Related>, R>
        firstOr(const std::function<R()> &callback) const;

        /*! Add a basic where clause to the query, and return the first result. */
        virtual std::optional<Related>
        firstWhere(const Column &column, const QString &comparison, // NOLINT(google-default-arguments)
                   const QVariant &value, const QString &condition = AND) const;
        /*! Add a basic where clause to the query, and return the first result. */
        virtual std::optional<Related>
        firstWhereEq(const Column &column, const QVariant &value, // NOLINT(google-default-arguments)
                     const QString &condition = AND) const;

        /*! Add a where clause on the primary key to the query. */
        const Relation<Model, Related> &whereKey(const QVariant &id) const;
        /*! Add a where clause on the primary key to the query. */
        const Relation<Model, Related> &whereKey(const QList<QVariant> &ids) const;
        /*! Add a where clause on the primary key to the query. */
        const Relation<Model, Related> &whereKeyNot(const QVariant &id) const;
        /*! Add a where clause on the primary key to the query. */
        const Relation<Model, Related> &whereKeyNot(const QList<QVariant> &ids) const;

        /*! Set the relationships that should be eager loaded. */
        template<typename = void>
        const Relation<Model, Related> &with(const QList<WithItem> &relations) const;
        /*! Set the relationships that should be eager loaded. */
        template<typename = void>
        const Relation<Model, Related> &with(QString relation) const;
        /*! Begin querying a model with eager loading. */
        const Relation<Model, Related> &with(const QList<QString> &relations) const;
        /*! Begin querying a model with eager loading. */
        const Relation<Model, Related> &with(QList<QString> &&relations) const;

        /*! Prevent the specified relations from being eager loaded. */
        const Relation<Model, Related> &without(const QList<QString> &relations) const;
        /*! Prevent the specified relations from being eager loaded. */
        const Relation<Model, Related> &without(QString relation) const;

        /*! Set the relationships that should be eager loaded while removing
            any previously added eager loading specifications. */
        template<typename = void>
        const Relation<Model, Related> &
        withOnly(const QList<WithItem> &relations) const;
        /*! Set the relationship that should be eager loaded while removing
            any previously added eager loading specifications. */
        template<typename = void>
        const Relation<Model, Related> &withOnly(QString relation) const;
        /*! Set the relationships that should be eager loaded while removing
            any previously added eager loading specifications. */
        const Relation<Model, Related> &withOnly(const QList<QString> &relations) const;
        /*! Set the relationship that should be eager loaded while removing
            any previously added eager loading specifications. */
        const Relation<Model, Related> &withOnly(QList<QString> &&relations) const;

        /* Insert, Update, Delete */
        /*! Create or update a related record matching the attributes, and fill it
            with values. */
        Related updateOrCreate(const QList<WhereItem> &attributes,
                               const QList<AttributeItem> &values = {}) const;

        /* Proxies to TinyBuilder -> QueryBuilder */
        /*! The table which the query is targeting. */
        const FromClause &from() const;

        /*! Get the SQL representation of the query. */
        QString toSql() const;
        /*! Get the current query value bindings as flattened QList. */
        QList<QVariant> getBindings() const;

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
        quint64 insertGetId(const QList<AttributeItem> &attributes,
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

        /*! Update records in the database. */
        std::tuple<int, TSqlQuery>
        update(const QList<UpdateItem> &values) const;

        /*! Insert new records or update the existing ones. */
        std::tuple<int, std::optional<TSqlQuery>>
        upsert(const QList<QVariantMap> &values, const QStringList &uniqueBy,
               const QStringList &update) const;
        /*! Insert new records or update the existing ones (update all columns). */
        std::tuple<int, std::optional<TSqlQuery>>
        upsert(const QList<QVariantMap> &values, const QStringList &uniqueBy) const;

        /*! Delete records from the database. */
        std::tuple<int, TSqlQuery> remove() const;
        /*! Delete records from the database. */
        std::tuple<int, TSqlQuery> deleteModels() const;

        /*! Run a truncate statement on the table. */
        void truncate() const;

        /* Select */
        /*! Retrieve the "count" result of the query. */
        quint64 count(const QList<Column> &columns = {ASTERISK}) const;
        /*! Retrieve the "count" result of the query. */
        template<typename = void>
        quint64 count(const Column &column) const;
        /*! Retrieve the minimum value of a given column. */
        QVariant min(const Column &column) const;
        /*! Retrieve the maximum value of a given column. */
        QVariant max(const Column &column) const;
        /*! Retrieve the sum of the values of a given column. */
        QVariant sum(const Column &column) const;
        /*! Retrieve the average of the values of a given column. */
        QVariant avg(const Column &column) const;
        /*! Alias for the "avg" method. */
        QVariant average(const Column &column) const;

        /*! Execute an aggregate function on the database. */
        QVariant aggregate(const QString &function,
                           const QList<Column> &columns = {ASTERISK}) const;

        /*! Determine if any rows exist for the current query. */
        bool exists() const;
        /*! Determine if no rows exist for the current query. */
        bool doesntExist() const;

        /*! Execute the given callback if no rows exist for the current query. */
        bool existsOr(const std::function<void()> &callback) const;
        /*! Execute the given callback if rows exist for the current query. */
        bool doesntExistOr(const std::function<void()> &callback) const;

        /*! Execute the given callback if no rows exist for the current query. */
        template<typename R>
        std::pair<bool, R> existsOr(const std::function<R()> &callback) const;
        template<typename R>
        /*! Execute the given callback if rows exist for the current query. */
        std::pair<bool, R> doesntExistOr(const std::function<R()> &callback) const;

        /*! Set the columns to be selected. */
        const Relation<Model, Related> &
        select(const QList<Column> &columns = {ASTERISK}) const;
        /*! Set the column to be selected. */
        const Relation<Model, Related> &select(const Column &column) const;
        /*! Add new select columns to the query. */
        const Relation<Model, Related> &addSelect(const QList<Column> &columns) const;
        /*! Add a new select column to the query. */
        const Relation<Model, Related> &addSelect(const Column &column) const;

        /*! Set the columns to be selected. */
        const Relation<Model, Related> &select(QList<Column> &&columns) const;
        /*! Set the column to be selected. */
        const Relation<Model, Related> &select(Column &&column) const;
        /*! Add new select columns to the query. */
        const Relation<Model, Related> &addSelect(QList<Column> &&columns) const;
        /*! Add a new select column to the query. */
        const Relation<Model, Related> &addSelect(Column &&column) const;

        /*! Set a select subquery on the query. */
        template<Queryable T>
        const Relation<Model, Related> &select(T &&query, const QString &as) const;
        /*! Add a select subquery to the query. */
        template<Queryable T>
        const Relation<Model, Related> &addSelect(T &&query, const QString &as) const;

        /*! Add a subselect expression to the query. */
        template<SubQuery T>
        const Relation<Model, Related> &selectSub(T &&query, const QString &as) const;
        /*! Add a new "raw" select expression to the query. */
        const Relation<Model, Related> &selectRaw(
                const QString &expression, const QList<QVariant> &bindings = {}) const;

        /*! Force the query to only return distinct results. */
        const Relation<Model, Related> &distinct() const;
        /*! Force the query to only return distinct results. */
        const Relation<Model, Related> &distinct(const QStringList &columns) const;
        /*! Force the query to only return distinct results. */
        const Relation<Model, Related> &distinct(QStringList &&columns) const;

        /* Joins */
        /*! Add a join clause to the query. */
        template<JoinTable T>
        const Relation<Model, Related> &join(
                T &&table, const QString &first, const QString &comparison,
                const QString &second, const QString &type = INNER,
                bool where = false) const;
        /*! Add an advanced join clause to the query. */
        template<JoinTable T>
        const Relation<Model, Related> &join(
                T &&table, const std::function<void(JoinClause &)> &callback,
                const QString &type = INNER) const;
        /*! Add a "join where" clause to the query. */
        template<JoinTable T>
        const Relation<Model, Related> &joinWhere(
                T &&table, const QString &first, const QString &comparison,
                const QVariant &second, const QString &type = INNER) const;

        /*! Add a left join to the query. */
        template<JoinTable T>
        const Relation<Model, Related> &leftJoin(
                T &&table, const QString &first, const QString &comparison,
                const QString &second) const;
        /*! Add an advanced left join to the query. */
        template<JoinTable T>
        const Relation<Model, Related> &leftJoin(
                T &&table, const std::function<void(JoinClause &)> &callback) const;
        /*! Add a "join where" clause to the query. */
        template<JoinTable T>
        const Relation<Model, Related> &leftJoinWhere(
                T &&table, const QString &first, const QString &comparison,
                const QVariant &second) const;

        /*! Add a right join to the query. */
        template<JoinTable T>
        const Relation<Model, Related> &rightJoin(
                T &&table, const QString &first, const QString &comparison,
                const QString &second) const;
        /*! Add an advanced right join to the query. */
        template<JoinTable T>
        const Relation<Model, Related> &rightJoin(
                T &&table, const std::function<void(JoinClause &)> &callback) const;
        /*! Add a "right join where" clause to the query. */
        template<JoinTable T>
        const Relation<Model, Related> &rightJoinWhere(
                T &&table, const QString &first, const QString &comparison,
                const QVariant &second) const;

        /*! Add a "cross join" clause to the query. */
        template<JoinTable T>
        const Relation<Model, Related> &crossJoin(
                T &&table, const QString &first, const QString &comparison,
                const QString &second) const;
        /*! Add an advanced "cross join" clause to the query. */
        template<JoinTable T>
        const Relation<Model, Related> &crossJoin(
                T &&table, const std::function<void(JoinClause &)> &callback) const;
        /*! Add a "cross join" clause to the query. */
        template<JoinTable T>
        const Relation<Model, Related> &crossJoin(T &&table) const;

        /*! Add a subquery join clause to the query. */
        template<SubQuery T>
        const Relation<Model, Related> &joinSub(
                T &&query, const QString &as, const QString &first,
                const QString &comparison, const QVariant &second,
                const QString &type = INNER, bool where = false) const;
        /*! Add a subquery join clause to the query. */
        template<SubQuery T>
        const Relation<Model, Related> &joinSub(
                T &&query, const QString &as,
                const std::function<void(JoinClause &)> &callback,
                const QString &type = INNER) const;

        /*! Add a subquery left join to the query. */
        template<SubQuery T>
        const Relation<Model, Related> &leftJoinSub(
                T &&query, const QString &as, const QString &first,
                const QString &comparison, const QVariant &second) const;
        /*! Add a subquery left join to the query. */
        template<SubQuery T>
        const Relation<Model, Related> &leftJoinSub(
                T &&query, const QString &as,
                const std::function<void(JoinClause &)> &callback) const;

        /*! Add a subquery right join to the query. */
        template<SubQuery T>
        const Relation<Model, Related> &rightJoinSub(
                T &&query, const QString &as, const QString &first,
                const QString &comparison, const QVariant &second) const;
        /*! Add a subquery right join to the query. */
        template<SubQuery T>
        const Relation<Model, Related> &rightJoinSub(
                T &&query, const QString &as,
                const std::function<void(JoinClause &)> &callback) const;

        /* General where */
        /*! Add a basic where clause to the query. */
        template<WhereValue T>
        const Relation<Model, Related> &where(
                const Column &column, const QString &comparison, T &&value,
                const QString &condition = AND) const;
        /*! Add an "or where" clause to the query. */
        template<WhereValue T>
        const Relation<Model, Related> &orWhere(
                const Column &column, const QString &comparison, T &&value) const;
        /*! Add a basic equal where clause to the query. */
        template<WhereValue T>
        const Relation<Model, Related> &whereEq(
                const Column &column, T &&value, const QString &condition = AND) const;
        /*! Add an equal "or where" clause to the query. */
        template<WhereValue T>
        const Relation<Model, Related> &orWhereEq(const Column &column, T &&value) const;

        /* General where not */
        /*! Add a basic "where not" clause to the query. */
        template<WhereValue T>
        const Relation<Model, Related> &whereNot(
                const Column &column, const QString &comparison, T &&value,
                const QString &condition = AND) const;
        /*! Add an "or where not" clause to the query. */
        template<WhereValue T>
        const Relation<Model, Related> &orWhereNot(
                const Column &column, const QString &comparison, T &&value) const;
        /*! Add a basic equal "where not" clause to the query. */
        template<WhereValue T>
        const Relation<Model, Related> &whereNotEq(
                const Column &column, T &&value, const QString &condition = AND) const;
        /*! Add an equal "or where not" clause to the query. */
        template<WhereValue T>
        const Relation<Model, Related> &orWhereNotEq(
                const Column &column, T &&value) const;

        /* Nested where */
        /*! Add a nested where clause to the query. */
        const Relation<Model, Related> &where(
                const std::function<void(Builder<Related> &)> &callback,
                const QString &condition = AND) const;
        /*! Add a nested "or where" clause to the query. */
        const Relation<Model, Related> &orWhere(
                const std::function<void(Builder<Related> &)> &callback) const;
        const Relation<Model, Related> &whereNot(
                const std::function<void(Builder<Related> &)> &callback,
                const QString &condition = AND) const;
        /*! Add a nested "or where not" clause to the query. */
        const Relation<Model, Related> &orWhereNot(
                const std::function<void(Builder<Related> &)> &callback) const;

        /* Array where */
        /*! Add a vector of basic where clauses to the query. */
        const Relation<Model, Related> &where(
                const QList<WhereItem> &values, const QString &condition = AND) const;
        /*! Add a vector of basic "or where" clauses to the query. */
        const Relation<Model, Related> &orWhere(const QList<WhereItem> &values) const;
        /*! Add a vector of basic "where not" clauses to the query. */
        const Relation<Model, Related> &whereNot(
                const QList<WhereItem> &values, const QString &condition = AND,
                const QString &defaultCondition = "") const;
        /*! Add a vector of basic "or where not" clauses to the query. */
        const Relation<Model, Related> &orWhereNot(
                const QList<WhereItem> &values,
                const QString &defaultCondition = "") const;

        /* where column */
        /*! Add a vector of where clauses comparing two columns to the query. */
        const Relation<Model, Related> &whereColumn(
                const QList<WhereColumnItem> &values,
                const QString &condition = AND) const;
        /*! Add a vector of "or where" clauses comparing two columns to the query. */
        const Relation<Model, Related> &orWhereColumn(
                const QList<WhereColumnItem> &values) const;

        /*! Add a "where" clause comparing two columns to the query. */
        const Relation<Model, Related> &whereColumn(
                const Column &first, const QString &comparison, const Column &second,
                const QString &condition = AND) const;
        /*! Add a "or where" clause comparing two columns to the query. */
        const Relation<Model, Related> &orWhereColumn(
                const Column &first, const QString &comparison,
                const Column &second) const;
        /*! Add an equal "where" clause comparing two columns to the query. */
        const Relation<Model, Related> &whereColumnEq(
                const Column &first, const Column &second,
                const QString &condition = AND) const;
        /*! Add an equal "or where" clause comparing two columns to the query. */
        const Relation<Model, Related> &orWhereColumnEq(
                const Column &first, const Column &second) const;

        /* where IN */
        /*! Add a "where in" clause to the query. */
        const Relation<Model, Related> &whereIn(
                const Column &column, const QList<QVariant> &values,
                const QString &condition = AND, bool nope = false) const;
        /*! Add an "or where in" clause to the query. */
        const Relation<Model, Related> &orWhereIn(
                const Column &column, const QList<QVariant> &values) const;
        /*! Add a "where not in" clause to the query. */
        const Relation<Model, Related> &whereNotIn(
                const Column &column, const QList<QVariant> &values,
                const QString &condition = AND) const;
        /*! Add an "or where not in" clause to the query. */
        const Relation<Model, Related> &orWhereNotIn(
                const Column &column, const QList<QVariant> &values) const;

        /* where null */
        /*! Add a "where null" clause to the query. */
        const Relation<Model, Related> &whereNull(
                const QList<Column> &columns = {ASTERISK},
                const QString &condition = AND, bool nope = false) const;
        /*! Add an "or where null" clause to the query. */
        const Relation<Model, Related> &orWhereNull(
                const QList<Column> &columns = {ASTERISK}) const;
        /*! Add a "where not null" clause to the query. */
        const Relation<Model, Related> &whereNotNull(
                const QList<Column> &columns = {ASTERISK},
                const QString &condition = AND) const;
        /*! Add an "or where not null" clause to the query. */
        const Relation<Model, Related> &orWhereNotNull(
                const QList<Column> &columns = {ASTERISK}) const;

        /*! Add a "where null" clause to the query. */
        const Relation<Model, Related> &whereNull(
                const Column &column, const QString &condition = AND,
                bool nope = false) const;
        /*! Add an "or where null" clause to the query. */
        const Relation<Model, Related> &orWhereNull(const Column &column) const;
        /*! Add a "where not null" clause to the query. */
        const Relation<Model, Related> &whereNotNull(
                const Column &column, const QString &condition = AND) const;
        /*! Add an "or where not null" clause to the query. */
        const Relation<Model, Related> &orWhereNotNull(const Column &column) const;

        /* where between */
        /*! Add a "where between" statement to the query. */
        const Relation<Model, Related> &whereBetween(
                const Column &column, const WhereBetweenItem &values,
                const QString &condition = AND, bool nope = false) const;
        /*! Add an "or where between" statement to the query. */
        const Relation<Model, Related> &orWhereBetween(
                const Column &column, const WhereBetweenItem &values) const;
        /*! Add a "where not between" statement to the query. */
        const Relation<Model, Related> &whereNotBetween(
                const Column &column, const WhereBetweenItem &values,
                const QString &condition = AND) const;
        /*! Add an "or where not between" statement to the query. */
        const Relation<Model, Related> &orWhereNotBetween(
                const Column &column, const WhereBetweenItem &values) const;

        /* where between columns */
        /*! Add a "where between" statement using columns to the query. */
        const Relation<Model, Related> &whereBetweenColumns(
                const Column &column, const WhereBetweenColumnsItem &betweenColumns,
                const QString &condition = AND, bool nope = false) const;
        /*! Add an "or where between" statement using columns to the query. */
        const Relation<Model, Related> &orWhereBetweenColumns(
                const Column &column,
                const WhereBetweenColumnsItem &betweenColumns) const;
        /*! Add a "where not between" statement using columns to the query. */
        const Relation<Model, Related> &whereNotBetweenColumns(
                const Column &column, const WhereBetweenColumnsItem &betweenColumns,
                const QString &condition = AND) const;
        /*! Add an "or where not between" statement using columns to the query. */
        const Relation<Model, Related> &orWhereNotBetweenColumns(
                const Column &column,
                const WhereBetweenColumnsItem &betweenColumns) const;

        /* where sub-queries */
        /*! Add a basic where clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        const Relation<Model, Related> &where(
                C &&column, const QString &comparison, V &&value,
                const QString &condition = AND) const;
        /*! Add an "or where" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        const Relation<Model, Related> &orWhere(
                C &&column, const QString &comparison, V &&value) const;
        /*! Add a basic equal where clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        const Relation<Model, Related> &whereEq(
                C &&column, V &&value, const QString &condition = AND) const;
        /*! Add an equal "or where" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        const Relation<Model, Related> &orWhereEq(C &&column, V &&value) const;

        /* where not sub-queries */
        /*! Add a basic "where not" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        const Relation<Model, Related> &whereNot(
                C &&column, const QString &comparison, V &&value,
                const QString &condition = AND) const;
        /*! Add an "or where not" clause to the query with a full sub-select column. */
        template<Queryable C, WhereValue V>
        const Relation<Model, Related> &orWhereNot(
                C &&column, const QString &comparison, V &&value) const;
        /*! Add a basic equal "where not" clause to the query with a full sub-select
            column. */
        template<Queryable C, WhereValue V>
        const Relation<Model, Related> &whereNotEq(
                C &&column, V &&value, const QString &condition = AND) const;
        /*! Add an equal "or where not" clause to the query with a full sub-select
            column. */
        template<Queryable C, WhereValue V>
        const Relation<Model, Related> &orWhereNotEq(C &&column, V &&value) const;

        /*! Add a full sub-select to the "where" clause. */
        template<WhereValueSubQuery T>
        const Relation<Model, Related> &whereSub(
                const Column &column, const QString &comparison, T &&query,
                const QString &condition = AND) const;

        /* where exists */
        /*! Add an exists clause to the query. */
        template<QueryableShared C>
        const Relation<Model, Related> &
        whereExists(C &&callback, const QString &condition = AND,
                    bool nope = false) const;
        /*! Add an or exists clause to the query. */
        template<QueryableShared C>
        const Relation<Model, Related> &
        orWhereExists(C &&callback, bool nope = false) const;
        /*! Add a where not exists clause to the query. */
        template<QueryableShared C>
        const Relation<Model, Related> &
        whereNotExists(C &&callback, const QString &condition = AND) const;
        /*! Add a where not exists clause to the query. */
        template<QueryableShared C>
        const Relation<Model, Related> &
        orWhereNotExists(C &&callback) const;

        /* where row values */
        /*! Adds a where condition using row values. */
        const Relation<Model, Related> &
        whereRowValues(const QList<Column> &columns, const QString &comparison,
                       const QList<QVariant> &values,
                       const QString &condition = AND) const;
        /*! Adds an or where condition using row values. */
        const Relation<Model, Related> &
        orWhereRowValues(const QList<Column> &columns, const QString &comparison,
                         const QList<QVariant> &values) const;
        const Relation<Model, Related> &
        whereRowValuesEq(const QList<Column> &columns, const QList<QVariant> &values,
                         const QString &condition = AND) const;
        /*! Adds an or where condition using row values. */
        const Relation<Model, Related> &
        orWhereRowValuesEq(const QList<Column> &columns,
                           const QList<QVariant> &values) const;

        /* where dates */
        /*! Add a "where date" statement to the query. */
        const Relation<Model, Related> &
        whereDate(const Column &column, const QString &comparison, QVariant value,
                  const QString &condition = AND) const;
        /*! Add a "where time" statement to the query. */
        const Relation<Model, Related> &
        whereTime(const Column &column, const QString &comparison, QVariant value,
                  const QString &condition = AND) const;
        /*! Add a "where day" statement to the query. */
        const Relation<Model, Related> &
        whereDay(const Column &column, const QString &comparison, QVariant value,
                 const QString &condition = AND) const;
        /*! Add a "where month" statement to the query. */
        const Relation<Model, Related> &
        whereMonth(const Column &column, const QString &comparison, QVariant value,
                   const QString &condition = AND) const;
        /*! Add a "where year" statement to the query. */
        const Relation<Model, Related> &
        whereYear(const Column &column, const QString &comparison, QVariant value,
                  const QString &condition = AND) const;

        /*! Add an equal "where date" statement to the query. */
        const Relation<Model, Related> &
        whereEqDate(const Column &column, QVariant value,
                    const QString &condition = AND) const;
        /*! Add an equal "where time" statement to the query. */
        const Relation<Model, Related> &
        whereEqTime(const Column &column, QVariant value,
                    const QString &condition = AND) const;
        /*! Add an equal "where day" statement to the query. */
        const Relation<Model, Related> &
        whereEqDay(const Column &column, QVariant value,
                   const QString &condition = AND) const;
        /*! Add an equal "where month" statement to the query. */
        const Relation<Model, Related> &
        whereEqMonth(const Column &column, QVariant value,
                     const QString &condition = AND) const;
        /*! Add an equal "where year" statement to the query. */
        const Relation<Model, Related> &
        whereEqYear(const Column &column, QVariant value,
                    const QString &condition = AND) const;

        /*! Add a "or where date" statement to the query. */
        const Relation<Model, Related> &
        orWhereDate(const Column &column, const QString &comparison,
                    QVariant value) const;
        /*! Add a "or where time" statement to the query. */
        const Relation<Model, Related> &
        orWhereTime(const Column &column, const QString &comparison,
                    QVariant value) const;
        /*! Add a "or where day" statement to the query. */
        const Relation<Model, Related> &
        orWhereDay(const Column &column, const QString &comparison,
                   QVariant value) const;
        /*! Add a "or where month" statement to the query. */
        const Relation<Model, Related> &
        orWhereMonth(const Column &column, const QString &comparison,
                     QVariant value) const;
        /*! Add a "or where year" statement to the query. */
        const Relation<Model, Related> &
        orWhereYear(const Column &column, const QString &comparison,
                    QVariant value) const;

        /* where raw */
        /*! Add a raw "where" clause to the query. */
        const Relation<Model, Related> &whereRaw(
                const QString &sql, const QList<QVariant> &bindings = {},
                const QString &condition = AND) const;
        /*! Add a raw "or where" clause to the query. */
        const Relation<Model, Related> &orWhereRaw(
                const QString &sql, const QList<QVariant> &bindings = {}) const;

        /* Group by and having */
        /*! Add a "group by" clause to the query. */
        const Relation<Model, Related> &groupBy(const QList<Column> &groups) const;
        /*! Add a "group by" clause to the query. */
        const Relation<Model, Related> &groupBy(const Column &group) const;
        /*! Add a "group by" clause to the query. */
        template<ColumnConcept ...Args>
        const Relation<Model, Related> &groupBy(Args &&...groups) const;

        /*! Add a raw "groupBy" clause to the query. */
        const Relation<Model, Related> &groupByRaw(
                const QString &sql, const QList<QVariant> &bindings = {}) const;

        /*! Add a "having" clause to the query. */
        const Relation<Model, Related> &having(
                const Column &column, const QString &comparison, const QVariant &value,
                const QString &condition = AND) const;
        /*! Add an "or having" clause to the query. */
        const Relation<Model, Related> &orHaving(
                const Column &column, const QString &comparison,
                const QVariant &value) const;

        /*! Add a raw "having" clause to the query. */
        const Relation<Model, Related> &havingRaw(
                const QString &sql, const QList<QVariant> &bindings = {},
                const QString &condition = AND) const;
        /*! Add a raw "or having" clause to the query. */
        const Relation<Model, Related> &orHavingRaw(
                const QString &sql, const QList<QVariant> &bindings = {}) const;

        /* Ordering */
        /*! Add an "order by" clause to the query. */
        const Relation<Model, Related> &orderBy(
                const Column &column, const QString &direction = ASC) const;
        /*! Add a descending "order by" clause to the query. */
        const Relation<Model, Related> &orderByDesc(const Column &column) const;

        /*! Add an "order by" clause to the query with a subquery ordering. */
        template<Queryable T>
        const Relation<Model, Related> &orderBy(
                T &&query, const QString &direction = ASC) const;
        /*! Add a descending "order by" clause to the query with a subquery ordering. */
        template<Queryable T>
        const Relation<Model, Related> &orderByDesc(T &&query) const;

        /*! Put the query's results in random order. */
        const Relation<Model, Related> &inRandomOrder(const QString &seed = "") const;
        /*! Add a raw "order by" clause to the query. */
        const Relation<Model, Related> &orderByRaw(
                const QString &sql, const QList<QVariant> &bindings = {}) const;

        /*! Add an "order by" clause for a timestamp to the query. */
        const Relation<Model, Related> &latest(const Column &column = "") const;
        /*! Add an "order by" clause for a timestamp to the query. */
        const Relation<Model, Related> &oldest(const Column &column = "") const;
        /*! Remove all existing orders. */
        const Relation<Model, Related> &reorder() const;
        /*! Remove all existing orders and optionally add a new order. */
        const Relation<Model, Related> &reorder(
                const Column &column, const QString &direction = ASC) const;

        /*! Set the "limit" value of the query. */
        const Relation<Model, Related> &limit(qint64 value) const;
        /*! Alias to set the "limit" value of the query. */
        const Relation<Model, Related> &take(qint64 value) const;
        /*! Set the "offset" value of the query. */
        const Relation<Model, Related> &offset(qint64 value) const;
        /*! Alias to set the "offset" value of the query. */
        const Relation<Model, Related> &skip(qint64 value) const;
        /*! Set the limit and offset for a given page. */
        const Relation<Model, Related> &forPage(qint64 page, qint64 perPage = 30) const;

        /*! Constrain the query to the previous "page" of results before a given ID. */
        const Relation<Model, Related> &
        forPageBeforeId(qint64 perPage = 30, const QVariant &lastId = {},
                        const QString &column = Orm::Constants::ID,
                        bool prependOrder = false) const;
        /*! Constrain the query to the next "page" of results after a given ID. */
        const Relation<Model, Related> &
        forPageAfterId(qint64 perPage = 30, const QVariant &lastId = {},
                       const QString &column = Orm::Constants::ID,
                       bool prependOrder = false) const;

        /* Others */
        /*! Increment a column's value by a given amount. */
        template<typename T = std::size_t> requires std::is_arithmetic_v<T>
        std::tuple<int, TSqlQuery>
        increment(const QString &column, T amount = 1,
                  const QList<UpdateItem> &extra = {}) const;
        /*! Decrement a column's value by a given amount. */
        template<typename T = std::size_t> requires std::is_arithmetic_v<T>
        std::tuple<int, TSqlQuery>
        decrement(const QString &column, T amount = 1,
                  const QList<UpdateItem> &extra = {}) const;

        /* Pessimistic Locking */
        /*! Lock the selected rows in the table for updating. */
        const Relation<Model, Related> &lockForUpdate() const;
        /*! Share lock the selected rows in the table. */
        const Relation<Model, Related> &sharedLock() const;
        /*! Lock the selected rows in the table. */
        const Relation<Model, Related> &lock(bool value = true) const;
        /*! Lock the selected rows in the table. */
        const Relation<Model, Related> &lock(const char *value) const;
        /*! Lock the selected rows in the table. */
        const Relation<Model, Related> &lock(const QString &value) const;
        /*! Lock the selected rows in the table. */
        const Relation<Model, Related> &lock(QString &&value) const;

        /* Debugging */
        /*! Dump the current SQL and bindings. */
        void dump(bool replaceBindings = true, bool simpleBindings = false) const;
        /*! Die and dump the current SQL and bindings. */
        void dd(bool replaceBindings = true, bool simpleBindings = false) const;

        /* Builds Queries */
        /*! Chunk the results of the query. */
        virtual bool
        chunk(qint64 count,
              const std::function<
                  bool(ModelsCollection<Related> &&models, qint64 page)> &callback) const;
        /*! Execute a callback over each item while chunking. */
        virtual bool
        each(const std::function<bool(Related &&model, qint64 index)> &callback, // NOLINT(google-default-arguments)
             qint64 count = 1000) const;

        /*! Run a map over each item while chunking. */
        virtual ModelsCollection<Related>
        chunkMap(const std::function<Related(Related &&model)> &callback, // NOLINT(google-default-arguments)
                 qint64 count = 1000) const;
        /*! Run a map over each item while chunking. */
        template<typename T>
        QList<T>
        chunkMap(const std::function<T(Related &&model)> &callback,
                 qint64 count = 1000) const;

        /*! Chunk the results of a query by comparing IDs. */
        virtual bool
        chunkById(qint64 count, // NOLINT(google-default-arguments)
                  const std::function<
                      bool(ModelsCollection<Related> &&models, qint64 page)> &callback,
                  const QString &column = "", const QString &alias = "") const;
        /*! Execute a callback over each item while chunking by ID. */
        virtual bool
        eachById(const std::function<bool(Related &&model, qint64 index)> &callback, // NOLINT(google-default-arguments)
                 qint64 count = 1000, const QString &column = "",
                 const QString &alias = "") const;

        /*! Execute the query and get the first result if it's the sole matching
            record. */
        Related sole(const QList<Column> &columns = {ASTERISK}) const;

        /*! Pass the query to a given callback. */
        Builder<Related> &
        tap(const std::function<void(Builder<Related> &query)> &callback) const;

        /* Querying Relationship Existence/Absence */
        /*! Add a relationship count / exists condition to the query. */
        template<typename HasRelated = void>
        const Relation<Model, Related> &
        has(const QString &relation, const QString &comparison = GE, qint64 count = 1,
            const QString &condition = AND,
            const std::function<void(
                CallbackType<HasRelated> &)> &callback = nullptr) const;

        /*! Add a relationship count / exists condition to the query with an "or". */
        template<typename HasRelated = void>
        const Relation<Model, Related> &
        orHas(const QString &relation, const QString &comparison = GE,
              qint64 count = 1) const;
        /*! Add a relationship count / exists condition to the query. */
        template<typename HasRelated = void>
        const Relation<Model, Related> &
        doesntHave(const QString &relation, const QString &condition = AND,
                   const std::function<void(
                       CallbackType<HasRelated> &)> &callback = nullptr) const;
        /*! Add a relationship count / exists condition to the query with an "or". */
        template<typename HasRelated = void>
        const Relation<Model, Related> &
        orDoesntHave(const QString &relation) const;

        /*! Add a relationship count / exists condition to the query. */
        template<typename HasRelated>
        const Relation<Model, Related> &
        has(std::unique_ptr<Relation<Related, HasRelated>> &&relation,
            const QString &comparison = GE, qint64 count = 1,
            const QString &condition = AND,
            const std::function<void(QueryBuilder &)> &callback = nullptr) const;
        /*! Add a relationship count / exists condition to the query, prefer this over
            above overload, void type to avoid ambiguity. */
        template<typename HasRelated, typename = void>
        const Relation<Model, Related> &
        has(std::unique_ptr<Relation<Related, HasRelated>> &&relation,
            const QString &comparison = GE, qint64 count = 1,
            const QString &condition = AND,
            const std::function<void(
                TinyBuilder<HasRelated> &)> &callback = nullptr) const;

        /*! Add a relationship count / exists condition to the query. */
#if defined(__clang__) && __clang_major__ < 16
        template<typename HasRelated, typename Method,
                 std::enable_if_t<std::is_member_function_pointer_v<Method>, bool> = true>
#else
        template<typename HasRelated, typename Method>
        requires std::is_member_function_pointer_v<Method>
#endif
        const Relation<Model, Related> &
        has(Method relation, const QString &comparison = GE, qint64 count = 1,
            const QString &condition = AND,
            const std::function<void(
                TinyBuilder<HasRelated> &)> &callback = nullptr) const;

        /*! Add a relationship count / exists condition to the query with where
            clauses. */
        template<typename HasRelated = void>
        const Relation<Model, Related> &
        whereHas(const QString &relation,
                 const std::function<void(
                     CallbackType<HasRelated> &)> &callback = nullptr,
                 const QString &comparison = GE, qint64 count = 1) const;

        /*! Add a relationship count / exists condition to the query with where
            clauses and an "or". */
        template<typename HasRelated = void>
        const Relation<Model, Related> &
        orWhereHas(const QString &relation,
                   const std::function<void(
                       CallbackType<HasRelated> &)> &callback = nullptr,
                   const QString &comparison = GE, qint64 count = 1) const;
        /*! Add a relationship count / exists condition to the query with where
            clauses. */
        template<typename HasRelated = void>
        const Relation<Model, Related> &
        whereDoesntHave(const QString &relation,
                        const std::function<void(
                            CallbackType<HasRelated> &)> &callback = nullptr) const;
        /*! Add a relationship count / exists condition to the query with where
            clauses and an "or". */
        template<typename HasRelated = void>
        const Relation<Model, Related> &
        orWhereDoesntHave(const QString &relation,
                          const std::function<void(
                              CallbackType<HasRelated> &)> &callback = nullptr) const;

        /*! Add a relationship count / exists condition to the query with where
            clauses. */
#if defined(__clang__) && __clang_major__ < 16
        template<typename HasRelated, typename Method,
                 std::enable_if_t<std::is_member_function_pointer_v<Method>, bool> = true>
#else
        template<typename HasRelated, typename Method>
        requires std::is_member_function_pointer_v<Method>
#endif
        const Relation<Model, Related> &
        whereHas(Method relation,
                 const std::function<void(
                     TinyBuilder<HasRelated> &)> &callback = nullptr,
                 const QString &comparison = GE, qint64 count = 1) const;

        /* Soft Deleting */
        /*! Constraint the TinyBuilder query to exclude trashed models
            (where deleted_at IS NULL). */
        const Relation<Model, Related> &withoutTrashed() const;
        /*! Constraint the TinyBuilder query to include trashed models
            (no where added). */
        const Relation<Model, Related> &withTrashed(bool withTrashed = true) const;
        /*! Constraint the TinyBuilder query to select only trashed models
            (where deleted_at IS NOT NULL). */
        const Relation<Model, Related> &onlyTrashed() const;

        /*! Restore all trashed models (calls update on deleted_at column). */
        std::tuple<int, TSqlQuery> restore() const;
        /*! Run the default delete function on the builder (sidestep soft deleting). */
        std::tuple<int, TSqlQuery> forceDelete() const;
        /*! Run the default delete function on the builder (sidestep soft deleting),
            alias. */
        std::tuple<int, TSqlQuery> forceRemove() const;

        /* Casting Attributes */
        /*! Apply query-time casts to the model instance. */
        const Relation<Model, Related> &
        withCasts(const std::unordered_map<QString, CastItem> &casts) const;
        /*! Apply query-time casts to the model instance. */
        const Relation<Model, Related> &
        withCasts(std::unordered_map<QString, CastItem> &casts) const;
        /*! Apply query-time casts to the model instance. */
        const Relation<Model, Related> &
        withCasts(std::unordered_map<QString, CastItem> &&casts) const;
        /*! Apply query-time cast to the model instance. */
        const Relation<Model, Related> &
        withCast(std::pair<QString, CastItem> cast) const;

    private:
        /*! Static cast this to a child's instance Relation type. */
        inline const Relation<Model, Related> &relation() const noexcept;
        /*! Static cast this to a child's instance Relation type, const version. */
        inline Relation<Model, Related> &relation() noexcept;
        /*! Get the underlying query for the relation. */
        inline TinyBuilder<Related> &getQuery() const noexcept;
        /*! Get the base QueryBuilder driving the TinyBuilder. */
        inline QueryBuilder &getBaseQuery() const noexcept;
    };

    /* public */

    template<class Model, class Related>
    RelationProxies<Model, Related>::~RelationProxies() = default;

    /* Retrieving results */

    template<class Model, class Related>
    QVariant
    RelationProxies<Model, Related>::value(const Column &column) const
    {
        return getQuery().value(column);
    }

    template<class Model, class Related>
    QVariant
    RelationProxies<Model, Related>::soleValue(const Column &column) const
    {
        return getQuery().soleValue(column);
    }

    template<class Model, class Related>
    QList<QVariant>
    RelationProxies<Model, Related>::pluck(const Column &column) const
    {
        return getQuery().pluck(column);
    }

    template<class Model, class Related>
    template<typename T>
    std::map<T, QVariant>
    RelationProxies<Model, Related>::pluck(const Column &column,
                                           const Column &key) const
    {
        return getQuery().template pluck<T>(column, key);
    }

    template<class Model, class Related>
    QString
    RelationProxies<Model, Related>::implode(const QString &column,
                                             const QString &glue) const
    {
        return getQuery().implode(column, glue);
    }

    template<class Model, class Related>
    std::optional<Related>
    RelationProxies<Model, Related>::find(const QVariant &id, // NOLINT(google-default-arguments)
                                          const QList<Column> &columns) const
    {
        return getQuery().find(id, columns);
    }

    template<class Model, class Related>
    Related
    RelationProxies<Model, Related>::findOrNew(const QVariant &id, // NOLINT(google-default-arguments)
                                               const QList<Column> &columns) const
    {
        return getQuery().findOrNew(id, columns);
    }

    template<class Model, class Related>
    Related
    RelationProxies<Model, Related>::findOrFail(const QVariant &id, // NOLINT(google-default-arguments)
                                                const QList<Column> &columns) const
    {
        return getQuery().findOrFail(id, columns);
    }

    template<class Model, class Related>
    std::optional<Related>
    RelationProxies<Model, Related>::findOr(
            const QVariant &id, const QList<Column> &columns,
            const std::function<void()> &callback) const
    {
        return getQuery().findOr(id, columns, callback);
    }

    template<class Model, class Related>
    std::optional<Related>
    RelationProxies<Model, Related>::findOr(
            const QVariant &id, const std::function<void()> &callback) const
    {
        return getQuery().findOr(id, {ASTERISK}, callback);
    }

    template<class Model, class Related>
    template<typename R>
    std::pair<std::optional<Related>, R>
    RelationProxies<Model, Related>::findOr(
            const QVariant &id, const QList<Column> &columns,
            const std::function<R()> &callback) const
    {
        return getQuery().template findOr<R>(id, columns, callback);
    }

    template<class Model, class Related>
    template<typename R>
    std::pair<std::optional<Related>, R>
    RelationProxies<Model, Related>::findOr(
            const QVariant &id, const std::function<R()> &callback) const
    {
        return getQuery().template findOr<R>(id, {ASTERISK}, callback);
    }

    template<class Model, class Related>
    std::optional<Related>
    RelationProxies<Model, Related>::first(const QList<Column> &columns) const // NOLINT(google-default-arguments)
    {
        return getQuery().first(columns);
    }

    template<class Model, class Related>
    Related
    RelationProxies<Model, Related>::firstOrNew( // NOLINT(google-default-arguments)
            const QList<WhereItem> &attributes,
            const QList<AttributeItem> &values) const
    {
        return getQuery().firstOrNew(attributes, values);
    }

    template<class Model, class Related>
    Related
    RelationProxies<Model, Related>::firstOrCreate(
            const QList<WhereItem> &attributes,
            const QList<AttributeItem> &values) const
    {
        return getQuery().firstOrCreate(attributes, values);
    }

    template<class Model, class Related>
    Related RelationProxies<Model, Related>::firstOrFail( // NOLINT(google-default-arguments)
            const QList<Column> &columns) const
    {
        return getQuery().firstOrFail(columns);
    }

    template<class Model, class Related>
    std::optional<Related>
    RelationProxies<Model, Related>::firstOr( // NOLINT(google-default-arguments)
            const QList<Column> &columns, const std::function<void()> &callback) const
    {
        return getQuery().firstOr(columns, callback);
    }

    template<class Model, class Related>
    std::optional<Related>
    RelationProxies<Model, Related>::firstOr(const std::function<void()> &callback) const // NOLINT(google-default-arguments)
    {
        return getQuery().firstOr({ASTERISK}, callback);
    }

    template<class Model, class Related>
    template<typename R>
    std::pair<std::optional<Related>, R>
    RelationProxies<Model, Related>::firstOr(
            const QList<Column> &columns, const std::function<R()> &callback) const
    {
        return getQuery().template firstOr<R>(columns, callback);
    }

    template<class Model, class Related>
    template<typename R>
    std::pair<std::optional<Related>, R>
    RelationProxies<Model, Related>::firstOr(const std::function<R()> &callback) const
    {
        return getQuery().template firstOr<R>({ASTERISK}, callback);
    }

    template<class Model, class Related>
    std::optional<Related>
    RelationProxies<Model, Related>::firstWhere( // NOLINT(google-default-arguments)
            const Column &column, const QString &comparison,
            const QVariant &value, const QString &condition) const
    {
        return getQuery().firstWhere(column, comparison, value, condition);
    }

    template<class Model, class Related>
    std::optional<Related>
    RelationProxies<Model, Related>::firstWhereEq( // NOLINT(google-default-arguments)
            const Column &column, const QVariant &value, const QString &condition) const
    {
        return getQuery().firstWhereEq(column, value, condition);
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereKey(const QVariant &id) const
    {
        getQuery().whereKey(id);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereKey(const QList<QVariant> &ids) const
    {
        getQuery().whereKey(ids);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereKeyNot(const QVariant &id) const
    {
        getQuery().whereKeyNot(id);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereKeyNot(const QList<QVariant> &ids) const
    {
        getQuery().whereKeyNot(ids);

        return relation();
    }

    template<class Model, class Related>
    template<typename>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::with(const QList<WithItem> &relations) const
    {
        getQuery().with(relations);

        return relation();
    }

    template<class Model, class Related>
    template<typename>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::with(QString relation) const
    {
        getQuery().with(std::move(relation));

        return this->relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::with(const QList<QString> &relations) const
    {
        getQuery().with(relations);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::with(QList<QString> &&relations) const
    {
        getQuery().with(std::move(relations));

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::without(const QList<QString> &relations) const
    {
        getQuery().without(relations);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::without(QString relation) const
    {
        getQuery().without(std::move(relation));

        return this->relation();
    }

    template<class Model, class Related>
    template<typename>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::withOnly(const QList<WithItem> &relations) const
    {
        getQuery().withOnly(relations);

        return relation();
    }

    template<class Model, class Related>
    template<typename>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::withOnly(QString relation) const
    {
        getQuery().withOnly(std::move(relation));

        return this->relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::withOnly(const QList<QString> &relations) const
    {
        getQuery().withOnly(relations);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::withOnly(QList<QString> &&relations) const
    {
        getQuery().withOnly(std::move(relations));

        return relation();
    }

    /* Insert, Update, Delete */

    template<class Model, class Related>
    Related
    RelationProxies<Model, Related>::updateOrCreate(
            const QList<WhereItem> &attributes,
            const QList<AttributeItem> &values) const
    {
        return getQuery().updateOrCreate(attributes, values);
    }

    /* Proxies to TinyBuilder -> QueryBuilder */

    template<class Model, class Related>
    const FromClause &RelationProxies<Model, Related>::from() const
    {
        /* The getQuery() to apply the SoftDeletes where null condition for
           the deleted_at column (of course if the SoftDeletes is enabled). */
        return getQuery().from();
    }

    template<class Model, class Related>
    QString RelationProxies<Model, Related>::toSql() const
    {
        /* The getQuery() to apply the SoftDeletes where null condition for
           the deleted_at column (of course if the SoftDeletes is enabled). */
        return getQuery().toSql();
    }

    template<class Model, class Related>
    QList<QVariant>
    RelationProxies<Model, Related>::getBindings() const
    {
        return getBaseQuery().getBindings();
    }

    /* Insert, Update, Delete */

    template<class Model, class Related>
    std::optional<SqlQuery>
    RelationProxies<Model, Related>::insert(const QList<AttributeItem> &values) const
    {
        return getQuery().insert(values);
    }

    template<class Model, class Related>
    std::optional<SqlQuery>
    RelationProxies<Model, Related>::insert(
            const QList<QList<AttributeItem>> &values) const
    {
        return getQuery().insert(values);
    }

    template<class Model, class Related>
    std::optional<SqlQuery>
    RelationProxies<Model, Related>::insert(
            const QList<QString> &columns, QList<QList<QVariant>> values) const
    {
        return getQuery().insert(columns, std::move(values));
    }

    // FEATURE dilemma primarykey, Model::KeyType vs QVariant silverqx
    template<class Model, class Related>
    quint64
    RelationProxies<Model, Related>::insertGetId(
            const QList<AttributeItem> &attributes, const QString &sequence) const
    {
        return getQuery().insertGetId(attributes, sequence);
    }

    template<class Model, class Related>
    std::tuple<int, std::optional<TSqlQuery>>
    RelationProxies<Model, Related>::insertOrIgnore(
            const QList<AttributeItem> &values) const
    {
        return getQuery().insertOrIgnore(values);
    }

    template<class Model, class Related>
    std::tuple<int, std::optional<TSqlQuery>>
    RelationProxies<Model, Related>::insertOrIgnore(
            const QList<QList<AttributeItem>> &values) const
    {
        return getQuery().insertOrIgnore(values);
    }

    template<class Model, class Related>
    std::tuple<int, std::optional<TSqlQuery>>
    RelationProxies<Model, Related>::insertOrIgnore(
            const QList<QString> &columns, QList<QList<QVariant>> values) const
    {
        return getQuery().insertOrIgnore(columns, std::move(values));
    }

    template<class Model, class Related>
    std::tuple<int, TSqlQuery>
    RelationProxies<Model, Related>::update(const QList<UpdateItem> &values) const
    {
        return getQuery().update(values);
    }

    template<class Model, class Related>
    std::tuple<int, std::optional<TSqlQuery>>
    RelationProxies<Model, Related>::upsert(
            const QList<QVariantMap> &values, const QStringList &uniqueBy,
            const QStringList &update) const
    {
        return getQuery().upsert(values, uniqueBy, update);
    }

    template<class Model, class Related>
    std::tuple<int, std::optional<TSqlQuery>>
    RelationProxies<Model, Related>::upsert(
            const QList<QVariantMap> &values, const QStringList &uniqueBy) const
    {
        return getQuery().upsert(values, uniqueBy);
    }

    template<class Model, class Related>
    std::tuple<int, TSqlQuery>
    RelationProxies<Model, Related>::remove() const
    {
        return getQuery().remove();
    }

    template<class Model, class Related>
    std::tuple<int, TSqlQuery>
    RelationProxies<Model, Related>::deleteModels() const
    {
        return getQuery().deleteModels();
    }

    template<class Model, class Related>
    void RelationProxies<Model, Related>::truncate() const
    {
        getQuery().truncate();
    }

    /* Select */

    template<class Model, class Related>
    quint64 RelationProxies<Model, Related>::count(const QList<Column> &columns) const
    {
        return getQuery().count(columns);
    }

    template<class Model, class Related>
    template<typename>
    quint64 RelationProxies<Model, Related>::count(const Column &column) const
    {
        return getQuery().count(QList<Column> {column});
    }

    template<class Model, class Related>
    QVariant RelationProxies<Model, Related>::min(const Column &column) const
    {
        return getQuery().min(column);
    }

    template<class Model, class Related>
    QVariant RelationProxies<Model, Related>::max(const Column &column) const
    {
        return getQuery().max(column);
    }

    template<class Model, class Related>
    QVariant RelationProxies<Model, Related>::sum(const Column &column) const
    {
        return getQuery().sum(column);
    }

    template<class Model, class Related>
    QVariant RelationProxies<Model, Related>::avg(const Column &column) const
    {
        return getQuery().avg(column);
    }

    template<class Model, class Related>
    QVariant RelationProxies<Model, Related>::average(const Column &column) const
    {
        return getQuery().avg(column);
    }

    template<class Model, class Related>
    QVariant RelationProxies<Model, Related>::aggregate(
            const QString &function, const QList<Column> &columns) const
    {
        return getQuery().aggregate(function, columns);
    }

    template<class Model, class Related>
    bool RelationProxies<Model, Related>::exists() const
    {
        return getQuery().exists();
    }

    template<class Model, class Related>
    bool RelationProxies<Model, Related>::doesntExist() const
    {
        return getQuery().doesntExist();
    }

    template<class Model, class Related>
    bool RelationProxies<Model, Related>::existsOr(
            const std::function<void()> &callback) const
    {
        return getQuery().existsOr(callback);
    }

    template<class Model, class Related>
    bool RelationProxies<Model, Related>::doesntExistOr(
            const std::function<void()> &callback) const
    {
        return getQuery().doesntExistOr(callback);
    }

    template<class Model, class Related>
    template<typename R>
    std::pair<bool, R>
    RelationProxies<Model, Related>::existsOr(
            const std::function<R()> &callback) const
    {
        return getQuery().template existsOr<R>(callback);
    }

    template<class Model, class Related>
    template<typename R>
    std::pair<bool, R>
    RelationProxies<Model, Related>::doesntExistOr(
            const std::function<R()> &callback) const
    {
        return getQuery().template doesntExistOr<R>(callback);
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::select(const QList<Column> &columns) const
    {
        getQuery().select(columns);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::select(const Column &column) const
    {
        getQuery().select(column);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::addSelect(const QList<Column> &columns) const
    {
        getQuery().addSelect(columns);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::addSelect(const Column &column) const
    {
        getQuery().addSelect(column);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::select(QList<Column> &&columns) const
    {
        getQuery().select(std::move(columns));

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::select(Column &&column) const
    {
        getQuery().select(std::move(column));

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::addSelect(QList<Column> &&columns) const
    {
        getQuery().addSelect(std::move(columns));

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::addSelect(Column &&column) const
    {
        getQuery().addSelect(std::move(column));

        return relation();
    }

    template<class Model, class Related>
    template<Queryable T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::select(T &&query, const QString &as) const
    {
        getQuery().select(std::forward<T>(query), as);

        return relation();
    }

    template<class Model, class Related>
    template<Queryable T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::addSelect(T &&query, const QString &as) const
    {
        getQuery().addSelect(std::forward<T>(query), as);

        return relation();
    }

    template<class Model, class Related>
    template<SubQuery T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::selectSub(T &&query, const QString &as) const
    {
        getQuery().selectSub(std::forward<T>(query), as);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::selectRaw(
            const QString &expression, const QList<QVariant> &bindings) const
    {
        getQuery().selectRaw(expression, bindings);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::distinct() const
    {
        getQuery().distinct();

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::distinct(const QStringList &columns) const
    {
        getQuery().distinct(columns);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::distinct(QStringList &&columns) const
    {
        getQuery().distinct(std::move(columns));

        return relation();
    }

    /* Joins */

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::join(
            T &&table, const QString &first, const QString &comparison,
            const QString &second, const QString &type, const bool where) const
    {
        getQuery().join(std::forward<T>(table), first, comparison, second, type, where);

        return relation();
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::join(
            T &&table, const std::function<void(JoinClause &)> &callback,
            const QString &type) const
    {
        getQuery().join(std::forward<T>(table), callback, type);

        return relation();
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::joinWhere(
            T &&table, const QString &first, const QString &comparison,
            const QVariant &second, const QString &type) const
    {
        getQuery().joinWhere(std::forward<T>(table), first, comparison, second, type);

        return relation();
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::leftJoin(
            T &&table, const QString &first, const QString &comparison,
            const QString &second) const
    {
        getQuery().leftJoin(std::forward<T>(table), first, comparison, second);

        return relation();
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::leftJoin(
            T &&table, const std::function<void(JoinClause &)> &callback) const
    {
        getQuery().leftJoin(std::forward<T>(table), callback);

        return relation();
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::leftJoinWhere(
            T &&table, const QString &first, const QString &comparison,
            const QVariant &second) const
    {
        getQuery().leftJoinWhere(std::forward<T>(table), first, comparison, second);

        return relation();
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::rightJoin(
            T &&table, const QString &first, const QString &comparison,
            const QString &second) const
    {
        getQuery().rightJoin(std::forward<T>(table), first, comparison, second);

        return relation();
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::rightJoin(
            T &&table, const std::function<void(JoinClause &)> &callback) const
    {
        getQuery().rightJoin(std::forward<T>(table), callback);

        return relation();
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::rightJoinWhere(
            T &&table, const QString &first, const QString &comparison,
            const QVariant &second) const
    {
        getQuery().rightJoinWhere(std::forward<T>(table), first, comparison, second);

        return relation();
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::crossJoin(
            T &&table, const QString &first, const QString &comparison,
            const QString &second) const
    {
        getQuery().crossJoin(std::forward<T>(table), first, comparison, second);

        return relation();
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::crossJoin(
            T &&table, const std::function<void(JoinClause &)> &callback) const
    {
        getQuery().crossJoin(std::forward<T>(table), callback);

        return relation();
    }

    template<class Model, class Related>
    template<JoinTable T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::crossJoin(T &&table) const
    {
        getQuery().crossJoin(std::forward<T>(table));

        return relation();
    }

    template<class Model, class Related>
    template<SubQuery T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::joinSub(
            T &&query, const QString &as, const QString &first,
            const QString &comparison, const QVariant &second,
            const QString &type, const bool where) const
    {
        getQuery().joinSub(std::forward<T>(query), as, first, comparison, second, type,
                           where);

        return relation();
    }

    template<class Model, class Related>
    template<SubQuery T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::joinSub(
            T &&query, const QString &as,
            const std::function<void(JoinClause &)> &callback,
            const QString &type) const
    {
        getQuery().joinSub(std::forward<T>(query), as, callback, type);

        return relation();
    }

    template<class Model, class Related>
    template<SubQuery T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::leftJoinSub(
            T &&query, const QString &as, const QString &first,
            const QString &comparison, const QVariant &second) const
    {
        getQuery().leftJoinSub(std::forward<T>(query), as, first, comparison, second);

        return relation();
    }

    template<class Model, class Related>
    template<SubQuery T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::leftJoinSub(
            T &&query, const QString &as,
            const std::function<void(JoinClause &)> &callback) const
    {
        getQuery().joinSub(std::forward<T>(query), as, callback, LEFT);

        return relation();
    }

    template<class Model, class Related>
    template<SubQuery T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::rightJoinSub(
            T &&query, const QString &as, const QString &first,
            const QString &comparison, const QVariant &second) const
    {
        getQuery().rightJoinSub(std::forward<T>(query), as, first, comparison, second);

        return relation();
    }

    template<class Model, class Related>
    template<SubQuery T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::rightJoinSub(
            T &&query, const QString &as,
            const std::function<void(JoinClause &)> &callback) const
    {
        getQuery().joinSub(std::forward<T>(query), as, callback, RIGHT);

        return relation();
    }

    /* General where */

    template<class Model, class Related>
    template<WhereValue T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::where(
            const Column &column, const QString &comparison, T &&value,
            const QString &condition) const
    {
        getQuery().where(column, comparison, std::forward<T>(value), condition);

        return relation();
    }

    template<class Model, class Related>
    template<WhereValue T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhere(
            const Column &column, const QString &comparison, T &&value) const
    {
        getQuery().orWhere(column, comparison, std::forward<T>(value));

        return relation();
    }

    template<class Model, class Related>
    template<WhereValue T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereEq(
            const Column &column, T &&value, const QString &condition) const
    {
        getQuery().whereEq(column, std::forward<T>(value), condition);

        return relation();
    }

    template<class Model, class Related>
    template<WhereValue T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereEq(const Column &column, T &&value) const
    {
        getQuery().orWhereEq(column, std::forward<T>(value));

        return relation();
    }

    /* General where not */

    template<class Model, class Related>
    template<WhereValue T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereNot(
            const Column &column, const QString &comparison, T &&value,
            const QString &condition) const
    {
        getQuery().whereNot(column, comparison, std::forward<T>(value), condition);

        return relation();
    }

    template<class Model, class Related>
    template<WhereValue T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereNot(
            const Column &column, const QString &comparison, T &&value) const
    {
        getQuery().orWhereNot(column, comparison, std::forward<T>(value));

        return relation();
    }

    template<class Model, class Related>
    template<WhereValue T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereNotEq(
            const Column &column, T &&value, const QString &condition) const
    {
        getQuery().whereNotEq(column, std::forward<T>(value), condition);

        return relation();
    }

    template<class Model, class Related>
    template<WhereValue T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereNotEq(const Column &column, T &&value) const
    {
        getQuery().orWhereNotEq(column, std::forward<T>(value));

        return relation();
    }

    /* Nested where */

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::where(
            const std::function<void(Builder<Related> &)> &callback,
            const QString &condition) const
    {
        getQuery().where(callback, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhere(
            const std::function<void(Builder<Related> &)> &callback) const
    {
        getQuery().orWhere(callback);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereNot(
            const std::function<void(Builder<Related> &)> &callback,
            const QString &condition) const
    {
        getQuery().whereNot(callback, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereNot(
            const std::function<void(Builder<Related> &)> &callback) const
    {
        getQuery().orWhereNot(callback);

        return relation();
    }

    /* Array where */

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::where(
            const QList<WhereItem> &values, const QString &condition) const
    {
        getQuery().where(values, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhere(const QList<WhereItem> &values) const
    {
        getQuery().orWhere(values);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereNot(
            const QList<WhereItem> &values, const QString &condition,
            const QString &defaultCondition) const
    {
        getQuery().whereNot(values, condition, defaultCondition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereNot(
            const QList<WhereItem> &values, const QString &defaultCondition) const
    {
        getQuery().orWhereNot(values, defaultCondition);

        return relation();
    }

    /* where column */

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereColumn(
            const QList<WhereColumnItem> &values, const QString &condition) const
    {
        getQuery().whereColumn(values, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereColumn(
            const QList<WhereColumnItem> &values) const
    {
        getQuery().orWhereColumn(values);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereColumn(
            const Column &first, const QString &comparison,
            const Column &second, const QString &condition) const
    {
        getQuery().whereColumn(first, comparison, second, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereColumn(
            const Column &first, const QString &comparison, const Column &second) const
    {
        getQuery().orWhereColumn(first, comparison, second);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereColumnEq(
            const Column &first, const Column &second, const QString &condition) const
    {
        getQuery().whereColumnEq(first, second, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereColumnEq(const Column &first,
                                                     const Column &second) const
    {
        getQuery().orWhereColumnEq(first, second);

        return relation();
    }

    /* where IN */

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereIn(
            const Column &column, const QList<QVariant> &values,
            const QString &condition, const bool nope) const
    {
        getQuery().whereIn(column, values, condition, nope);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereIn(const Column &column,
                                               const QList<QVariant> &values) const
    {
        getQuery().orWhereIn(column, values);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereNotIn(
            const Column &column, const QList<QVariant> &values,
            const QString &condition) const
    {
        getQuery().whereNotIn(column, values, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereNotIn(const Column &column,
                                                  const QList<QVariant> &values) const
    {
        getQuery().orWhereNotIn(column, values);

        return relation();
    }

    /* where null */

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereNull(
            const QList<Column> &columns, const QString &condition,
            const bool nope) const
    {
        getQuery().whereNull(columns, condition, nope);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereNull(const QList<Column> &columns) const
    {
        getQuery().orWhereNull(columns);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereNotNull(const QList<Column> &columns,
                                                  const QString &condition) const
    {
        getQuery().whereNotNull(columns, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereNotNull(const QList<Column> &columns) const
    {
        getQuery().orWhereNotNull(columns);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereNull(
            const Column &column, const QString &condition, const bool nope) const
    {
        getQuery().whereNull(column, condition, nope);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereNull(const Column &column) const
    {
        getQuery().orWhereNull(column);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereNotNull(const Column &column,
                                                  const QString &condition) const
    {
        getQuery().whereNotNull(column, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereNotNull(const Column &column) const
    {
        getQuery().orWhereNotNull(column);

        return relation();
    }

    /* where between */

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereBetween(
            const Column &column, const WhereBetweenItem &values,
            const QString &condition, const bool nope) const
    {
        getQuery().whereBetween(column, values, condition, nope);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereBetween(
            const Column &column, const WhereBetweenItem &values) const
    {
        getQuery().whereBetween(column, values, OR);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereNotBetween(
            const Column &column, const WhereBetweenItem &values,
            const QString &condition) const
    {
        getQuery().whereBetween(column, values, condition, true);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereNotBetween(
            const Column &column, const WhereBetweenItem &values) const
    {
        getQuery().whereBetween(column, values, OR, true);

        return relation();
    }

    /* where between columns */

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereBetweenColumns(
            const Column &column, const WhereBetweenColumnsItem &betweenColumns,
            const QString &condition, const bool nope) const
    {
        getQuery().whereBetweenColumns(column, betweenColumns, condition, nope);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereBetweenColumns(
            const Column &column, const WhereBetweenColumnsItem &betweenColumns) const
    {
        getQuery().whereBetweenColumns(column, betweenColumns, OR);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereNotBetweenColumns(
            const Column &column, const WhereBetweenColumnsItem &betweenColumns,
            const QString &condition) const
    {
        getQuery().whereBetweenColumns(column, betweenColumns, condition, true);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereNotBetweenColumns(
            const Column &column, const WhereBetweenColumnsItem &betweenColumns) const
    {
        getQuery().whereBetweenColumns(column, betweenColumns, OR, true);

        return relation();
    }

    /* where sub-queries */

    template<class Model, class Related>
    template<Queryable C, WhereValue V>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::where(
            C &&column, const QString &comparison, V &&value,
            const QString &condition) const
    {
        getQuery().where(std::forward<C>(column), comparison, std::forward<V>(value),
                         condition);

        return relation();
    }

    template<class Model, class Related>
    template<Queryable C, WhereValue V>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhere(
            C &&column, const QString &comparison, V &&value) const
    {
        getQuery().where(std::forward<C>(column), comparison, std::forward<V>(value),
                         OR);

        return relation();
    }

    template<class Model, class Related>
    template<Queryable C, WhereValue V>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereEq(
            C &&column, V &&value, const QString &condition) const
    {
        getQuery().where(std::forward<C>(column), EQ, std::forward<V>(value), condition);

        return relation();
    }

    template<class Model, class Related>
    template<Queryable C, WhereValue V>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereEq(C &&column, V &&value) const
    {
        getQuery().where(std::forward<C>(column), EQ, std::forward<V>(value), OR);

        return relation();
    }

    /* where not sub-queries */

    template<class Model, class Related>
    template<Queryable C, WhereValue V>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereNot(
            C &&column, const QString &comparison, V &&value,
            const QString &condition) const
    {
        getQuery().whereNot(std::forward<C>(column), comparison,
                            std::forward<V>(value), condition);

        return relation();
    }

    template<class Model, class Related>
    template<Queryable C, WhereValue V>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereNot(
            C &&column, const QString &comparison, V &&value) const
    {
        getQuery().orWhereNot(std::forward<C>(column), comparison,
                              std::forward<V>(value));

        return relation();
    }

    template<class Model, class Related>
    template<Queryable C, WhereValue V>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereNotEq(
            C &&column, V &&value, const QString &condition) const
    {
        getQuery().whereNotEq(std::forward<C>(column), std::forward<V>(value),
                              condition);

        return relation();
    }

    template<class Model, class Related>
    template<Queryable C, WhereValue V>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereNotEq(C &&column, V &&value) const
    {
        getQuery().orWhereNotEq(std::forward<C>(column), std::forward<V>(value));

        return relation();
    }

    template<class Model, class Related>
    template<WhereValueSubQuery T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereSub(
            const Column &column, const QString &comparison, T &&query,
            const QString &condition) const
    {
        getQuery().whereSub(column, comparison, std::forward<T>(query), condition);

        return relation();
    }

    /* where exists */

    template<class Model, class Related>
    template<QueryableShared C>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereExists(
            C &&callback, const QString &condition, const bool nope) const
    {
        getQuery().whereExists(std::forward<C>(callback), condition, nope);

        return relation();
    }

    template<class Model, class Related>
    template<QueryableShared C>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereExists(C &&callback, const bool nope) const
    {
        getQuery().whereExists(std::forward<C>(callback), OR, nope);

        return relation();
    }

    template<class Model, class Related>
    template<QueryableShared C>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereNotExists(
            C &&callback, const QString &condition) const
    {
        getQuery().whereExists(std::forward<C>(callback), condition, true);

        return relation();
    }

    template<class Model, class Related>
    template<QueryableShared C>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereNotExists(C &&callback) const
    {
        getQuery().whereExists(std::forward<C>(callback), OR, true);

        return relation();
    }

    /* where row values */

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereRowValues(
            const QList<Column> &columns, const QString &comparison,
            const QList<QVariant> &values, const QString &condition) const
    {
        getQuery().whereRowValues(columns, comparison, values, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereRowValues(
            const QList<Column> &columns, const QString &comparison,
            const QList<QVariant> &values) const
    {
        getQuery().whereRowValues(columns, comparison, values, OR);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereRowValuesEq(
            const QList<Column> &columns, const QList<QVariant> &values,
            const QString &condition) const
    {
        getQuery().whereRowValues(columns, EQ, values, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereRowValuesEq(
            const QList<Column> &columns, const QList<QVariant> &values) const
    {
        getQuery().whereRowValues(columns, EQ, values, OR);

        return relation();
    }

    /* where dates */

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereDate(
            const Column &column, const QString &comparison, QVariant value,
            const QString &condition) const
    {
        getQuery().whereDate(column, comparison, std::move(value), condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereTime(
            const Column &column, const QString &comparison, QVariant value,
            const QString &condition) const
    {
        getQuery().whereTime(column, comparison, std::move(value), condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereDay(
            const Column &column, const QString &comparison, QVariant value,
            const QString &condition) const
    {
        getQuery().whereDay(column, comparison, std::move(value), condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereMonth(
            const Column &column, const QString &comparison, QVariant value,
            const QString &condition) const
    {
        getQuery().whereMonth(column, comparison, std::move(value), condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereYear(
            const Column &column, const QString &comparison, QVariant value,
            const QString &condition) const
    {
        getQuery().whereYear(column, comparison, std::move(value), condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereEqDate(
            const Column &column, QVariant value, const QString &condition) const
    {
        getQuery().whereDate(column, EQ, std::move(value), condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereEqTime(
            const Column &column, QVariant value, const QString &condition) const
    {
        getQuery().whereTime(column, EQ, std::move(value), condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereEqDay(
            const Column &column, QVariant value, const QString &condition) const
    {
        getQuery().whereDay(column, EQ, std::move(value), condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereEqMonth(
            const Column &column, QVariant value, const QString &condition) const
    {
        getQuery().whereMonth(column, EQ, std::move(value), condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereEqYear(
            const Column &column, QVariant value, const QString &condition) const
    {
        getQuery().whereYear(column, EQ, std::move(value), condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereDate(
            const Column &column, const QString &comparison, QVariant value) const
    {
        getQuery().whereDate(column, comparison, std::move(value), OR);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereTime(
            const Column &column, const QString &comparison, QVariant value) const
    {
        getQuery().whereTime(column, comparison, std::move(value), OR);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereDay(
            const Column &column, const QString &comparison, QVariant value) const
    {
        getQuery().whereDay(column, comparison, std::move(value), OR);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereMonth(
            const Column &column, const QString &comparison, QVariant value) const
    {
        getQuery().whereMonth(column, comparison, std::move(value), OR);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereYear(
            const Column &column, const QString &comparison, QVariant value) const
    {
        getQuery().whereYear(column, comparison, std::move(value), OR);

        return relation();
    }

    /* where raw */

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereRaw(
            const QString &sql, const QList<QVariant> &bindings,
            const QString &condition) const
    {
        getQuery().whereRaw(sql, bindings, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereRaw(
            const QString &sql, const QList<QVariant> &bindings) const
    {
        getQuery().whereRaw(sql, bindings, OR);

        return relation();
    }

    /* Group by and having */

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::groupBy(const QList<Column> &groups) const
    {
        getQuery().groupBy(groups);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::groupBy(const Column &group) const
    {
        getQuery().groupBy(group);

        return relation();
    }

    template<class Model, class Related>
    template<ColumnConcept ...Args>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::groupBy(Args &&...groups) const
    {
        getQuery().groupBy(QList<Column> {std::forward<Args>(groups)...});

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::groupByRaw(const QString &sql,
                                                const QList<QVariant> &bindings) const
    {
        getQuery().groupByRaw(sql, bindings);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::having(
            const Column &column, const QString &comparison,
            const QVariant &value, const QString &condition) const
    {
        getQuery().having(column, comparison, value, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orHaving(
            const Column &column, const QString &comparison, const QVariant &value) const
    {
        getQuery().orHaving(column, comparison, value);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::havingRaw(
            const QString &sql,  const QList<QVariant> &bindings,
            const QString &condition) const
    {
        getQuery().havingRaw(sql, bindings, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orHavingRaw(
            const QString &sql,  const QList<QVariant> &bindings) const
    {
        getQuery().havingRaw(sql, bindings, OR);

        return relation();
    }

    /* Ordering */

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orderBy(const Column &column,
                                             const QString &direction) const
    {
        getQuery().orderBy(column, direction);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orderByDesc(const Column &column) const
    {
        getQuery().orderByDesc(column);

        return relation();
    }

    template<class Model, class Related>
    template<Queryable T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orderBy(T &&query, const QString &direction) const
    {
        getQuery().orderBy(std::forward<T>(query), direction);

        return relation();
    }

    template<class Model, class Related>
    template<Queryable T>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orderByDesc(T &&query) const
    {
        getQuery().orderBy(std::forward<T>(query), DESC);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::inRandomOrder(const QString &seed) const
    {
        getQuery().inRandomOrder(seed);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orderByRaw(const QString &sql,
                                                const QList<QVariant> &bindings) const
    {
        getQuery().orderByRaw(sql, bindings);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::latest(const Column &column) const
    {
        getQuery().latest(column);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::oldest(const Column &column) const
    {
        getQuery().oldest(column);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::reorder() const
    {
        getQuery().reorder();

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::reorder(const Column &column,
                                             const QString &direction) const
    {
        getQuery().reorder(column, direction);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::limit(const qint64 value) const
    {
        getQuery().limit(value);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::take(const qint64 value) const
    {
        getQuery().take(value);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::offset(const qint64 value) const
    {
        getQuery().offset(value);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::skip(const qint64 value) const
    {
        getQuery().skip(value);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::forPage(const qint64 page,
                                             const qint64 perPage) const
    {
        getQuery().forPage(page, perPage);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::forPageBeforeId(
            const qint64 perPage, const QVariant &lastId, const QString &column,
            const bool prependOrder) const
    {
        getQuery().forPageBeforeId(perPage, lastId, column, prependOrder);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::forPageAfterId(
            const qint64 perPage, const QVariant &lastId, const QString &column,
            const bool prependOrder) const
    {
        getQuery().forPageAfterId(perPage, lastId, column, prependOrder);

        return relation();
    }

    /* Others */

    template<class Model, class Related>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, TSqlQuery>
    RelationProxies<Model, Related>::increment(const QString &column, const T amount,
                                               const QList<UpdateItem> &extra) const
    {
        return getQuery().increment(column, amount, extra);
    }

    template<class Model, class Related>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, TSqlQuery>
    RelationProxies<Model, Related>::decrement(const QString &column, const T amount,
                                               const QList<UpdateItem> &extra) const
    {
        return getQuery().decrement(column, amount, extra);
    }

    /* Pessimistic Locking */

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::lockForUpdate() const
    {
        getQuery().lock(true);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::sharedLock() const
    {
        getQuery().lock(false);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::lock(const bool value) const
    {
        getQuery().lock(value);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::lock(const char *value) const
    {
        getQuery().lock(value);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::lock(const QString &value) const
    {
        getQuery().lock(value);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::lock(QString &&value) const
    {
        getQuery().lock(std::move(value));

        return relation();
    }

    /* Debugging */

    template<class Model, class Related>
    void RelationProxies<Model, Related>::dump(const bool replaceBindings,
                                               const bool simpleBindings) const
    {
        getQuery().dump(replaceBindings, simpleBindings);
    }

    template<class Model, class Related>
    void RelationProxies<Model, Related>::dd(const bool replaceBindings,
                                             const bool simpleBindings) const
    {
        getQuery().dd(replaceBindings, simpleBindings);
    }

    /* Builds Queries */

    template<class Model, class Related>
    bool RelationProxies<Model, Related>::chunk(
            const qint64 count,
            const std::function<
                bool(ModelsCollection<Related> &&, qint64)> &callback) const
    {
        return getQuery().chunk(count, callback);
    }

    template<class Model, class Related>
    bool RelationProxies<Model, Related>::each( // NOLINT(google-default-arguments)
            const std::function<bool(Related &&, qint64)> &callback,
            const qint64 count) const
    {
        return getQuery().each(callback, count);
    }

    template<class Model, class Related>
    ModelsCollection<Related>
    RelationProxies<Model, Related>::chunkMap( // NOLINT(google-default-arguments)
            const std::function<Related(Related &&)> &callback, const qint64 count) const
    {
        return getQuery().chunkMap(callback, count);
    }

    template<class Model, class Related>
    template<typename T>
    QList<T>
    RelationProxies<Model, Related>::chunkMap(
            const std::function<T(Related &&)> &callback, const qint64 count) const
    {
        return getQuery().template chunkMap<T>(callback, count);
    }

    template<class Model, class Related>
    bool RelationProxies<Model, Related>::chunkById( // NOLINT(google-default-arguments)
            const qint64 count,
            const std::function<bool(ModelsCollection<Related> &&, qint64)> &callback,
            const QString &column, const QString &alias) const
    {
        return getQuery().chunkById(count, callback, column, alias);
    }

    template<class Model, class Related>
    bool RelationProxies<Model, Related>::eachById( // NOLINT(google-default-arguments)
            const std::function<bool(Related &&, qint64)> &callback,
            const qint64 count, const QString &column, const QString &alias) const
    {
        return getQuery().eachById(callback, count, column, alias);
    }

    template<class Model, class Related>
    Related RelationProxies<Model, Related>::sole(const QList<Column> &columns) const
    {
        return getQuery().sole(columns);
    }

    template<class Model, class Related>
    Builder<Related> &
    RelationProxies<Model, Related>::tap(
            const std::function<void(Builder<Related> &)> &callback) const
    {
        return getQuery().tap(callback);
    }

    /* Querying Relationship Existence/Absence */

    template<class Model, class Related>
    template<typename HasRelated>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::has(
            const QString &relation, const QString &comparison, const qint64 count,
            const QString &condition,
            const std::function<void(CallbackType<HasRelated> &)> &callback) const
    {
        getQuery().template has<HasRelated>(relation, comparison, count, condition,
                                            callback);

        return this->relation();
    }

    template<class Model, class Related>
    template<typename HasRelated>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orHas(
            const QString &relation, const QString &comparison, const qint64 count) const
    {
        getQuery().template orHas<HasRelated>(relation, comparison, count);

        return this->relation();
    }

    template<class Model, class Related>
    template<typename HasRelated>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::doesntHave(
            const QString &relation, const QString &condition,
            const std::function<void(CallbackType<HasRelated> &)> &callback) const
    {
        getQuery().template doesntHave<HasRelated>(relation, condition, callback);

        return this->relation();
    }

    template<class Model, class Related>
    template<typename HasRelated>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orDoesntHave(const QString &relation) const
    {
        getQuery().template orDoesntHave<HasRelated>(relation);

        return this->relation();
    }

    template<class Model, class Related>
    template<typename HasRelated>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::has(
            std::unique_ptr<Relation<Related, HasRelated>> &&relation,
            const QString &comparison, const qint64 count, const QString &condition,
            const std::function<void(QueryBuilder &)> &callback) const
    {
        getQuery().template has<HasRelated>(std::move(relation), comparison, count,
                                            condition, callback);

        return this->relation();
    }

    template<class Model, class Related>
    template<typename HasRelated, typename>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::has(
            std::unique_ptr<Relation<Related, HasRelated>> &&relation,
            const QString &comparison, const qint64 count, const QString &condition,
            const std::function<void(TinyBuilder<HasRelated> &)> &callback) const
    {
        getQuery().template has<HasRelated, void>(std::move(relation), comparison, count,
                                                  condition, callback);

        return this->relation();
    }

    template<class Model, class Related>
#if defined(__clang__) && __clang_major__ < 16
    template<typename HasRelated, typename Method,
             std::enable_if_t<std::is_member_function_pointer_v<Method>, bool>>
#else
    template<typename HasRelated, typename Method>
    requires std::is_member_function_pointer_v<Method>
#endif
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::has(
            const Method relation, const QString &comparison, const qint64 count,
            const QString &condition,
            const std::function<void(TinyBuilder<HasRelated> &)> &callback) const
    {
        getQuery().template has<HasRelated>(relation, comparison, count, condition,
                                            callback);

        return this->relation();
    }

    template<class Model, class Related>
    template<typename HasRelated>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereHas(
            const QString &relation,
            const std::function<void(CallbackType<HasRelated> &)> &callback,
            const QString &comparison, const qint64 count) const
    {
        getQuery().template whereHas<HasRelated>(relation, callback, comparison, count);

        return this->relation();
    }

    template<class Model, class Related>
    template<typename HasRelated>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereHas(
            const QString &relation,
            const std::function<void(CallbackType<HasRelated> &)> &callback,
            const QString &comparison, const qint64 count) const
    {
        getQuery().template orWhereHas<HasRelated>(relation, callback, comparison,
                                                   count);

        return this->relation();
    }

    template<class Model, class Related>
    template<typename HasRelated>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereDoesntHave(
            const QString &relation,
            const std::function<void(CallbackType<HasRelated> &)> &callback) const
    {
        getQuery().template whereDoesntHave<HasRelated>(relation, callback);

        return this->relation();
    }

    template<class Model, class Related>
    template<typename HasRelated>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereDoesntHave(
            const QString &relation,
            const std::function<void(CallbackType<HasRelated> &)> &callback) const
    {
        getQuery().template orWhereDoesntHave<HasRelated>(relation, callback);

        return this->relation();
    }

    template<class Model, class Related>
#if defined(__clang__) && __clang_major__ < 16
    template<typename HasRelated, typename Method,
             std::enable_if_t<std::is_member_function_pointer_v<Method>, bool>>
#else
    template<typename HasRelated, typename Method>
    requires std::is_member_function_pointer_v<Method>
#endif
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereHas(
            const Method relation,
            const std::function<void(TinyBuilder<HasRelated> &)> &callback,
            const QString &comparison, const qint64 count) const
    {
        getQuery().template whereHas<HasRelated>(relation, callback, comparison, count);

        return this->relation();
    }

    /* Soft Deleting */

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::withoutTrashed() const
    {
        getQuery().withoutTrashed();

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::withTrashed(const bool withTrashed) const
    {
        getQuery().withTrashed(withTrashed);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::onlyTrashed() const
    {
        getQuery().onlyTrashed();

        return relation();
    }

    template<class Model, class Related>
    std::tuple<int, TSqlQuery>
    RelationProxies<Model, Related>::restore() const
    {
        return getQuery().restore();
    }

    template<class Model, class Related>
    std::tuple<int, TSqlQuery>
    RelationProxies<Model, Related>::forceDelete() const
    {
        return getQuery().forceDelete();
    }

    template<class Model, class Related>
    std::tuple<int, TSqlQuery>
    RelationProxies<Model, Related>::forceRemove() const
    {
        return getQuery().forceRemove();
    }

    /* Casting Attributes */

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::withCasts(
            const std::unordered_map<QString, CastItem> &casts) const
    {
        getQuery().withCasts(casts);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::withCasts(
            std::unordered_map<QString, CastItem> &casts) const
    {
        getQuery().withCasts(casts);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::withCasts(
            std::unordered_map<QString, CastItem> &&casts) const
    {
        getQuery().withCasts(std::move(casts));

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::withCast(std::pair<QString, CastItem> cast) const
    {
        getQuery().withCasts({std::move(cast)});

        return relation();
    }

    /* private */

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::relation() const noexcept
    {
        return static_cast<const Relation<Model, Related> &>(*this);
    }

    template<class Model, class Related>
    Relation<Model, Related> &
    RelationProxies<Model, Related>::relation() noexcept
    {
        return static_cast<Relation<Model, Related> &>(*this);
    }

    template<class Model, class Related>
    TinyBuilder<Related> &
    RelationProxies<Model, Related>::getQuery() const noexcept
    {
        return relation().getQuery();
    }

    template<class Model, class Related>
    QueryBuilder &
    RelationProxies<Model, Related>::getBaseQuery() const noexcept
    {
        return getQuery().getQuery();
    }

} // namespace Tiny::Relations
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_RELATIONS_RELATIONPROXIES_HPP
