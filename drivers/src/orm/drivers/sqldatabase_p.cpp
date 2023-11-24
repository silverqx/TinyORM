#include "orm/drivers/sqldatabase_p.hpp"

#include "orm/drivers/mysql/mysqldriver.hpp"
#include "orm/drivers/sqldrivererror.hpp"
#include "orm/drivers/support/connectionshash_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Drivers::Support::ConnectionsHash;

using namespace Qt::StringLiterals;

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

    if (!db.isValid())
        return db;

    // CUR drivers finish QThread::currentThread() silverqx
//    if (db.driver()->thread() != QThread::currentThread()) {
//        qWarning("SqlDatabasePrivate::database: requested database does not belong to the calling thread.");
//        return SqlDatabase();
//    }

    // Try to open a database connection and show a warning if failed
    if (open && !db.isOpen() && !db.open())
        qWarning() << "SqlDatabasePrivate::database: unable to open database:"
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
                << QStringLiteral(
                       "SqlDatabasePrivate::addDatabase: duplicate connection name '%1', "
                       "old connection has been removed.")
                   .arg(connection);
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
                << QStringLiteral(
                       "SqlDatabasePrivate::invalidateDb: connection '%1' is still "
                       "in use, all queries will stop working.")
                   .arg(connection);

    db.d->resetDriver();
    db.d->connectionName.clear();
}

ConnectionsHash &SqlDatabasePrivate::connections()
{
    return *g_connections;
}

/* Factory methods */

std::shared_ptr<SqlDriver> SqlDatabasePrivate::createDriver(const QString &driver)
{
    Q_ASSERT(!driver.isEmpty());

    if (driver == "QMYSQL"_L1)
        return std::make_shared<MySql::MySqlDriver>();

    throw std::exception(
                QStringLiteral("Unsupported driver '%1', available drivers: %2.")
                .arg(driver, SqlDatabase::drivers().join(QChar(' ')))
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

    throw std::exception("The SqlDatabasePrivate::sqldriver smart pointer is null.");
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE
