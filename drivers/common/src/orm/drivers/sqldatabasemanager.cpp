#include "orm/drivers/sqldatabasemanager.hpp"

#include "orm/drivers/exceptions/invalidargumenterror.hpp"
#include "orm/drivers/sqldatabase_p.hpp"
#include "orm/drivers/support/connectionshash_p.hpp" // IWYU pragma: keep
#include "orm/drivers/utils/type_p.hpp"

#ifdef TINYDRIVERS_MYSQL_DRIVER
#  include "orm/drivers/constants_p.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

#ifdef TINYDRIVERS_MYSQL_DRIVER
using Orm::Drivers::Constants::QMYSQL;
#endif

namespace Orm::Drivers
{

/* public */

const QString
SqlDatabaseManager::defaultConnection = u"qt_sql_default_connection"_s; // Don't change the default connection name

SqlDatabase SqlDatabaseManager::database(const QString &connection, const bool open)
{
    return SqlDatabasePrivate::database(connection, open);
}

bool SqlDatabaseManager::contains(const QString &connection)
{
    return SqlDatabasePrivate::connections().contains_ts(connection);
}

SqlDatabase
SqlDatabaseManager::addDatabase(const QString &driver, const QString &connection)
{
    return SqlDatabasePrivate::addDatabase(SqlDatabase(driver, connection), connection);
}

SqlDatabase
SqlDatabaseManager::addDatabase(QString &&driver, const QString &connection)
{
    return SqlDatabasePrivate::addDatabase(SqlDatabase(std::move(driver), connection),
                                           connection);
}

SqlDatabase
SqlDatabaseManager::addDatabase(std::unique_ptr<SqlDriver> &&driver,
                                const QString &connection)
{
    throwIfDriverIsNullptr(driver, connection);

    return SqlDatabasePrivate::addDatabase(SqlDatabase(std::move(driver)), connection);
}

void SqlDatabaseManager::removeDatabase(const QString &connection)
{
    SqlDatabasePrivate::removeDatabase(connection);
}

SqlDatabase
SqlDatabaseManager::cloneDatabase(const SqlDatabase &other, const QString &connection)
{
    if (!other.isValid())
        throw Exceptions::InvalidArgumentError(
                u"Can't clone an invalid '%1' database connection in %2()."_s
                .arg(other.connectionName(), __tiny_func__));

    SqlDatabase db(other.driverName(), connection);
    db.d->cloneDatabase(*other.d);

    return SqlDatabasePrivate::addDatabase(std::move(db), connection);
}

SqlDatabase
SqlDatabaseManager::cloneDatabase(const QString &otherConnection,
                                  const QString &connection)
{
    // Nothing to clone, the otherConnection isn't registered (doesn't exist)
    SqlDatabasePrivate::throwIfNoConnection(otherConnection);

    return cloneDatabase(SqlDatabasePrivate::connections().at_ts(otherConnection),
                         connection);
}

/* Getters / Setters */

QStringList SqlDatabaseManager::drivers()
{
    return {
#ifdef TINYDRIVERS_MYSQL_DRIVER
        QMYSQL,
#endif
    // Not implemented yet :/
// #ifdef TINYDRIVERS_PSQL_DRIVER
//         QPSQL,
// #endif
// #ifdef TINYDRIVERS_SQLITE_DRIVER
//         QSQLITE,
// #endif
    };
}

QStringList SqlDatabaseManager::connectionNames()
{
    return SqlDatabasePrivate::connections().keys_ts();
}

QStringList SqlDatabaseManager::openedConnectionNames()
{
    return SqlDatabasePrivate::connections().openedConnectionNames_ts();
}

bool SqlDatabaseManager::isDriverAvailable(const QString &name)
{
    return drivers().contains(name);
}

/* private */

void SqlDatabaseManager::throwIfDriverIsNullptr(const std::unique_ptr<SqlDriver> &driver,
                                                const QString &connection)
{
    // Nothing to do
    if (driver)
        return;

    throw Exceptions::InvalidArgumentError(
                u"The 'driver' argument can't be nullptr while adding the '%1' "
                 "database connection in %2()."_s.arg(connection, __tiny_func__));
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE
