#include "orm/drivers/exceptions/queryerror.hpp"

#ifdef TINYDRIVERS_MYSQL_DRIVER
#  include <orm/support/replacebindings.hpp>
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

#ifdef TINYDRIVERS_MYSQL_DRIVER
using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)
#endif

namespace Orm::Drivers::Exceptions
{

/* public */

#ifdef TINYDRIVERS_MYSQL_DRIVER
QueryError::QueryError(const QString &connectionName, const char *message,
                       MySqlErrorType &&mysqlError,
                       const QString &query, const QList<QVariant> &bindings)
    : SqlError(formatMessage(connectionName, message, mysqlError, query, bindings),
               std::move(mysqlError), 1)
    , m_connectionName(connectionName)
    , m_sql(query)
    , m_bindings(bindings)
{}

QueryError::QueryError(const QString &connectionName, const QString &message,
                       MySqlErrorType &&mysqlError,
                       const QString &query, const QList<QVariant> &bindings)
    : QueryError(connectionName, message.toUtf8().constData(), std::move(mysqlError),
                 query, bindings)
{}
#endif // TINYDRIVERS_MYSQL_DRIVER

/* protected */

#ifdef TINYDRIVERS_MYSQL_DRIVER
QString QueryError::formatMessage(
        const QString &connectionName, const char *message,
        const MySqlErrorType &mysqlError, const QString &query,
        const QList<QVariant> &bindings)
{
    const auto mysqlErrorStr = SqlError::formatMessage(message, mysqlError);
    const auto executedQuery = bindings.isEmpty()
                               ? query
                               : Support::ReplaceBindings
                                        ::replaceBindingsInSql(query, bindings).first;
    // Format SQL error message
    QString result;
    // +14 and +7 are QStringLiteral-s and +32 as a reserve
    result.reserve(mysqlErrorStr.size() + 14 + connectionName.size() +
                                           7 + executedQuery.size() + 32);

    result += mysqlErrorStr;

    // Connection name must always be provided
    Q_ASSERT(!connectionName.isEmpty());

    // Connection name
    result += u", Connection: %1"_s.arg(connectionName);

    // Executed SQL query
    if (!executedQuery.isEmpty())
        result += u", SQL: %1"_s.arg(executedQuery);

    return result;
}
#endif // TINYDRIVERS_MYSQL_DRIVER

} // namespace Orm::Drivers::Exceptions

TINYORM_END_COMMON_NAMESPACE
