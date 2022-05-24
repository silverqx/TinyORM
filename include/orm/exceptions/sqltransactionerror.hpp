#pragma once
#ifndef ORM_EXCEPTIONS_SQLTRANSACTIONERROR_HPP
#define ORM_EXCEPTIONS_SQLTRANSACTIONERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/exceptions/sqlerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! TinyORM Sql transaction exception. */
    class SqlTransactionError : public SqlError // clazy:exclude=copyable-polymorphic
    {
        /*! Inherit constructors. */
        using SqlError::SqlError;
    };

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_EXCEPTIONS_SQLTRANSACTIONERROR_HPP
