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
    // Nothing to split
    if (string.size() <= width)
        return {string};

    std::vector<QString> lines;
    QString line;

    for (auto token : string.tokenize(SPACE)) {
        /* If there is still a space on the line then append the token */
        if (line.size() + token.size() + 1 <= width) {
            // Don't prepend the space at beginning of an empty line
            if (!line.isEmpty())
                line.append(' ');

            line.append(token);
            continue;
        }

        /* If a token is longer than the width or an empty space on the current line */
        const auto spaceSize = line.isEmpty() ? 0 : 1;

        if (const auto emptySpace = width - line.size() + spaceSize;
            token.size() > emptySpace
        ) {
            // If on the line is still more than 30% of an empty space, use/fill it
            if (emptySpace > llround(static_cast<float>(width) * 0.3)) {
                // Position where to split the token
                auto pos = width - line.size() - spaceSize;

                // Don't prepend the space at beginning of the line
                if (!line.isEmpty())
                    line.append(SPACE);

                line.append(token.left(pos));
                // Cut the appended part
                token = token.mid(pos);
            }

            // In every case no more space on the line here, push to lines
            lines.emplace_back(std::move(line));
            // Start a new line
            line.clear(); // NOLINT(bugprone-use-after-move)

            // Process a long token or rest of the token after the previous 30% filling
            while (!token.isEmpty()) {
                // Token is shorter than the width, indicates processing of the last token
                if (token.size() <= width) {
                    line.append(token); // NOLINT(bugprone-use-after-move)
                    break;
                }

                // Fill the whole line
                line.append(token.left(width));
                // Cut the appended part
                token = token.mid(width);
                // Push to lines
                lines.emplace_back(std::move(line));
                // Start a new line
                line.clear();
            }

            continue;
        }

        // No space on the line, push to lines and start a new line
        lines.emplace_back(std::move(line));

        // Start a new line
        line.clear(); // NOLINT(bugprone-use-after-move)
        line.append(token);
    }

    /* This can happen if a simple append of the token was the last operation, can happen
       on the two places above. */
    if (!line.isEmpty())
        lines.emplace_back(std::move(line));

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
