#include "orm/query/grammars/mysqlgrammar.hpp"

#include "orm/mysqlconnection.hpp"
#include "orm/query/querybuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

namespace Orm::Query::Grammars
{

/* public */

QString MySqlGrammar::compileInsert(const QueryBuilder &query,
                                    const QList<QVariantMap> &values) const
{
    // MySQL doesn't support 'default values' statement
    if (values.isEmpty())
        return Grammar::compileInsert(query, {{}});

    return Grammar::compileInsert(query, values);
}

QString MySqlGrammar::compileInsertOrIgnore(const QueryBuilder &query,
                                            const QList<QVariantMap> &values) const
{
    return compileInsert(query, values).replace(0, 6, u"insert ignore"_s);
}

QString MySqlGrammar::compileUpsert(
        QueryBuilder &query, const QList<QVariantMap> &values,
        const QStringList &/*unused*/, const QStringList &update) const
{
    static const auto TinyOrmUpsertAlias = u"tinyorm_upsert_alias"_s;

    // Use an upsert alias on the MySQL >=8.0.19
    const auto useUpsertAlias = dynamic_cast<MySqlConnection &>(query.getConnection())
                                .useUpsertAlias();

    auto sql = compileInsert(query, values);
    // ~64 is manually counted size of string literals below, exactly it's 49
    sql.reserve(sql.size() + 64);

    if (useUpsertAlias)
        sql += u" as %1"_s.arg(wrap(u"tinyorm_upsert_alias"_s));

    sql += u" on duplicate key update "_s;

    QStringList columns;
    columns.reserve(update.size());

    for (const auto &column : update) {
        const auto wrappedColumn = wrap(column);

        columns << (useUpsertAlias ? u"%1 = %2"_s
                                     .arg(wrappedColumn,
                                          DOT_IN.arg(wrap(TinyOrmUpsertAlias),
                                                     wrappedColumn))
                                   : u"%1 = values(%2)"_s
                                     .arg(wrappedColumn, wrappedColumn));
    }

    return NOSPACE.arg(sql, columns.join(COMMA));
}

QString MySqlGrammar::compileLock(const QueryBuilder &query) const
{
    const auto &lock = query.getLock();

    if (!std::holds_alternative<QString>(lock))
        return std::get<bool>(lock) ? u"for update"_s : u"lock in share mode"_s;

    return std::get<QString>(lock);
}

QString MySqlGrammar::compileRandom(const QString &seed) const
{
    return u"RAND(%1)"_s.arg(seed);
}

const std::unordered_set<QString> &MySqlGrammar::getOperators() const
{
    static const std::unordered_set<QString> cachedOperators {
        u"sounds like"_s,
    };

    return cachedOperators;
}

/* protected */

QString MySqlGrammar::wrapValue(QString value) const
{
    if (value == ASTERISK_C)
        return value;

    return u"`%1`"_s.arg(value.replace(u"`"_s, u"``"_s));
}

const QList<Grammar::SelectComponentValue> &
MySqlGrammar::getCompileMap() const
{
    /* Needed, because some compileXx() methods are overloaded, this way I will capture
       'this' reference and the compileMethod rvalue reference in the following lambda
       and simply save std::function<> in the SelectComponentValue's compileMethod data
       member. */
    const auto bind = [](auto &&compileMethod)
    {
        return [compileMethod = std::forward<decltype (compileMethod)>(compileMethod)]
               (const Grammar &grammar, const QueryBuilder &query)
        {
            /* We can be at 100% sure that this is the MySqlGrammar instance because
               this method is virtual; used the reinterpret_cast<> to avoid useless
               and slower dynamic_cast<>. */
            return std::invoke(compileMethod,
                               reinterpret_cast<const MySqlGrammar &>(grammar), query); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        };
    };

    // Pointers to compile methods, yes yes c++ 😂
    static const QList<SelectComponentValue> cached {
        {bind(&MySqlGrammar::compileAggregate),
         [](const auto &query) { return shouldCompileAggregate(query.getAggregate()); }},
        {bind(&MySqlGrammar::compileColumns),
         [](const auto &query) { return shouldCompileColumns(query); }},
        {bind(&MySqlGrammar::compileFrom),
         [](const auto &query) { return shouldCompileFrom(query.getFrom()); }},
        {bind(&MySqlGrammar::compileJoins),
         [](const auto &query) { return !query.getJoins().isEmpty(); }},
        {bind(&MySqlGrammar::compileWheres),
         [](const auto &query) { return !query.getWheres().isEmpty(); }},
        {bind(&MySqlGrammar::compileGroups),
         [](const auto &query) { return !query.getGroups().isEmpty(); }},
        {bind(&MySqlGrammar::compileHavings),
         [](const auto &query) { return !query.getHavings().isEmpty(); }},
        {bind(&MySqlGrammar::compileOrders),
         [](const auto &query) { return !query.getOrders().isEmpty(); }},
        {bind(&MySqlGrammar::compileLimit),
         [](const auto &query) { return query.getLimit() > -1; }},
        {bind(&MySqlGrammar::compileOffset),
         [](const auto &query) { return query.getOffset() > -1; }},
        {bind(&MySqlGrammar::compileLock),
         [](const auto &query) { return query.getLock().index() != 0; }},
    };

    // TODO correct way to return const & for cached (static) local variable for QHash/QMap, check all 👿🤔 silverqx
    return cached;
}

const Grammar::WhereMemFn &
MySqlGrammar::getWhereMethod(const WhereType whereType) const
{
    /* Needed, because some compileXx() methods are overloaded, this way I will capture
       'this' reference and the compileMethod rvalue reference in the following lambda
       and simply save std::function<> in the SelectComponentValue's compileMethod data
       member. */
    const auto bind = [](auto &&compileMethod)
    {
        return [compileMethod = std::forward<decltype (compileMethod)>(compileMethod)]
               (const Grammar &grammar, const WhereConditionItem &where)
        {
            /* We can be at 100% sure that this is the MySqlGrammar instance because
               this method is virtual; used the reinterpret_cast<> to avoid useless
               and slower dynamic_cast<>. */
            return std::invoke(compileMethod,
                               reinterpret_cast<const MySqlGrammar &>(grammar), where); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        };
    };

    /* Pointers to a where member methods by whereType, yes yes c++. 😂
       An order has to be the same as in enum struct WhereType.
       QList is ideal for this as we lookup using the index. */
    static const QList<WhereMemFn> cached {
        bind(&MySqlGrammar::whereBasic),
        bind(&MySqlGrammar::whereNested),
        bind(&MySqlGrammar::whereColumn),
        bind(&MySqlGrammar::whereIn),
        bind(&MySqlGrammar::whereNotIn),
        bind(&MySqlGrammar::whereNull),
        bind(&MySqlGrammar::whereNotNull),
        bind(&MySqlGrammar::whereRaw),
        bind(&MySqlGrammar::whereExists),
        bind(&MySqlGrammar::whereNotExists),
        bind(&MySqlGrammar::whereRowValues),
        bind(&MySqlGrammar::whereBetween),
        bind(&MySqlGrammar::whereBetweenColumns),
        bind(&MySqlGrammar::whereDate),
        bind(&MySqlGrammar::whereTime),
        bind(&MySqlGrammar::whereDay),
        bind(&MySqlGrammar::whereMonth),
        bind(&MySqlGrammar::whereYear),
    };

    static const auto size = cached.size();

    // Check if whereType is in the range, just for sure 😏
    using SizeType = std::remove_const_t<decltype (cached)>::size_type;
    const auto type = static_cast<SizeType>(whereType);
    Q_ASSERT((type >= 0) && (type < size));

    return cached.at(type);
}

QString
MySqlGrammar::compileUpdateWithoutJoins(
        const QueryBuilder &query, const QString &table,
        const QString &columns, const QString &wheres) const
{
    // The table argument is already wrapped
    auto sql = Grammar::compileUpdateWithoutJoins(query, table, columns, wheres);

    /* When using MySQL, udpate statements may contain order by statements and limits
       so we will compile both of those here. */
    if (!query.getOrders().isEmpty())
        sql += u" %1"_s.arg(compileOrders(query));

    if (query.getLimit() > -1)
        sql += u" %1"_s.arg(compileLimit(query));

    return sql;
}

QString
MySqlGrammar::compileDeleteWithoutJoins(const QueryBuilder &query, const QString &table,
                                        const QString &wheres) const
{
    // The table argument is already wrapped
    auto sql = Grammar::compileDeleteWithoutJoins(query, table, wheres);

    /* When using MySQL, delete statements may contain order by statements and limits
       so we will compile both of those here. Once we have finished compiling this
       we will return the completed SQL statement so it will be executed for us. */
    if (!query.getOrders().isEmpty())
        sql += u" %1"_s.arg(compileOrders(query));

    if (query.getLimit() > -1)
        sql += u" %1"_s.arg(compileLimit(query));

    return sql;
}

} // namespace Orm::Query::Grammars

TINYORM_END_COMMON_NAMESPACE
