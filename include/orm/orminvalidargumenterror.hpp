#ifndef ORMINVALIDARGUMENTERROR_H
#define ORMINVALIDARGUMENTERROR_H

#include <stdexcept>

#include "export.hpp"
#include "orm/ormlogicerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    class SHAREDLIB_EXPORT OrmInvalidArgumentError : public OrmLogicError
    {
        using OrmLogicError::OrmLogicError;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // ORMINVALIDARGUMENTERROR_H
