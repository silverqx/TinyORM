#include "orm/drivers/mysql/mysqldriver.hpp"

extern "C" Q_DECL_EXPORT
#ifdef TINYORM_COMMON_NAMESPACE
/*! Factory method to create the MySqlDriver instance. */
const std::shared_ptr<TINYORM_COMMON_NAMESPACE::Orm::Drivers::SqlDriver> *
TinyDriverInstance()
{
    static const std::shared_ptr<TINYORM_COMMON_NAMESPACE::Orm::Drivers::SqlDriver>
    cachedInstance = std::make_shared<
                     TINYORM_COMMON_NAMESPACE::Orm::Drivers::MySql::MySqlDriver>();
#else
/*! Factory method to create the MySqlDriver instance. */
const std::shared_ptr<Orm::Drivers::SqlDriver> *TinyDriverInstance()
{
    static const std::shared_ptr<Orm::Drivers::SqlDriver>
    cachedInstance = std::make_shared<Orm::Drivers::MySql::MySqlDriver>();
#endif

    return std::addressof(cachedInstance);
}
