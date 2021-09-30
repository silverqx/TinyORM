#pragma once
#ifndef INVALIDARGUMENTERROR_HPP
#define INVALIDARGUMENTERROR_HPP

#include <stdexcept>

#include "orm/exceptions/logicerror.hpp"
#include "orm/utils/export.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Exceptions
{

    /*! Invalid argument exception. */
    class SHAREDLIB_EXPORT InvalidArgumentError : public LogicError
    {
        /*! Inherit constructors. */
        using LogicError::LogicError;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // INVALIDARGUMENTERROR_HPP
