#include "orm/utils/string.hpp"

#include <QStringList>

#include <cmath>

#include <range/v3/view/reverse.hpp>

#include "orm/constants.hpp"

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
using Orm::Constants::NEWLINE_C;
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

    const auto *itBegin = string.constBegin();
    if (string.front() == PLUS || string.front() == MINUS) {
        if (allowPlusMinus)
            ++itBegin; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        else
            return false;
    }

    // Only one dot allowed
    auto dotAlreadyFound = false;

    const auto *const nonDigit = std::find_if(
                                     itBegin, string.constEnd(),
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

    return nonDigit == string.constEnd();
}

namespace
{
    /* Doesn't make sense to make these two functions constexpr as there is only one
       code branch that is constexpr and that's when the characters are empty and
       the T is QStringView. */

    /*! Strip the given characters from the beginning of a string (common logic). */
    template<QStringLikeConcept T>
    T ltrimInternal(const T &string, const QString &characters)
    {
        Q_ASSERT(!characters.isEmpty());

        typename T::size_type position = 0;

        for (const auto ch : string)
            if (characters.contains(ch))
                ++position;
            else
                break;

        return string.sliced(position);
    }

    /*! Strip the given characters from the end of a string (common logic). */
    template<QStringLikeConcept T>
    T rtrimInternal(const T &string, const QString &characters)
    {
        Q_ASSERT(!characters.isEmpty());

        /* The ++ and -- isn't bug, I'm doing this comment months after I wrote the code
           but I still remember that it's correct. */
        typename T::size_type position = string.size();

        for (const auto ch : string | ranges::views::reverse)
            if (characters.contains(ch))
                --position;
            else
                break;

        return string.first(position);
    }
} // namespace

QString String::ltrim(const QString &string, const QString &characters)
{
    return ltrimInternal(string, characters);
}

QString String::rtrim(const QString &string, const QString &characters)
{
    return rtrimInternal(string, characters);
}

QStringView String::ltrim(const QStringView string, const QString &characters)
{
    return ltrimInternal(string, characters);
}

QStringView String::rtrim(const QStringView string, const QString &characters)
{
    return rtrimInternal(string, characters);
}

