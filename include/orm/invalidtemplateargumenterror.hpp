#ifndef INVALIDTEMPLATEARGUMENTERROR_HPP
#define INVALIDTEMPLATEARGUMENTERROR_HPP

#include "orm/invalidargumenterror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    class SHAREDLIB_EXPORT InvalidTemplateArgumentError : public InvalidArgumentError
    {
        using InvalidArgumentError::InvalidArgumentError;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // INVALIDTEMPLATEARGUMENTERROR_HPP
