#pragma once
#ifndef ORM_DRIVERS_EXCEPTIONS_DRIVERSERROR_HPP
#define ORM_DRIVERS_EXCEPTIONS_DRIVERSERROR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::Exceptions
{

    /*! TinyDrivers exceptions tag, all TinyDrivers exceptions are derived from this
        class. */
    class DriversError // clazy:exclude=copyable-polymorphic
    {
    public:
        /*! Pure virtual destructor. */
        inline virtual ~DriversError() = 0;
    };

    /* public */

    DriversError::~DriversError() = default;

} // namespace Orm::Drivers::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_EXCEPTIONS_DRIVERSERROR_HPP
