#include "orm/utils/string.hpp"

#include <QStringList>

#include <cmath>

#include <range/v3/view/reverse.hpp>

#include "orm/constants.hpp"

#ifndef sl
/*! Alias for the QStringLiteral(). */
#  define sl(str) QStringLiteral(str)
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::DASH;
using Orm::Constants::DOT;
using Orm::Constants::EMPTY;
using Orm::Constants::EQ_C;
using Orm::Constants::GT_C;
using Orm::Constants::LT_C;
using Orm::Constants::MINUS;
using Orm::Constants::PLUS;
using Orm::Constants::SPACE;
using Orm::Constants::SQUOTE;
using Orm::Constants::UNDERSCORE;

#ifdef TINYORM_TESTS_CODE
using Orm::Constants::NEWLINE;
#endif

namespace Orm::Utils
{

/* This is only one translation unit from the Tiny namespace also used in the tom
   project, so I leave it enabled in the build system when the tom is going to build,
   I will not extract these 3 used methods to own dll or static library, they simply
   will be built into the tinyorm shared library because of this
   #ifndef TINYORM_DISABLE_ORM/TOM exists, methods are enabled/disabled on the base of
   whether the orm or tom is built. */

/* public */

bool String::isNumber(const QStringView string, const bool allowFloating,
                      const bool allowPlusMinus)
{
    /* Performance boost was amazing after the QRegularExpression has been removed,
       around 50% on the Playground app 👀, from 800ms to 400ms. */
    if (string.isEmpty())
        return false;

    const auto *itBegin = string.cbegin();
    if (string.front() == PLUS || string.front() == MINUS) {
        if (allowPlusMinus)
            ++itBegin; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        else
            return false;
    }

    // Only one dot allowed
    auto dotAlreadyFound = false;

    const auto *const nonDigit = std::find_if(
                                     itBegin, string.cend(),
                                     [allowFloating, &dotAlreadyFound](const auto &ch)
    {
        // Integer type
        if (!allowFloating)
            // Is not numeric == 0
            return std::isdigit(ch.toLatin1()) == 0;

        // Floating-point type
        // Only one dot allowed
        const auto isDot = ch.toLatin1() == DOT;

        // Is not numeric == 0
        const auto result = std::isdigit(ch.toLatin1()) == 0 &&
                            (!isDot || (isDot && dotAlreadyFound));

        if (isDot)
            dotAlreadyFound = true;

        return result;
    });

    return nonDigit == string.cend();
}

QString String::ltrim(const QString &string, const QString &characters)
{
    QString::size_type position = 0;

    for (const auto ch : string)
        if (characters.contains(ch))
            ++position;
        else
            break;

    return string.sliced(position);
}

QString String::rtrim(const QString &string, const QString &characters)
{
    /* The ++ and -- isn't bug, I'm doing this comment months after I wrote the code but
       I still remember that it's correct. */
    QString::size_type position = string.size();

    for (const auto itChar : string | ranges::views::reverse)
        if (characters.contains(itChar))
            --position;
        else
            break;

    return string.first(position);
}

QString String::stripTags(QString string)
{
    QString::size_type posStart = 0;
    QString::size_type from = 0;

    while ((posStart = string.indexOf(LT_C, from, Qt::CaseInsensitive)) != -1) {
        const auto posEnd = string.indexOf(GT_C, posStart + 1, Qt::CaseInsensitive);

        // Nothing to do, the > char not found
        if (posEnd == -1)
            break;

        // Strip a tag if all tag chars are allowed
        if (allTagCharsAllowed(string, posStart, posEnd)) {
            string.remove(posStart, posEnd - posStart + 1);
            from = posStart;

        // Otherwise, start searching a next tag
        } else
            from = posEnd + 1;

        // Nothing to do, the last tag already processed
        if (from >= string.size())
            break;
    }

    return string;
}

#if !defined(TINYORM_DISABLE_TOM) || !defined(TINYORM_DISABLE_ORM)
/*! Snake cache type. */
using SnakeCache = std::unordered_map<QString, QString>;

/*! Snake cache for already computed strings. */
Q_GLOBAL_STATIC(SnakeCache, snakeCache) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)

