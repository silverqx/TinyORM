#ifndef QUERYBUILDER_H
#define QUERYBUILDER_H

#include <QtSql/QSqlQuery>

#include <optional>

#include "orm/expression.h"
#include "orm/ormtypes.h"

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif

namespace Orm
{
    class DatabaseConnection;
    class Grammar;
}

namespace Orm::Query
{

    class JoinClause;

    class Builder
    {
    public:
        Builder(const DatabaseConnection &db, const Grammar &grammar);
        // WARNING solver pure virtual dtor vs default silverqx
        virtual ~Builder() = default;

        inline Builder &distinct()
        { m_distinct = true; return *this; };
        inline Builder &from(const QString &table)
        { m_from = table; return *this; }
        inline Builder &table(const QString &table)
        { m_from = table; return *this; }

        /*! Execute the query as a "select" statement. */
        std::tuple<bool, QSqlQuery>
        get(const QStringList &columns = {"*"});
        /*! Execute the query and get the first result. */
        std::tuple<bool, QSqlQuery>
        first(const QStringList &columns = {"*"});
        /*! Get a single column's value from the first result of a query. */
        QVariant value(const QString &column);
        /*! Execute a query for a single record by ID. */
        std::tuple<bool, QSqlQuery>
        find(const QVariant &id, const QStringList &columns = {"*"});

        /*! Get the SQL representation of the query. */
        QString toSql() const;

        /*! Insert new records into the database. */
        std::tuple<bool, std::optional<QSqlQuery>>
        insert(const QVariantMap &values) const;
        /*! Insert new records into the database. */
        std::tuple<bool, std::optional<QSqlQuery>>
        insert(const QVector<QVariantMap> &values) const;
        /*! Insert new records into the database while ignoring errors. */
        std::tuple<int, std::optional<QSqlQuery>>
        insertOrIgnore(const QVector<QVariantMap> &values) const;
        std::tuple<int, std::optional<QSqlQuery>>
        insertOrIgnore(const QVariantMap &values) const;
        /*! Insert a new record and get the value of the primary key. */
        quint64 insertGetId(const QVariantMap &values) const;

        /*! Update records in the database. */
        std::tuple<int, QSqlQuery>
        update(const QVector<UpdateItem> &values) const;

        /*! Delete records from the database. */
        inline std::tuple<int, QSqlQuery>
        deleteRow() const
        { return remove(); }
        /*! Delete records from the database. */
        std::tuple<int, QSqlQuery>
        remove() const;
        /*! Delete records from the database. */
        inline std::tuple<int, QSqlQuery>
        deleteRow(const quint64 id)
        { return remove(id); }
        /*! Delete records from the database. */
        std::tuple<int, QSqlQuery>
        remove(const quint64 id);

        /*! Run a truncate statement on the table. */
        std::tuple<bool, QSqlQuery> truncate() const;

        /*! Add a join clause to the query. */
        Builder &join(const QString &table, const QString &first,
                      const QString &comparison, const QString &second,
                      const QString &type = "inner", bool where = false);
        Builder &join(const QString &table,
                      const std::function<void(JoinClause &)> &callback,
                      const QString &type = "inner");
        Builder &leftJoin(const QString &table,
                          const std::function<void(JoinClause &)> &callback);
        Builder &rightJoin(const QString &table,
                           const std::function<void(JoinClause &)> &callback);
        Builder &crossJoin(const QString &table,
                           const std::function<void(JoinClause &)> &callback);
        Builder &joinWhere(const QString &table, const QString &first,
                           const QString &comparison, const QString &second,
                           const QString &type = "inner");
        Builder &leftJoin(const QString &table, const QString &first,
                          const QString &comparison, const QString &second);
        Builder &leftJoinWhere(const QString &table, const QString &first,
                               const QString &comparison, const QString &second);
        Builder &rightJoin(const QString &table, const QString &first,
                           const QString &comparison, const QString &second);
        Builder &rightJoinWhere(const QString &table, const QString &first,
                                const QString &comparison, const QString &second);
        Builder &crossJoin(const QString &table, const QString &first,
                           const QString &comparison, const QString &second);

        /*! Add a basic where clause to the query. */
        Builder &where(const QString &column, const QString &comparison,
                       const QVariant &value, const QString &condition = "and");
        Builder &orWhere(const QString &column, const QString &comparison,
                         const QVariant &value);
        /*! Add a nested where clause to the query. */
        Builder &where(const std::function<void(Builder &)> &callback,
                       const QString &condition = "and");
        Builder &orWhere(const std::function<void(Builder &)> &callback);
        Builder &whereEq(const QString &column, const QVariant &value,
                         const QString &condition = "and");
        Builder &orWhereEq(const QString &column, const QVariant &value);

