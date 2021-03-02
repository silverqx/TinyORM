#ifndef UTILS_STRING_H
#define UTILS_STRING_H

#include <QString>

#include "export.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Utils::String
{
    /*! Convert a string to snake case. */
    SHAREDLIB_EXPORT QString toSnake(const QString &string);

} // namespace Orm::Utils::String
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // UTILS_STRING_H
