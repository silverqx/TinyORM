#include "orm/query/grammars/mysqlgrammar.hpp"

#include "orm/macros/threadlocal.hpp"
#include "orm/query/querybuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query::Grammars
{

QString MySqlGrammar::compileInsert(const QueryBuilder &query,
                                    const QVector<QVariantMap> &values) const
{
    // MySQL doesn't support 'default values' statement
    if (values.isEmpty())
        return Grammar::compileInsert(query, {{}});

    return Grammar::compileInsert(query, values);
}

QString MySqlGrammar::compileInsertOrIgnore(const QueryBuilder &query,
                                            const QVector<QVariantMap> &values) const
{
    return compileInsert(query, values).replace(0, 6, QStringLiteral("insert ignore"));
}

QString MySqlGrammar::compileLock(const QueryBuilder &query) const
{
    const auto &lock = query.getLock();

    if (!std::holds_alternative<QString>(lock))
        return std::get<bool>(lock) ? QStringLiteral("for update") :
                                      QStringLiteral("lock in share mode");

    return std::get<QString>(lock);
}

const QVector<QString> &MySqlGrammar::getOperators() const
{
    static const QVector<QString> cachedOperators {QLatin1String("sounds like")};

    return cachedOperators;
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
        sql += QStringLiteral(" %1").arg(compileOrders(query));

    if (query.getLimit() > -1)
        sql += QStringLiteral(" %1").arg(compileLimit(query));

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
        sql += QStringLiteral(" %1").arg(compileOrders(query));

    if (query.getLimit() > -1)
        sql += QStringLiteral(" %1").arg(compileLimit(query));

    return sql;
}

QString MySqlGrammar::wrapValue(QString value) const
{
    if (value == ASTERISK_C)
        return value;

    return QStringLiteral("`%1`").arg(value.replace(QStringLiteral("`"),
                                                    QStringLiteral("``")));
}

const QMap<Grammar::SelectComponentType, Grammar::SelectComponentValue> &
MySqlGrammar::getCompileMap() const
{
    /* Needed, because some compileXx() methods are overloaded, this way I will capture
       'this' reference and the compileMethod rvalue reference in the following lambda
       and simply save std::function<> in the SelectComponentValue's compileMethod data
       member. */
    const auto bind = [this](auto &&compileMethod)
    {
        return [this,
                compileMethod = std::forward<decltype (compileMethod)>(compileMethod)]
               (const auto &query)
        {
            return std::invoke(compileMethod, this, query);
        };
    };

    // Pointers to a where member methods by whereType, yes yes c++ 😂
    T_THREAD_LOCAL
    static const QMap<SelectComponentType, SelectComponentValue> cached {
        {SelectComponentType::AGGREGATE, {bind(&MySqlGrammar::compileAggregate),
                        [this]
                        (const auto &query)
                        { return shouldCompileAggregate(query.getAggregate()); }}},
        {SelectComponentType::COLUMNS,   {bind(&MySqlGrammar::compileColumns),
                        [this]
                        (const auto &query) { return shouldCompileColumns(query); }}},
        {SelectComponentType::FROM,      {bind(&MySqlGrammar::compileFrom),
                        [this]
                        (const auto &query)
                        { return shouldCompileFrom(query.getFrom()); }}},
        {SelectComponentType::JOINS,     {bind(&MySqlGrammar::compileJoins),
                        [](const auto &query) { return !query.getJoins().isEmpty(); }}},
        {SelectComponentType::WHERES,    {bind(&MySqlGrammar::compileWheres),
                        [](const auto &query) { return !query.getWheres().isEmpty(); }}},
        {SelectComponentType::GROUPS,    {bind(&MySqlGrammar::compileGroups),
                        [](const auto &query) { return !query.getGroups().isEmpty(); }}},
        {SelectComponentType::HAVINGS,   {bind(&MySqlGrammar::compileHavings),
                        [](const auto &query) { return !query.getHavings().isEmpty(); }}},
        {SelectComponentType::ORDERS,    {bind(&MySqlGrammar::compileOrders),
                        [](const auto &query) { return !query.getOrders().isEmpty(); }}},
        {SelectComponentType::LIMIT,     {bind(&MySqlGrammar::compileLimit),
                        [](const auto &query) { return query.getLimit() > -1; }}},
        {SelectComponentType::OFFSET,    {bind(&MySqlGrammar::compileOffset),
                        [](const auto &query) { return query.getOffset() > -1; }}},
        {SelectComponentType::LOCK,      {bind(&MySqlGrammar::compileLock),
                        [](const auto &query) { return query.getLock().index() != 0; }}},
    };

    // TODO correct way to return const & for cached (static) local variable for QHash/QMap, check all 👿🤔 silverqx
    return cached;
}

const std::function<QString(const WhereConditionItem &)> &
MySqlGrammar::getWhereMethod(const WhereType whereType) const
{
    /* Needed, because some compileXx() methods are overloaded, this way I will capture
       'this' reference and the compileMethod rvalue reference in the following lambda
       and simply save std::function<> in the SelectComponentValue's compileMethod data
       member. */
    const auto bind = [this](auto &&compileMethod)
    {
        return [this,
                compileMethod = std::forward<decltype (compileMethod)>(compileMethod)]
               (const auto &query)
        {
            return std::invoke(compileMethod, this, query);
        };
    };

    // Pointers to a where member methods by whereType, yes yes c++ 😂
    // An order has to be the same as in enum struct WhereType
    // FUTURE QHash would has faster lookup, I should choose QHash, fix also another Grammars silverx
    T_THREAD_LOCAL
    static const QVector<std::function<QString(const WhereConditionItem &)>> cached {
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
    };

    T_THREAD_LOCAL
    static const auto size = cached.size();

    // Check if whereType is in the range, just for sure 😏
    const auto type = static_cast<int>(whereType);
    Q_ASSERT((0 <= type) && (type < size));

    return cached.at(type);
}

} // namespace Orm::Query::Grammars

TINYORM_END_COMMON_NAMESPACE
