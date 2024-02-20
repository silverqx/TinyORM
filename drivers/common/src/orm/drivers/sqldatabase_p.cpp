#include "orm/drivers/sqldatabase_p.hpp"

#include <mutex>

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
#  include <QDir>
#  include <QLibrary>

#  include <orm/macros/likely.hpp>
#endif

#include "orm/drivers/constants_p.hpp"
#include "orm/drivers/exceptions/invalidargumenterror.hpp"
#include "orm/drivers/support/connectionshash_p.hpp"
#include "orm/drivers/utils/type_p.hpp"

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
#  include "orm/drivers/exceptions/runtimeerror.hpp"
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

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
/*! SQL loadable drivers' factory methods hash type. */
using DriverLoadedHashType = std::unordered_map<QString, std::function<SqlDriver *()>>;
/*! SQL loadable drivers' factory methods hash (used to check if driver is loaded too). */
Q_GLOBAL_STATIC(DriverLoadedHashType, g_driversLoaded) // NOLINT(misc-use-anonymous-namespace)
/*! Mutex to secure loading of loadable driver shared libraries. */
Q_GLOBAL_STATIC(std::shared_mutex, mx_driversLoaded) // NOLINT(misc-use-anonymous-namespace)
#endif

/* public */

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
                u"The database connection '%1' already exists in %2()."_s
                .arg(connection, __tiny_func__));

    db.d->connectionName = connection;

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
    if (warn && db.d.use_count() > 1)
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

bool &SqlDatabasePrivate::checkSameThread() noexcept
{
    static auto isEnabled = true;
    return isEnabled;
}

/* Factory methods */

std::shared_ptr<SqlDriver> SqlDatabasePrivate::createSqlDriver(const QString &driver)
{
    Q_ASSERT(!driver.isEmpty());

    if (driver == QMYSQL)
        return createMySqlDriver();

    // if (driver == QPSQL)
    //     return createPostgresDriver();

    // if (driver == QSQLITE)
    //     return createSQLiteDriver();

    throw Exceptions::InvalidArgumentError(
                u"Unsupported driver '%1', available drivers: %2; in %3()."_s
                .arg(driver, SqlDatabase::drivers().join(SPACE), __tiny_func__));
}

