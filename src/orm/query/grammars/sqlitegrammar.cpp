#include "orm/query/grammars/sqlitegrammar.hpp"

#include "orm/query/querybuilder.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
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
    const auto &table = query.getFrom();

    return {
        {"delete from sqlite_sequence where name = ?", {table}},
        {QStringLiteral("delete from %1").arg(table), {}},
    };
}

const QVector<QString> &SQLiteGrammar::getOperators() const
{
    static const QVector<QString> cachedOperators {
        "=", "<", ">", "<=", ">=", "<>", "!=",
        "like", "not like", "ilike",
        "&", "|", "<<", ">>",
    };

    return cachedOperators;
}

QString SQLiteGrammar::compileUpdateColumns(const QVector<UpdateItem> &values) const
{
    QStringList compiledAssignments;

    for (const auto &assignment : values)
        compiledAssignments << QStringLiteral("%1 = %2").arg(
                                   unqualifyColumn(assignment.column),
                                   parameter(assignment.value));

    return compiledAssignments.join(", ");
}

QString
SQLiteGrammar::compileUpdateWithJoinsOrLimit(QueryBuilder &query,
                                             const QVector<UpdateItem> &values) const
{
    const auto &table = query.getFrom();

    const auto columns = compileUpdateColumns(values);

    const auto alias = getAliasFromFrom(table);

    const auto selectSql = compileSelect(query.select(alias + ".rowid"));

    return QStringLiteral("update %1 set %2 where %3 in (%4)")
            .arg(table, columns, "rowid", selectSql);
}

QString SQLiteGrammar::compileDeleteWithJoinsOrLimit(QueryBuilder &query) const
{
    const auto &table = query.getFrom();

    const auto alias = getAliasFromFrom(table);

    const auto selectSql = compileSelect(query.select(alias + ".rowid"));

    return QStringLiteral("delete from %1 where %2 in (%3)")
            .arg(table, "rowid", selectSql);
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
