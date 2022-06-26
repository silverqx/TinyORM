#pragma once
#ifndef ORM_TINY_UTILS_STRING_HPP
#define ORM_TINY_UTILS_STRING_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#ifndef TINYORM_DISABLE_TOM
#  include <vector>
#endif

#include "orm/constants.hpp"
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

        /*! Convert a string to snake case (snake_case). */
        static QString snake(QString string, QChar delimiter = '_');
        /*! Convert a value to studly caps case (StudlyCase). */
        static QString studly(QString string);
        /*! Convert a value to camel case (camelCase). */
        static QString camel(QString string);
        /*! Convert a string to kebab case. (kebab-case). */
        inline static QString kebab(const QString &string);

        /*! Count number of the given character before the given position. */
        static QString::size_type countBefore(QString string, QChar character,
                                              QString::size_type position);

#if !defined(TINYORM_DISABLE_TOM) || !defined(TINYORM_DISABLE_ORM)
        /*! Check if the given string is the number, signed or unsigned. */
        static bool isNumber(QStringView string, bool allowFloating = false);
#endif

#ifndef TINYORM_DISABLE_TOM
        /*! Split a string by the given width (not in the middle of a word). */
        static std::vector<QString>
        splitStringByWidth(const QString &string, int width);
#endif

#ifndef TINYORM_DISABLE_ORM
        /*! Get the singular form of an English word. */
        static QString singular(const QString &string);
#endif
    };

    /* public */

    QString String::kebab(const QString &string)
    {
        return snake(string, Orm::Constants::DASH);
    }

} // namespace Orm::Tiny::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_TINY_UTILS_STRING_HPP
