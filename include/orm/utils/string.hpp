#pragma once
#ifndef ORM_UTILS_STRING_HPP
#define ORM_UTILS_STRING_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils::String
{
    /*! Convert a string to snake case. */
    SHAREDLIB_EXPORT QString toSnake(QString string);

    /*! Get the singular form of an English word. */
    SHAREDLIB_EXPORT QString singular(const QString &string);

    /*! Check if the given string is the number, signed or unsigned. */
    SHAREDLIB_EXPORT bool isNumber(const QString &string);

} // namespace Orm::Utils::String

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_STRING_HPP
