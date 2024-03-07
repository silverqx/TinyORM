#include "orm/drivers/support/sqldriverfactory_p.hpp"

#include <mutex>

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
#  include <shared_mutex>

#  include <QDir>
#  include <QLibrary>

#  include <orm/macros/likely.hpp>
#endif

#include "orm/drivers/constants_p.hpp"
#include "orm/drivers/exceptions/invalidargumenterror.hpp"
#include "orm/drivers/sqldatabase.hpp"
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

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
using Orm::Drivers::Constants::COMMA;
#endif

namespace Orm::Drivers::Support
{

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
/*! SQL loadable drivers' factory methods hash type. */
using DriverLoadedHashType = std::unordered_map<QString, std::function<SqlDriver *()>>;
/*! SQL loadable drivers' factory methods hash (used to check if driver is loaded too). */
Q_GLOBAL_STATIC(DriverLoadedHashType, g_driversLoaded) // NOLINT(misc-use-anonymous-namespace)
/*! Mutex to secure loading of loadable driver shared libraries. */
Q_GLOBAL_STATIC(std::shared_mutex, mx_driversLoaded) // NOLINT(misc-use-anonymous-namespace)
#endif

/* public */

std::shared_ptr<SqlDriver> SqlDriverFactoryPrivate::make() const
{
    Q_ASSERT(!driverName.isEmpty());

    if (driverName == QMYSQL)
        return createMySqlDriver();

    // if (driver == QPSQL)
    //     return createPostgresDriver();

    // if (driver == QSQLITE)
    //     return createSQLiteDriver();

    throw Exceptions::InvalidArgumentError(
                u"Unsupported driver '%1', available drivers: %2; "
                 "for '%3' database connection in %4()."_s
                .arg(driverName, SqlDatabase::drivers().join(SPACE), connectionName,
                     __tiny_func__));
}

/* private */

/* Our conventions for database names are MySql, Postgres, and SQLite, the same names
   like TinyORM connections or driver classes are named. 🫤 I will not break this, so
   shared/static libraries will also have these names. */

std::shared_ptr<SqlDriver> SqlDriverFactoryPrivate::createMySqlDriver() const // NOLINT(readability-convert-member-functions-to-static)
{
#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
    return createSqlDriverLoadable(u"TinyMySql"_s);
#else
    return std::make_shared<MySql::MySqlDriver>();
#endif
}

// std::shared_ptr<SqlDriver> SqlDriverFactory::createPostgresDriver() const
// {
// #ifdef TINYDRIVERS_PSQL_LOADABLE_LIBRARY
//     return createSqlDriverLoadable(QPSQL, u"TinyPostgres"_s);
// #else
//     return std::make_shared<Postgres::PostgresDriver>();
// #endif
// }

// std::shared_ptr<SqlDriver> SqlDriverFactory::createSQLiteDriver() const
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
    inline QStringList getDriverBasenameMsvc(const QString &driverBasename)
    {
        auto driverBasenameTmp = driverBasename;

        return {
            driverBasenameTmp.append(QString::number(TINYDRIVERS_VERSION_MAJOR)), // TinyMySql0.dll
            // qmake build doesn't support appending d after the basename for Debug builds
#if defined(TINYDRIVERS_DEBUG) && !defined(TINY_QMAKE_BUILD)
            driverBasenameTmp.append('d'_L1), // TinyMySql0d.dll
#endif
            /* As the last, try to load from the raw basename without any postfixes,
               we don't use this but it can be helpful in some situations. */
            driverBasename, // TinyMySql.dll
        };
    }
#endif

#ifdef __linux__
    /*! Get the MySQL driver shared library basename on Linux. */
    inline QStringList getDriverBasenameLinux(const QString &driverBasename)
    {
        auto driverBasenameTmp = driverBasename;

        return {
            driverBasenameTmp.prepend(u"lib"_s), // libTinyMySql.so
            // qmake build doesn't support appending d after the basename for Debug builds
#if defined(TINYDRIVERS_DEBUG) && !defined(TINY_QMAKE_BUILD)
            driverBasenameTmp.append('d'_L1), // libTinyMySqld.so
#endif
            /* As the last, try to load from the raw basename without any postfixes,
               we don't use this but it can be helpful in some situations. */
            driverBasename, // TinyMySql.so
        };
    }
#endif

#ifdef __MINGW32__
    /*! Get the MySQL driver shared library basename on MSYS2/MinGW. */
    inline QStringList getDriverBasenameMinGW(const QString &driverBasename)
    {
        auto driverBasenameTmp = driverBasename;

        return {
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
        };
    }
#endif

