#pragma once
#ifndef TOM_EXCEPTIONS_INVALIDARGUMENTERROR_HPP
#define TOM_EXCEPTIONS_INVALIDARGUMENTERROR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/exceptions/logicerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Exceptions
{

    /*! Tom Invalid argument exception. */
    class InvalidArgumentError : public LogicError
    {
        /*! Inherit constructors. */
        using LogicError::LogicError;
    };

} // namespace Tom::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_EXCEPTIONS_INVALIDARGUMENTERROR_HPP
