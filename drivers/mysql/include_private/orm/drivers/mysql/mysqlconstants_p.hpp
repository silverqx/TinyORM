#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLCONSTANTS_P_HPP
#define ORM_DRIVERS_MYSQL_MYSQLCONSTANTS_P_HPP

/* Currently, there is no need to create a separate config.hpp file for the TinyMySql
   project because it uses the same constants as the TinyDrivers project.
   I will not support separate constants for the TinyMySql/SQLite/Postgres projects.
   Everything will be configured using a single set of constants, although they may
   end up as separate shared libraries (dll-s). */
#include "orm/drivers/config_p.hpp" // IWYU pragma: keep

#ifdef TINYDRIVERS_EXTERN_CONSTANTS
#  include "orm/drivers/mysql/mysqlconstants_extern_p.hpp" // IWYU pragma: export
#else
#  include "orm/drivers/mysql/mysqlconstants_inline_p.hpp" // IWYU pragma: export
#endif

#endif // ORM_DRIVERS_MYSQL_MYSQLCONSTANTS_P_HPP
