#pragma once
#ifndef TOM_EXCEPTIONS_TOMERROR_HPP
#define TOM_EXCEPTIONS_TOMERROR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Exceptions
{

    /*! Tom exceptions tag, all Tom exceptions are derived from this class. */
    class TomError // clazy:exclude=copyable-polymorphic
    {
    public:
        /*! Pure virtual destructor. */
        inline virtual ~TomError() = 0;
    };

    /* public */

    TomError::~TomError() = default;

} // namespace Tom::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_EXCEPTIONS_TOMERROR_HPP
