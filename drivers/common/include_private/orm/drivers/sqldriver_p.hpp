#pragma once
#ifndef ORM_DRIVERS_SQLDRIVER_P_HPP
#define ORM_DRIVERS_SQLDRIVER_P_HPP

#include <thread>

#include "orm/drivers/sqldriver.hpp"

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
        /*! Pure virtual destructor. */
        inline virtual ~SqlDriverPrivate() = 0;

        /* Data members */
        /*! Database driver type. */
        SqlDriver::DbmsType dbmsType;
        /*! The default numerical precision policy. */
        NumericalPrecisionPolicy defaultPrecisionPolicy = LowPrecisionDouble;

        /*! The thread ID in which the driver was instantiated. */
        std::thread::id threadId = std::this_thread::get_id();

        /*! Is the connection currently open? */
        bool isOpen = false;
    };

    /* public */

    SqlDriverPrivate::~SqlDriverPrivate() = default;

    SqlDriverPrivate::SqlDriverPrivate(const SqlDriver::DbmsType type) noexcept
        : dbmsType(type)
    {}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SQLDRIVER_P_HPP
