#pragma once
#ifndef ORM_EXCEPTIONS_DOMAINERROR_HPP
#define ORM_EXCEPTIONS_DOMAINERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/exceptions/logicerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! TinyORM Domain exception. */
    class DomainError : public LogicError
    {
        /*! Inherit constructors. */
        using LogicError::LogicError;
    };

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_EXCEPTIONS_DOMAINERROR_HPP
