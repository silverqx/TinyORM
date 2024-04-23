#include "orm/drivers/support/sqldriverfactory_p.hpp"

#ifdef _WIN32
#  include <qt_windows.h>
#endif

#include <mutex>

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
#  include <filesystem>
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
#  include "orm/drivers/utils/fs_p.hpp"
#  include "orm/drivers/version.hpp" // IWYU pragma: keep
#endif

#if defined(TINYDRIVERS_MYSQL_DRIVER) && !defined(TINYDRIVERS_MYSQL_LOADABLE_LIBRARY)
#  include "orm/drivers/mysql/mysqldriver.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

using Orm::Drivers::Constants::QMYSQL;
using Orm::Drivers::Constants::SPACE;

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Orm::Drivers::Constants::COMMA;

using FsUtils = Orm::Drivers::Utils::FsPrivate;
#endif

namespace Orm::Drivers::Support
{

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
/*! SQL loadable drivers' factory methods hash type. */
using DriverLoadedHashType = std::unordered_map<QString, std::function<SqlDriver *()>>;
/*! SQL loadable drivers' factory methods hash (used to check if driver is loaded too). */
Q_GLOBAL_STATIC(DriverLoadedHashType, g_driversLoaded) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
/*! Mutex to secure loading of loadable driver shared libraries. */
Q_GLOBAL_STATIC(std::shared_mutex, mx_driversLoaded) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
#endif

/* public */

std::shared_ptr<SqlDriver> SqlDriverFactoryPrivate::make() const
{
    Q_ASSERT(!driverName.isEmpty());

#ifdef TINYDRIVERS_MYSQL_DRIVER
    if (driverName == QMYSQL)
        return createMySqlDriver();
#endif

#ifdef TINYDRIVERS_PSQL_DRIVER
    // if (driver == QPSQL)
    //     return createPostgresDriver();
#endif

#ifdef TINYDRIVERS_SQLITE_DRIVER
    // if (driver == QSQLITE)
    //     return createSQLiteDriver();
#endif

    throw Exceptions::InvalidArgumentError(
                u"Unsupported driver '%1', available drivers: %2; "
                 "for '%3' database connection in %4()."_s
                .arg(driverName, SqlDatabase::drivers().join(SPACE), connectionName,
                     __tiny_func__));
}

/* private */

/* Our conventions for database names are MySql, Postgres, and SQLite, the same names
   like TinyORM connections or driver classes are named. 🫤 I will not break this, so
   shared/static libraries will also have these names.
   To be more confusing all preprocessor macros for PostgeSQL are named
   TINYDRIVERS_PSQL_ I will not change it to TINYDRIVERS_POSTGRES_ I like it this way.*/

#ifdef TINYDRIVERS_MYSQL_DRIVER
std::shared_ptr<SqlDriver> SqlDriverFactoryPrivate::createMySqlDriver() const // NOLINT(readability-convert-member-functions-to-static)
{
#  ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
    return createSqlDriverLoadable(u"TinyMySql"_s);
#  else
    return std::make_shared<MySql::MySqlDriver>();
#  endif
}
#endif

#ifdef TINYDRIVERS_PSQL_DRIVER
// std::shared_ptr<SqlDriver> SqlDriverFactory::createPostgresDriver() const
// {
// #  ifdef TINYDRIVERS_PSQL_LOADABLE_LIBRARY
//     return createSqlDriverLoadable(QPSQL, u"TinyPostgres"_s);
// #  else
//     return std::make_shared<Postgres::PostgresDriver>();
// #  endif
// }
#endif

#ifdef TINYDRIVERS_SQLITE_DRIVER
// std::shared_ptr<SqlDriver> SqlDriverFactory::createSQLiteDriver() const
// {
// #  ifdef TINYDRIVERS_SQLITE_LOADABLE_LIBRARY
//     return createSqlDriverLoadable(QSQLITE, u"TinySQLite"_s);
// #  else
//     return std::make_shared<SQLite::SQLiteDriver>();
// #  endif
// }
#endif

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
namespace
{
#ifdef _MSC_VER
    /*! Get the MySQL driver shared library basename for MSVC. */
    inline QStringList getDriverBasenameMsvc(const QString &driverBasenameRaw)
    {
        static const auto versionMajor = QString::number(TINYDRIVERS_VERSION_MAJOR);
        static const auto driverBasenameVersion = driverBasenameRaw + versionMajor;

        return {
            // qmake build doesn't support appending d after the basename for Debug builds
#if defined(TINYDRIVERS_DEBUG) && !defined(TINY_QMAKE_BUILD_PRIVATE)
            driverBasenameVersion + 'd'_L1, // TinyMySql0d.dll
#endif
            driverBasenameVersion, // TinyMySql0.dll
            /* As the last, try to load from the raw basename without any postfixes,
               we don't use this but it can be helpful in some situations. */
            driverBasenameRaw, // TinyMySql.dll
        };
    }
#endif

#ifdef __linux__
    /*! Get the MySQL driver shared library basename on Linux. */
    inline QStringList getDriverBasenameLinux(const QString &driverBasenameRaw)
    {
        static const auto driverBasenameLib = u"lib"_s + driverBasenameRaw;

        return {
            // qmake build doesn't support appending d after the basename for Debug builds
#if defined(TINYDRIVERS_DEBUG) && !defined(TINY_QMAKE_BUILD_PRIVATE)
            driverBasenameLib + 'd'_L1, // libTinyMySqld.so
#endif
            driverBasenameLib, // libTinyMySql.so
#if defined(TINYDRIVERS_DEBUG) && !defined(TINY_QMAKE_BUILD_PRIVATE)
            driverBasenameRaw + 'd'_L1, // TinyMySqld.so
#endif
            /* As the last, try to load from the raw basename without any postfixes,
               we don't use this but it can be helpful in some situations. */
            driverBasenameRaw, // TinyMySql.so
        };
    }
#endif

#ifdef __MINGW32__
    /*! Get the MySQL driver shared library basename on MSYS2/MinGW. */
    inline QStringList getDriverBasenameMinGW(const QString &driverBasenameRaw)
    {
        static const auto versionMajor = QString::number(TINYDRIVERS_VERSION_MAJOR);

        // qmake build doesn't prepend lib for shared libraries on MSYS2
#ifdef TINY_QMAKE_BUILD_PRIVATE
        static const auto driverBasename = driverBasenameRaw;
#else
        static const auto driverBasename = u"lib"_s + driverBasenameRaw;
        static const auto driverBasenameVersion = driverBasename + versionMajor;
#endif
        static const auto driverBasenameRawVersion = driverBasenameRaw + versionMajor;

        return {
            // CMake builds only (or any other build system that prepends lib prefix)
#ifndef TINY_QMAKE_BUILD_PRIVATE
#  ifdef TINYDRIVERS_DEBUG
            driverBasenameVersion + 'd'_L1, // libTinyMySql0d.dll
#  endif
            driverBasenameVersion, // libTinyMySql0.dll
            driverBasename, // libTinyMySql.dll
#endif
            // Also, try all other combinations without the lib prefix
            // qmake build doesn't support appending d after the basename for Debug builds
#if defined(TINYDRIVERS_DEBUG) && !defined(TINY_QMAKE_BUILD_PRIVATE)
            driverBasenameRawVersion + 'd'_L1,
#endif
            driverBasenameRawVersion,
            /* As the last, try to load from the raw basename without any postfixes,
               we don't use this but it can be helpful in some situations. */
            driverBasenameRaw, // TinyMySql.dll
        };
    }
#endif

