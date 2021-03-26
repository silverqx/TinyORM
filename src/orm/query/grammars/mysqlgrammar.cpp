#include "orm/query/grammars/mysqlgrammar.hpp"

#include "orm/query/querybuilder.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
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

const QVector<QString> &MySqlGrammar::getOperators() const
{
    static const QVector<QString> cachedOperators {"sounds like"};

    return cachedOperators;
}

QString
MySqlGrammar::compileUpdateWithoutJoins(
        const QueryBuilder &query, const QString &table,
        const QString &columns, const QString &wheres) const
{
    auto sql = Grammar::compileUpdateWithoutJoins(query, table, columns, wheres);

    /* When using MySQL, udpate statements may contain order by statements and limits
       so we will compile both of those here. */
    if (!query.getOrders().isEmpty())
        sql += QChar(' ') + compileOrders(query);

    if (query.getLimit() > -1)
        sql += QChar(' ') + compileLimit(query);

    return sql;
}

QString
MySqlGrammar::compileDeleteWithoutJoins(const QueryBuilder &query, const QString &table,
                                        const QString &wheres) const
{
    auto sql = Grammar::compileDeleteWithoutJoins(query, table, wheres);

    /* When using MySQL, delete statements may contain order by statements and limits
       so we will compile both of those here. Once we have finished compiling this
       we will return the completed SQL statement so it will be executed for us. */
    if (!query.getOrders().isEmpty())
        sql += QChar(' ') + compileOrders(query);

    if (query.getLimit() > -1)
        sql += QChar(' ') + compileLimit(query);

    return sql;
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
