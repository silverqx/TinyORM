#pragma once
#ifndef INVALIDTEMPLATEARGUMENTERROR_HPP
#define INVALIDTEMPLATEARGUMENTERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/exceptions/invalidargumenterror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! Invalid template argument exception. */
    class SHAREDLIB_EXPORT InvalidTemplateArgumentError : public InvalidArgumentError
    {
        /*! Inherit constructors. */
        using InvalidArgumentError::InvalidArgumentError;
    };

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // INVALIDTEMPLATEARGUMENTERROR_HPP
