#ifndef ORMDOMAINERROR_H
#define ORMDOMAINERROR_H

#include <stdexcept>

#include "export.hpp"
#include "orm/ormlogicerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    class SHAREDLIB_EXPORT OrmDomainError : public OrmLogicError
    {
        using OrmLogicError::OrmLogicError;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // ORMDOMAINERROR_H
