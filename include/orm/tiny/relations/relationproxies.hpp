#ifndef ORM_RELATIONPROXIES_HPP
#define ORM_RELATIONPROXIES_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtSql/QSqlQuery>

#include "orm/concepts.hpp"
#include "orm/tiny/tinytypes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

namespace Query
{
    class JoinClause;
}

namespace Tiny::Relations
{

    template<class Model, class Related>
    class Relation;

    /*! Contains proxy methods to the TinyBuilder. */
    template<class Model, class Related>
    class RelationProxies
    {
        /*! JoinClause alias. */
        using JoinClause = Orm::Query::JoinClause;
        /*! Alias for the QueriesRelationships callback type. */
        template<typename HasRelated>
        using CallbackType = Orm::Tiny::Concerns
                                      ::QueriesRelationshipsCallback<HasRelated>;

    public:
        /*! Virtual destructor. */
        inline virtual ~RelationProxies() = default;

        /*! Get a single column's value from the first result of a query. */
        QVariant value(const Column &column) const;

        /*! Get the vector with the values of a given column. */
        QVector<QVariant> pluck(const QString &column) const;
        /*! Get the vector with the values of a given column. */
        template<typename T>
        std::map<T, QVariant> pluck(const QString &column, const QString &key) const;

        /*! Find a model by its primary key. */
        virtual std::optional<Related>
        find(const QVariant &id, const QVector<Column> &columns = {ASTERISK}) const;
        /*! Find a model by its primary key or return fresh model instance. */
        virtual Related
        findOrNew(const QVariant &id, const QVector<Column> &columns = {ASTERISK}) const;
        /*! Find a model by its primary key or throw an exception. */
        virtual Related
        findOrFail(const QVariant &id,
                   const QVector<Column> &columns = {ASTERISK}) const;
        // findMany() is missing intentionally doesn't make sense for one type relations

        /*! Execute the query and get the first result. */
        virtual std::optional<Related>
        first(const QVector<Column> &columns = {ASTERISK}) const;
        /*! Get the first record matching the attributes or instantiate it. */
        virtual Related
        firstOrNew(const QVector<WhereItem> &attributes = {},
                   const QVector<AttributeItem> &values = {}) const;
        /*! Get the first record matching the attributes or create it. */
        Related
        firstOrCreate(const QVector<WhereItem> &attributes = {},
                      const QVector<AttributeItem> &values = {}) const;
        /*! Execute the query and get the first result or throw an exception. */
        virtual Related firstOrFail(const QVector<Column> &columns = {ASTERISK}) const;

        /*! Add a basic where clause to the query, and return the first result. */
        virtual std::optional<Related>
        firstWhere(const Column &column, const QString &comparison,
                   const QVariant &value, const QString &condition = AND) const;
        /*! Add a basic where clause to the query, and return the first result. */
        virtual std::optional<Related>
        firstWhereEq(const Column &column, const QVariant &value,
                     const QString &condition = AND) const;

        /*! Add a where clause on the primary key to the query. */
        const Relation<Model, Related> &whereKey(const QVariant &id) const;
        /*! Add a where clause on the primary key to the query. */
        const Relation<Model, Related> &whereKey(const QVector<QVariant> &ids) const;
        /*! Add a where clause on the primary key to the query. */
        const Relation<Model, Related> &whereKeyNot(const QVariant &id) const;
        /*! Add a where clause on the primary key to the query. */
        const Relation<Model, Related> &whereKeyNot(const QVector<QVariant> &ids) const;

        /*! Set the relationships that should be eager loaded. */
        template<typename = void>
        const Relation<Model, Related> &with(const QVector<WithItem> &relations) const;
        /*! Set the relationships that should be eager loaded. */
        template<typename = void>
        const Relation<Model, Related> &with(const QString &relation) const;
        /*! Begin querying a model with eager loading. */
        const Relation<Model, Related> &with(const QVector<QString> &relations) const;
        /*! Begin querying a model with eager loading. */
        const Relation<Model, Related> &with(QVector<QString> &&relations) const;

        /*! Prevent the specified relations from being eager loaded. */
        const Relation<Model, Related> &without(const QVector<QString> &relations) const;
        /*! Prevent the specified relations from being eager loaded. */
        const Relation<Model, Related> &without(const QString &relation) const;

        /*! Set the relationships that should be eager loaded while removing
            any previously added eager loading specifications. */
        const Relation<Model, Related> &
        withOnly(const QVector<WithItem> &relations) const;
        /*! Set the relationship that should be eager loaded while removing
            any previously added eager loading specifications. */
        const Relation<Model, Related> &withOnly(const QString &relation) const;

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
        /*! Retrieve the "count" result of the query. */
        quint64 count(const QVector<Column> &columns = {ASTERISK}) const;
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
                           const QVector<Column> &columns = {ASTERISK}) const;

