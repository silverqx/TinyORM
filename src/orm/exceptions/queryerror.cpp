#include "orm/exceptions/queryerror.hpp"

#include <QtSql/QSqlQuery>

#include "orm/utils/query.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using QueryUtils = Orm::Utils::Query;

namespace Orm::Exceptions
{

/* We don't need the Orm::SqlQuery overload as all bindings are already prepared.
   Used the QSqlQuery directly to make it more clear that TSqlXyz mappings are not
   needed. */

/* public */

QueryError::QueryError(QString connectionName, const char *message,
                       const QSqlQuery &query, const QList<QVariant> &bindings)
    : SqlError(formatMessage(connectionName, message, query), query.lastError(), 1)
    , m_connectionName(std::move(connectionName))
    , m_sql(query.executedQuery())
    , m_bindings(bindings)
{}

QueryError::QueryError(QString connectionName, const QString &message,
                       const QSqlQuery &query, const QList<QVariant> &bindings)
    : QueryError(std::move(connectionName), message.toUtf8().constData(), query, bindings)
{}

/* protected */

QString QueryError::formatMessage(const QString &connectionName, const char *message,
                                  const QSqlQuery &query)
{
    const auto sqlError = SqlError::formatMessage(message, query.lastError());
    const auto executedQuery = QueryUtils::parseExecutedQuery(query);

    // Format SQL error message
    QString result;
    // +14 and +7 are QStringLiteral-s and +32 as a reserve
    result.reserve(sqlError.size() +
                   14 + connectionName.size() +
                    7 + executedQuery.size() + 32);

    result += sqlError;

    // Connection name
    result += QStringLiteral(", Connection: %1").arg(connectionName);

    // Executed SQL query
    if (!executedQuery.isEmpty())
        result += QStringLiteral(", SQL: %1").arg(executedQuery);

    return result;
}

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE
