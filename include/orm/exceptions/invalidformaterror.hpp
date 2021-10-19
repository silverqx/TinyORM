#pragma once
#ifndef INVALIDFORMATERROR_HPP
#define INVALIDFORMATERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/exceptions/logicerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! Invalid format exception. */
    class SHAREDLIB_EXPORT InvalidFormatError : public LogicError
    {
        /*! Inherit constructors. */
        using LogicError::LogicError;
    };

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // INVALIDFORMATERROR_HPP
