#pragma once
#ifndef INVALIDFORMATERROR_H
#define INVALIDFORMATERROR_H

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

#endif // INVALIDFORMATERROR_H