QString String::stripTags(QString string)
{
    QString::size_type posStart = 0;
    QString::size_type from = 0;

    while ((posStart = string.indexOf(LT_C, from)) != -1) {
        const auto posEnd = string.indexOf(GT_C, posStart + 1);

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

QList<QString> String::splitAtFirst(const QString &string, const QChar separator,
                                    const Qt::SplitBehavior splitBehavior)
{
    const auto isSkipEmptyParts = splitBehavior == Qt::SkipEmptyParts;

    // Nothing to do
    if (string.isEmpty()) {
        if (isSkipEmptyParts)
            return {};

        return {EMPTY};
    }

    const auto separatorIdx = string.indexOf(separator);

    // Nothing to do, separator was not found
    if (separatorIdx == -1)
        return {string};

    const auto stringSize = string.size();
    Q_ASSERT(separatorIdx >= 0 && separatorIdx < stringSize);

    const auto *const itBegin          = string.constBegin();
    const auto *const itAfterSeparator = itBegin + separatorIdx + 1; // +1 to skip the separator; NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    const auto lengthToEndAfterSep     = stringSize - separatorIdx - 1;

    if (isSkipEmptyParts) {
        const auto *const itSeparator     = itBegin + separatorIdx; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        const auto isSeparatorAtBeginning = itSeparator == itBegin;
        const auto isSeparatorAtEnd       = itAfterSeparator == string.constEnd();

        if (isSeparatorAtBeginning && isSeparatorAtEnd)
            return {};

        if (isSeparatorAtBeginning)
            return {QString(itAfterSeparator, lengthToEndAfterSep)};
        if (isSeparatorAtEnd)
            return {QString(itBegin, separatorIdx)};
    }
    // Don't use the else here

    /* This is correct in all cases, overflow can't happen if there is nothing after
       the separator, eg. key=, in this case the itAfterSeparator will point
       to the constEnd() and the lengthToEndAfterSep == 0, so the result will be like
       {string.constEnd(), 0} what is an empty string (whatever with the size 0 is
       an empty string (not/null based on the origin string). */
    return {QString(itBegin, separatorIdx),
            QString(itAfterSeparator, lengthToEndAfterSep)};
}

QList<QStringView> String::splitAtFirst(const QStringView string, const QChar separator,
                                        const Qt::SplitBehavior splitBehavior)
{
    const auto isSkipEmptyParts = splitBehavior == Qt::SkipEmptyParts;

    // Nothing to do
    if (string.isEmpty()) {
        if (isSkipEmptyParts)
            return {};

        return {string}; // Don't use the EMPTY here
    }

    const auto separatorIdx = string.indexOf(separator);

    // Nothing to do, separator was not found
    if (separatorIdx == -1)
        return {string};

    const auto stringSize = string.size();
    Q_ASSERT(separatorIdx >= 0 && separatorIdx < stringSize);

    const auto *const itBegin          = string.constBegin();
    const auto *const itEnd            = string.constEnd();
    const auto *const itSeparator      = itBegin + separatorIdx; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    const auto *const itAfterSeparator = itSeparator + 1; // +1 to skip the separator; NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    if (isSkipEmptyParts) {
        const auto isSeparatorAtBeginning = itSeparator == itBegin;
        const auto isSeparatorAtEnd       = itAfterSeparator == itEnd;

        if (isSeparatorAtBeginning && isSeparatorAtEnd)
            return {};

        if (isSeparatorAtBeginning)
            return {{itAfterSeparator, itEnd}};
        if (isSeparatorAtEnd)
            return {{itBegin, itSeparator}};
    }
    // Don't use the else here

    /* This is correct in all cases, overflow can't happen if there is nothing after
       the separator, eg. key=, in this case the itAfterSeparator will point to the
       constEnd(), so the result will be like {string.constEnd(), string.constEnd()}
       what is an empty string view (not/null based on the origin string). */
    return {{itBegin, itSeparator}, {itAfterSeparator, itEnd}};
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

    // RegEx not used for performance reasons
    std::vector<QString::size_type> positions;
    positions.reserve(
                static_cast<decltype (positions)::size_type>(string.size() / 2) + 2);

    for (QString::size_type i = 0; i < string.size(); ++i) {
        const auto ch = string.at(i);

        if (i > 0) {
            const auto previousChar = string.at(i - 1);

                // xY to x_y or 0Y to 0_y
            if ((ch >= u'A' && ch <= u'Z' &&
                 ((previousChar >= u'a' &&
                   previousChar <= u'z') ||
                  (previousChar >= u'0' &&
                   previousChar <= u'9'))) ||
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
    /*! Minimum free space size to append a space character. */
    constexpr QString::size_type MinFreeSpace = 2;

    /*! Push the current line to the lines and start processing a new line. */
    inline void startNewLine(QStringList &lines, QString &line) {
        // Push to lines
        lines << std::move(line);
        // Start a new line
        line.clear(); // NOLINT(bugprone-use-after-move), need to clear it anyway even after the move
    }

    /*! Handle an empty token (edge case). */
    inline void handleEmptyToken(QStringList &lines, QString &line, const int width)
    {
        /* In this case, we have to manually handle the start of a new line because
           the splitLongToken() cannot be invoked. */
        if (line.size() + 1 > width)
            startNewLine(lines, line);

        line.append(SPACE);
    }

    /*! Start a new line or append a space character. */
    void startNewLineOrAppendSpace(
            QStringList &lines, QString &line, const QStringView token, const int width,
            const String::SplitWordsBehavior splitBehavior)
    {
        /*! Expose the SplitWordsBehavior enum. */
        using enum String::SplitWordsBehavior;

        // Compute all values only once
        const auto tokenSize       = token.size();
        const auto lineSize        = line.size();
        const auto freeSpace       = width - lineSize;
        const auto isTokenOverflow = lineSize + 1 + tokenSize > width; // +1 for prepended space
        const auto freeSpaceFactor = width > 34 ? 0.15F : 0.3F;
        const auto isFreeSpace30   = freeSpace >= // Is there more than 30% of free space?
                                     std::llround(static_cast<float>(width) *
                                                  freeSpaceFactor);
        const auto isTokenShorter  = tokenSize <= width; // Shorter or equal as the current width

        /* If word splitting is not preferred, there must be free space for the entire
           token with a space character before; if not, start a new line.
           It also helps to avoid maintaining another bool or int state variable
           for the following case: Append the token if there is enough free space
           on the line, because this case also needs to know if a space character
           was appended. */
            // The current line is already full (considering also the space character)
        if ((lineSize == width || lineSize + 1 == width) ||
            (splitBehavior == cNeverSplitWords && isTokenShorter && isTokenOverflow) ||
            (splitBehavior == cSplitWords30    && isTokenShorter && isTokenOverflow &&
                                                 !isFreeSpace30)
        )
            return startNewLine(lines, line); // NOLINT(readability-avoid-return-with-void-value) clazy:exclude=returning-void-expression

        const auto isTokenLonger = !isTokenShorter; // Longer than the current width
        const auto isTokenFit    = !isTokenOverflow;

        /* Don't append a space character to the beginning of an empty line, and if
           there is no free space for at least one more letter when word splitting is
           preferred or if is not preferred, there must be free space for the entire
           token, but only if the token fits or is smaller than a line; if not, use
           the same logic as when word splitting is preferred. 😂😵‍💫🤯
           (there is no reason to append a space character in these cases). */
        if (const auto lineSizeMinFreeSpace = lineSize + MinFreeSpace;
            (splitBehavior == cSplitWords && lineSizeMinFreeSpace <= width) ||
            (splitBehavior == cNeverSplitWords &&
             ((isTokenLonger  && lineSizeMinFreeSpace <= width) ||
              (isTokenShorter && isTokenFit))) ||
            (splitBehavior == cSplitWords30 &&
             (((isTokenLonger  && lineSizeMinFreeSpace <= width) ||
               (isTokenShorter && isTokenFit)) ||
              isFreeSpace30))
        )
            line.append(SPACE);
    }

    /*! Split the token to multiple lines by the given width. */
    void splitLongToken(QStringList &lines, QString &line, QStringView token,
                        const int width)
    {
        while (!token.isEmpty()) {
            /* Token is shorter than the available free space (occurs when the last part
               of the token is currently being processed). */
            if (line.size() + token.size() <= width) {
                line.append(token);
                break;
            }

            // Available/remaining free space
            const auto freeSpace = width - line.size();

            // Fill the entire line (entire free space)
            line.append(token.first(freeSpace));
            // Cut the currently/above appended token part
            token = token.sliced(freeSpace);

            startNewLine(lines, line);
        }
    }
} // namespace

QStringList String::splitStringByWidth(const QStringView string, const int maxWidth,
                                       const SplitWordsBehavior splitBehavior)
{
    const auto width = std::max(1, maxWidth);

    // Nothing to split
    if (string.size() <= width)
        return {string.toString()};

    QStringList lines;
    /* See also the computeReserveForErrorWall() because these two reserve() relate.
       +2 because cSplitWords algorithm fills the lines fully.
       +6 because cNeverSplitWords algorithm can decide to start a new line if there
       isn't enough free space. */
    lines.reserve(static_cast<QStringList::size_type>(
                      std::ceil(static_cast<double>(string.size()) / width)) +
                  (splitBehavior == cSplitWords ? 2 : 6));

    QString line;
    line.reserve(width + 8);

    // QStringView is trivially copy constructible
    for (const auto token : string.split(SPACE, Qt::KeepEmptyParts)) { // clazy:exclude=range-loop-detach
        /* Edge case for Qt::KeepEmptyParts, if there are multiple spaces in the row.
           In this case, each space character will produce an empty token. */
        if (token.isEmpty()) {
            handleEmptyToken(lines, line, width);
            continue;
        }

        // Start a new line or append a space character
        if (!line.isEmpty())
            startNewLineOrAppendSpace(lines, line, token, width, splitBehavior);

        // Append the token if there is enough free space on the line
        if (line.size() + token.size() <= width) // line.size() - fresh value needed
            line.append(token);

        // If the token is longer than the available free space (exceeds the line width)
        else
            splitLongToken(lines, line, token, width);

        /* Extreme edge case when the box width is 1, the line is always empty in this
           case, so it cannot be handled above. */
        if (width == 1) {
            startNewLine(lines, line);
            line.append(SPACE);
        }
    }

    // Append the last line processed
    if (!line.isEmpty())
        lines << std::move(line);

    return lines;
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

    static const auto LoremIpsum511 =
    u"Lorem ipsum dolor sit amet consectetuer cursus euismod eget Vestibulum sodales. "
     "Pellentesque neque Phasellus id id Pellentesque Integer mauris nibh nibh. "
     "Non Morbi pharetra cursus in interdum fringilla Donec quam nunc vitae. "
     "Nulla purus eget et Quisque congue Maecenas Phasellus at Curabitur. "
     "Tellus vel Sed ac nulla dis Vestibulum tellus turpis. "
     "Convallis elit Vestibulum turpis metus Integer nunc quis Sed Integer. "
     "Semper a rutrum at In nibh cursus Nam libero tempus. "
     "Risus nibh semper quis volutpat facilisi."_s;

#  if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return QStringList(count, LoremIpsum511).join(NEWLINE_C);
            // Insert the s character before the last . to make it 512, 1024, ...
            // .insert(-1, u's');
#  else
    QStringList result;
    result.reserve(count);

    for (QStringList::size_type index = 0; index < count; ++index)
        result << lorem511;

    return result.join(NEWLINE_C);
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
                                string.at(firstAllowedPos) == u'/' ? posStart + 2
                                                                   : firstAllowedPos;

    for (QString::size_type i = allowdPosStart; i < posEnd; ++i) {
        const auto ch = string.at(i);

        // All allowed chars
            // A-Z a-z 0-9
        if ((ch >= u'A' && ch <= u'Z') ||
            (ch >= u'a' && ch <= u'z') ||
            (ch >= u'0' && ch <= u'9') ||
            // space ! " # $ % & '
            (ch >= SPACE && ch <= SQUOTE) ||
            // - = ? _ @
            ch == MINUS || ch == EQ_C || ch == u'?' || ch == UNDERSCORE || ch == u'@'
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
