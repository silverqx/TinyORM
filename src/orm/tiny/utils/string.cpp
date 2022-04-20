#include "orm/tiny/utils/string.hpp"

#include "orm/constants.hpp"

using Orm::Constants::DASH;
using Orm::Constants::DOT;
using Orm::Constants::MINUS;
using Orm::Constants::PLUS;
using Orm::Constants::SPACE;
using Orm::Constants::UNDERSCORE;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Utils
{

/* This is only one translation unit from the Tiny namespace also used in the tom
   project, so I leave it enabled in the build system when the tom is going to build,
   I will not extract these 3 used methods to own dll or static library, they simply
   will be built into the tinyorm shared library because of this
   #ifndef TINYORM_DISABLE_ORM/TOM exists, methods are enabled/disabled on the base of
   whether the orm or tom is built. */

/* public */

namespace
{
    using SnakeCache = std::unordered_map<QString, QString>;

    /*! Snake cache for already computed strings. */
    Q_GLOBAL_STATIC(SnakeCache, snakeCache);
} // namespace

QString String::snake(QString string, const QChar delimiter)
{
    auto key = string;

    if (snakeCache->contains(key))
        return (*snakeCache)[key];

    // RegExp not used for performance reasons
    std::vector<int> positions;
    positions.reserve(static_cast<std::size_t>(string.size() / 2) + 2);

    for (auto i = 0; i < string.size(); ++i) {
        const auto ch = string.at(i);

        if (i > 0 && ch >= QChar('A') && ch <= QChar('Z')) {
            const auto previousChar = string.at(i - 1);

            if ((previousChar >= QChar('a') && previousChar <= QChar('z')) ||
                (previousChar >= QChar('0') && previousChar <= QChar('9'))
            )
                positions.push_back(i);
        }
    }

    // Positions stay valid after inserts because reverse iterators used
    std::for_each(positions.crbegin(), positions.crend(),
                  [&string, delimiter](const int pos)
    {
        string.insert(pos, delimiter);
    });

    return (*snakeCache)[std::move(key)] = string.toLower();;
}

namespace
{
    using StudlyCache = std::unordered_map<QString, QString>;

    /*! Studly cache for already computed strings. */
    Q_GLOBAL_STATIC(StudlyCache, studlyCache);
} // namespace

QString String::studly(QString string)
{
    auto value = string.trimmed();

    // Nothing to do
    if (value.isEmpty())
        return string;

    // Cache key
    auto key = value;

    if (studlyCache->contains(key))
        return (*studlyCache)[key];

    value.replace(DASH,       SPACE)
         .replace(UNDERSCORE, SPACE);

    auto size = value.size();

    // Always upper a first character
    if (size > 1)
        value[0] = value[0].toUpper();

    QString::size_type pos = 0;

    while ((pos = value.indexOf(SPACE, pos)) != -1) {
        // Avoid out of bound exception
        if (++pos >= size)
            break;

        value[pos] = value[pos].toUpper();
    }

    return (*studlyCache)[std::move(key)] = value.replace(SPACE, "");
}

bool String::isNumber(const QString &string, const bool allowFloating)
{
    /* Performance boost was amazing after the QRegularExpression has been removed,
       around 50% on the Playground app 👀, from 800ms to 400ms. */
    if (string.isEmpty())
        return false;

    const auto *itBegin = string.cbegin();
    if (string.front() == PLUS || string.front() == MINUS)
        ++itBegin;

    // Only one dot allowed
    auto dotAlreadyFound = false;

    const auto *nonDigit = std::find_if(itBegin, string.cend(),
                                       [allowFloating, &dotAlreadyFound](const auto &ch)
    {
        // Integer type
        if (!allowFloating)
            return !std::isdigit(ch.toLatin1());

        // Floating-point type
        // Only one dot allowed
        const auto isDot = ch.toLatin1() == DOT;

        const auto result = !std::isdigit(ch.toLatin1()) &&
                            (!isDot || (isDot && dotAlreadyFound));

        if (isDot)
            dotAlreadyFound = true;

        return result;
    });

    return nonDigit == string.cend();
}

#ifndef TINYORM_DISABLE_TOM
/*! Split a string by the given width (not in the middle of a word). */
std::vector<QString> String::splitStringByWidth(const QString &string, const int width)
{
    const auto stringSize = string.size();

    // Nothing to split
    if (stringSize <= width)
        return {string};

    QString::size_type from = 0;

    std::vector<QString> lines;

    while (true) {
        /* Section - find split position */
        auto searchFrom = from + width - 1;

        // Hit the end - the last text block - !(searchFrom < stringSize)
        const auto isEnd = searchFrom >= stringSize - 1;

        // Returns pos == -1 if not found
        auto pos = string.lastIndexOf(SPACE, isEnd ? -1 : searchFrom);

        /* Section - compute how much chars to copy */
        QString::size_type copySize = -1;

        // No space found in the current range, eg. long path so copy whole searched block
        if (pos < from)
            pos = searchFrom;

        // The last text block, -1 for copy the rest
        if (isEnd)
            copySize = -1;

        // If pos == -1 (no space found) and not at end then copy a whole width block
        else if (pos == -1 && !isEnd)
            copySize = width;

        // Copy to the found space char
        else
            copySize = pos - from;

        /* Section - done, copy a text */
        lines.emplace_back(string.mid(from, copySize));

        // Hit the end - the last text block - !(searchFrom < stringSize)
        if (isEnd)
            break;

        /* Section - prepare 'from' for the next loop */
        /* Start after the whole width block (if pos == -1 then pos + 1 is not
           correct here). */
        if (pos == -1) {
            from += width;

            // When whole block was copied, the next char can or can not be a space
            if (string.at(from) == SPACE)
                ++from;
        }
        // Start from a last found space
        /* +1 means - don't copy a space at beginning (skip space at beginning),
           is guaranteed that the first char will be a space. */
        else
            // Don't skip space if no space found in the searched block
            from = pos == searchFrom ? pos : pos + 1;
    }

    return lines;
}
#endif

#ifndef TINYORM_DISABLE_ORM
QString String::singular(const QString &string)
{
    if (!string.endsWith(QChar('s')))
        return string;

    return string.chopped(1);
}
#endif

} // namespace Orm::Tiny::Utils

TINYORM_END_COMMON_NAMESPACE
