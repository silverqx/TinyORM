#pragma once
#ifndef ORM_UTILS_STRING_HPP
#define ORM_UTILS_STRING_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/constants.hpp"
#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"
#include "orm/ormconcepts.hpp" // IWYU pragma: keep

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

    /*! String related library class. */
    class TINYORM_EXPORT String
    {
        Q_DISABLE_COPY_MOVE(String)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        String() = delete;
        /*! Deleted destructor. */
        ~String() = delete;

        /*! Check if the given string is the number, signed or unsigned. */
        static bool isNumber(QStringView string, bool allowFloating = false,
                             bool allowPlusMinus = false);

        /*! Strip whitespaces (or other characters) from the beginning and end
            of a string. */
        inline static QString
        trim(const QString &string,
             const QString &characters = QStringLiteral(" \n\r\t\v\f"));
        /*! Strip whitespaces (or other characters) from the beginning of a string. */
        static QString
        ltrim(const QString &string,
              const QString &characters = QStringLiteral(" \n\r\t\v\f"));
        /*! Strip whitespaces (or other characters) from the end of a string. */
        static QString
        rtrim(const QString &string,
              const QString &characters = QStringLiteral(" \n\r\t\v\f"));

        /*! Remove tags from the given string. */
        [[maybe_unused]]
        static QString stripTags(QString string);

#if !defined(TINYORM_DISABLE_TOM) || !defined(TINYORM_DISABLE_ORM)
        /*! Convert a string to snake case (snake_case). */
        static QString snake(QString string, QChar delimiter = Constants::UNDERSCORE);
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
        /*! Split a string at the first given character. */
        static QList<QStringView>
        splitAtFirst(QStringView string, QChar separator,
                     Qt::SplitBehavior behavior = Qt::KeepEmptyParts);

        /*! Count number of the given character before the given position. */
        static QString::size_type countBefore(QString string, QChar character,
                                              QString::size_type position);

        /*! Wrap a string inside the given character. */
        static QString wrapValue(const QString &string, QChar character);
        /*! Wrap a string inside the given character. */
        static QString wrapValue(const QString &string, QChar firstCharacter,
                                 QChar lastCharacter);
#endif

#ifdef TINYORM_TESTS_CODE
        /*! Lorem ipsum paragraphs generator (511 characters in paragraph + newline). */
        static QString loremIpsum512Paragraph(QStringList::size_type count);
#endif

    private:
        /*! Determine whether all characters in the given tag are allowed. */
        static bool allTagCharsAllowed(const QString &string, QString::size_type posStart,
                                       QString::size_type posEnd);
    };

    /* public */

    QString String::trim(const QString &string, const QString &characters)
    {
        return rtrim(ltrim(string, characters), characters);
    }

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

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_STRING_HPP
