#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLDRIVER_HPP
#define ORM_DRIVERS_MYSQL_MYSQLDRIVER_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "orm/drivers/sqldriver.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::MySql
{

    class MySqlDriverPrivate;

    /*! MySQL database driver. */
    class TINYDRIVERS_EXPORT MySqlDriver final : public SqlDriver
    {
        Q_DISABLE_COPY_MOVE(MySqlDriver)
        Q_DECLARE_PRIVATE(MySqlDriver)

        /* To access MySqlDriver::d_func() through the MySqlResultPrivate::drv_d_func(),
           what means that the MySqlResultPrivate will have access
           to the MySqlDriverPrivate. */
        friend class MySqlResultPrivate;

    public:
        /*! Default constructor. */
        MySqlDriver();
        /*! Virtual destructor. */
        inline ~MySqlDriver() final = default;

        /*! Open the database connection using the given values. */
        bool open(const QString &database, const QString &username,
                  const QString &password, const QString &host, int port,
                  const QString &options) final;
        /*! Close the current database connection. */
        void close() final;

        /* Getters / Setters */
        /*! Determine whether the current driver supports the given feature. */
        bool hasFeature(DriverFeature feature) const final;

        /*! Get the low-level MySQL database handle (MYSQL *). */
        QVariant handle() const final;
        /*! Get the MySQL database driver name. */
        inline QString driverName() const final;

        /* Transactions */
        /*! Start a new database transaction. */
        bool beginTransaction() final;
        /*! Commit the active database transaction. */
        bool commitTransaction() final;
        /*! Rollback the active database transaction. */
        bool rollbackTransaction() final;

        /* Others */
        /*! Determine whether the given MySQL identifier is escaped. */
        bool isIdentifierEscaped(const QString &identifier,
                                 IdentifierType type) const final;

        /*! Factory method to create an empty MySQL result. */
        std::unique_ptr<SqlResult>
        createResult(const std::weak_ptr<SqlDriver> &driver) const final;
    };

    /* public */

    /* Getters / Setters */

    QString MySqlDriver::driverName() const
    {
        using namespace Qt::StringLiterals;

        return u"QMYSQL"_s;
    }

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_MYSQL_MYSQLDRIVER_HPP
