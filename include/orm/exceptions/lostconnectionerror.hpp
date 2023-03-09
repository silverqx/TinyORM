#pragma once
#ifndef ORM_EXCEPTIONS_LOSTCONNECTIONERROR_HPP
#define ORM_EXCEPTIONS_LOSTCONNECTIONERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/exceptions/logicerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! TinyORM lost connection exception. */
    class LostConnectionError : public LogicError // clazy:exclude=copyable-polymorphic
    {
        /*! Inherit constructors. */
        using LogicError::LogicError;
    };

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_EXCEPTIONS_LOSTCONNECTIONERROR_HPP
