#include "orm/query/grammars/mysqlgrammar.hpp"

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
    using std::placeholders::_1;
    // Needed, because some compileXx() methods are overloaded
    const auto getBind = [this](auto &&func)
    {
        return std::bind(std::forward<decltype (func)>(func), this, _1);
    };

    // Pointers to a where member methods by whereType, yes yes c++ 😂
    static const QMap<SelectComponentType, SelectComponentValue> cached {
        {SelectComponentType::AGGREGATE, {getBind(&MySqlGrammar::compileAggregate),
                        [this]
                        (const auto &query)
                        { return shouldCompileAggregate(query.getAggregate()); }}},
        {SelectComponentType::COLUMNS,   {getBind(&MySqlGrammar::compileColumns),
                        [this]
                        (const auto &query) { return shouldCompileColumns(query); }}},
        {SelectComponentType::FROM,      {getBind(&MySqlGrammar::compileFrom),
                        [this]
                        (const auto &query)
                        { return shouldCompileFrom(query.getFrom()); }}},
        {SelectComponentType::JOINS,     {getBind(&MySqlGrammar::compileJoins),
                        [](const auto &query) { return !query.getJoins().isEmpty(); }}},
        {SelectComponentType::WHERES,    {getBind(&MySqlGrammar::compileWheres),
                        [](const auto &query) { return !query.getWheres().isEmpty(); }}},
        {SelectComponentType::GROUPS,    {getBind(&MySqlGrammar::compileGroups),
                        [](const auto &query) { return !query.getGroups().isEmpty(); }}},
        {SelectComponentType::HAVINGS,   {getBind(&MySqlGrammar::compileHavings),
                        [](const auto &query) { return !query.getHavings().isEmpty(); }}},
        {SelectComponentType::ORDERS,    {getBind(&MySqlGrammar::compileOrders),
                        [](const auto &query) { return !query.getOrders().isEmpty(); }}},
        {SelectComponentType::LIMIT,     {getBind(&MySqlGrammar::compileLimit),
                        [](const auto &query) { return query.getLimit() > -1; }}},
        {SelectComponentType::OFFSET,    {getBind(&MySqlGrammar::compileOffset),
                        [](const auto &query) { return query.getOffset() > -1; }}},
        {SelectComponentType::LOCK,      {getBind(&MySqlGrammar::compileLock),
                        [](const auto &query) { return query.getLock().index() != 0; }}},
    };

    // TODO correct way to return const & for cached (static) local variable for QHash/QMap, check all 👿🤔 silverqx
    return cached;
}

const std::function<QString(const WhereConditionItem &)> &
MySqlGrammar::getWhereMethod(const WhereType whereType) const
{
    using std::placeholders::_1;
    const auto getBind = [this](auto &&func)
    {
        return std::bind(std::forward<decltype (func)>(func), this, _1);
    };

    // Pointers to a where member methods by whereType, yes yes c++ 😂
    // An order has to be the same as in enum struct WhereType
    // FUTURE QHash would has faster lookup, I should choose QHash, fix also another Grammars silverx
    static const QVector<std::function<QString(const WhereConditionItem &)>> cached {
        getBind(&MySqlGrammar::whereBasic),
        getBind(&MySqlGrammar::whereNested),
        getBind(&MySqlGrammar::whereColumn),
        getBind(&MySqlGrammar::whereIn),
        getBind(&MySqlGrammar::whereNotIn),
        getBind(&MySqlGrammar::whereNull),
        getBind(&MySqlGrammar::whereNotNull),
        getBind(&MySqlGrammar::whereRaw),
        getBind(&MySqlGrammar::whereExists),
        getBind(&MySqlGrammar::whereNotExists),
    };

    static const auto size = cached.size();

    // Check if whereType is in the range, just for sure 😏
    const auto type = static_cast<int>(whereType);
    Q_ASSERT((0 <= type) && (type < size));

    return cached.at(type);
}

} // namespace Orm::Query::Grammars

TINYORM_END_COMMON_NAMESPACE