        /*! Set the columns to be selected. */
        const Relation<Model, Related> &
        select(const QVector<Column> &columns = {ASTERISK}) const;
        /*! Set the column to be selected. */
        const Relation<Model, Related> &select(const Column &column) const;
        /*! Add new select columns to the query. */
        const Relation<Model, Related> &addSelect(const QVector<Column> &columns) const;
        /*! Add a new select column to the query. */
        const Relation<Model, Related> &addSelect(const Column &column) const;

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
                const QString &expression, const QVector<QVariant> &bindings = {}) const;

        /*! Force the query to only return distinct results. */
        const Relation<Model, Related> &distinct() const;
        /*! Force the query to only return distinct results. */
        const Relation<Model, Related> &distinct(const QStringList &columns) const;
        /*! Force the query to only return distinct results. */
        const Relation<Model, Related> &distinct(QStringList &&columns) const;

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

        /*! Add a nested where clause to the query. */
        const Relation<Model, Related> &where(
                const std::function<void(Builder<Related> &)> &callback,
                const QString &condition = AND) const;
        /*! Add a nested "or where" clause to the query. */
        const Relation<Model, Related> &orWhere(
                const std::function<void(Builder<Related> &)> &callback) const;

        /*! Add a vector of basic where clauses to the query. */
        const Relation<Model, Related> &where(
                const QVector<WhereItem> &values, const QString &condition = AND) const;
        /*! Add a vector of basic "or where" clauses to the query. */
        const Relation<Model, Related> &orWhere(const QVector<WhereItem> &values) const;

        /*! Add a vector of where clauses comparing two columns to the query. */
        const Relation<Model, Related> &whereColumn(
                const QVector<WhereColumnItem> &values,
                const QString &condition = AND) const;
        /*! Add a vector of "or where" clauses comparing two columns to the query. */
        const Relation<Model, Related> &orWhereColumn(
                const QVector<WhereColumnItem> &values) const;

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

        /*! Add a "where in" clause to the query. */
        const Relation<Model, Related> &whereIn(
                const Column &column, const QVector<QVariant> &values,
                const QString &condition = AND, bool nope = false) const;
        /*! Add an "or where in" clause to the query. */
        const Relation<Model, Related> &orWhereIn(
                const Column &column, const QVector<QVariant> &values) const;
        /*! Add a "where not in" clause to the query. */
        const Relation<Model, Related> &whereNotIn(
                const Column &column, const QVector<QVariant> &values,
                const QString &condition = AND) const;
        /*! Add an "or where not in" clause to the query. */
        const Relation<Model, Related> &orWhereNotIn(
                const Column &column, const QVector<QVariant> &values) const;

        /*! Add a "where null" clause to the query. */
        const Relation<Model, Related> &whereNull(
                const QVector<Column> &columns = {ASTERISK},
                const QString &condition = AND, bool nope = false) const;
        /*! Add an "or where null" clause to the query. */
        const Relation<Model, Related> &orWhereNull(
                const QVector<Column> &columns = {ASTERISK}) const;
        /*! Add a "where not null" clause to the query. */
        const Relation<Model, Related> &whereNotNull(
                const QVector<Column> &columns = {ASTERISK},
                const QString &condition = AND) const;
        /*! Add an "or where not null" clause to the query. */
        const Relation<Model, Related> &
        orWhereNotNull(const QVector<Column> &columns = {ASTERISK}) const;

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

        /*! Add a full sub-select to the "where" clause. */
        template<WhereValueSubQuery T>
        const Relation<Model, Related> &whereSub(
                const Column &column, const QString &comparison, T &&query,
                const QString &condition = AND) const;

        /*! Add a raw "where" clause to the query. */
        const Relation<Model, Related> &whereRaw(
                const QString &sql, const QVector<QVariant> &bindings = {},
                const QString &condition = AND) const;
        /*! Add a raw "or where" clause to the query. */
        const Relation<Model, Related> &orWhereRaw(
                const QString &sql, const QVector<QVariant> &bindings = {}) const;

        /*! Add a "group by" clause to the query. */
        const Relation<Model, Related> &groupBy(const QVector<Column> &groups) const;
        /*! Add a "group by" clause to the query. */
        const Relation<Model, Related> &groupBy(const Column &group) const;
        /*! Add a "group by" clause to the query. */
        template<ColumnConcept ...Args>
        const Relation<Model, Related> &groupBy(Args &&...groups) const;

        /*! Add a raw "groupBy" clause to the query. */
        const Relation<Model, Related> &groupByRaw(
                const QString &sql, const QVector<QVariant> &bindings = {}) const;

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
                const QString &sql, const QVector<QVariant> &bindings = {},
                const QString &condition = AND) const;
        /*! Add a raw "or having" clause to the query. */
        const Relation<Model, Related> &orHavingRaw(
                const QString &sql, const QVector<QVariant> &bindings = {}) const;

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

        /*! Add a raw "order by" clause to the query. */
        const Relation<Model, Related> &orderByRaw(
                const QString &sql, const QVector<QVariant> &bindings = {}) const;

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
        const Relation<Model, Related> &limit(int value) const;
        /*! Alias to set the "limit" value of the query. */
        const Relation<Model, Related> &take(int value) const;
        /*! Set the "offset" value of the query. */
        const Relation<Model, Related> &offset(int value) const;
        /*! Alias to set the "offset" value of the query. */
        const Relation<Model, Related> &skip(int value) const;
        /*! Set the limit and offset for a given page. */
        const Relation<Model, Related> &forPage(int page, int perPage = 30) const;

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
#ifdef __clang__
        template<typename HasRelated, typename Method,
                 std::enable_if_t<std::is_member_function_pointer_v<Method>, bool> = true>
