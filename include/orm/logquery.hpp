#ifndef LOGQUERY_H
#define LOGQUERY_H

#include <QString>

class QSqlQuery;

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif

// TODO code which was here, will be reverted after merge of the logQuery() and logExecutedQuery() silverqx
QString parseExecutedQuery(const QSqlQuery &query);

#ifdef QT_DEBUG
    Q_DECL_UNUSED
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