QString String::snake(QString string, const QChar delimiter)
{
    auto key = string;

    if (snakeCache->contains(key))
        return (*snakeCache)[key];

    // Avoid underscores at the beginning and end
    string = trim(string, SPACE);

    // RegExp not used for performance reasons
    std::vector<QString::size_type> positions;
    positions.reserve(
                static_cast<decltype (positions)::size_type>(string.size() / 2) + 2);

    for (QString::size_type i = 0; i < string.size(); ++i) {
        const auto ch = string.at(i);

        if (i > 0) {
            const auto previousChar = string.at(i - 1);

                // xY to x_y or 0Y to 0_y
            if ((ch >= QLatin1Char('A') && ch <= QLatin1Char('Z') &&
                 ((previousChar >= QLatin1Char('a') &&
                   previousChar <= QLatin1Char('z')) ||
                  (previousChar >= QLatin1Char('0') &&
                   previousChar <= QLatin1Char('9')))) ||
                // x y to x_y                       Avoid more underscores __
                (i >= 2 && previousChar == SPACE && string.at(i - 2) != SPACE)
            )
                positions.push_back(i);
        }
    }

    // Positions stay valid after inserts because reverse iterators used
    for (const QString::size_type pos : positions | ranges::views::reverse) {
        const auto previousPos = pos - 1;

        // Change space to _
        if (string.at(previousPos) == SPACE)
            string[previousPos] = delimiter;
        // Prepend the _ before A-Z
        else
            string.insert(pos, delimiter);
    }

    return (*snakeCache)[std::move(key)] = string.replace(SPACE, EMPTY).toLower();
}
#endif

#ifndef TINYORM_DISABLE_TOM
/*! Studly cache type. */
using StudlyCache = std::unordered_map<QString, QString>;
/*! Camel cache type. */
using CamelCache  = std::unordered_map<QString, QString>;

/*! Studly cache for already computed strings. */
Q_GLOBAL_STATIC(StudlyCache, studlyCache) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)
/*! Camel cache for already computed strings. */
Q_GLOBAL_STATIC(CamelCache, camelCache) // NOLINT(misc-use-anonymous-namespace, cppcoreguidelines-avoid-non-const-global-variables)

QString String::camel(QString string)
{
    auto value = string.trimmed();

    // Nothing to do
    if (value.isEmpty())
        return string;

    // Cache key
    auto key = value;

    if (camelCache->contains(key))
        return (*camelCache)[key];

    value = studly(value);

    value[0] = value[0].toLower();

    return (*camelCache)[std::move(key)] = value;
}

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

    return (*studlyCache)[std::move(key)] = value.replace(SPACE, EMPTY);
}

namespace
{
    /*! Split the token to multiple lines by the given width. */
    bool splitLongToken(QStringView token, const int width, QString &line,
                        std::vector<QString> &lines)
    {
        auto shouldContinue = false;

        const auto spaceSize = line.isEmpty() ? 0 : 1;

        if (const auto emptySpace = width - line.size() + spaceSize;
            token.size() > emptySpace
        ) {
            // If on the line is still more than 30% of an empty space, use/fill it
            if (emptySpace > std::llround(static_cast<float>(width) * 0.3F)) {
                // Position where to split the token
                auto pos = width - line.size() - spaceSize;

                // Don't prepend the space at beginning of an empty line
                if (!line.isEmpty())
                    line.append(SPACE);

                // Guaranteed by the token.size() > emptySpace
                line.append(token.first(pos));
                // Cut the appended part
                token = token.sliced(pos);
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

                // Guaranteed by the token.size() <= width, so token.size() > width
                // Fill the whole line
                line.append(token.first(width));
                // Cut the appended part
                token = token.sliced(width);
                // Push to lines
                lines.emplace_back(std::move(line));
                // Start a new line
                line.clear(); // NOLINT(bugprone-use-after-move)
            }

            shouldContinue = true;
        }

        return shouldContinue;
    }
} // namespace

