#pragma once
#ifndef ORM_DRIVERS_EXCEPTIONS_INVALIDARGUMENTERROR_HPP
#define ORM_DRIVERS_EXCEPTIONS_INVALIDARGUMENTERROR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "orm/drivers/exceptions/logicerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::Exceptions
{

    /*! TinyDrivers invalid argument exception. */
    class InvalidArgumentError : public LogicError // clazy:exclude=copyable-polymorphic
    {
        /*! Inherit constructors. */
        using LogicError::LogicError;
    };

} // namespace Orm::Drivers::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_EXCEPTIONS_INVALIDARGUMENTERROR_HPP
