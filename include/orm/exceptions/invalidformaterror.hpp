#pragma once
#ifndef INVALIDFORMATERROR_HPP
#define INVALIDFORMATERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/exceptions/logicerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Exceptions
{

    /*! Invalid format exception. */
    class SHAREDLIB_EXPORT InvalidFormatError : public LogicError
    {
        /*! Inherit constructors. */
        using LogicError::LogicError;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // INVALIDFORMATERROR_HPP
