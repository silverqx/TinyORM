#include "orm/query/grammars/sqlitegrammar.hpp"

#include "orm/query/querybuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query::Grammars
{

QString SQLiteGrammar::compileInsertOrIgnore(const QueryBuilder &query,
                                             const QVector<QVariantMap> &values) const
{
    return compileInsert(query, values)
            .replace(0, 6, QStringLiteral("insert or ignore"));
}

QString SQLiteGrammar::compileUpdate(QueryBuilder &query,
                                     const QVector<UpdateItem> &values) const
{
    if (!query.getJoins().isEmpty() || query.getLimit() > -1)
        return compileUpdateWithJoinsOrLimit(query, values);

    return Grammar::compileUpdate(query, values);
}

QString SQLiteGrammar::compileDelete(QueryBuilder &query) const
{
    if (!query.getJoins().isEmpty() || query.getLimit() > -1)
        return compileDeleteWithJoinsOrLimit(query);

    return Grammar::compileDelete(query);
}

std::unordered_map<QString, QVector<QVariant>>
SQLiteGrammar::compileTruncate(const QueryBuilder &query) const
{
    const auto table = wrapTable(query.getFrom());

    return {
        {QStringLiteral("delete from sqlite_sequence where name = ?"), {table}},
        {QStringLiteral("delete from %1").arg(table), {}},
    };
}

QString SQLiteGrammar::compileLock(const QueryBuilder &) const
{
    return QLatin1String("");
}

const QVector<QString> &SQLiteGrammar::getOperators() const
{
    static const QVector<QString> cachedOperators {
        EQ, LT, GT, LE, GE, NE_, NE,
        LIKE, NLIKE, ILIKE,
        B_AND, B_OR, "<<", ">>",
    };

    return cachedOperators;
}

QString SQLiteGrammar::compileUpdateColumns(const QVector<UpdateItem> &values) const
{
    QStringList compiledAssignments;
    compiledAssignments.reserve(values.size());

    for (const auto &assignment : values)
        compiledAssignments << QStringLiteral("%1 = %2").arg(
                                   wrap(unqualifyColumn(assignment.column)),
                                   parameter(assignment.value));

    return compiledAssignments.join(COMMA);
}

const QMap<Grammar::SelectComponentType, Grammar::SelectComponentValue> &
SQLiteGrammar::getCompileMap() const
{
    using std::placeholders::_1;
    // Needed, because some compileXx() methods are overloaded
    const auto getBind = [this](auto &&func)
    {
        return std::bind(std::forward<decltype (func)>(func), this, _1);
    };

    // Pointers to a where member methods by whereType, yes yes c++ 😂
    static const QMap<SelectComponentType, SelectComponentValue> cached {
        {SelectComponentType::AGGREGATE, {getBind(&SQLiteGrammar::compileAggregate),
                        [this]
                        (const auto &query)
                        { return shouldCompileAggregate(query.getAggregate()); }}},
        {SelectComponentType::COLUMNS,   {getBind(&SQLiteGrammar::compileColumns),
                        [this]
                        (const auto &query) { return shouldCompileColumns(query); }}},
        {SelectComponentType::FROM,      {getBind(&SQLiteGrammar::compileFrom),
                        [this]
                        (const auto &query)
                        { return shouldCompileFrom(query.getFrom()); }}},
        {SelectComponentType::JOINS,     {getBind(&SQLiteGrammar::compileJoins),
                        [](const auto &query) { return !query.getJoins().isEmpty(); }}},
        {SelectComponentType::WHERES,    {getBind(&SQLiteGrammar::compileWheres),
                        [](const auto &query) { return !query.getWheres().isEmpty(); }}},
        {SelectComponentType::GROUPS,    {getBind(&SQLiteGrammar::compileGroups),
                        [](const auto &query) { return !query.getGroups().isEmpty(); }}},
        {SelectComponentType::HAVINGS,   {getBind(&SQLiteGrammar::compileHavings),
                        [](const auto &query) { return !query.getHavings().isEmpty(); }}},
        {SelectComponentType::ORDERS,    {getBind(&SQLiteGrammar::compileOrders),
                        [](const auto &query) { return !query.getOrders().isEmpty(); }}},
        {SelectComponentType::LIMIT,     {getBind(&SQLiteGrammar::compileLimit),
                        [](const auto &query) { return query.getLimit() > -1; }}},
        {SelectComponentType::OFFSET,    {getBind(&SQLiteGrammar::compileOffset),
                        [](const auto &query) { return query.getOffset() > -1; }}},
        {SelectComponentType::LOCK,      {getBind(&SQLiteGrammar::compileLock),
                        [](const auto &query) { return query.getLock().index() != 0; }}},
    };

    return cached;
}

const std::function<QString(const WhereConditionItem &)> &
SQLiteGrammar::getWhereMethod(const WhereType whereType) const
{
    using std::placeholders::_1;
    const auto getBind = [this](auto &&func)
    {
        return std::bind(std::forward<decltype (func)>(func), this, _1);
    };

    // Pointers to a where member methods by whereType, yes yes c++ 😂
    // An order has to be the same as in enum struct WhereType
    static const QVector<std::function<QString(const WhereConditionItem &)>> cached {
        getBind(&SQLiteGrammar::whereBasic),
        getBind(&SQLiteGrammar::whereNested),
        getBind(&SQLiteGrammar::whereColumn),
        getBind(&SQLiteGrammar::whereIn),
        getBind(&SQLiteGrammar::whereNotIn),
        getBind(&SQLiteGrammar::whereNull),
        getBind(&SQLiteGrammar::whereNotNull),
        getBind(&SQLiteGrammar::whereRaw),
        getBind(&SQLiteGrammar::whereExists),
        getBind(&SQLiteGrammar::whereNotExists),
    };

    static const auto size = cached.size();

    // Check if whereType is in the range, just for sure 😏
    const auto type = static_cast<int>(whereType);
    Q_ASSERT((0 <= type) && (type < size));

    return cached.at(type);
}

QString
SQLiteGrammar::compileUpdateWithJoinsOrLimit(QueryBuilder &query,
                                             const QVector<UpdateItem> &values) const
{
    const auto table = std::get<QString>(query.getFrom());

    const auto tableWrapped = wrapTable(table);

    const auto columns = compileUpdateColumns(values);

    const auto alias = getAliasFromFrom(table);

    const auto selectSql = compileSelect(
                               query.select(QStringLiteral("%1.rowid").arg(alias)));

    return QStringLiteral("update %1 set %2 where %3 in (%4)")
            .arg(tableWrapped, columns, wrap(QStringLiteral("rowid")), selectSql);
}

QString SQLiteGrammar::compileDeleteWithJoinsOrLimit(QueryBuilder &query) const
{
    const auto table = std::get<QString>(query.getFrom());

    const auto tableWrapped = wrapTable(table);

    const auto alias = getAliasFromFrom(table);

    const auto selectSql = compileSelect(
                               query.select(QStringLiteral("%1.rowid").arg(alias)));

    return QStringLiteral("delete from %1 where %2 in (%3)")
            .arg(tableWrapped, wrap(QStringLiteral("rowid")), selectSql);
}

} // namespace Orm::Query::Grammars

TINYORM_END_COMMON_NAMESPACE
