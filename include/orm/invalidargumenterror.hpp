#pragma once
#ifndef INVALIDARGUMENTERROR_H
#define INVALIDARGUMENTERROR_H

#include <stdexcept>

#include "orm/logicerror.hpp"
#include "orm/utils/export.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    // CUR move all exception classes to exceptions folder and own exception NS? silverqx
    class SHAREDLIB_EXPORT InvalidArgumentError : public LogicError
    {
        using LogicError::LogicError;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // INVALIDARGUMENTERROR_H