/*! Split a string by the given width (not in the middle of a word). */
std::vector<QString> String::splitStringByWidth(const QString &string, const int width)
{
    // Nothing to split
    if (string.size() <= width)
        return {string};

    std::vector<QString> lines;
    lines.reserve(static_cast<decltype (lines)::size_type>( // omg 😵‍💫🤯
                      std::llround(static_cast<double>(string.size()) / width)) + 4);

    QString line;

    for (auto &&token : QStringView(string).split(SPACE)) {
        // If there is still a space on the line then append the token
        if (line.size() + token.size() + 1 <= width) {
            // Don't prepend the space at beginning of an empty line
            if (!line.isEmpty())
                line.append(SPACE);

            line.append(token);
            continue;
        }

        // If a token is longer than the width or an empty space on the current line
        if (splitLongToken(token, width, line, lines))
            continue;

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

QList<QStringView> String::splitAtFirst(const QStringView string, const QChar separator,
                                        const Qt::SplitBehavior behavior)
{
    // Nothing to do
    if (string.isEmpty())
        return {};

    const auto index = string.indexOf(separator);

    // Nothing to do, separator was not found
    if (index == -1)
        return {string};

    const auto *const begin = string.constBegin();
    const auto *const end = string.constEnd();
    const auto *const itSeparator = string.constBegin() + index;
    const auto *const itAfterSeparator = string.constBegin() + index + 1; // +1 to skip the separator

    // Currently, a value before the separator must contain at least one character
    Q_ASSERT(begin < itSeparator);
    // Standard development check, therefore is separated from the above
    Q_ASSERT(itAfterSeparator <= end);

    if (behavior == Qt::SkipEmptyParts && itAfterSeparator == end)
        return {{begin, itSeparator}};

    /* This is correct in all cases, overflow can't happen if there is nothing after
       the separator, eg. key=, in this case the beginIndex will point to the constEnd(),
       so the result will be like {string.constEnd(), string.constEnd()} what is an empty
       string view. */
    return {{begin, itSeparator}, {itAfterSeparator, end}};
}

QString::size_type String::countBefore(QString string, const QChar character,
                                       const QString::size_type position)
{
    string.truncate(position);

    return string.count(character);
}

QString String::wrapValue(const QString &string, const QChar character)
{
    QString result;
    result.reserve(string.size() + 8);

    return result.append(character).append(string).append(character);
}

QString String::wrapValue(const QString &string, const QChar firstCharacter,
                          const QChar lastCharacter)
{
    QString result;
    result.reserve(string.size() + 8);

    return result.append(firstCharacter).append(string).append(lastCharacter);
}
#endif

#ifdef TINYORM_TESTS_CODE
QString String::loremIpsum512Paragraph(const QStringList::size_type count)
{
    Q_ASSERT(count != 0);

    static const auto lorem511 =
    sl("Lorem ipsum dolor sit amet consectetuer cursus euismod eget Vestibulum sodales. "
       "Pellentesque neque Phasellus id id Pellentesque Integer mauris nibh nibh. "
       "Non Morbi pharetra cursus in interdum fringilla Donec quam nunc vitae. "
       "Nulla purus eget et Quisque congue Maecenas Phasellus at Curabitur. "
       "Tellus vel Sed ac nulla dis Vestibulum tellus turpis. "
       "Convallis elit Vestibulum turpis metus Integer nunc quis Sed Integer. "
       "Semper a rutrum at In nibh cursus Nam libero tempus. "
       "Risus nibh semper quis volutpat facilisi.");

#  if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return QStringList(count, lorem511).join(NEWLINE);
            // Insert the s character before the last . to make it 512, 1024, ...
            // .insert(-1, QLatin1Char('s'));
#  else
    QStringList result;
    result.reserve(count);

    for (QStringList::size_type index = 0; index < count; ++index)
        result << lorem511;

    return result.join(NEWLINE);
#  endif
}
#endif

/* private */

bool String::allTagCharsAllowed(const QString &string, const QString::size_type posStart,
                                const QString::size_type posEnd)
{
    auto allCharsAllowed = true;

    const auto firstAllowedPos = posStart + 1;
    // Compute the start position skipping the / char if it's a first char
    const auto allowdPosStart = firstAllowedPos < posEnd &&
                                string.at(firstAllowedPos) == QLatin1Char('/')
                                ? posStart + 2
                                : firstAllowedPos;

    for (QString::size_type i = allowdPosStart; i < posEnd; ++i) {
        const auto ch = string.at(i);

        // All allowed chars
            // A-Z a-z 0-9
        if ((ch >= QLatin1Char('A') && ch <= QLatin1Char('Z')) ||
            (ch >= QLatin1Char('a') && ch <= QLatin1Char('z')) ||
            (ch >= QLatin1Char('0') && ch <= QLatin1Char('9')) ||
            // space ! " # $ % & '
            (ch >= SPACE && ch <= SQUOTE) ||
            // - = ? _ @
            ch == MINUS || ch == EQ_C || ch == QLatin1Char('?') || ch == UNDERSCORE ||
            ch == QLatin1Char('@')
        )
            continue;

        // Char is not allowed, break and return
        allCharsAllowed = false;
        break;
    }

    return allCharsAllowed;
}

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE
