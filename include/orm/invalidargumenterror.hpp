#ifndef INVALIDARGUMENTERROR_H
#define INVALIDARGUMENTERROR_H

#include <stdexcept>

#include "export.hpp"
#include "orm/logicerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    class SHAREDLIB_EXPORT InvalidArgumentError : public LogicError
    {
        using LogicError::LogicError;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // INVALIDARGUMENTERROR_H
