#include "orm/drivers/mysql/mysqldriver.hpp"

using TINYORM_PREPEND_NAMESPACE(Orm::Drivers::SqlDriver);
using TINYORM_PREPEND_NAMESPACE(Orm::Drivers::MySql::MySqlDriver);

/*! Factory method to create the MySqlDriver instance. */
extern "C" Q_DECL_EXPORT
const std::shared_ptr<SqlDriver> *TinyDriverInstance()
{
    static const std::shared_ptr<SqlDriver>
    cachedInstance = std::make_shared<MySqlDriver>();

    return std::addressof(cachedInstance);
}
