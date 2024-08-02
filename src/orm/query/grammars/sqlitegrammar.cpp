#include "orm/query/grammars/sqlitegrammar.hpp"

#include "orm/exceptions/invalidargumenterror.hpp"
#include "orm/query/querybuilder.hpp" // IWYU pragma: keep
#include "orm/utils/type.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Query::Grammars
{

/* public */

QString SQLiteGrammar::compileInsertOrIgnore(const QueryBuilder &query,
                                             const QList<QVariantMap> &values) const
{
    return compileInsert(query, values)
            .replace(0, 6, QStringLiteral("insert or ignore"));
}

QString SQLiteGrammar::compileUpdate(QueryBuilder &query,
                                     const QList<UpdateItem> &values) const
{
    if (!query.getJoins().isEmpty() || query.getLimit() > -1)
        return compileUpdateWithJoinsOrLimit(query, values);

    return Grammar::compileUpdate(query, values);
}

QString SQLiteGrammar::compileUpsert(
            QueryBuilder &query, const QList<QVariantMap> &values,
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

    return NOSPACE.arg(sql, columns.join(COMMA));
}

QString SQLiteGrammar::compileDelete(QueryBuilder &query) const
{
    if (!query.getJoins().isEmpty() || query.getLimit() > -1)
        return compileDeleteWithJoinsOrLimit(query);

    return Grammar::compileDelete(query);
}

std::unordered_map<QString, QList<QVariant>>
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
    return EMPTY;
}

const std::unordered_set<QString> &SQLiteGrammar::getOperators() const
{
    static const std::unordered_set<QString> cachedOperators {
        EQ, LT, GT, LE, GE, NE, NE_,
        LIKE, NLIKE, ILIKE,
        B_AND, B_OR, QLatin1String("<<"), QLatin1String(">>"),
    };

    return cachedOperators;
}

/* protected */

const QList<Grammar::SelectComponentValue> &
SQLiteGrammar::getCompileMap() const
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
            /* We can be at 100% sure that this is the SQLiteGrammar instance because
               this method is virtual; used the reinterpret_cast<> to avoid useless
               and slower dynamic_cast<>. */
            return std::invoke(compileMethod,
                               reinterpret_cast<const SQLiteGrammar &>(grammar), query); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        };
    };

    // Pointers to compile methods, yes yes c++ 😂
    static const QList<SelectComponentValue> cached {
        {bind(&SQLiteGrammar::compileAggregate),
         [](const auto &query) { return shouldCompileAggregate(query.getAggregate()); }},
        {bind(&SQLiteGrammar::compileColumns),
         [](const auto &query) { return shouldCompileColumns(query); }},
        {bind(&SQLiteGrammar::compileFrom),
         [](const auto &query) { return shouldCompileFrom(query.getFrom()); }},
        {bind(&SQLiteGrammar::compileJoins),
         [](const auto &query) { return !query.getJoins().isEmpty(); }},
        {bind(&SQLiteGrammar::compileWheres),
         [](const auto &query) { return !query.getWheres().isEmpty(); }},
        {bind(&SQLiteGrammar::compileGroups),
         [](const auto &query) { return !query.getGroups().isEmpty(); }},
        {bind(&SQLiteGrammar::compileHavings),
         [](const auto &query) { return !query.getHavings().isEmpty(); }},
        {bind(&SQLiteGrammar::compileOrders),
         [](const auto &query) { return !query.getOrders().isEmpty(); }},
        {bind(&SQLiteGrammar::compileLimit),
         [](const auto &query) { return query.getLimit() > -1; }},
        {bind(&SQLiteGrammar::compileOffset),
         [](const auto &query) { return query.getOffset() > -1; }},
        {bind(&SQLiteGrammar::compileLock),
         [](const auto &query) { return query.getLock().index() != 0; }},
    };

    return cached;
}

const Grammar::WhereMemFn &
SQLiteGrammar::getWhereMethod(const WhereType whereType) const
{
    /* Needed, because some compileXx() methods are overloaded, this way I will capture
       'this' reference and the compileMethod rvalue reference in the following lambda
       and simply save std::function<> in the SelectComponentValue's compileMethod data
       member. */
    const auto bind = [](auto &&compileMethod)
    {
        return [compileMethod = std::forward<decltype (compileMethod)>(compileMethod)]
               (const Grammar &grammar, const WhereConditionItem &query)
        {
            /* We can be at 100% sure that this is the SQLiteGrammar instance because
               this method is virtual; used the reinterpret_cast<> to avoid useless
               and slower dynamic_cast<>. */
            return std::invoke(compileMethod,
                               reinterpret_cast<const SQLiteGrammar &>(grammar), query); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        };
    };

    /* Pointers to a where member methods by whereType, yes yes c++. 😂
       An order has to be the same as in enum struct WhereType.
       QList is ideal for this as we lookup using the index. */
    static const QList<WhereMemFn> cached {
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
        bind(&SQLiteGrammar::whereBetween),
        bind(&SQLiteGrammar::whereBetweenColumns),
        bind(&SQLiteGrammar::whereDate),
        bind(&SQLiteGrammar::whereTime),
        bind(&SQLiteGrammar::whereDay),
        bind(&SQLiteGrammar::whereMonth),
        bind(&SQLiteGrammar::whereYear),
    };

    static const auto size = cached.size();

    // Check if whereType is in the range, just for sure 😏
    using SizeType = std::remove_const_t<decltype (cached)>::size_type;
    const auto type = static_cast<SizeType>(whereType);
    Q_ASSERT((type >= 0) && (type < size));

    return cached.at(type);
}

QString SQLiteGrammar::whereDate(const WhereConditionItem &where) const
{
    return dateBasedWhere(QStringLiteral("%Y-%m-%d"), where);
}

QString SQLiteGrammar::whereTime(const WhereConditionItem &where) const
{
    return dateBasedWhere(QStringLiteral("%H:%M:%S"), where);
}

QString SQLiteGrammar::whereDay(const WhereConditionItem &where) const
{
    return dateBasedWhere(QStringLiteral("%d"), where);
}

QString SQLiteGrammar::whereMonth(const WhereConditionItem &where) const
{
    return dateBasedWhere(QStringLiteral("%m"), where);
}

QString SQLiteGrammar::whereYear(const WhereConditionItem &where) const
{
    return dateBasedWhere(QStringLiteral("%Y"), where);
}

QString SQLiteGrammar::dateBasedWhere(const QString &type,
                                      const WhereConditionItem &where) const
{
    return QStringLiteral("%1 %2 %3")
            .arg(dateBasedWhereColumn(type, where), where.comparison,
                 parameter(where.value));
}

QString SQLiteGrammar::dateBasedWhereColumn(const QString &type,
                                            const WhereConditionItem &where) const
{
    switch (where.type) {
    // Compare as text types
    case WhereType::DATE:
    case WhereType::TIME:
        Q_ASSERT(where.value.typeId() == QMetaType::QString);

        return QStringLiteral("strftime('%1', %2)").arg(type, wrap(where.column));

    // Compare as integral types
    case WhereType::DAY:
    case WhereType::MONTH:
    case WhereType::YEAR:
        Q_ASSERT(where.value.typeId() == QMetaType::Int);

        return QStringLiteral("cast(strftime('%1', %2) as integer)")
                .arg(type, wrap(where.column));

    default:
        throw Exceptions::InvalidArgumentError(
                    QStringLiteral("Wrong value for enum struct WhereType in %1().")
                    .arg(__tiny_func__));
    }
}

QString SQLiteGrammar::compileUpdateColumns(const QList<UpdateItem> &values) const
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

QString
SQLiteGrammar::compileUpdateWithJoinsOrLimit(QueryBuilder &query,
                                             const QList<UpdateItem> &values) const
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
