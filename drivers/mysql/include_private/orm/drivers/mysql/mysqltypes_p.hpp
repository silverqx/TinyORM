#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLTYPES_P_HPP
#define ORM_DRIVERS_MYSQL_MYSQLTYPES_P_HPP

#include "orm/drivers/mysql/macros/includemysqlh_p.hpp"

/* MySQL >=8.0.1 removed the my_bool typedef while MariaDB kept it, so it's still needed
   for MariaDB because my_bool == char and compilation fails with the bool type.
   MySQL replaced it with bool type and MariaDB uses char type and that's a problem.
   See https://bugs.mysql.com/bug.php?id=85131 */
#if defined(MARIADB_VERSION_ID) || MYSQL_VERSION_ID < 80001
using my_bool = decltype (mysql_stmt_bind_result(nullptr, nullptr));
#else
using my_bool = bool;
#endif

#endif // ORM_DRIVERS_MYSQL_MYSQLTYPES_P_HPP
