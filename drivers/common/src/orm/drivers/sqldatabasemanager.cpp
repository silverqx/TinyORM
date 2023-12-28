#include "orm/drivers/sqldatabasemanager.hpp"

#include "orm/drivers/constants_p.hpp"
#include "orm/drivers/sqldatabase_p.hpp"
#include "orm/drivers/support/connectionshash_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals;

using Orm::Drivers::Constants::QMYSQL;

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
    SqlDatabase db(driver);
    SqlDatabasePrivate::addDatabase(db, connection);

    return db;
}

SqlDatabase
SqlDatabaseManager::addDatabase(std::unique_ptr<SqlDriver> &&driver,
                                const QString &connection)
{
    throwIfDriverIsNullptr(driver, connection);

    SqlDatabase db(std::move(driver));
    SqlDatabasePrivate::addDatabase(db, connection);

    return db;
}

void SqlDatabaseManager::removeDatabase(const QString &connection)
{
    SqlDatabasePrivate::removeDatabase(connection);
}

SqlDatabase
SqlDatabaseManager::cloneDatabase(const SqlDatabase &other, const QString &connection)
{
    if (!other.isValid())
        return {};

    SqlDatabase db(other.driverName());
    db.d->cloneDatabase(*other.d);

    SqlDatabasePrivate::addDatabase(db, connection);

    return db;
}

SqlDatabase
SqlDatabaseManager::cloneDatabase(const QString &otherConnection,
                                  const QString &connection)
{
    return cloneDatabase(SqlDatabasePrivate::connections().at_ts(otherConnection),
                         connection);
}

/* Getters / Setters */

QStringList SqlDatabaseManager::drivers()
{
    return {QMYSQL};
}

QStringList SqlDatabaseManager::connectionNames()
{
    return SqlDatabasePrivate::connections().keys_ts();
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

    throw std::exception(
                u"The driver can't be nullptr while adding the '%1' connection."_s
                .arg(connection).toUtf8().constData());
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE