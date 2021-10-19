#pragma once
#ifndef LOGQUERY_HPP
#define LOGQUERY_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>
#include <QVariant>

class QSqlQuery;

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif

/*! Get the last executed query with replaced placeholders ( ideal for logging ). */
QString parseExecutedQuery(const QSqlQuery &query);
/*! Get pretended query with replaced placeholders ( ideal for logging ). */
QString parseExecutedQueryForPretend(QString query, const QVector<QVariant> &bindings);

#if !defined(TINYORM_NO_DEBUG)
/*! Log the last executed query to the debug output. */
[[maybe_unused]]
void logExecutedQuery(const QSqlQuery &query);
#endif

#ifndef LOG_EXECUTED_QUERY
#  if !defined(TINYORM_NO_DEBUG)
#    define LOG_EXECUTED_QUERY(query) logExecutedQuery(query)
#  else
#    define LOG_EXECUTED_QUERY(query) qt_noop()
#  endif
#endif

#ifdef TINYORM_COMMON_NAMESPACE
}
#endif

#endif // LOGQUERY_HPP
