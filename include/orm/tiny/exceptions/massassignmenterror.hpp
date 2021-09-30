#pragma once
#ifndef MASSASSIGNMENTERROR_HPP
#define MASSASSIGNMENTERROR_HPP

#include "orm/exceptions/runtimeerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Tiny::Exceptions
{

    /*! Mass assignment exception. */
    class SHAREDLIB_EXPORT MassAssignmentError : public Orm::Exceptions::RuntimeError
    {
        /*! Inherit constructors. */
        using Orm::Exceptions::RuntimeError::RuntimeError;
    };

} // namespace Orm::Tiny
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // MASSASSIGNMENTERROR_HPP
