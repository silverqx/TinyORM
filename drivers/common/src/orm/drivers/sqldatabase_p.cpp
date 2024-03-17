#include "orm/drivers/sqldatabase_p.hpp"

#include <mutex>

#include "orm/drivers/exceptions/invalidargumenterror.hpp"
#include "orm/drivers/sqldriver.hpp"
#include "orm/drivers/support/connectionshash_p.hpp"
#include "orm/drivers/support/sqldriverfactory_p.hpp"
#include "orm/drivers/utils/type_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

using ConnectionsHash = Orm::Drivers::Support::ConnectionsHashPrivate;

namespace Orm::Drivers
{

/*! Database connections hash, maps connection names to SqlDatabase instances. */
Q_GLOBAL_STATIC(ConnectionsHash, g_connections) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)

/* public */

/* No need to check the driver for nullptr because that's already checked
   in the SqlDatabaseManager::addDatabase(). */

SqlDatabasePrivate::SqlDatabasePrivate(std::shared_ptr<SqlDriver> &&driver) noexcept
    : sqldriver(std::move(driver))
    , driverName(sqldriver->driverName())
{}

/* Database connection */

void SqlDatabasePrivate::cloneDatabase(const SqlDatabasePrivate &other) noexcept
{
    driverName   = other.driverName;
    hostName     = other.hostName;
    databaseName = other.databaseName;
    username     = other.username;
    password     = other.password;
    port         = other.port;

    connectionOptions = other.connectionOptions;
    precisionPolicy   = other.precisionPolicy;

    /* The connectionName is set in SqlDatabasePrivate::addDatabase() (after this method).
       The other.sqldriver was already set in the SqlDatabase(const QString &driver)
       constructor so it's always set. */

    if (sqldriver)
        sqldriver->setDefaultNumericalPrecisionPolicy(
                    other.sqldriver->defaultNumericalPrecisionPolicy());
}

/* Database Manager */

SqlDatabase SqlDatabasePrivate::database(const QString &connection, const bool open)
{
    // Throw an exception if a given database connection isn't registered (doesn't exist)
    throwIfNoConnection(connection);

    auto &db = g_connections->at_ts(connection);

    // Throw an exception if a connection was created in a different thread
    throwIfDifferentThread(db, connection);

    // Try to open a database connection
    if (open && !db.isOpen())
        db.open();

    return db;
}

SqlDatabase SqlDatabasePrivate::addDatabase(SqlDatabase &&db, const QString &connection)
{
    auto &connections = *g_connections;

    // Exclusive/write lock
    const std::scoped_lock lock(connections.mutex());

    if (connections.contains(connection))
        throw Exceptions::InvalidArgumentError(
                u"The '%1' database connection already exists in %2()."_s
                .arg(connection, __tiny_func__));

    /* Set a connection name for the connection and also for the associated SQL driver,
       it will be used in exceptions and logs. The SqlResult obtains a connection name
       from the SqlDriver. */
    db.setConnectionName(connection);

    auto [itDatabase, ok] = connections.try_emplace(connection, std::move(db));
    // Insertion must always happen here
    Q_ASSERT(ok);

    return itDatabase->second;
}

void SqlDatabasePrivate::removeDatabase(const QString &connection)
{
    auto &connections = *g_connections;

    // Exclusive/write lock
    const std::scoped_lock lock(connections.mutex());

    // Nothing to do
    if (!connections.contains(connection))
        return;

    invalidateDatabase(connections.extract(connection).mapped(), connection);
}

void
SqlDatabasePrivate::invalidateDatabase(const SqlDatabase &db, const QString &connection,
                                       const bool warn)
{
    if (warn && db.d.get().use_count() > 1)
        qWarning().noquote()
                << u"The database connection '%1' is still in use, all queries for this "
                    "connection will stop working. Make sure to destroy all SqlDatabase "
                    "instances to avoid this warning, in %2()."_s
                   .arg(connection, __tiny_func__);

    /* This method is only called from the removeDatabase(), so the close() method must
       always be called to properly finish and free the database connection.
       Also, always reset the database connection regardless of use_count(). */
    db.d->driver().close();
    db.d->resetDriver();
    db.d->connectionName.clear();
}

ConnectionsHash &SqlDatabasePrivate::connections()
{
    return *g_connections;
}

std::atomic_bool &SqlDatabasePrivate::checkSameThread() noexcept
{
    static std::atomic_bool isEnabled = true;
    return isEnabled;
}

/* Factory methods */

std::shared_ptr<SqlDatabasePrivate>
SqlDatabasePrivate::createSqlDatabasePrivate(const QString &driver,
                                             const QString &connection)
{
    return std::make_shared<SqlDatabasePrivate>(
                Support::SqlDriverFactoryPrivate(driver, connection).make());
}

std::shared_ptr<SqlDatabasePrivate>
SqlDatabasePrivate::createSqlDatabasePrivate(QString &&driver, const QString &connection)
{
    return std::make_shared<SqlDatabasePrivate>(
                Support::SqlDriverFactoryPrivate(std::move(driver), connection).make());
}

std::shared_ptr<SqlDatabasePrivate>
SqlDatabasePrivate::createSqlDatabasePrivate(std::shared_ptr<SqlDriver> &&driver)
{
    /* Revisited, passing the driverName as a separate argument is ok, it's about how
       the SqlDatabase class is instantiated, exactly two ways how to instantiate,
       from the QString and from the unique_ptr<SqlDriver>. */
    return std::make_shared<SqlDatabasePrivate>(std::move(driver));
}

/* Others */

void SqlDatabasePrivate::throwIfNoConnection(const QString &connection)
{
    // Nothing to do, the connection exists
    if (g_connections->contains_ts(connection))
        return;

    throw Exceptions::InvalidArgumentError(
                u"The '%1' database connection isn't registered (doesn't exist), please "
                 "register it using the SqlDatabase::addDatabase() method, in %2()."_s
                .arg(connection, __tiny_func__));
}

/* private */

/* Others */

void SqlDatabasePrivate::throwIfSqlDriverIsNull() const
{
    // Nothing to do
    if (sqldriver)
        return;

    throw Exceptions::LogicError(
                u"The SqlDatabasePrivate::sqldriver smart pointer is nullptr. "
                 "The SqlDatabase instance is invalid after calling removeDatabase(), "
                 "for '%1' database connection in %2()."_s
                .arg(connectionName, __tiny_func__));
}

void SqlDatabasePrivate::throwIfDifferentThread(const SqlDatabase &db,
                                                const QString &connection)
{
    /* Nothing to do, the same thread check is disabled or a connection was created
       in the same thread as the current thread. */
    if (!checkSameThread() ||
        db.driverWeak().lock()->threadId() == std::this_thread::get_id()
    )
        return;

    throw Exceptions::LogicError(
                u"The requested '%1' database connection does not belong to the calling "
                 "thread, it was created in another thread. This check can be disabled "
                 "using the SqlDatabase::disableThreadCheck(), then you have to "
                 "synchronize your threads to avoid race conditions, in %2()."_s
                .arg(connection, __tiny_func__));
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE
