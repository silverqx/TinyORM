#pragma once
#ifndef ORM_EXCEPTIONS_RECORDSNOTFOUNDERROR_HPP
#define ORM_EXCEPTIONS_RECORDSNOTFOUNDERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/exceptions/runtimeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! Found zero records (used by Builder::sole()). */
    class RecordsNotFoundError : public RuntimeError // clazy:exclude=copyable-polymorphic
    {
        /*! Inherit constructors. */
        using RuntimeError::RuntimeError;
    };

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_EXCEPTIONS_RECORDSNOTFOUNDERROR_HPP
