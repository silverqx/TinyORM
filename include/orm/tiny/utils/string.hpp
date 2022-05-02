#pragma once
#ifndef ORM_TINY_UTILS_STRING_HPP
#define ORM_TINY_UTILS_STRING_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#ifndef TINYORM_DISABLE_TOM
#  include <vector>
#endif

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Utils
{

    /*! String related library class. */
    class SHAREDLIB_EXPORT String
    {
        Q_DISABLE_COPY(String)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        String() = delete;
        /*! Deleted destructor. */
        ~String() = delete;

        /*! Convert a string to snake case. */
        static QString snake(QString string, QChar delimiter = '_');
        /*! Convert a value to studly caps case. */
        static QString studly(QString string);

#ifndef TINYORM_DISABLE_TOM
        /*! Check if the given string is the number, signed or unsigned. */
        static bool isNumber(const QStringView string, bool allowFloating = false);

        /*! Split a string by the given width (not in the middle of a word). */
        static std::vector<QString>
        splitStringByWidth(const QString &string, int width);
#endif

#ifndef TINYORM_DISABLE_ORM
        /*! Get the singular form of an English word. */
        static QString singular(const QString &string);
#endif
    };

} // namespace Orm::Tiny::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_UTILS_STRING_HPP