#else
        template<typename HasRelated, typename Method>
        requires std::is_member_function_pointer_v<Method>
#endif
        const Relation<Model, Related> &
        has(const Method relation, const QString &comparison = GE, qint64 count = 1,
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
#ifdef __clang__
        template<typename HasRelated, typename Method,
                 std::enable_if_t<std::is_member_function_pointer_v<Method>, bool> = true>
#else
        template<typename HasRelated, typename Method>
        requires std::is_member_function_pointer_v<Method>
#endif
        const Relation<Model, Related> &
        whereHas(const Method relation,
                 const std::function<void(
                     TinyBuilder<HasRelated> &)> &callback = nullptr,
                 const QString &comparison = GE, qint64 count = 1) const;

    private:
        /*! Static cast this to a child's instance Relation type. */
        const Relation<Model, Related> &relation() const;
        /*! Static cast this to a child's instance Relation type, const version. */
        Relation<Model, Related> &relation();
        /*! Get the underlying query for the relation. */
        TinyBuilder<Related> &getQuery() const;
        /*! Get the base QueryBuilder driving the TinyBuilder. */
        QueryBuilder &getBaseQuery() const;
    };

    template<class Model, class Related>
    QVariant
    RelationProxies<Model, Related>::value(const Column &column) const
    {
        return getQuery().value(column);
    }

    template<class Model, class Related>
    QVector<QVariant>
    RelationProxies<Model, Related>::pluck(const QString &column) const
    {
        return getQuery().pluck(column);
    }

    template<class Model, class Related>
    template<typename T>
    std::map<T, QVariant>
    RelationProxies<Model, Related>::pluck(const QString &column,
                                           const QString &key) const
    {
        return getQuery().template pluck<T>(column, key);
    }

    template<class Model, class Related>
    std::optional<Related>
    RelationProxies<Model, Related>::find(const QVariant &id,
                                          const QVector<Column> &columns) const
    {
        return getQuery().find(id, columns);
    }

    template<class Model, class Related>
    Related
    RelationProxies<Model, Related>::findOrNew(const QVariant &id,
                                               const QVector<Column> &columns) const
    {
        return getQuery().findOrNew(id, columns);
    }

    template<class Model, class Related>
    Related
    RelationProxies<Model, Related>::findOrFail(const QVariant &id,
                                                const QVector<Column> &columns) const
    {
        return getQuery().findOrFail(id, columns);
    }

    template<class Model, class Related>
    std::optional<Related>
    RelationProxies<Model, Related>::first(const QVector<Column> &columns) const
    {
        return getQuery().first(columns);
    }

    template<class Model, class Related>
    Related
    RelationProxies<Model, Related>::firstOrNew(
            const QVector<WhereItem> &attributes,
            const QVector<AttributeItem> &values) const
    {
        return getQuery().firstOrNew(attributes, values);
    }

    template<class Model, class Related>
    Related
    RelationProxies<Model, Related>::firstOrCreate(
            const QVector<WhereItem> &attributes,
            const QVector<AttributeItem> &values) const
    {
        return getQuery().firstOrCreate(attributes, values);
    }

    template<class Model, class Related>
    Related RelationProxies<Model, Related>::firstOrFail(
            const QVector<Column> &columns) const
    {
        return getQuery().firstOrFail(columns);
    }

    template<class Model, class Related>
    std::optional<Related>
    RelationProxies<Model, Related>::firstWhere(
            const Column &column, const QString &comparison,
            const QVariant &value, const QString &condition) const
    {
        return getQuery().firstWhere(column, comparison, value, condition);
    }

    template<class Model, class Related>
    std::optional<Related>
    RelationProxies<Model, Related>::firstWhereEq(
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
    RelationProxies<Model, Related>::whereKey(const QVector<QVariant> &ids) const
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
    RelationProxies<Model, Related>::whereKeyNot(const QVector<QVariant> &ids) const
    {
        getQuery().whereKeyNot(ids);

        return relation();
    }

    template<class Model, class Related>
    template<typename>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::with(const QVector<WithItem> &relations) const
    {
        getQuery().with(relations);

        return relation();
    }

    template<class Model, class Related>
    template<typename>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::with(const QString &relation) const
    {
        getQuery().with(relation);

        return this->relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::with(const QVector<QString> &relations) const
    {
        getQuery().with(relations);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::with(QVector<QString> &&relations) const
    {
        getQuery().with(std::move(relations));

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::without(const QVector<QString> &relations) const
    {
        getQuery().without(relations);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::without(const QString &relation) const
    {
        getQuery().without(relation);

        return this->relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::withOnly(const QVector<WithItem> &relations) const
    {
        getQuery().withOnly(relations);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::withOnly(const QString &relation) const
    {
        getQuery().withOnly(relation);

        return this->relation();
    }

    template<class Model, class Related>
    Related
    RelationProxies<Model, Related>::updateOrCreate(
            const QVector<WhereItem> &attributes,
            const QVector<AttributeItem> &values) const
    {
        return getQuery().updateOrCreate(attributes, values);
    }

    template<class Model, class Related>
    inline QString RelationProxies<Model, Related>::toSql() const
    {
        return getBaseQuery().toSql();
    }

    template<class Model, class Related>
    inline QVector<QVariant>
    RelationProxies<Model, Related>::getBindings() const
    {
        return getBaseQuery().getBindings();
    }

    template<class Model, class Related>
    std::optional<QSqlQuery>
    RelationProxies<Model, Related>::insert(const QVector<AttributeItem> &values) const
    {
        return getQuery().insert(values);
    }

    template<class Model, class Related>
    std::optional<QSqlQuery>
    RelationProxies<Model, Related>::insert(
            const QVector<QVector<AttributeItem>> &values) const
    {
        return getQuery().insert(values);
    }

    // FEATURE dilemma primarykey, Model::KeyType vs QVariant silverqx
    template<class Model, class Related>
    quint64
    RelationProxies<Model, Related>::insertGetId(
            const QVector<AttributeItem> &attributes, const QString &sequence) const
    {
        return getQuery().insertGetId(attributes, sequence);
    }

    template<class Model, class Related>
    std::tuple<int, std::optional<QSqlQuery>>
    RelationProxies<Model, Related>::insertOrIgnore(
            const QVector<AttributeItem> &values) const
    {
        return getQuery().insertOrIgnore(values);
    }

    template<class Model, class Related>
    std::tuple<int, std::optional<QSqlQuery>>
    RelationProxies<Model, Related>::insertOrIgnore(
            const QVector<QVector<AttributeItem>> &values) const
    {
        return getQuery().insertOrIgnore(values);
    }

    template<class Model, class Related>
    std::tuple<int, QSqlQuery>
    RelationProxies<Model, Related>::update(const QVector<UpdateItem> &values) const
    {
        return getQuery().update(values);
    }

    template<class Model, class Related>
    std::tuple<int, QSqlQuery>
    RelationProxies<Model, Related>::remove() const
    {
        return getQuery().remove();
    }

    template<class Model, class Related>
    std::tuple<int, QSqlQuery>
    RelationProxies<Model, Related>::deleteModels() const
    {
        return getQuery().deleteModels();
    }

    template<class Model, class Related>
    void RelationProxies<Model, Related>::truncate() const
    {
        getQuery().truncate();
    }

    template<class Model, class Related>
    quint64 RelationProxies<Model, Related>::count(const QVector<Column> &columns) const
    {
        return getQuery().count(columns);
    }

    template<class Model, class Related>
    template<typename>
    quint64 RelationProxies<Model, Related>::count(const Column &column) const
    {
        return getQuery().count(QVector<Column> {column});
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
            const QString &function, const QVector<Column> &columns) const
    {
        return getQuery().aggregate(function, columns);
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::select(const QVector<Column> &columns) const
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
    RelationProxies<Model, Related>::addSelect(const QVector<Column> &columns) const
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
            const QString &expression, const QVector<QVariant> &bindings) const
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
    RelationProxies<Model, Related>::where(
            const QVector<WhereItem> &values, const QString &condition) const
    {
        getQuery().where(values, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhere(const QVector<WhereItem> &values) const
    {
        getQuery().orWhere(values);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereColumn(
            const QVector<WhereColumnItem> &values, const QString &condition) const
    {
        getQuery().whereColumn(values, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereColumn(
            const QVector<WhereColumnItem> &values) const
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

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereIn(
            const Column &column, const QVector<QVariant> &values,
            const QString &condition, const bool nope) const
    {
        getQuery().whereIn(column, values, condition, nope);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereIn(const Column &column,
                                               const QVector<QVariant> &values) const
    {
        getQuery().orWhereIn(column, values);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereNotIn(
            const Column &column, const QVector<QVariant> &values,
            const QString &condition) const
    {
        getQuery().whereNotIn(column, values, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereNotIn(const Column &column,
                                                  const QVector<QVariant> &values) const
    {
        getQuery().orWhereNotIn(column, values);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereNull(
            const QVector<Column> &columns, const QString &condition,
            const bool nope) const
    {
        getQuery().whereNull(columns, condition, nope);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereNull(const QVector<Column> &columns) const
    {
        getQuery().orWhereNull(columns);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereNotNull(const QVector<Column> &columns,
                                                  const QString &condition) const
    {
        getQuery().whereNotNull(columns, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereNotNull(const QVector<Column> &columns) const
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

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::whereRaw(
            const QString &sql, const QVector<QVariant> &bindings,
            const QString &condition) const
    {
        getQuery().whereRaw(sql, bindings, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orWhereRaw(
            const QString &sql, const QVector<QVariant> &bindings) const
    {
        getQuery().whereRaw(sql, bindings, OR);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::groupBy(const QVector<Column> &groups) const
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
        getQuery().groupBy(QVector<Column> {std::forward<Args>(groups)...});

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::groupByRaw(const QString &sql,
                                                const QVector<QVariant> &bindings) const
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
            const QString &sql,  const QVector<QVariant> &bindings,
            const QString &condition) const
    {
        getQuery().havingRaw(sql, bindings, condition);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::orHavingRaw(
            const QString &sql,  const QVector<QVariant> &bindings) const
    {
        getQuery().havingRaw(sql, bindings, OR);

        return relation();
    }

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
    RelationProxies<Model, Related>::orderByRaw(const QString &sql,
                                                const QVector<QVariant> &bindings) const
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
    RelationProxies<Model, Related>::limit(const int value) const
    {
        getQuery().limit(value);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::take(const int value) const
    {
        getQuery().take(value);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::offset(const int value) const
    {
        getQuery().offset(value);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::skip(const int value) const
    {
        getQuery().skip(value);

        return relation();
    }

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::forPage(const int page, const int perPage) const
    {
        getQuery().forPage(page, perPage);

        return relation();
    }

    template<class Model, class Related>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, QSqlQuery>
    RelationProxies<Model, Related>::increment(const QString &column, const T amount,
                                               const QVector<UpdateItem> &extra) const
    {
        return getQuery().increment(column, amount, extra);
    }

    template<class Model, class Related>
    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, QSqlQuery>
    RelationProxies<Model, Related>::decrement(const QString &column, const T amount,
                                               const QVector<UpdateItem> &extra) const
    {
        return getQuery().decrement(column, amount, extra);
    }

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
#ifdef __clang__
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
#ifdef __clang__
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

    template<class Model, class Related>
    const Relation<Model, Related> &
    RelationProxies<Model, Related>::relation() const
    {
        return static_cast<const Relation<Model, Related> &>(*this);
    }

    template<class Model, class Related>
    Relation<Model, Related> &
    RelationProxies<Model, Related>::relation()
    {
        return static_cast<Relation<Model, Related> &>(*this);
    }

    template<class Model, class Related>
    TinyBuilder<Related> &
    RelationProxies<Model, Related>::getQuery() const
    {
        return relation().getQuery();
    }

    template<class Model, class Related>
    QueryBuilder &
    RelationProxies<Model, Related>::getBaseQuery() const
    {
        return getQuery().getQuery();
    }

} // namespace Orm::Tiny::Relations
} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_RELATIONPROXIES_HPP
