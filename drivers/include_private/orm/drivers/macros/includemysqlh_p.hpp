#pragma once
#ifndef ORM_DRIVERS_MACROS_INCLUDEMYSQLH_P_HPP
#define ORM_DRIVERS_MACROS_INCLUDEMYSQLH_P_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#if __has_include(<mysql/mysql.h>)
#  include <mysql/mysql.h> // IWYU pragma: export
#elif __has_include(<mysql.h>)
#  include <mysql.h> // IWYU pragma: export
#else
#  error Can't find the <mysql.h> header file, please install the MySQL C client library.
#endif

#endif // ORM_DRIVERS_MACROS_INCLUDEMYSQLH_P_HPP
