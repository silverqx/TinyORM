#include "orm/exceptions/queryerror.hpp"

#include <QtSql/QSqlQuery>

#include "orm/utils/query.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using QueryUtils = Orm::Utils::Query;

namespace Orm::Exceptions
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

const QString &QueryError::getSql() const
{
    return m_sql;
}

const QVector<QVariant> &QueryError::getBindings() const
{
    return m_bindings;
}

QString QueryError::formatMessage(const char *message, const QSqlQuery &query)
{
    // Format SQL error message
    QString result(SqlError::formatMessage(message, query.lastError()));

    // Also append executed query
    if (const auto executedQuery = QueryUtils::parseExecutedQuery(query);
        !executedQuery.isEmpty()
    )
        result += QStringLiteral(", SQL: %1").arg(executedQuery);

    return result;
}

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE
