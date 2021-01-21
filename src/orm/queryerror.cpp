#include "orm/queryerror.hpp"

#include <QtSql/QSqlQuery>

#include "orm/logquery.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

QueryError::QueryError(const char *message, const QSqlQuery &query,
                       const QVector<QVariant> &bindings)
    : SqlError(formatMessage(message, query), query.lastError(), 1)
    , m_sql(query.executedQuery())
    , m_bindings(bindings)
{}

QueryError::QueryError(const QString &message, const QSqlQuery &query,
                       const QVector<QVariant> &bindings)
    : QueryError(message.toUtf8().constData(), query, bindings)
{}

const QString &Orm::QueryError::getSql() const
{
    return m_sql;
}

const QVector<QVariant> &Orm::QueryError::getBindings() const
{
    return m_bindings;
}

QString QueryError::formatMessage(const char *message, const QSqlQuery &query)
{
    // Format SQL error message
    QString result(SqlError::formatMessage(message, query.lastError()));

    // Also append executed query
    result += QStringLiteral(", SQL: ") + parseExecutedQuery(query);

    return result;
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
