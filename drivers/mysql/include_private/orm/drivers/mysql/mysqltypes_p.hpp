#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLTYPES_P_HPP
#define ORM_DRIVERS_MYSQL_MYSQLTYPES_P_HPP

#include "orm/drivers/mysql/macros/includemysqlh_p.hpp"

/* MySQL >=8.0.1 removed my_bool typedef while MariaDB kept it, so it's still needed to
   define it for MariaDB because my_bool == char and compilation fails with the bool type.
   See https://bugs.mysql.com/bug.php?id=85131 */
#if defined(MARIADB_VERSION_ID) || MYSQL_VERSION_ID < 80001
using my_bool = decltype (mysql_stmt_bind_result(nullptr, nullptr));
#else
using my_bool = bool;
#endif

#endif // ORM_DRIVERS_MYSQL_MYSQLTYPES_P_HPP
