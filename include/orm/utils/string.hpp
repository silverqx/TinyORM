#pragma once
#ifndef ORM_UTILS_STRING_HPP
#define ORM_UTILS_STRING_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#ifdef QT_LEAN_HEADERS
#  include <QStringList>
#endif

#include "orm/constants.hpp"
#include "orm/ormconcepts.hpp" // IWYU pragma: keep

#ifndef TINYORM_EXTERN_CONSTANTS
#  include "orm/macros/export.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

    /*! String related library class. */
    class TINYORM_EXPORT String
    {
        Q_DISABLE_COPY_MOVE(String)

    public:
        /*! The behavior of splitting the last word on a line. */
        enum struct SplitWordsBehavior : quint8
        {
            /*! Split the word anywhere to fill the free space on a line. */
            cSplitWords,
            /*! Never split words, the word is placed at the beginning of the next line. */
            cNeverSplitWords,
        };
        /*! Expose the SplitWordsBehavior enum. */
        using enum SplitWordsBehavior;

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
             const QString &characters = Constants::TrimCharacters);
        /*! Strip whitespaces (or other characters) from the beginning of a string. */
        static QString
        ltrim(const QString &string,
              const QString &characters = Constants::TrimCharacters);
        /*! Strip whitespaces (or other characters) from the end of a string. */
        static QString
        rtrim(const QString &string,
              const QString &characters = Constants::TrimCharacters);

        /*! Strip whitespaces (or other characters) from the beginning and end
            of a string. */
        inline static QStringView
        trim(QStringView string, const QString &characters = Constants::TrimCharacters);
        /*! Strip whitespaces (or other characters) from the beginning of a string. */
        static QStringView
        ltrim(QStringView string, const QString &characters = Constants::TrimCharacters);
        /*! Strip whitespaces (or other characters) from the end of a string. */
        static QStringView
        rtrim(QStringView string, const QString &characters = Constants::TrimCharacters);

        /*! Remove tags from the given string. */
        static QString stripTags(QString string);

        /*! Split a string at the first given character. */
        static QList<QString>
        splitAtFirst(const QString &string, QChar separator,
                     Qt::SplitBehavior splitBehavior = Qt::KeepEmptyParts); // Never change the Qt::KeepEmptyParts

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
        template<ColumnContainer T, typename U = std::remove_cvref_t<T>>
        static U studly(T &&strings);

        /*! Split a string by the given width (with or w/o splitting words preference). */
        static QStringList
        splitStringByWidth(QStringView string, int width,
                           SplitWordsBehavior splitBehavior = cNeverSplitWords);
        /*! Split a string view at the first given character. */
        static QList<QStringView>
        splitAtFirst(QStringView string, QChar separator,
                     Qt::SplitBehavior splitBehavior = Qt::KeepEmptyParts); // Never change the Qt::KeepEmptyParts

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

    QStringView String::trim(const QStringView string, const QString &characters)
    {
        return rtrim(ltrim(string, characters), characters);
    }

#ifndef TINYORM_DISABLE_TOM
    template<ColumnContainer T, typename U>
    U String::studly(T &&strings) // NOLINT(cppcoreguidelines-missing-std-forward)
    {
        U result;
        result.reserve(strings.size());

        for (auto &&string : strings)
            result.push_back(studly(std::move(string)));

        return result;
    }
#endif

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_STRING_HPP
