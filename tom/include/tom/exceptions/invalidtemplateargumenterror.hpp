#pragma once
#ifndef TOM_EXCEPTIONS_INVALIDTEMPLATEARGUMENTERROR_HPP
#define TOM_EXCEPTIONS_INVALIDTEMPLATEARGUMENTERROR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/exceptions/invalidargumenterror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Exceptions
{

    /*! Tom invalid template argument exception. */
    class InvalidTemplateArgumentError : public InvalidArgumentError
    {
        /*! Inherit constructors. */
        using InvalidArgumentError::InvalidArgumentError;
    };

} // namespace Tom::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_EXCEPTIONS_INVALIDTEMPLATEARGUMENTERROR_HPP