    /*! Get driver library basenames for which to try to load the shared library. */
    inline QStringList getDriverBasenames(const QString &driverBasename)
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
SqlDriverFactoryPrivate::createSqlDriverLoadable(const QString &driverBasenameRaw) const
{
    // Load a SQL driver shared library at runtime and return the driver factory method
    const auto &createDriverMemFn = loadSqlDriver(driverBasenameRaw);

    // This should never happen :/, it's checked earlier in loadSqlDriverCommon()
    Q_ASSERT(createDriverMemFn);

    return std::shared_ptr<SqlDriver>(std::invoke(createDriverMemFn));
}

const std::function<SqlDriver *()> &
SqlDriverFactoryPrivate::loadSqlDriver(const QString &driverBasenameRaw) const
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
    if (g_driversLoaded->contains(driverName)) T_LIKELY
        return g_driversLoaded->at(driverName);

    else T_UNLIKELY
        return loadSqlDriverCommon(driverBasenameRaw);
}

const std::function<SqlDriver *()> &
SqlDriverFactoryPrivate::loadSqlDriverCommon(const QString &driverBasenameRaw) const
{
    const auto driverBasenames = getDriverBasenames(driverBasenameRaw);

    /* Try to load library from paths from TINY/QT_PLUGIN_PATH environment variables and
       from paths from qmake/CMake build system (inside the build tree). */
    for (const auto &driverPath : sqlDriverPaths())
        // Try to load all our basenames
        for (const auto &driverBasename : driverBasenames)
            // Load a Tiny SQL driver shared library and resolve driver factory function
            if (auto createDriverMemFn = loadSqlDriverAndResolve(
                                             joinDriverPath(driverPath, driverBasename));
                createDriverMemFn
            ) {
                // Cache the result
                const auto [itDriverMemFn, ok] = g_driversLoaded
                                                 ->emplace(driverName, createDriverMemFn);
                // Insertion must always happen here
                Q_ASSERT(ok);

                return itDriverMemFn->second;
            }

    throw Exceptions::RuntimeError(
                u"Can't load '%1' shared library for '%2' driver at runtime, basenames "
                 "trying to load were: %3; for '%4' database connection in %5()."_s
                .arg(driverBasenameRaw, driverName, driverBasenames.join(COMMA),
                     connectionName, __tiny_func__));
}

SqlDriverFactoryPrivate::CreateSqlDriverMemFn
SqlDriverFactoryPrivate::loadSqlDriverAndResolve(const QString &driverFilepath) const
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
                u"The QLibrary(\"%1\") was loaded successfully but "
                 "the resolve(\"TinyDriverInstance\") failed for '%2' database "
                 "connection in %3()."_s
                .arg(driverFilepath, connectionName, __tiny_func__));
}

QStringList SqlDriverFactoryPrivate::sqlDriverPaths() const
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
    if (auto &&sqlDriverPath = sqlDriverPathFromBuildSystem(); sqlDriverPath)
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
SqlDriverFactoryPrivate::sqlDriverPathFromBuildSystem() const
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
    if (driverName == QMYSQL)
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

QString SqlDriverFactoryPrivate::getSqlDriverPath(const char *const driverPathRaw)
{
    auto driverPath = QString::fromUtf8(driverPathRaw);

    Q_ASSERT(!driverPath.isEmpty());

    return driverPath;
}

QString
SqlDriverFactoryPrivate::joinDriverPath(const QString &driverPath,
                                        const QString &driverBasename)
{
   return QDir::toNativeSeparators(u"%1/%2"_s.arg(driverPath, driverBasename));
}
#endif // TINYDRIVERS_MYSQL_LOADABLE_LIBRARY

} // namespace Orm::Drivers::Support

TINYORM_END_COMMON_NAMESPACE
