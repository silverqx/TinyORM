#pragma once
#ifndef ORM_TINY_EXCEPTIONS_MASSASSIGNMENTERROR_HPP
#define ORM_TINY_EXCEPTIONS_MASSASSIGNMENTERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/exceptions/runtimeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Exceptions
{

    /*! Mass assignment exception. */
    class MassAssignmentError : public Orm::Exceptions::RuntimeError // clazy:exclude=copyable-polymorphic
    {
        /*! Inherit constructors. */
        using Orm::Exceptions::RuntimeError::RuntimeError;
    };

} // namespace Orm::Tiny::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_EXCEPTIONS_MASSASSIGNMENTERROR_HPP
