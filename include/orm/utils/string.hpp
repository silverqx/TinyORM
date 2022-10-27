#pragma once
#ifndef ORM_UTILS_STRING_HPP
#define ORM_UTILS_STRING_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/constants.hpp"
#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"
#include "orm/ormconcepts.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
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

        /*! Check if the given string is the number, signed or unsigned. */
        static bool isNumber(QStringView string, bool allowFloating = false,
                             bool allowPlusMinus = false);

#if !defined(TINYORM_DISABLE_TOM) || !defined(TINYORM_DISABLE_ORM)
        /*! Convert a string to snake case (snake_case). */
        static QString snake(QString string, QChar delimiter = '_');
#endif

#ifndef TINYORM_DISABLE_TOM
        /*! Convert a value to camel case (camelCase). */
        static QString camel(QString string);
        /*! Convert a value to studly caps case (StudlyCase). */
        static QString studly(QString string);
        /*! Convert values in the container to studly caps case (StudlyCase). */
        template<ColumnContainer T>
        static T studly(T &&strings);

        /*! Split a string by the given width (not in the middle of a word). */
        static std::vector<QString>
        splitStringByWidth(const QString &string, int width);

        /*! Count number of the given character before the given position. */
        static QString::size_type countBefore(QString string, QChar character,
                                              QString::size_type position);

        /*! Wrap a string inside the given character. */
        static QString wrapValue(const QString &string, QChar character);
        /*! Wrap a string inside the given character. */
        static QString wrapValue(const QString &string, QChar firstCharacter,
                                 QChar lastCharacter);
#endif

//        /*! Convert a string to kebab case. (kebab-case). */
//        inline static QString kebab(const QString &string);
//        /*! Get the singular form of an English word. */
//        static QString singular(const QString &string);
    };

    /* public */

#ifndef TINYORM_DISABLE_TOM
    template<ColumnContainer T>
    T String::studly(T &&strings)
    {
        T result;

        for (auto &&string : strings)
            result.push_back(studly(std::forward<decltype (string)>(string)));

        return result;
    }
#endif

//    QString String::kebab(const QString &string)
//    {
//        return snake(string, Orm::Constants::DASH);
//    }

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_STRING_HPP
