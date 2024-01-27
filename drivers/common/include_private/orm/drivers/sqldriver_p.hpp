#pragma once
#ifndef ORM_DRIVERS_SQLDRIVER_P_HPP
#define ORM_DRIVERS_SQLDRIVER_P_HPP

#include <thread>

#include "orm/drivers/sqldriver.hpp"
#include "orm/drivers/sqlerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    /*! SqlDriver private implementation. */
    class SqlDriverPrivate
    {
        Q_DISABLE_COPY_MOVE(SqlDriverPrivate)

        /*! Expose the NumericalPrecisionPolicy enum. */
        using enum Orm::Drivers::NumericalPrecisionPolicy;

    public:
        /*! Constructor. */
        inline explicit SqlDriverPrivate(SqlDriver::DbmsType type) noexcept;
        // CUR drivers check virtual here, without it the ~MySqlDriverPrivate() is not called of course silverqx
        /*! Virtual destructor. */
        inline virtual ~SqlDriverPrivate() = default;

        /* Data members */
        /*! The last error that occurred on the database. */
        SqlError lastError {};
        /*! Database driver type. */
        SqlDriver::DbmsType dbmsType;
        /*! The default numerical precision policy. */
        NumericalPrecisionPolicy defaultPrecisionPolicy = LowPrecisionDouble;

        /*! The thread ID in which the driver was instantiated. */
        std::thread::id threadId = std::this_thread::get_id();

        /*! Is the connection currently open? */
        bool isOpen = false;
        /*! Was there an error while opening a new database connection? */
        bool isOpenError = false;
    };

    /* public */

    SqlDriverPrivate::SqlDriverPrivate(const SqlDriver::DbmsType type) noexcept
        : dbmsType(type)
    {}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SQLDRIVER_P_HPP
