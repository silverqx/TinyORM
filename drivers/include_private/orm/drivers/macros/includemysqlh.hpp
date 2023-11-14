#pragma once
#ifndef ORM_MACROS_INCLUDEMYSQLH_HPP
#define ORM_MACROS_INCLUDEMYSQLH_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#if __has_include(<mysql/mysql.h>)
#  include <mysql/mysql.h>
#elif __has_include(<mysql.h>)
#  include <mysql.h>
#else
#  error Can not find <mysql.h> header file, install the MySQL C client library or \
disable TINYORM_MYSQL_PING preprocessor directive.
#endif

#endif // ORM_MACROS_INCLUDEMYSQLH_HPP
