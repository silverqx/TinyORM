#pragma once
#ifndef UTILS_STRING_H
#define UTILS_STRING_H

#include <QString>

#include "orm/utils/export.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Utils::String
{
    /*! Convert a string to snake case. */
    SHAREDLIB_EXPORT QString toSnake(QString string);

    /*! Get the singular form of an English word. */
    SHAREDLIB_EXPORT QString singular(const QString &string);

    /*! Check if the given string is the number, signed or unsigned. */
    SHAREDLIB_EXPORT bool isNumber(const QString &string);

} // namespace Orm::Utils::String
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // UTILS_STRING_H
