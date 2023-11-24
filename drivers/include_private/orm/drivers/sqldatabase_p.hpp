#pragma once
#ifndef ORM_DRIVERS_SQLDATABASE_P_HPP
#define ORM_DRIVERS_SQLDATABASE_P_HPP

#include <QString>
#include <QtSql/private/qtsqlglobal_p.h>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{
namespace Support
{
    class ConnectionsHash;
}

    class SqlDatabase;
    class SqlDriver;

    class SqlDatabasePrivate
    {
        Q_DISABLE_COPY_MOVE(SqlDatabasePrivate)

        /*! Alias for the ConnectionsHash. */
        using ConnectionsHash = Orm::Drivers::Support::ConnectionsHash;

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

        inline bool isDriverValid() const noexcept;

        /*! Get driver reference. */
        inline SqlDriver &driver();
        /*! Get driver reference, const version. */
        inline const SqlDriver &driver() const;
        /*! Reset and free driver smart pointer. */
        inline void resetDriver() noexcept;

        /* Database Manager */
        static SqlDatabase database(const QString &connection, bool open);

        static void addDatabase(const SqlDatabase &db, const QString &connection);
        static void removeDatabase(const QString &connection);
        static void invalidateDb(const SqlDatabase &db, const QString &connection,
                                 bool warn = true);

        static ConnectionsHash &connections();

        /* Factory methods */
        static std::shared_ptr<SqlDriver> createDriver(const QString &driver);

        inline static std::shared_ptr<SqlDatabasePrivate>
        createSqlDatabasePrivate(const QString &driver);
        static std::shared_ptr<SqlDatabasePrivate>
        createSqlDatabasePrivate(std::shared_ptr<SqlDriver> &&driver);

        /* Data members */
        // It's shared_ptr because of the SqlDatabase::driver()
        std::shared_ptr<SqlDriver> sqldriver;

        QString driverName;
        QString connectionName;

        QString hostName;
        QString databaseName;
        QString username;
        QString password;
        QString connectionOptions;

        int port = -1;
        QSql::NumericalPrecisionPolicy precisionPolicy = QSql::LowPrecisionDouble;

    private:
        void throwIfSqlDriverIsNull() const;
    };

    /* public */

    SqlDatabasePrivate::SqlDatabasePrivate(std::shared_ptr<SqlDriver> &&driver,
                                           const QString &driverName)
        : sqldriver(std::move(driver))
        , driverName(driverName)
    {
        // CUR drivers check all other smart pointers in drivers to nullptr like below, especially in constructors silverqx
        Q_ASSERT(this->sqldriver);
    }

    SqlDatabasePrivate::SqlDatabasePrivate(std::shared_ptr<SqlDriver> &&driver,
                                           QString &&driverName)
        : sqldriver(std::move(driver))
        , driverName(std::move(driverName))
    {
        // CUR drivers check all other smart pointers in drivers to nullptr like below, especially in constructors silverqx
        Q_ASSERT(this->sqldriver);
    }

    /* Database connection */

    bool SqlDatabasePrivate::isDriverValid() const noexcept
    {
        return static_cast<bool>(sqldriver);
    }

    SqlDriver &SqlDatabasePrivate::driver()
    {
        throwIfSqlDriverIsNull();

        return *sqldriver;
    }

    const SqlDriver &SqlDatabasePrivate::driver() const
    {
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
