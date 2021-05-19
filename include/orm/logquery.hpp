#ifndef LOGQUERY_H
#define LOGQUERY_H

#include <QString>

class QSqlQuery;

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif

/*! Get the last executed query with replaced placeholders ( ideal for logging ). */
QString parseExecutedQuery(const QSqlQuery &query);
/*! Get pretended query with replaced placeholders ( ideal for logging ). */
QString parseExecutedQueryForPretend(QString query, const QVariantMap &bindings);

#ifdef QT_DEBUG
/*! Log the last executed query to the debug output. */
[[maybe_unused]]
void logExecutedQuery(const QSqlQuery &query);
#endif

#ifndef LOG_EXECUTED_QUERY
#  ifdef QT_DEBUG
#    define LOG_EXECUTED_QUERY(query) logExecutedQuery(query)
#  else
#    define LOG_EXECUTED_QUERY(query) qt_noop()
#  endif
#endif

#ifdef TINYORM_COMMON_NAMESPACE
}
#endif

#endif // LOGQUERY_H
