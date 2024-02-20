#pragma once
#ifndef ORM_DRIVERS_SQLDATABASE_P_HPP
#define ORM_DRIVERS_SQLDATABASE_P_HPP

#include <QString>

#include <orm/macros/commonnamespace.hpp>

#include "orm/drivers/driverstypes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{
namespace Support
{
    class ConnectionsHash;
}

    class SqlDatabase;
    class SqlDriver;

    /*! SqlDatabase private implementation. */
    class SqlDatabasePrivate
    {
        Q_DISABLE_COPY_MOVE(SqlDatabasePrivate)

        /*! Alias for the ConnectionsHash. */
        using ConnectionsHash = Orm::Drivers::Support::ConnectionsHash;

        /*! Expose the NumericalPrecisionPolicy enum. */
        using enum Orm::Drivers::NumericalPrecisionPolicy;

    public:
        /*! Constructor. */
        inline SqlDatabasePrivate(std::shared_ptr<SqlDriver> &&driver,
                                  const QString &driverName_) noexcept;
        /*! Constructor. */
        inline SqlDatabasePrivate(std::shared_ptr<SqlDriver> &&driver,
                                  QString &&driverName_) noexcept;
        /*! Default destructor. */
        inline ~SqlDatabasePrivate() = default;

        /* Database connection */
        /*! Clone the SqlDatabasePrivate. */
        void cloneDatabase(const SqlDatabasePrivate &other) noexcept;

        /*! Determine whether the database driver isn't nullptr. */
        inline bool isDriverValid() const noexcept;

        /*! Get the driver reference. */
        inline SqlDriver &driver();
        /*! Get the driver reference, const version. */
        inline const SqlDriver &driver() const;
        /*! Get the driver pointer. */
        inline SqlDriver *driverPtr() const noexcept;
        /*! Reset and free the driver smart pointer. */
        inline void resetDriver() noexcept;

        /* Database Manager */
        /*! Obtain the given connection from the connections hash. */
        static SqlDatabase database(const QString &connection, bool open);

        /*! Insert a new database connection into the connections hash. */
        static SqlDatabase addDatabase(SqlDatabase &&db, const QString &connection);
        /*! Remove the given database connection from the connections hash. */
        static void removeDatabase(const QString &connection);
        /*! Invalidate the given connection (reset driver and clear connection name). */
        static void invalidateDatabase(const SqlDatabase &db, const QString &connection,
                                       bool warn = true);

        /*! Get a reference to the database connections hash. */
        static ConnectionsHash &connections();

        /*! Determine whether the same thread check is enabled during database() call. */
        static bool &checkSameThread() noexcept;

        /* Factory methods */
        /*! Factory method to create a new database driver by the given driver name. */
        static std::shared_ptr<SqlDriver> createSqlDriver(const QString &driver);

        /*! Factory method to create the SqlDatabase private implementation instance. */
        inline static std::shared_ptr<SqlDatabasePrivate>
        createSqlDatabasePrivate(const QString &driver);
        /*! Factory method to create the SqlDatabase private implementation instance. */
        inline static std::shared_ptr<SqlDatabasePrivate>
        createSqlDatabasePrivate(QString &&driver);
        /*! Factory method to create the SqlDatabase private implementation instance. */
        static std::shared_ptr<SqlDatabasePrivate>
        createSqlDatabasePrivate(std::shared_ptr<SqlDriver> &&driver);

        /* Others */
        /*! Throw if a given database connection isn't registered (doesn't exist). */
        static void throwIfNoConnection(const QString &connection);

        /* Data members */
        /* It's shared_ptr because of the SqlDatabase::driverWeak(),
           see the SqlQuery::driverWeak() for more details. */
        /*! Database driver instance. */
        std::shared_ptr<SqlDriver> sqldriver;

        /*! Connection driver name. */
        QString driverName;
        /*! Connection name. */
        QString connectionName {};

        /*! Connection hostname. */
        QString hostName {};
        /*! Connection database name. */
        QString databaseName {};
        /*! Connection username. */
        QString username {};
        /*! Connection password. */
        QString password {};
        /*! Connection extra options. */
        QString connectionOptions {};

        /*! Connection port. */
        int port = -1;
        /*! Connection numerical precision policy. */
        NumericalPrecisionPolicy precisionPolicy = LowPrecisionDouble;

    private:
        /* Factory methods */
        /*! Factory method to create a new MySQL driver instance (shared/loadable). */
        static std::shared_ptr<SqlDriver> createMySqlDriver();
        /*! Factory method to create new PostgreSQL driver instance (shared/loadable). */
        // static std::shared_ptr<SqlDriver> createPostgresDriver();
        /*! Factory method to create a new SQLite driver instance (shared/loadable). */
        // static std::shared_ptr<SqlDriver> createSQLiteDriver();

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
        /*! SQL driver factory function pointer type (allows construction in-place). */
        using CreateSqlDriverMemFn = SqlDriver *(*)();

        /*! Create a Tiny SQL driver instance (loads a shared library at runtime). */
        static std::shared_ptr<SqlDriver>
        createSqlDriverLoadable(const QString &driver, const QString &driverBasenameRaw);

        /*! Wrapper for loading a SQL driver shared library at runtime (thread-safe). */
        static const std::function<SqlDriver *()> &
        loadSqlDriver(const QString &driver, const QString &driverBasenameRaw);
        /*! Load a Tiny SQL driver shared library at runtime (actual code). */
        static const std::function<SqlDriver *()> &
        loadSqlDriverCommon(const QString &driver, const QString &driverBasenameRaw);
        /*! Load a Tiny SQL driver shared library and resolve driver factory function. */
        static CreateSqlDriverMemFn
        loadSqlDriverAndResolve(const QString &driverFilepath);

        /*! Get all non-standard Tiny SQL driver locations that LoadLibrary() doesn't
            load from (TINY_PLUGIN_PATH/QT_PLUGIN_PATH, qmake build folder). */
        static QStringList sqlDriverPaths(const QString &driver);
        /*! Get a shared library path inside the build tree (from qmake/CMake build
            system). */
        static std::optional<QString> sqlDriverPathFromBuildSystem(const QString &driver);
        /*! Get a shared library path inside the build tree (from qmake/CMake build
            system). */
        static QString getSqlDriverPath(const char *driverPathRaw);
        /*! Join the given driver path and driver basename (w/o file extension). */
        static QString joinDriverPath(const QString &driverPath,
                                      const QString &driverBasename);
#endif // TINYDRIVERS_MYSQL_LOADABLE_LIBRARY

        /* Others */
        /*! Throw an exception if the sqldriver is nullptr. */
        void throwIfSqlDriverIsNull() const;
        /*! Throw an exception if a connection was created in a different thread. */
        static void throwIfDifferentThread(const SqlDatabase &db,
                                           const QString &connection);
    };

    /* public */

    /* No need to check the driver for nullptr because that's already checked
       in the SqlDatabaseManager::addDatabase(). */

    SqlDatabasePrivate::SqlDatabasePrivate(std::shared_ptr<SqlDriver> &&driver,
                                           const QString &driverName_) noexcept
        : sqldriver(std::move(driver))
        , driverName(driverName_)
    {}

    SqlDatabasePrivate::SqlDatabasePrivate(std::shared_ptr<SqlDriver> &&driver,
                                           QString &&driverName_) noexcept
        : sqldriver(std::move(driver))
        , driverName(std::move(driverName_))
    {}

    /* Database connection */

    bool SqlDatabasePrivate::isDriverValid() const noexcept
    {
        return static_cast<bool>(sqldriver);
    }

    SqlDriver &SqlDatabasePrivate::driver()
    {
        // Throw an exception if the sqldriver is nullptr
        throwIfSqlDriverIsNull();

        return *sqldriver;
    }

    const SqlDriver &SqlDatabasePrivate::driver() const
    {
        // Throw an exception if the sqldriver is nullptr
        throwIfSqlDriverIsNull();

        return *sqldriver;
    }

    SqlDriver *SqlDatabasePrivate::driverPtr() const noexcept
    {
        return sqldriver.get();
    }

    void SqlDatabasePrivate::resetDriver() noexcept
    {
        sqldriver.reset();
    }

    /* Factory methods */

    std::shared_ptr<SqlDatabasePrivate>
    SqlDatabasePrivate::createSqlDatabasePrivate(const QString &driver)
    {
        return std::make_shared<SqlDatabasePrivate>(
                    SqlDatabasePrivate::createSqlDriver(driver), driver);
    }

    std::shared_ptr<SqlDatabasePrivate>
    SqlDatabasePrivate::createSqlDatabasePrivate(QString &&driver)
    {
        return std::make_shared<SqlDatabasePrivate>(
                    SqlDatabasePrivate::createSqlDriver(driver), std::move(driver));
    }

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SQLDATABASE_P_HPP
