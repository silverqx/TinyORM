#pragma once
#ifndef ORM_EXCEPTIONS_ORMERROR_HPP
#define ORM_EXCEPTIONS_ORMERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! TinyORM exceptions tag, all TinyORM exceptions are derived from this class. */
    class OrmError
    {
    public:
        /*! Virtual destructor. */
        inline virtual ~OrmError() = default;
    };

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_EXCEPTIONS_ORMERROR_HPP
