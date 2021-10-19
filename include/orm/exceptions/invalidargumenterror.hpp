#pragma once
#ifndef INVALIDARGUMENTERROR_HPP
#define INVALIDARGUMENTERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <stdexcept>

#include "orm/exceptions/logicerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! Invalid argument exception. */
    class SHAREDLIB_EXPORT InvalidArgumentError : public LogicError
    {
        /*! Inherit constructors. */
        using LogicError::LogicError;
    };

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // INVALIDARGUMENTERROR_HPP
