#ifndef INVALIDFORMATERROR_H
#define INVALIDFORMATERROR_H

#include "orm/ormlogicerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    class InvalidFormatError : public OrmLogicError
    {
    public:
        using OrmLogicError::OrmLogicError;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // INVALIDFORMATERROR_H
