#include "orm/query/grammars/postgresgrammar.hpp"

#include "orm/query/querybuilder.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Query::Grammars
{

QString PostgresGrammar::compileInsertOrIgnore(const QueryBuilder &query,
                                               const QVector<QVariantMap> &values) const
{
    return QStringLiteral("%1 on conflict do nothing").arg(compileInsert(query, values));
}

QString PostgresGrammar::compileInsertGetId(const QueryBuilder &query,
                                            const QVector<QVariantMap> &values,
                                            const QString &sequence) const
{
    return QStringLiteral("%1 returning %2")
            .arg(compileInsert(query, values),
                 wrap(sequence.isEmpty() ? "id" : sequence));
}

QString PostgresGrammar::compileUpdate(QueryBuilder &query,
                                       const QVector<UpdateItem> &values) const
{
    if (!query.getJoins().isEmpty() || query.getLimit() > -1)
        return compileUpdateWithJoinsOrLimit(query, values);

    return Grammar::compileUpdate(query, values);
}

QString PostgresGrammar::compileDelete(QueryBuilder &query) const
{
    if (!query.getJoins().isEmpty() || query.getLimit() > -1)
        return compileDeleteWithJoinsOrLimit(query);

    return Grammar::compileDelete(query);
}

std::unordered_map<QString, QVector<QVariant>>
PostgresGrammar::compileTruncate(const QueryBuilder &query) const
{
    return {{QStringLiteral("truncate %1 restart identity cascade")
                    .arg(wrapTable(query.getFrom())),
            {}}};
}

QString PostgresGrammar::compileLock(const QueryBuilder &query) const
{
    const auto &lock = query.getLock();

    if (!std::holds_alternative<QString>(lock))
        return std::get<bool>(lock) ? "for update" : "for share";

    return std::get<QString>(lock);
}

const QVector<QString> &PostgresGrammar::getOperators() const
{
    static const QVector<QString> cachedOperators {
        "=", "<", ">", "<=", ">=", "<>", "!=",
        "like", "not like", "between", "ilike", "not ilike",
        "~", "&", "|", "#", "<<", ">>", "<<=", ">>=",
        "&&", "@>", "<@", "?", "?|", "?&", "||", "-", "@?", "@@", "#-",
        "is distinct from", "is not distinct from",
    };

    return cachedOperators;
}

QString PostgresGrammar::whereBasic(const WhereConditionItem &where) const
{
    if (!where.comparison.contains("like", Qt::CaseInsensitive))
        return Grammar::whereBasic(where);

    return QStringLiteral("%1::text %2 %3").arg(wrap(where.column),
                                                where.comparison,
                                                parameter(where.value));
}

QString PostgresGrammar::compileUpdateColumns(const QVector<UpdateItem> &values) const
{
    QStringList compiledAssignments;
    compiledAssignments.reserve(values.size());

    for (const auto &assignment : values)
        compiledAssignments << QStringLiteral("%1 = %2").arg(
                                   wrap(unqualifyColumn(assignment.column)),
                                   parameter(assignment.value));

    return compiledAssignments.join(", ");
}

const QMap<Grammar::SelectComponentType, Grammar::SelectComponentValue> &
PostgresGrammar::getCompileMap() const
{
    using std::placeholders::_1;
    // Needed, because some compileXx() methods are overloaded
    const auto getBind = [this](auto &&func)
    {
        return std::bind(std::forward<decltype (func)>(func), this, _1);
    };

    // Pointers to a where member methods by whereType, yes yes c++ 😂
    static const QMap<SelectComponentType, SelectComponentValue> cached {
//        {ComponentType::AGGREGATE, {}},
        {SelectComponentType::COLUMNS,   {getBind(&PostgresGrammar::compileColumns),
                        [](const auto &query) { return !query.getColumns().isEmpty(); }}},
        {SelectComponentType::FROM,      {getBind(&PostgresGrammar::compileFrom),
                        [](const auto &query) { return !query.getFrom().isEmpty(); }}},
        {SelectComponentType::JOINS,     {getBind(&PostgresGrammar::compileJoins),
                        [](const auto &query) { return !query.getJoins().isEmpty(); }}},
        {SelectComponentType::WHERES,    {getBind(&PostgresGrammar::compileWheres),
                        [](const auto &query) { return !query.getWheres().isEmpty(); }}},
        {SelectComponentType::GROUPS,    {getBind(&PostgresGrammar::compileGroups),
                        [](const auto &query) { return !query.getGroups().isEmpty(); }}},
        {SelectComponentType::HAVINGS,   {getBind(&PostgresGrammar::compileHavings),
                        [](const auto &query) { return !query.getHavings().isEmpty(); }}},
        {SelectComponentType::ORDERS,    {getBind(&PostgresGrammar::compileOrders),
                        [](const auto &query) { return !query.getOrders().isEmpty(); }}},
        {SelectComponentType::LIMIT,     {getBind(&PostgresGrammar::compileLimit),
                        [](const auto &query) { return query.getLimit() > -1; }}},
        {SelectComponentType::OFFSET,    {getBind(&PostgresGrammar::compileOffset),
                        [](const auto &query) { return query.getOffset() > -1; }}},
        {SelectComponentType::LOCK,      {getBind(&PostgresGrammar::compileLock),
                        [](const auto &query) { return query.getLock().index() != 0; }}},
    };

    return cached;
}

const std::function<QString(const WhereConditionItem &)> &
PostgresGrammar::getWhereMethod(const WhereType whereType) const
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
        getBind(&PostgresGrammar::whereBasic),
        getBind(&PostgresGrammar::whereNested),
        getBind(&PostgresGrammar::whereColumn),
        getBind(&PostgresGrammar::whereIn),
        getBind(&PostgresGrammar::whereNotIn),
        getBind(&PostgresGrammar::whereNull),
        getBind(&PostgresGrammar::whereNotNull),
    };

    static const auto size = cached.size();

    // Check if whereType is in the range, just for sure 😏
    const auto type = static_cast<int>(whereType);
    Q_ASSERT((0 <= type) && (type < size));

    return cached.at(type);
}

QString PostgresGrammar::compileColumns(const QueryBuilder &query) const
{
    QString select;

    const auto &distinct = query.getDistinct();

    if (std::holds_alternative<QStringList>(distinct))
        select.append(QStringLiteral("select distinct on (%1) ")
                      .arg(columnize(std::get<QStringList>(distinct))));

    else if (std::holds_alternative<bool>(distinct) && std::get<bool>(distinct))
        select.append("select distinct ");

    else
        select.append("select ");

    return select.append(columnize(query.getColumns()));
}

QString PostgresGrammar::compileUpdateWithJoinsOrLimit(
        QueryBuilder &query, const QVector<UpdateItem> &values) const
{
    const auto &table = query.getFrom();

    const auto tableWrapped = wrapTable(table);

    const auto columns = compileUpdateColumns(values);

    const auto alias = getAliasFromFrom(table);

    const auto selectSql = compileSelect(
                               query.select(QStringLiteral("%1.ctid").arg(alias)));

    return QStringLiteral("update %1 set %2 where %3 in (%4)")
            .arg(tableWrapped, columns, wrap(QStringLiteral("ctid")), selectSql);
}

QString PostgresGrammar::compileDeleteWithJoinsOrLimit(QueryBuilder &query) const
{
    const auto &table = query.getFrom();

    const auto tableWrapped = wrapTable(table);

    const auto alias = getAliasFromFrom(table);

    const auto selectSql = compileSelect(
                               query.select(QStringLiteral("%1.ctid").arg(alias)));

    return QStringLiteral("delete from %1 where %2 in (%3)")
            .arg(tableWrapped, wrap(QStringLiteral("ctid")), selectSql);
}

} // namespace Orm::Query::Grammars
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
