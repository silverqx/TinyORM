#include "orm/query/grammars/postgresgrammar.hpp"

#include "orm/macros/threadlocal.hpp"
#include "orm/query/querybuilder.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query::Grammars
{

/* public */

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
                 wrap(sequence.isEmpty() ? ID : sequence));
}

QString PostgresGrammar::compileUpdate(QueryBuilder &query,
                                       const QVector<UpdateItem> &values) const
{
    if (!query.getJoins().isEmpty() || query.getLimit() > -1)
        return compileUpdateWithJoinsOrLimit(query, values);

    return Grammar::compileUpdate(query, values);
}

QString PostgresGrammar::compileUpsert(
            QueryBuilder &query, const QVector<QVariantMap> &values,
            const QStringList &uniqueBy, const QStringList &update) const
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
        return std::get<bool>(lock) ? QStringLiteral("for update")
                                    : QStringLiteral("for share");

    return std::get<QString>(lock);
}

const QVector<QString> &PostgresGrammar::getOperators() const
{
    static const QVector<QString> cachedOperators {
        EQ, LT, GT, LE, GE, NE, NE_,
        LIKE, NLIKE, "between", ILIKE, "not ilike",
        "~", B_AND, B_OR, "#", "<<", ">>", "<<=", ">>=",
        AND_, "@>", "<@", "?", "?|", "?&", OR_, "-", "@?", "@@", "#-",
        "is distinct from", "is not distinct from",
    };

    return cachedOperators;
}

QString PostgresGrammar::whereBasic(const WhereConditionItem &where) const
{
    if (!where.comparison.contains(LIKE, Qt::CaseInsensitive))
        return Grammar::whereBasic(where);

    return QStringLiteral("%1::text %2 %3").arg(wrap(where.column),
                                                where.comparison,
                                                parameter(where.value));
}

/* protected */

const QMap<Grammar::SelectComponentType, Grammar::SelectComponentValue> &
PostgresGrammar::getCompileMap() const
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
        {SelectComponentType::AGGREGATE, {bind(&PostgresGrammar::compileAggregate),
                        [this]
                        (const auto &query)
                        { return shouldCompileAggregate(query.getAggregate()); }}},
        {SelectComponentType::COLUMNS,   {bind(&PostgresGrammar::compileColumns),
                        [this]
                        (const auto &query) { return shouldCompileColumns(query); }}},
        {SelectComponentType::FROM,      {bind(&PostgresGrammar::compileFrom),
                        [this]
                        (const auto &query)
                        { return shouldCompileFrom(query.getFrom()); }}},
        {SelectComponentType::JOINS,     {bind(&PostgresGrammar::compileJoins),
                        [](const auto &query) { return !query.getJoins().isEmpty(); }}},
        {SelectComponentType::WHERES,    {bind(&PostgresGrammar::compileWheres),
                        [](const auto &query) { return !query.getWheres().isEmpty(); }}},
        {SelectComponentType::GROUPS,    {bind(&PostgresGrammar::compileGroups),
                        [](const auto &query) { return !query.getGroups().isEmpty(); }}},
        {SelectComponentType::HAVINGS,   {bind(&PostgresGrammar::compileHavings),
                        [](const auto &query) { return !query.getHavings().isEmpty(); }}},
        {SelectComponentType::ORDERS,    {bind(&PostgresGrammar::compileOrders),
                        [](const auto &query) { return !query.getOrders().isEmpty(); }}},
        {SelectComponentType::LIMIT,     {bind(&PostgresGrammar::compileLimit),
                        [](const auto &query) { return query.getLimit() > -1; }}},
        {SelectComponentType::OFFSET,    {bind(&PostgresGrammar::compileOffset),
                        [](const auto &query) { return query.getOffset() > -1; }}},
        {SelectComponentType::LOCK,      {bind(&PostgresGrammar::compileLock),
                        [](const auto &query) { return query.getLock().index() != 0; }}},
    };

    return cached;
}

const std::function<QString(const WhereConditionItem &)> &
PostgresGrammar::getWhereMethod(const WhereType whereType) const
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
        bind(&PostgresGrammar::whereBasic),
        bind(&PostgresGrammar::whereNested),
        bind(&PostgresGrammar::whereColumn),
        bind(&PostgresGrammar::whereIn),
        bind(&PostgresGrammar::whereNotIn),
        bind(&PostgresGrammar::whereNull),
        bind(&PostgresGrammar::whereNotNull),
        bind(&PostgresGrammar::whereRaw),
        bind(&PostgresGrammar::whereExists),
        bind(&PostgresGrammar::whereNotExists),
        bind(&PostgresGrammar::whereRowValues),
        bind(&PostgresGrammar::whereBetween),
        bind(&PostgresGrammar::whereBetweenColumns),
        bind(&PostgresGrammar::whereDate),
        bind(&PostgresGrammar::whereTime),
        bind(&PostgresGrammar::whereDay),
        bind(&PostgresGrammar::whereMonth),
        bind(&PostgresGrammar::whereYear),
    };

    T_THREAD_LOCAL
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
        select.append(QStringLiteral("select distinct "));

    else
        select.append(QStringLiteral("select "));

    return select.append(columnize(query.getColumns()));
}

QString PostgresGrammar::whereDate(const WhereConditionItem &where) const
{
    return QStringLiteral("%1::date %3 %4").arg(wrap(where.column),
                                                where.comparison,
                                                parameter(where.value));
}

QString PostgresGrammar::whereTime(const WhereConditionItem &where) const
{
    return QStringLiteral("%1::time %3 %4").arg(wrap(where.column),
                                                where.comparison,
                                                parameter(where.value));
}

QString PostgresGrammar::dateBasedWhere(const QString &type,
                                        const WhereConditionItem &where) const
{
    return QStringLiteral("extract(%1 from %2) %3 %4").arg(type,
                                                           wrap(where.column),
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

    return columnizeWithoutWrap(compiledAssignments);
}

/* private */

QString PostgresGrammar::compileUpdateWithJoinsOrLimit(
        QueryBuilder &query, const QVector<UpdateItem> &values) const
{
    const auto table = std::get<QString>(query.getFrom());

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
    const auto table = std::get<QString>(query.getFrom());

    const auto tableWrapped = wrapTable(table);

    const auto alias = getAliasFromFrom(table);

    const auto selectSql = compileSelect(
                               query.select(QStringLiteral("%1.ctid").arg(alias)));

    return QStringLiteral("delete from %1 where %2 in (%3)")
            .arg(tableWrapped, wrap(QStringLiteral("ctid")), selectSql);
}

} // namespace Orm::Query::Grammars

TINYORM_END_COMMON_NAMESPACE
