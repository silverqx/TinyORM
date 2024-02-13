#pragma once
#ifndef ORM_DRIVERS_EXCEPTIONS_SQLTRANSACTIONERROR_HPP
#define ORM_DRIVERS_EXCEPTIONS_SQLTRANSACTIONERROR_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "orm/drivers/exceptions/sqlerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::Exceptions
{

    /*! TinyDrivers Sql transaction exception. */
    class SqlTransactionError : public SqlError // clazy:exclude=copyable-polymorphic
    {
        /*! Inherit constructors. */
        using SqlError::SqlError;
    };

} // namespace Orm::Drivers::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_EXCEPTIONS_SQLTRANSACTIONERROR_HPP
