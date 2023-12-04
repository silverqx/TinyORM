#include "orm/drivers/sqldatabase_p.hpp"

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
#  include <QDir>
#  include <QLibrary>
#endif

#include "orm/drivers/constants_p.hpp"
#include "orm/drivers/sqldrivererror.hpp"
#include "orm/drivers/support/connectionshash_p.hpp"

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
#  include "orm/drivers/sqldriver.hpp"
#  include "orm/drivers/version.hpp"
#else
#  include "orm/drivers/mysql/mysqldriver.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

using Orm::Drivers::Constants::QMYSQL;
using Orm::Drivers::Constants::SPACE;

using Orm::Drivers::Support::ConnectionsHash;

namespace Orm::Drivers
{

/*! Database connections hash, maps connection names to SqlDatabase instances. */
Q_GLOBAL_STATIC(ConnectionsHash, g_connections) // NOLINT(misc-use-anonymous-namespace)

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
    const std::scoped_lock lock(connections.mutex());

    if (connections.contains(connection)) {
        /* This is correct, invaliding the origin connection is a good thing, even if
           that origin connection would normally work after being replaced. */
        invalidateDb(connections.extract(connection).mapped(), connection);

        qWarning().noquote()
                << u"SqlDatabasePrivate::addDatabase: duplicate connection name '%1', "
                    "old connection has been removed."_s.arg(connection);
    }

    connections.try_emplace(connection, db);

    db.d->connectionName = connection;
}

void SqlDatabasePrivate::removeDatabase(const QString &connection)
{
    auto &connections = *g_connections;

    // Exclusive/write lock
    const std::scoped_lock lock(connections.mutex());

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
                << u"SqlDatabasePrivate::invalidateDb: connection '%1' is still "
                    "in use, all queries will stop working."_s.arg(connection);

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

    if (driver == QMYSQL)
#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
        return loadSqlDriver(driver, u"TinyMySql"_s);
#else
        return std::make_shared<MySql::MySqlDriver>();
#endif

    throw std::exception(
                u"Unsupported driver '%1', available drivers: %2."_s
                .arg(driver, SqlDatabase::drivers().join(SPACE))
                .toUtf8().constData());
}

std::shared_ptr<SqlDatabasePrivate>
SqlDatabasePrivate::createSqlDatabasePrivate(std::shared_ptr<SqlDriver> &&driver)
{
    return std::make_shared<SqlDatabasePrivate>(std::move(driver), driver->driverName());
}

/* private */

/* Factory methods */

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
namespace
{
    /*! Get driver library basenames for which to try to load the shared library. */
    inline auto getDriverBasenames(QString driverBasename)
    {
        return std::to_array<QString>({
            driverBasename.append(QString::number(TINYDRIVERS_VERSION_MAJOR)),
            // qmake build doesn't support appending d after the basename for Debug builds
#ifndef TINY_QMAKE_BUILD
            driverBasename.append('d'_L1),
#endif
            /* As the last, try to load from the raw basename without any postfixes,
               we don't use this but it can be helpful in some situations. */
            std::move(driverBasename),
        });
    }
} // namespace

std::shared_ptr<SqlDriver>
SqlDatabasePrivate::loadSqlDriver(const QString &driver, const QString &driverBasenameRaw)
{
    const auto driverBasenames = getDriverBasenames(driverBasenameRaw);

    /* Try to load library from paths from TINY/QT_PLUGIN_PATH environment variables and
       from paths from qmake/CMake build system (inside build tree). */
    for (const auto &driverPath : sqlDriverPaths(driver))
        for (const auto &driverBasename : driverBasenames)
            if (auto driverInstance = loadSqlDriverCommon(joinDriverPath(driverPath,
                                                                         driverBasename));
                driverInstance
            )
                return driverInstance;

    throw std::runtime_error(
                u"Can't load '%1' shared library for '%2' driver at runtime."_s
                .arg(driverBasenameRaw, driver)
                .toUtf8().constData());
}

std::shared_ptr<SqlDriver>
SqlDatabasePrivate::loadSqlDriverCommon(const QString &driverFilepath)
{
    // CUR drivers UNIX version number silverqx
    QLibrary myLib(driverFilepath);

    /*! Return type for the factory driver method. */
    using ReturnType = const std::shared_ptr<SqlDriver> *;

    std::function<ReturnType()>
    createDriverMemFn(reinterpret_cast<ReturnType(*)()>(
                          myLib.resolve("TinyDriverInstance")));

    if (createDriverMemFn)
        return *std::invoke(createDriverMemFn);

    return nullptr;
}

