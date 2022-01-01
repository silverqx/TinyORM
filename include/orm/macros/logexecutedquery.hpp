#pragma once
#ifndef ORM_MACROS_LOGEXECUTEDQUERY_HPP
#define ORM_MACROS_LOGEXECUTEDQUERY_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#if !defined(LOG_EXECUTED_QUERY) && !defined(TINYORM_NO_DEBUG)
#  include "orm/utils/query.hpp"
#endif

#ifndef LOG_EXECUTED_QUERY
#  if !defined(TINYORM_NO_DEBUG)
#    define LOG_EXECUTED_QUERY(query) Orm::Utils::Query::logExecutedQuery(query)
#  else
#    define LOG_EXECUTED_QUERY(query) qt_noop()
#  endif
#endif

#endif // ORM_MACROS_LOGEXECUTEDQUERY_HPP