        /*! Add a "where" clause comparing two columns to the query. */
        Builder &whereColumn(const QString &first, const QString &comparison,
                             const QString &second, const QString &condition = "and");
        Builder &orWhereColumn(const QString &first, const QString &comparison,
                               const QString &second);

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
                  const QVector<UpdateItem> &extra = {}) const;
        /*! Decrement a column's value by a given amount. */
        template<typename T> requires std::is_arithmetic_v<T>
        std::tuple<int, QSqlQuery>
        decrement(const QString &column, T amount = 1,
                  const QVector<UpdateItem> &extra = {}) const;

        /*! Get a database connection. */
        inline const DatabaseConnection &getConnection() const
        { return m_db; }
        inline const Grammar &getGrammar() const
        { return m_grammar; }

        /*! Get the current query value bindings as flattened QVector. */
        QVector<QVariant> getBindings() const;
        inline const BindingsMap &getRawBindings() const
        { return m_bindings; }

        inline bool getDistinct() const
        { return m_distinct; }
        // TODO check up all code and return references when appropriate silverqx
        inline const QStringList &getColumns() const
        { return m_columns; }
        inline const QString &getFrom() const
        { return m_from; }
        inline const QString &getTable() const
        { return m_from; }
        inline const QVector<QSharedPointer<JoinClause>> &getJoins() const
        { return m_joins; }
        inline const QVector<WhereConditionItem> &getWheres() const
        { return m_wheres; }
        inline const QStringList &getGroups() const
        { return m_groups; }
        inline const QVector<HavingConditionItem> &getHavings() const
        { return m_havings; }
        inline const QVector<OrderByItem> &getOrders() const
        { return m_orders; }
        inline int getLimit() const
        { return m_limit; }
        inline int getOffset() const
        { return m_offset; }

        /*! Get a new instance of the query builder. */
        QSharedPointer<Builder> newQuery() const;
        /*! Create a new query instance for nested where condition. */
        QSharedPointer<Builder> forNestedWhere() const;

        /*! Create a raw database expression. */
        Expression raw(const QVariant &value) const;

    protected:
        bool invalidOperator(const QString &comparison) const;

        /*! Add a binding to the query. */
        Builder &addBinding(const QVariant &binding,
                            BindingType type = BindingType::WHERE);
        Builder &addBinding(const QVector<QVariant> &bindings,
                            BindingType type = BindingType::WHERE);
        /*! Remove all of the expressions from a list of bindings. */
        QVector<QVariant> cleanBindings(const QVector<QVariant> &bindings) const;

        /*! Get a new join clause. */
        QSharedPointer<JoinClause>
        newJoinClause(const Builder &query, const QString &type, const QString &table) const;

        /*! Add another query builder as a nested where to the query builder. */
        Builder &addNestedWhereQuery(QSharedPointer<Builder> query,
                                     const QString &condition);

    private:
        std::tuple<bool, QSqlQuery> runSelect() const;

        /*! All of the available clause operators. */
        const QVector<QString> m_operators {
            "=", "<", ">", "<=", ">=", "<>", "!=", "<=>",
            "like", "like binary", "not like", "ilike",
            "&", "|", "^", "<<", ">>",
            "rlike", "not rlike", "regexp", "not regexp",
            "~", "~*", "!~", "!~*", "similar to",
            "not similar to", "not ilike", "~~*", "!~~*",
        };

        const DatabaseConnection &m_db;
        const Grammar &m_grammar;

        /*! Prepared statement bindings. Order is crucial here because of that
            QMap with an enum struct is used. */
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

        bool m_distinct = false;
        QStringList m_columns {"*"};
        QString m_from;
        QVector<QSharedPointer<JoinClause>> m_joins;
        QVector<WhereConditionItem> m_wheres;
        QStringList m_groups;
        QVector<HavingConditionItem> m_havings;
        QVector<OrderByItem> m_orders;
        int m_limit = -1;
        int m_offset = -1;
    };

    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, QSqlQuery>
    Builder::increment(const QString &column, const T amount,
                       const QVector<UpdateItem> &extra) const
    {
        const auto &expression = QStringLiteral("%1 + %2").arg(column).arg(amount);
        QVector<UpdateItem> columns {{column, raw(expression)}};
        std::copy(extra.cbegin(), extra.cend(), std::back_inserter(columns));
        return update(columns);
    }

    template<typename T> requires std::is_arithmetic_v<T>
    std::tuple<int, QSqlQuery>
    Builder::decrement(const QString &column, const T amount,
                       const QVector<UpdateItem> &extra) const
    {
        const auto &expression = QStringLiteral("%1 - %2").arg(column).arg(amount);
        QVector<UpdateItem> columns {{column, raw(expression)}};
        std::copy(extra.cbegin(), extra.cend(), std::back_inserter(columns));
        return update(columns);
    }

} // namespace Orm::Query
#ifdef MANGO_COMMON_NAMESPACE
} // namespace MANGO_COMMON_NAMESPACE
#endif

#endif // QUERYBUILDER_H