std::shared_ptr<SqlDatabasePrivate>
SqlDatabasePrivate::createSqlDatabasePrivate(std::shared_ptr<SqlDriver> &&driver)
{
    /* Revisited, passing the driverName as a separate argument is ok, it's about how
       the SqlDatabase class is instantiated, exactly two ways how to instantiate,
       from the QString and from the unique_ptr<SqlDriver>. */
    return std::make_shared<SqlDatabasePrivate>(std::move(driver), driver->driverName());
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

/* Factory methods */

/* Our conventions for database names are MySql, Postgres, and SQLite, the same names
   like TinyORM connection or driver classes are named. 🫤 I will not break this, so
   shared/static libraries will also have these names. */

std::shared_ptr<SqlDriver> SqlDatabasePrivate::createMySqlDriver()
{
#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
    return createSqlDriverLoadable(QMYSQL, u"TinyMySql"_s);
#else
    return std::make_shared<MySql::MySqlDriver>();
#endif
}

// std::shared_ptr<SqlDriver> SqlDatabasePrivate::createPostgresDriver()
// {
// #ifdef TINYDRIVERS_PSQL_LOADABLE_LIBRARY
//     return createSqlDriverLoadable(QPSQL, u"TinyPostgres"_s);
// #else
//     return std::make_shared<Postgres::PostgresDriver>();
// #endif
// }

// std::shared_ptr<SqlDriver> SqlDatabasePrivate::createSQLiteDriver()
// {
// #ifdef TINYDRIVERS_SQLITE_LOADABLE_LIBRARY
//     return createSqlDriverLoadable(QSQLITE, u"TinySQLite"_s);
// #else
//     return std::make_shared<SQLite::SQLiteDriver>();
// #endif
// }

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
namespace
{
#ifdef _MSC_VER
    /*! Get the MySQL driver shared library basename for MSVC. */
    inline auto getDriverBasenameMsvc(const QString &driverBasename)
    {
        auto driverBasenameTmp = driverBasename;

        return std::to_array<QString>({
            driverBasenameTmp.append(QString::number(TINYDRIVERS_VERSION_MAJOR)), // TinyMySql0.dll
            // qmake build doesn't support appending d after the basename for Debug builds
#if defined(TINYDRIVERS_DEBUG) && !defined(TINY_QMAKE_BUILD)
            driverBasenameTmp.append('d'_L1), // TinyMySql0d.dll
#endif
            /* As the last, try to load from the raw basename without any postfixes,
               we don't use this but it can be helpful in some situations. */
            driverBasename, // TinyMySql.dll
        });
    }
#endif

#ifdef __linux__
    /*! Get the MySQL driver shared library basename on Linux. */
    inline auto getDriverBasenameLinux(const QString &driverBasename)
    {
        auto driverBasenameTmp = driverBasename;

        return std::to_array<QString>({
            driverBasenameTmp.prepend(u"lib"_s), // libTinyMySql.so
            // qmake build doesn't support appending d after the basename for Debug builds
#if defined(TINYDRIVERS_DEBUG) && !defined(TINY_QMAKE_BUILD)
            driverBasenameTmp.append('d'_L1), // libTinyMySqld.so
#endif
            /* As the last, try to load from the raw basename without any postfixes,
               we don't use this but it can be helpful in some situations. */
            driverBasename, // TinyMySql.so
        });
    }
#endif

#ifdef __MINGW32__
    /*! Get the MySQL driver shared library basename on MSYS2/MinGW. */
    inline auto getDriverBasenameMinGW(const QString &driverBasename)
    {
        auto driverBasenameTmp = driverBasename;

        return std::to_array<QString>({
            // qmake build doesn't prepend lib for shared libraries on MSYS2
#ifndef TINY_QMAKE_BUILD
            driverBasenameTmp.prepend(u"lib"_s), // libTinyMySql.dll
#endif
            driverBasenameTmp.append(QString::number(TINYDRIVERS_VERSION_MAJOR)), // lib/TinyMySql0.dll
            // qmake build doesn't support appending d after the basename for Debug builds
#if defined(TINYDRIVERS_DEBUG) && !defined(TINY_QMAKE_BUILD)
            driverBasenameTmp.append('d'_L1), // lib/TinyMySqld.dll
#endif
            /* As the last, try to load from the raw basename without any postfixes,
               we don't use this but it can be helpful in some situations. */
            driverBasename, // TinyMySql.dll
        });
    }
#endif

    /*! Get driver library basenames for which to try to load the shared library. */
    inline auto getDriverBasenames(const QString &driverBasename)
    {
#ifdef _MSC_VER
        return getDriverBasenameMsvc(driverBasename);
#elif defined(__linux__)
        return getDriverBasenameLinux(driverBasename);
#elif defined(__MINGW32__)
        return getDriverBasenameMinGW(driverBasename);
#else
#  error Unsupported OS or platform in getDriverBasenames().
#endif
    }
} // namespace

std::shared_ptr<SqlDriver>
SqlDatabasePrivate::createSqlDriverLoadable(const QString &driver,
                                            const QString &driverBasenameRaw)
{
    // Load a SQL driver shared library at runtime and return the driver factory method
    const auto &createDriverMemFn = loadSqlDriver(driver, driverBasenameRaw);

    // This should never happen :/, it's checked earlier in loadSqlDriverCommon()
    Q_ASSERT(createDriverMemFn);

    return std::shared_ptr<SqlDriver>(std::invoke(createDriverMemFn));
}

const std::function<SqlDriver *()> &
SqlDatabasePrivate::loadSqlDriver(const QString &driver, const QString &driverBasenameRaw)
{
    /* Everything between the g_driversLoaded->contains() and g_driversLoaded->emplace()
       must be synchronized (secured using the write mutex). I implemented
       the g_driversLoaded the same way as the g_connections is implemented but I had
       to rework it to this mutex way. Simply, all threads must wait until the driver
       library is loaded and the result is cached. */

    // Exclusive/write lock
    const std::scoped_lock lock(*mx_driversLoaded);

    /* This secures that the QLibrary(driver) will be instantiated only once and also
       the resolve("TinyDriverInstance") method will be called only once. Calling them
       always when a new SQL driver instance is needed is unnecessary. */
    if (g_driversLoaded->contains(driver)) T_LIKELY
        return g_driversLoaded->at(driver);

    else T_UNLIKELY
        return loadSqlDriverCommon(driver, driverBasenameRaw);
}

const std::function<SqlDriver *()> &
SqlDatabasePrivate::loadSqlDriverCommon(const QString &driver,
                                        const QString &driverBasenameRaw)
{
    const auto driverBasenames = getDriverBasenames(driverBasenameRaw);

    /* Try to load library from paths from TINY/QT_PLUGIN_PATH environment variables and
       from paths from qmake/CMake build system (inside the build tree). */
    for (const auto &driverPath : sqlDriverPaths(driver))
        // Try to load all our basenames
        for (const auto &driverBasename : driverBasenames)
            // Load a Tiny SQL driver shared library and resolve driver factory function
            if (auto createDriverMemFn = loadSqlDriverAndResolve(
                                             joinDriverPath(driverPath, driverBasename));
                createDriverMemFn
            ) {
                // Cache the result
                const auto [itDriverMemFn, ok] = g_driversLoaded
                                                 ->emplace(driver, createDriverMemFn);
                // Insertion must always happen here
                Q_ASSERT(ok);

                return itDriverMemFn->second;
            }

    throw Exceptions::RuntimeError(
                u"Can't load '%1' shared library for '%2' driver at runtime in %3()."_s
                .arg(driverBasenameRaw, driver, __tiny_func__));
}

SqlDatabasePrivate::CreateSqlDriverMemFn
SqlDatabasePrivate::loadSqlDriverAndResolve(const QString &driverFilepath)
{
    // CUR drivers UNIX version number silverqx
    QLibrary sqlDriverLib(driverFilepath);

    // Nothing to do, the driver isn't on the given filepath
    if (!sqlDriverLib.load())
        return nullptr;

    // Resolve the TinyDriverInstance function
    auto *const createSqlDriverMemFn = reinterpret_cast<CreateSqlDriverMemFn>(
                                           sqlDriverLib.resolve("TinyDriverInstance"));

    if (createSqlDriverMemFn != nullptr)
        return createSqlDriverMemFn;

    throw Exceptions::RuntimeError(
                u"The QLibrary('%1') was loaded successfully but "
                 "the resolve('TinyDriverInstance') failed in %2()."_s
                .arg(driverFilepath, __tiny_func__));
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
    /* The TINYDRIVERS_MYSQL/PSQL/SQLITE_PATH-s are optional so whoever will deal with
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

// #ifdef TINYDRIVERS_PSQL_LOADABLE_LIBRARY
//     if (driver == QPSQL)
//         return getSqlDriverPath(TINY_STRINGIFY(TINYDRIVERS_PSQL_PATH));
// #endif

// #ifdef TINYDRIVERS_SQLITE_LOADABLE_LIBRARY
//     if (driver == QSQLITE)
//         return getSqlDriverPath(TINY_STRINGIFY(TINYDRIVERS_SQLITE_PATH));
// #endif

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
#endif // TINYDRIVERS_MYSQL_LOADABLE_LIBRARY

/* Others */

void SqlDatabasePrivate::throwIfSqlDriverIsNull() const
{
    // Nothing to do
    if (sqldriver)
        return;

    throw Exceptions::LogicError(
                u"The SqlDatabasePrivate::sqldriver smart pointer is nullptr. "
                 "The SqlDatabase instance is invalid after calling removeDatabase(), "
                 "in %1()."_s.arg(__tiny_func__));
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
