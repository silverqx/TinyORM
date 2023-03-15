#pragma once
#ifndef ORM_MACROS_LOGEXECUTEDQUERY_HPP
#define ORM_MACROS_LOGEXECUTEDQUERY_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/config.hpp" // IWYU pragma: keep

#include "orm/macros/commonnamespace.hpp"

#if !defined(LOG_EXECUTED_QUERY) && !defined(TINYORM_NO_DEBUG)
#  include "orm/utils/query.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

#ifndef LOG_EXECUTED_QUERY
#  ifndef TINYORM_NO_DEBUG
#    define LOG_EXECUTED_QUERY(query) Orm::Utils::Query::logExecutedQuery(query)
#  else
#    define LOG_EXECUTED_QUERY(query) qt_noop()
#  endif
#endif

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_MACROS_LOGEXECUTEDQUERY_HPP
