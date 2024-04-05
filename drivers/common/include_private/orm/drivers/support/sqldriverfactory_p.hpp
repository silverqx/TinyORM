#pragma once
#ifndef ORM_DRIVERS_SUPPORT_SQLDRIVERFACTORY_P_HPP
#define ORM_DRIVERS_SUPPORT_SQLDRIVERFACTORY_P_HPP

#include <QString>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{
    class SqlDriver;

namespace Support
{

    /*! SQL driver factory. */
    class SqlDriverFactoryPrivate
    {
        Q_DISABLE_COPY_MOVE(SqlDriverFactoryPrivate)

    public:
        /*! Constructor. */
        inline SqlDriverFactoryPrivate(const QString &driver, const QString &connection);
        /*! Constructor. */
        inline SqlDriverFactoryPrivate(QString &&driver, const QString &connection);
        /*! Deleted destructor. */
        ~SqlDriverFactoryPrivate() = default;

        /*! Create a SqlDriver instance based on the driverName data member. */
        std::shared_ptr<SqlDriver> make() const;

    private:
#ifdef TINYDRIVERS_MYSQL_DRIVER
        /*! Factory method to create a new MySQL driver instance (shared/loadable). */
        std::shared_ptr<SqlDriver> createMySqlDriver() const;
#endif
#ifdef TINYDRIVERS_PSQL_DRIVER
        /*! Factory method to create new PostgreSQL driver instance (shared/loadable). */
        // std::shared_ptr<SqlDriver> createPostgresDriver() const;
#endif
#ifdef TINYDRIVERS_SQLITE_DRIVER
        /*! Factory method to create a new SQLite driver instance (shared/loadable). */
        // std::shared_ptr<SqlDriver> createSQLiteDriver() const;
#endif

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
        /*! SQL driver factory function pointer type (allows construction in-place). */
        using CreateSqlDriverMemFn = SqlDriver *(*)();

        /*! Create a Tiny SQL driver instance (loads a shared library at runtime). */
        std::shared_ptr<SqlDriver>
        createSqlDriverLoadable(const QString &driverBasenameRaw) const;

        /*! Wrapper for loading a SQL driver shared library at runtime (thread-safe). */
        const std::function<SqlDriver *()> &
        loadSqlDriver(const QString &driverBasenameRaw) const;
        /*! Load a Tiny SQL driver shared library at runtime (actual code). */
        const std::function<SqlDriver *()> &
        loadSqlDriverCommon(const QString &driverBasenameRaw) const;
        /*! Load a Tiny SQL driver shared library and resolve driver factory function. */
        CreateSqlDriverMemFn
        loadSqlDriverAndResolve(const QString &driverFilepath) const;

        /*! Get all non-standard Tiny SQL driver locations that LoadLibrary() doesn't
            load from (TINY_PLUGIN_PATH environment variable and qmake build folder). */
        QStringList sqlDriverPaths() const;
        /*! Get a shared library path inside the build tree (from qmake/CMake build
            system). */
        std::optional<QString> sqlDriverPathFromBuildSystem() const;
        /*! Get a shared library path inside the build tree (from qmake/CMake build
            system). */
        static QString getSqlDriverPath(const char *driverPathRaw);
        /*! Join the given driver path and driver basename (w/o file extension). */
        static QString joinDriverPath(const QString &driverPath,
                                      const QString &driverBasename);
        /*! Determine if the TinyDrivers shared library is located in the build tree. */
        static bool isTinyDriversInBuildTree();
#endif // TINYDRIVERS_MYSQL_LOADABLE_LIBRARY

        /*! Driver name to create SqlDriver instance for. */
        QString driverName;
        /*! Connection name (used in exceptions). */
        QString connectionName;
    };

    /* public */

    SqlDriverFactoryPrivate::SqlDriverFactoryPrivate(const QString &driver,
                                                     const QString &connection) // NOLINT(modernize-pass-by-value)
        : driverName(driver)
        , connectionName(connection)
    {}

    SqlDriverFactoryPrivate::SqlDriverFactoryPrivate(QString &&driver,
                                                     const QString &connection) // NOLINT(modernize-pass-by-value)
        : driverName(std::move(driver))
        , connectionName(connection)
    {}

} // namespace Support
} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SUPPORT_SQLDRIVERFACTORY_P_HPP