    /*! Get driver library basenames for which to try to load the shared library. */
    inline QStringList getDriverBasenames(const QString &driverBasenameRaw)
    {
#ifdef _MSC_VER
        return getDriverBasenameMsvc(driverBasenameRaw);
#elif defined(__linux__)
        return getDriverBasenameLinux(driverBasenameRaw);
#elif defined(__MINGW32__)
        return getDriverBasenameMinGW(driverBasenameRaw);
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

    /* Try to load library from paths from TINY_PLUGIN_PATH environment variable and
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
                u"Unable to load '%1' shared library for '%2' driver at runtime, "
                 "basenames attempted to load: %3; for '%4' database connection "
                 "in %5()."_s
                .arg(driverBasenameRaw, driverName, driverBasenames.join(COMMA),
                     connectionName, __tiny_func__));
}

SqlDriverFactoryPrivate::CreateSqlDriverMemFn
SqlDriverFactoryPrivate::loadSqlDriverAndResolve(const QString &driverFilepath) const
{
    /* I will not use QLibrary() overload with the version number on Linux because CMake
       doesn't create namelinks for MODULE libraries and also it doesn't write SONAME
       into the library header. I have created a separate implementation for Linux that
       uses QLibrary() with a version number, but I forgot about this, also, a few weeks
       ago I disabled writting the SONAME into the library header for qmake builds using
       the CONFIG *= unversioned_libname. So I definitely giving up about this QLibrary()
       overload for Linux. I have stashed this new implementation if needed. */

    // Version number doesn't work on MSYS2, it uses qlibrary_win.cpp implementation
    QLibrary sqlDriverLib(driverFilepath);

    // Nothing to do, the driver isn't on the given filepath
    if (!sqlDriverLib.load())
        return nullptr;

    // Resolve the TinyDriverInstance function
    auto *const createSqlDriverMemFn = reinterpret_cast<CreateSqlDriverMemFn>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
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
    /* The order eg. for the TinyMySql driver on MSVC and Debug configuration will be:
       TINY_PLUGIN_PATH/TinyMySql0d
       TINY_PLUGIN_PATH/TinyMySql0
       TINY_PLUGIN_PATH/TinyMySql
       TinyMySql0d
       TinyMySql0
       TinyMySql
       TINYDRIVERS_MYSQL_PATH/TinyMySql0d

       What means, TINY_PLUGIN_PATH has higher priority, then the default QLibrary()
       behavior, and as the last load from the build tree if TinyDrivers library itself is
       located in the build tree.
       Also, first try 0d because that is what we are expecting, and then other less
       likely possibilities like 0 and as the last without any postfixes (raw basename).
    */

    const auto envTinyPluginPath        = qEnvironmentVariable("TINY_PLUGIN_PATH");
    const auto hasTinyPluginPaths       = !envTinyPluginPath.isEmpty();
    const auto isTinyDriversInBuildTree = SqlDriverFactoryPrivate::
                                          isTinyDriversInBuildTree();

    constexpr static auto listSeparator = QDir::listSeparator();

    QStringList result;
    // +1 for an empty path
    result.reserve(envTinyPluginPath.count(listSeparator) +
                   (hasTinyPluginPaths ? 1 : 0) + 1 +
                   (isTinyDriversInBuildTree ? 1 : 0));

    // Paths from the TINY_PLUGIN_PATH environment variable
    if (hasTinyPluginPaths)
        result << envTinyPluginPath.split(listSeparator, Qt::SkipEmptyParts);

    // An empty path, QLibrary(driverBasename) will try system-specific prefixes/suffixes
    result << QString();

    /* Path from qmake/CMake build system (shared library path inside the build tree).
       If loading from all the paths above failed then load the SQL driver library from
       the build tree. Load it only if the TinyDrivers library location is
       in the build tree. This ensures that it will not be loaded from the build tree,
       e.g. after installation because it is non-standard behavior. */
    if (isTinyDriversInBuildTree)
        if (auto &&sqlDriverPath = sqlDriverPathFromBuildSystem(); sqlDriverPath)
            result << std::move(*sqlDriverPath);

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

    // Don't call cleanPath() here, will be called later in joinDriverPath()
    return driverPath;
}

QString
SqlDriverFactoryPrivate::joinDriverPath(const QString &driverPath,
                                        const QString &driverBasename)
{
    // Nothing to do, return the basename without any path
    if (driverPath.isEmpty())
        return driverBasename;

    /* Don't call absolutePath() or canonicalPath() here, we need to preserve what ever
       path a user passed into, cleanPath() is ok to have / slashes only and to remove
       redundant '..'. */
    return QDir(QDir::cleanPath(driverPath)).filePath(driverBasename);
}

namespace
{
#ifdef _WIN32
    /*! Get module handle for the current TinyDrivers DLL library. */
    inline HMODULE getModuleHandle() noexcept
    {
        HMODULE handle = nullptr;

        // Obtain the module handle from the given static method address
        ::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCTSTR>(&SqlDatabase::database), &handle); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

        // No need to check if function failed, it sets the handle to nullptr if failed

        return handle;
    }
#endif // _WIN32

    /*! Get the fully qualified path for the current TinyDrivers DLL library. */
    inline auto getModuleFilepath()
    {
#ifdef _WIN32
        auto *const handle = getModuleHandle();

        // Nothing to do
        if (handle == nullptr)
            return std::wstring();

        return FsUtils::getModuleFileName(handle);
#elif __linux__
        return FsUtils::getModuleFileName(
                    reinterpret_cast<void *>(&SqlDatabase::database));
#else
#  error Unsupported OS or platform in Orm::Drivers::Utils::FsFsPrivate.
#endif
    }
} // namespace

bool SqlDriverFactoryPrivate::isTinyDriversInBuildTree()
{
    // Get the fully qualified path for the current TinyDrivers DLL library
    const auto moduleFilepath = getModuleFilepath();

    // Nothing to do, obtaining the module filepath failed
    if (moduleFilepath.empty())
        return false;

    return fs::exists(fspath(moduleFilepath).replace_filename(".build_tree"));
}
#endif // TINYDRIVERS_MYSQL_LOADABLE_LIBRARY

} // namespace Orm::Drivers::Support

TINYORM_END_COMMON_NAMESPACE
