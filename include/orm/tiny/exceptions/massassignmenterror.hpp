#pragma once
#ifndef MASSASSIGNMENTERROR_H
#define MASSASSIGNMENTERROR_H

#include "orm/exceptions/runtimeerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny::Exceptions
{

    class SHAREDLIB_EXPORT MassAssignmentError : public Orm::Exceptions::RuntimeError
    {
    public:
        using Orm::Exceptions::RuntimeError::RuntimeError;
    };

} // namespace Orm::Tiny
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // MASSASSIGNMENTERROR_H
