#pragma once
#ifndef DOMAINERROR_HPP
#define DOMAINERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <stdexcept>

#include "orm/exceptions/logicerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! Domain exception. */
    class SHAREDLIB_EXPORT DomainError : public LogicError
    {
        /*! Inherit constructors. */
        using LogicError::LogicError;
    };

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // DOMAINERROR_HPP
