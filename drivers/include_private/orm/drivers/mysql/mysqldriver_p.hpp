#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLDRIVER_P_HPP
#define ORM_DRIVERS_MYSQL_MYSQLDRIVER_P_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "orm/drivers/macros/includemysqlh.hpp" // IWYU pragma: keep

#include "orm/drivers/sqldriver_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::MySql
{

    class MySqlDriverPrivate : public SqlDriverPrivate
    {
        Q_DISABLE_COPY_MOVE(MySqlDriverPrivate)

    public:
        /*! Default constructor. */
        inline MySqlDriverPrivate();
        /*! Default destructor. */
        inline ~MySqlDriverPrivate() = default;

        static void setOptionFlag(uint &optionFlags, QStringView option);
        static bool setOptionString(MYSQL *mysql, mysql_option option, QStringView value);
        static bool setOptionInt(MYSQL *mysql, mysql_option option, QStringView value);
        static bool setOptionBool(MYSQL *mysql, mysql_option option, QStringView value);
        static bool setOptionProtocol(MYSQL *mysql, mysql_option option,
                                      QStringView value);
// The MYSQL_OPT_SSL_MODE was added in MySQL 5.7.11
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID >= 50711 && !defined(MARIADB_VERSION_ID)
        static bool setOptionSslMode(MYSQL *mysql, mysql_option option,
                                     QStringView value);
#endif

        inline bool supportsTransactions() const noexcept;

        /* Data members */
        MYSQL *mysql = nullptr;
        QString dbName {};
    };

    /* public */

    MySqlDriverPrivate::MySqlDriverPrivate()
        : SqlDriverPrivate(SqlDriver::MySqlServer)
    {}

    bool MySqlDriverPrivate::supportsTransactions() const noexcept
    {
        return mysql != nullptr &&
               (mysql->server_capabilities & CLIENT_TRANSACTIONS) == CLIENT_TRANSACTIONS;
    }

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_MYSQL_MYSQLDRIVER_P_HPP
