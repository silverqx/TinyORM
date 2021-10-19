#pragma once
#ifndef MASSASSIGNMENTERROR_HPP
#define MASSASSIGNMENTERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/exceptions/runtimeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Exceptions
{

    /*! Mass assignment exception. */
    class SHAREDLIB_EXPORT MassAssignmentError : public Orm::Exceptions::RuntimeError
    {
        /*! Inherit constructors. */
        using Orm::Exceptions::RuntimeError::RuntimeError;
    };

} // namespace Orm::Tiny

TINYORM_END_COMMON_NAMESPACE

#endif // MASSASSIGNMENTERROR_HPP
