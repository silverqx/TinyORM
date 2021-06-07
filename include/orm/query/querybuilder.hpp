#ifndef QUERYBUILDER_H
#define QUERYBUILDER_H

#include <QtSql/QSqlQuery>

#include <optional>

#include "orm/ormtypes.hpp"
#include "orm/query/grammars/grammar.hpp"
#include "orm/query/expression.hpp"

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

    // FEATURE subqueries, add support for subqueries, first in where() silverqx
    // TODO add inRandomOrder() silverqx
    class SHAREDLIB_EXPORT Builder
    {
        using QueryGrammar = Query::Grammars::Grammar;

    public:
        Builder(ConnectionInterface &connection, const QueryGrammar &grammar);
        // WARNING solve pure virtual dtor vs default silverqx
        /* Need to be the polymorphic type because of dynamic_cast<>
           in Grammar::concatenateWhereClauses(). */
        inline virtual ~Builder() = default;

        /*! Execute the query as a "select" statement. */
        QSqlQuery get(const QStringList &columns = {"*"});
        /*! Execute a query for a single record by ID. */
        QSqlQuery find(const QVariant &id, const QStringList &columns = {"*"});
        /*! Execute the query and get the first result. */
        QSqlQuery first(const QStringList &columns = {"*"});
        /*! Get a single column's value from the first result of a query. */
        QVariant value(const QString &column);

        /*! Get the SQL representation of the query. */
        QString toSql();
        // TODO next implement dd silverqx
        /*! Die and dump the current SQL and bindings. */
//        void dd() const
//        { dd($this->toSql(), $this->getBindings()); }

        /* Insert, Update, Delete */
        /*! Insert new records into the database. */
        std::optional<QSqlQuery>
        insert(const QVariantMap &values);
        /*! Insert new records into the database. */
        std::optional<QSqlQuery>
        insert(const QVector<QVariantMap> &values);
        /*! Insert a new record and get the value of the primary key. */
        quint64 insertGetId(const QVariantMap &values, const QString &sequence = "");
        /*! Insert new records into the database while ignoring errors. */
        std::tuple<int, std::optional<QSqlQuery>>
        insertOrIgnore(const QVector<QVariantMap> &values);
        /*! Insert a new record into the database while ignoring errors. */
        std::tuple<int, std::optional<QSqlQuery>>
        insertOrIgnore(const QVariantMap &values);

        /*! Update records in the database. */
        std::tuple<int, QSqlQuery>
        update(const QVector<UpdateItem> &values);

        /*! Delete records from the database. */
        std::tuple<int, QSqlQuery> deleteRow();
        /*! Delete records from the database. */
        std::tuple<int, QSqlQuery> remove();
        /*! Delete records from the database. */
        std::tuple<int, QSqlQuery> deleteRow(const quint64 id);
        /*! Delete records from the database. */
        std::tuple<int, QSqlQuery> remove(const quint64 id);

        /*! Run a truncate statement on the table. */
        void truncate();

        /* Select */
        /*! Set the columns to be selected. */
        Builder &select(const QStringList &columns = {"*"});
        /*! Set the column to be selected. */
        Builder &select(const QString &column);
        /*! Add new select columns to the query. */
        Builder &addSelect(const QStringList &columns);
        /*! Add a new select column to the query. */
        Builder &addSelect(const QString &column);

        /*! Force the query to only return distinct results. */
        Builder &distinct();
        /*! Force the query to only return distinct results. */
        Builder &distinct(const QStringList &columns);
        /*! Force the query to only return distinct results. */
        Builder &distinct(QStringList &&columns);

        /*! Set the table which the query is targeting. */
        Builder &from(const QString &table, const QString &as = "");

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
        Builder &latest(const QString &column = "created_at");
        /*! Add an "order by" clause for a timestamp to the query. */
        Builder &oldest(const QString &column = "created_at");
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
        inline ConnectionInterface &getConnection() const
        { return m_connection; }
        /*! Get the query grammar instance. */
        inline const QueryGrammar &getGrammar() const
        { return m_grammar; }

        /*! Get the current query value bindings as flattened QVector. */
        QVector<QVariant> getBindings() const;
        /*! Get the raw map of bindings. */
        inline const BindingsMap &getRawBindings() const
        { return m_bindings; }

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
        inline const QStringList &getColumns() const
        { return m_columns; }
        /*! Set the columns that should be returned. */
        inline Builder &setColumns(const QStringList &columns)
        { m_columns = columns; return *this; }
        /*! Get the table associated with the query builder. */
        inline const QString &getFrom() const
        { return m_from; }
        /*! Get the table joins for the query. */
        inline const QVector<QSharedPointer<JoinClause>> &getJoins() const
        { return m_joins; }
        /*! Get the where constraints for the query. */
        inline const QVector<WhereConditionItem> &getWheres() const
        { return m_wheres; }
        /*! Get the groupings for the query. */
        inline const QStringList &getGroups() const
        { return m_groups; }
        /*! Get the having constraints for the query. */
        inline const QVector<HavingConditionItem> &getHavings() const
        { return m_havings; }
        /*! Get the orderings for the query. */
        inline const QVector<OrderByItem> &getOrders() const
        { return m_orders; }
        /*! Get the maximum number of records to return. */
        inline int getLimit() const
        { return m_limit; }
        /*! Get the number of records to skip. */
        inline int getOffset() const
        { return m_offset; }
        /*! Get the row locking. */
        inline const std::variant<std::monostate, bool, QString> &getLock() const
        { return m_lock; }

        /* Other methods */
        /*! Get a new instance of the query builder. */
        QSharedPointer<Builder> newQuery() const;
        /*! Create a new query instance for nested where condition. */
        QSharedPointer<Builder> forNestedWhere() const;

        /*! Create a raw database expression. */
        Expression raw(const QVariant &value) const;

        /*! Add another query builder as a nested where to the query builder. */
        Builder &addNestedWhereQuery(const QSharedPointer<Builder> &query,
                                     const QString &condition);

    protected:
        /*! Determine if the given operator is supported. */
        bool invalidOperator(const QString &comparison) const;

        /*! Add a binding to the query. */
        Builder &addBinding(const QVariant &binding,
                            BindingType type = BindingType::WHERE);
        /*! Add bindings to the query. */
        Builder &addBinding(const QVector<QVariant> &bindings,
                            BindingType type = BindingType::WHERE);
        /*! Remove all of the expressions from a list of bindings. */
        QVector<QVariant> cleanBindings(const QVector<QVariant> &bindings) const;

        /*! Add a vector of basic where clauses to the query. */
        Builder &
        addArrayOfWheres(const QVector<WhereItem> &values,
                         const QString &condition = "and");
        /*! Add a vector of where clauses comparing two columns to the query. */
        Builder &
        addArrayOfWheres(const QVector<WhereColumnItem> &values,
                         const QString &condition = "and");

        /*! Get a new join clause. */
        QSharedPointer<JoinClause>
        newJoinClause(const Builder &query, const QString &type,
                      const QString &table) const;

        /*! Remove all existing columns and column bindings. */
        Builder &clearColumns();
        /*! Execute the given callback while selecting the given columns. */
        QSqlQuery
        onceWithColumns(const QStringList &columns,
                        const std::function<QSqlQuery()> &callback);

    private:
        /*! Run the query as a "select" statement against the connection. */
        QSqlQuery runSelect();

        /*! All of the available clause operators. */
        const QVector<QString> m_operators {
            "=", "<", ">", "<=", ">=", "<>", "!=", "<=>",
            "like", "like binary", "not like", "ilike",
            "&", "|", "^", "<<", ">>",
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

        /*! Indicates if the query returns distinct results. */
        std::variant<bool, QStringList> m_distinct = false;
        /*! The columns that should be returned. */
        QStringList m_columns;
        /*! The table which the query is targeting. */
        QString m_from;
        /*! The table joins for the query. */
        QVector<QSharedPointer<JoinClause>> m_joins;
        /*! The where constraints for the query. */
        QVector<WhereConditionItem> m_wheres;
        /*! The groupings for the query. */
        QStringList m_groups;
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

} // namespace Orm::Query
} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // QUERYBUILDER_H
