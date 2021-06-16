#pragma once
#ifndef MASSASSIGNMENTERROR_H
#define MASSASSIGNMENTERROR_H

#include "orm/runtimeerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny
{

    class SHAREDLIB_EXPORT MassAssignmentError : public RuntimeError
    {
    public:
        using RuntimeError::RuntimeError;
    };

} // namespace Orm::Tiny
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // MASSASSIGNMENTERROR_H
