#ifndef LOGQUERY_H
#define LOGQUERY_H

class QSqlQuery;

#ifdef MANGO_COMMON_NAMESPACE
namespace MANGO_COMMON_NAMESPACE
{
#endif

// TODO code which was here, will be reverted after merge of the logQuery() and logExecutedQuery() silverqx
QString parseExecutedQuery(const QSqlQuery &query);

#ifdef QT_DEBUG
#  ifndef Q_CC_MSVC
    Q_NORETURN
#  endif
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

#ifdef MANGO_COMMON_NAMESPACE
}
#endif

#endif // LOGQUERY_H
