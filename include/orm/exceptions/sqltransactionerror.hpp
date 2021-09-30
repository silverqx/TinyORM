#pragma once
#ifndef SQLTRANSACTIONERROR_HPP
#define SQLTRANSACTIONERROR_HPP

#include "orm/exceptions/sqlerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Exceptions
{

    /*! Sql transaction exception. */
    class SHAREDLIB_EXPORT SqlTransactionError : public SqlError
    {
        /*! Inherit constructors. */
        using SqlError::SqlError;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // SQLTRANSACTIONERROR_HPP
