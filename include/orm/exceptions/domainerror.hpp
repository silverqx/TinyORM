#pragma once
#ifndef DOMAINERROR_H
#define DOMAINERROR_H

#include <stdexcept>

#include "orm/exceptions/logicerror.hpp"
#include "orm/utils/export.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Exceptions
{

    /*! Domain exception. */
    class SHAREDLIB_EXPORT DomainError : public LogicError
    {
        /*! Inherit constructors. */
        using LogicError::LogicError;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // DOMAINERROR_H
