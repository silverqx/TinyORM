#pragma once
#ifndef SQLTRANSACTIONERROR_H
#define SQLTRANSACTIONERROR_H

#include "orm/exceptions/sqlerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Exceptions
{

    class SHAREDLIB_EXPORT SqlTransactionError : public SqlError
    {
    public:
        using SqlError::SqlError;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // SQLTRANSACTIONERROR_H
