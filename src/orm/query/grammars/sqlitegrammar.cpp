#include "orm/query/grammars/sqlitegrammar.hpp"

#include "orm/macros/threadlocal.hpp"
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

QString SQLiteGrammar::compileUpsert(
            QueryBuilder &query, const QVector<QVariantMap> &values,
            const QVector<QString> &uniqueBy, const QVector<QString> &update) const
{
    auto sql = compileInsert(query, values);

    sql += QStringLiteral(" on conflict (%1) do update set ").arg(columnize(uniqueBy));

    QStringList columns;
    columns.reserve(update.size());

    for (const auto &column : update)
        columns << QStringLiteral("%1 = %2")
                   .arg(wrap(column),
                        DOT_IN.arg(wrapValue(QStringLiteral("excluded")),
                                   wrap(column)));

    return NOSPACE.arg(std::move(sql), columns.join(COMMA));
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

QString SQLiteGrammar::compileLock(const QueryBuilder &/*unused*/) const
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

    return columnizeWithoutWrap(compiledAssignments);
}

const QMap<Grammar::SelectComponentType, Grammar::SelectComponentValue> &
SQLiteGrammar::getCompileMap() const
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
        {SelectComponentType::AGGREGATE, {bind(&SQLiteGrammar::compileAggregate),
                        [this]
                        (const auto &query)
                        { return shouldCompileAggregate(query.getAggregate()); }}},
        {SelectComponentType::COLUMNS,   {bind(&SQLiteGrammar::compileColumns),
                        [this]
                        (const auto &query) { return shouldCompileColumns(query); }}},
        {SelectComponentType::FROM,      {bind(&SQLiteGrammar::compileFrom),
                        [this]
                        (const auto &query)
                        { return shouldCompileFrom(query.getFrom()); }}},
        {SelectComponentType::JOINS,     {bind(&SQLiteGrammar::compileJoins),
                        [](const auto &query) { return !query.getJoins().isEmpty(); }}},
        {SelectComponentType::WHERES,    {bind(&SQLiteGrammar::compileWheres),
                        [](const auto &query) { return !query.getWheres().isEmpty(); }}},
        {SelectComponentType::GROUPS,    {bind(&SQLiteGrammar::compileGroups),
                        [](const auto &query) { return !query.getGroups().isEmpty(); }}},
        {SelectComponentType::HAVINGS,   {bind(&SQLiteGrammar::compileHavings),
                        [](const auto &query) { return !query.getHavings().isEmpty(); }}},
        {SelectComponentType::ORDERS,    {bind(&SQLiteGrammar::compileOrders),
                        [](const auto &query) { return !query.getOrders().isEmpty(); }}},
        {SelectComponentType::LIMIT,     {bind(&SQLiteGrammar::compileLimit),
                        [](const auto &query) { return query.getLimit() > -1; }}},
        {SelectComponentType::OFFSET,    {bind(&SQLiteGrammar::compileOffset),
                        [](const auto &query) { return query.getOffset() > -1; }}},
        {SelectComponentType::LOCK,      {bind(&SQLiteGrammar::compileLock),
                        [](const auto &query) { return query.getLock().index() != 0; }}},
    };

    return cached;
}

const std::function<QString(const WhereConditionItem &)> &
SQLiteGrammar::getWhereMethod(const WhereType whereType) const
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
    T_THREAD_LOCAL
    static const QVector<std::function<QString(const WhereConditionItem &)>> cached {
        bind(&SQLiteGrammar::whereBasic),
        bind(&SQLiteGrammar::whereNested),
        bind(&SQLiteGrammar::whereColumn),
        bind(&SQLiteGrammar::whereIn),
        bind(&SQLiteGrammar::whereNotIn),
        bind(&SQLiteGrammar::whereNull),
        bind(&SQLiteGrammar::whereNotNull),
        bind(&SQLiteGrammar::whereRaw),
        bind(&SQLiteGrammar::whereExists),
        bind(&SQLiteGrammar::whereNotExists),
        bind(&SQLiteGrammar::whereRowValues),
    };

    T_THREAD_LOCAL
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
