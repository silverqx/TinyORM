#include "orm/drivers/sqldatabase_p.hpp"

#include <orm/constants.hpp>

#include "orm/drivers/mysql/mysqldriver.hpp"
#include "orm/drivers/sqldrivererror.hpp"
#include "orm/drivers/support/connectionshash_p.hpp"

#define sl(str) QStringLiteral(str)

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals;

using Orm::Constants::SPACE;

using Orm::Drivers::Support::ConnectionsHash;

namespace Orm::Drivers
{

Q_GLOBAL_STATIC(ConnectionsHash, g_connections)

/* public */

/* Database connection */

void SqlDatabasePrivate::cloneDatabase(const SqlDatabasePrivate &other)
{
    driverName   = other.driverName;
    hostName     = other.hostName;
    databaseName = other.databaseName;
    username     = other.username;
    password     = other.password;
    port         = other.port;

    connectionOptions = other.connectionOptions;
    precisionPolicy   = other.precisionPolicy;

    /* The connectionName is set in the SqlDatabasePrivate::addDatabase().
       The other.sqldriver in the SqlDatabase(const QString &driver) constructor so
       it's always set. */

    if (sqldriver)
        sqldriver->setDefaultNumericalPrecisionPolicy(
                    other.sqldriver->defaultNumericalPrecisionPolicy());
}

/* Database Manager */

SqlDatabase SqlDatabasePrivate::database(const QString &connection, const bool open)
{
    auto &db = g_connections->at_ts(connection);

    // Nothing to do, connection is Invalid
    if (isInvalidWithWarning(db, connection))
        return db;

    // Throw an exception if a connection was created in a different thread
    throwIfDifferentThread(db, connection);

    // Try to open a database connection and show a warning if failed
    if (open && !db.isOpen() && !db.open())
        qWarning().noquote().nospace()
                << "SqlDatabasePrivate::database: unable to open database: "
                << db.lastError().text();

    return db;
}

void SqlDatabasePrivate::addDatabase(const SqlDatabase &db, const QString &connection)
{
    auto &connections = *g_connections;

    // Exclusive/write lock
    std::scoped_lock lock(connections.mutex());

    if (connections.contains(connection)) {
        /* This is correct, invaliding the origin connection is a good thing, even if
           that origin connection would normally work after being replaced. */
        invalidateDb(connections.extract(connection).mapped(), connection);

        qWarning().noquote()
                << sl("SqlDatabasePrivate::addDatabase: duplicate connection name '%1', "
                      "old connection has been removed.").arg(connection);
    }

    connections.try_emplace(connection, db);

    db.d->connectionName = connection;
}

void SqlDatabasePrivate::removeDatabase(const QString &connection)
{
    auto &connections = *g_connections;

    // Exclusive/write lock
    std::scoped_lock lock(connections.mutex());

    // Nothing to do
    if (!connections.contains(connection))
        return;

    invalidateDb(connections.extract(connection).mapped(), connection);
}

void SqlDatabasePrivate::invalidateDb(const SqlDatabase &db, const QString &connection,
                                      const bool warn)
{
    if (db.d.use_count() > 1 && warn)
        qWarning().noquote()
                << sl("SqlDatabasePrivate::invalidateDb: connection '%1' is still "
                      "in use, all queries will stop working.").arg(connection);

    db.d->resetDriver();
    db.d->connectionName.clear();
}

ConnectionsHash &SqlDatabasePrivate::connections()
{
    return *g_connections;
}

bool &SqlDatabasePrivate::checkDifferentThread() noexcept
{
    static auto isEnabled = true;
    return isEnabled;
}

/* Factory methods */

std::shared_ptr<SqlDriver> SqlDatabasePrivate::createDriver(const QString &driver)
{
    Q_ASSERT(!driver.isEmpty());

    if (driver == "QMYSQL"_L1)
        return std::make_shared<MySql::MySqlDriver>();

    throw std::exception(
                sl("Unsupported driver '%1', available drivers: %2.")
                .arg(driver, SqlDatabase::drivers().join(SPACE))
                .toUtf8().constData());
}

std::shared_ptr<SqlDatabasePrivate>
SqlDatabasePrivate::createSqlDatabasePrivate(std::shared_ptr<SqlDriver> &&driver)
{
    return std::make_shared<SqlDatabasePrivate>(std::move(driver), driver->driverName());
}

void SqlDatabasePrivate::throwIfSqlDriverIsNull() const
{
    // Nothing to do
    if (sqldriver)
        return;

    throw std::exception("The SqlDatabasePrivate::sqldriver smart pointer is nullptr.");
}

void SqlDatabasePrivate::throwIfDifferentThread(const SqlDatabase &db,
                                                const QString &connection)
{
    /* Nothing to do, the thread check is disabled or a connection was created
       in the same thread as the current thread. */
    if (!checkDifferentThread() ||
        db.driver().lock()->threadId() == std::this_thread::get_id()
    )
        return;

    throw std::exception(
                sl("SqlDatabasePrivate::database: requested '%1' database connection "
                   "does not belong to the calling thread.")
                .arg(connection).toUtf8().constData());
}

bool SqlDatabasePrivate::isInvalidWithWarning(const SqlDatabase &db,
                                              const QString &connection)
{
    // Nothing to do, connection is valid
    if (db.isValid())
        return false;

    qWarning().noquote()
            << sl("SqlDatabasePrivate::database: requested '%1' database connection "
                  "is invalid.").arg(connection);

    return true;
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE
