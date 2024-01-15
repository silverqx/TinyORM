#include "orm/drivers/mysql/mysqldriver.hpp"

using TINYORM_PREPEND_NAMESPACE(Orm::Drivers::SqlDriver);
using TINYORM_PREPEND_NAMESPACE(Orm::Drivers::MySql::MySqlDriver);

/* There is no way to return anything other than a raw pointer as the extern "C" is used.
   Also, don't use the static local variable to cache the driver, it causes weird bugs.
   The SqlDatabase will be responsible for destroying the MySqlDriver instance during
   the destruction or the removeDatabase() call inside the invalidateDatabase() method. */

/*! Factory method to create the MySqlDriver instance. */
extern "C" Q_DECL_EXPORT SqlDriver *TinyDriverInstance()
{
    return new MySqlDriver();
}
