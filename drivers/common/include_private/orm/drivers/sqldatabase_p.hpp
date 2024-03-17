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
    class ConnectionsHashPrivate;
}

    class SqlDatabase;
    class SqlDriver;

    /*! SqlDatabase private implementation. */
    class SqlDatabasePrivate
    {
        Q_DISABLE_COPY_MOVE(SqlDatabasePrivate)

        /*! Alias for the ConnectionsHashPrivate. */
        using ConnectionsHash = Orm::Drivers::Support::ConnectionsHashPrivate;

        /*! Expose the NumericalPrecisionPolicy enum. */
        using enum Orm::Drivers::NumericalPrecisionPolicy;

    public:
        /*! Constructor. */
        inline explicit SqlDatabasePrivate(std::shared_ptr<SqlDriver> &&driver) noexcept;
        /*! Default destructor. */
        ~SqlDatabasePrivate() = default;

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
        static std::atomic_bool &checkSameThread() noexcept;

        /* Factory methods */
        /*! Factory method to create the SqlDatabase private implementation instance. */
        static std::shared_ptr<SqlDatabasePrivate>
        createSqlDatabasePrivate(const QString &driver, const QString &connection);
        /*! Factory method to create the SqlDatabase private implementation instance. */
        static std::shared_ptr<SqlDatabasePrivate>
        createSqlDatabasePrivate(QString &&driver, const QString &connection);
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

        /* The sqldriver instance also has the same driverName and connectionName data
           members but we need to make local copies here because sqldriver can be
           invalidated (destroyed) and we would lost them. */
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
        /* Others */
        /*! Throw an exception if the sqldriver is nullptr. */
        void throwIfSqlDriverIsNull() const;
        /*! Throw an exception if a connection was created in a different thread. */
        static void throwIfDifferentThread(const SqlDatabase &db,
                                           const QString &connection);
    };

    /* public */

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

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SQLDATABASE_P_HPP