QStringList SqlDatabasePrivate::sqlDriverPaths(const QString &driver)
{
    const auto envTinyPluginPath = qEnvironmentVariable("TINY_PLUGIN_PATH");
    const auto envQtPluginPath   = qEnvironmentVariable("QT_PLUGIN_PATH");

    const auto hasTinyPluginPaths = !envTinyPluginPath.isEmpty();
    const auto hasQtPluginPaths   = !envQtPluginPath.isEmpty();

    QStringList result;
    // +1 for path from qmake/CMake build system
    result.reserve(envTinyPluginPath.count(QDir::listSeparator()) +
                   envQtPluginPath.count(QDir::listSeparator()) + 1);

    // Path from qmake/CMake build system (shared library path inside the build tree)
    if (auto &&sqlDriverPath = sqlDriverPathFromBuildSystem(driver); sqlDriverPath)
        result << std::move(*sqlDriverPath);

    constexpr static auto listSeparator = QDir::listSeparator();

    // Paths from TINY_PLUGIN_PATH environment variable
    if (hasTinyPluginPaths)
        result << envTinyPluginPath.split(listSeparator, Qt::SkipEmptyParts);

    // Paths from QT_PLUGIN_PATH environment variable
    if (hasQtPluginPaths)
        result << envQtPluginPath.split(listSeparator, Qt::SkipEmptyParts);

    return result;
}

std::optional<QString>
SqlDatabasePrivate::sqlDriverPathFromBuildSystem(const QString &driver)
{
    /* The TINYDRIVERS_MYSQL/PSQL/SQLITE_PATH-s are optional; whoever will deal with
       the TinyDrivers to be able to avoid defining these macros. */
#if (defined TINYDRIVERS_MYSQL_LOADABLE_LIBRARY && !defined(TINYDRIVERS_MYSQL_PATH)) || \
    (defined TINYDRIVERS_PSQL_LOADABLE_LIBRARY && !defined(TINYDRIVERS_PSQL_PATH))   || \
    (defined TINYDRIVERS_SQLITE_LOADABLE_LIBRARY && !defined(TINYDRIVERS_SQLITE_PATH))
    return std::nullopt;
#endif

    // Path from qmake/CMake build system (shared library path inside the build tree)
#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY // NOLINT(readability-redundant-preprocessor)
    if (driver == QMYSQL)
        return getSqlDriverPath(TINY_STRINGIFY(TINYDRIVERS_MYSQL_PATH));
#endif

#ifdef TINYDRIVERS_PSQL_LOADABLE_LIBRARY
    if (driver == QPSQL)
        return getSqlDriverPath(TINY_STRINGIFY(TINYDRIVERS_PSQL_PATH));
#endif

#ifdef TINYDRIVERS_SQLITE_LOADABLE_LIBRARY
    if (driver == QSQLITE)
        return getSqlDriverPath(TINY_STRINGIFY(TINYDRIVERS_SQLITE_PATH));
#endif

    Q_UNREACHABLE();
}

QString SqlDatabasePrivate::getSqlDriverPath(const char *const driverPathRaw)
{
    auto driverPath = QString::fromUtf8(driverPathRaw);

    Q_ASSERT(!driverPath.isEmpty());

    return driverPath;
}

QString
SqlDatabasePrivate::joinDriverPath(const QString &driverPath,
                                   const QString &driverBasename)
{
    return QDir::toNativeSeparators(u"%1/%2"_s.arg(driverPath, driverBasename));
}
#endif

/* Others */

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
                u"SqlDatabasePrivate::database: requested '%1' database connection "
                 "does not belong to the calling thread."_s
                .arg(connection).toUtf8().constData());
}

bool SqlDatabasePrivate::isInvalidWithWarning(const SqlDatabase &db,
                                              const QString &connection)
{
    // Nothing to do, connection is valid
    if (db.isValid())
        return false;

    qWarning().noquote()
            << u"SqlDatabasePrivate::database: requested '%1' database connection "
                "is invalid."_s.arg(connection);

    return true;
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE
