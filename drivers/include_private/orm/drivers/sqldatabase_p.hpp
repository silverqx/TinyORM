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
                                  const QString &driverName);
        /*! Constructor. */
        inline SqlDatabasePrivate(std::shared_ptr<SqlDriver> &&driver,
                                  QString &&driverName);
        /*! Default destructor. */
        inline ~SqlDatabasePrivate() = default;

        /* Database connection */
        /*! Clone the SqlDatabasePrivate. */
        void cloneDatabase(const SqlDatabasePrivate &other);

        /*! Determine whether the database driver isn't nullptr. */
        inline bool isDriverValid() const noexcept;

        /*! Get the driver reference. */
        inline SqlDriver &driver();
        /*! Get the driver reference, const version. */
        inline const SqlDriver &driver() const;
        /*! Reset and free the driver smart pointer. */
        inline void resetDriver() noexcept;

        /* Database Manager */
        /*! Obtain the given connection from the connections hash. */
        static SqlDatabase database(const QString &connection, bool open);

        /*! Insert a new database connection into the connections hash. */
        static void addDatabase(const SqlDatabase &db, const QString &connection);
        /*! Remove the given database connection from the connections hash. */
        static void removeDatabase(const QString &connection);
        /*! Invalidate the given connection (reset driver and clear connection name). */
        static void invalidateDb(const SqlDatabase &db, const QString &connection,
                                 bool warn = true);

        /*! Get a reference to the database connections hash. */
        static ConnectionsHash &connections();

        /*! Determine whether the different thread check is enabled during database(). */
        static bool &checkDifferentThread() noexcept;

        /* Factory methods */
        /*! Factory method to create a new database driver by the given driver name. */
        static std::shared_ptr<SqlDriver> createDriver(const QString &driver);

        /*! Factory method to create the SqlDatabase private implementation instance. */
        inline static std::shared_ptr<SqlDatabasePrivate>
        createSqlDatabasePrivate(const QString &driver);
        /*! Factory method to create the SqlDatabase private implementation instance. */
        static std::shared_ptr<SqlDatabasePrivate>
        createSqlDatabasePrivate(std::shared_ptr<SqlDriver> &&driver);

        /* Data members */
        // It's shared_ptr because of the SqlDatabase::driver()
        /*! Database driver instance. */
        std::shared_ptr<SqlDriver> sqldriver;

        /*! Connection driver name. */
        QString driverName;
        /*! Connection name. */
        QString connectionName;

        /*! Connection hostname. */
        QString hostName;
        /*! Connection database name. */
        QString databaseName;
        /*! Connection username. */
        QString username;
        /*! Connection password. */
        QString password;
        /*! Connection extra options. */
        QString connectionOptions;

        /*! Connection port. */
        int port = -1;
        /*! Connection numerical precision policy. */
        NumericalPrecisionPolicy precisionPolicy = LowPrecisionDouble;

    private:
        /*! Throw an exception if the sqldriver is nullptr. */
        void throwIfSqlDriverIsNull() const;
        /*! Throw an exception if a connection was created in a different thread. */
        static void throwIfDifferentThread(const SqlDatabase &db,
                                           const QString &connection);

        /*! Determine whether the given connection is invalid and log a warning. */
        static bool isInvalidWithWarning(const SqlDatabase &db,
                                         const QString &connection);
    };

    /* public */

    /* No need to check the driver for nullptr because that's already checked
       in the SqlDatabaseManager::addDatabase(). */

    SqlDatabasePrivate::SqlDatabasePrivate(std::shared_ptr<SqlDriver> &&driver,
                                           const QString &driverName)
        : sqldriver(std::move(driver))
        , driverName(driverName)
    {}

    SqlDatabasePrivate::SqlDatabasePrivate(std::shared_ptr<SqlDriver> &&driver,
                                           QString &&driverName)
        : sqldriver(std::move(driver))
        , driverName(std::move(driverName))
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

    void SqlDatabasePrivate::resetDriver() noexcept
    {
        sqldriver.reset();
    }

    /* Factory methods */

    std::shared_ptr<SqlDatabasePrivate>
    SqlDatabasePrivate::createSqlDatabasePrivate(const QString &driver)
    {
        return std::make_shared<SqlDatabasePrivate>(
                    SqlDatabasePrivate::createDriver(driver), driver);
    }

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SQLDATABASE_P_HPP
