#pragma once
#ifndef ORM_DRIVERS_EXCEPTIONS_OUTOFRANGEERROR_HPP
#define ORM_DRIVERS_EXCEPTIONS_OUTOFRANGEERROR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "orm/drivers/exceptions/logicerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::Exceptions
{

    /*! TinyDrivers out of range exception. */
    class OutOfRangeError : public LogicError // clazy:exclude=copyable-polymorphic
    {
        /*! Inherit constructors. */
        using LogicError::LogicError;
    };

} // namespace Orm::Drivers::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_EXCEPTIONS_OUTOFRANGEERROR_HPP
