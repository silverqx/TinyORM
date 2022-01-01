#include "orm/tiny/utils/string.hpp"

#include <vector>

#include "orm/constants.hpp"

using Orm::Constants::MINUS;
using Orm::Constants::PLUS;
using Orm::Constants::UNDERSCORE;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Tiny::Utils::String
{

QString toSnake(QString string)
{
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
    std::for_each(positions.crbegin(), positions.crend(), [&string](const int pos)
    {
        string.insert(pos, UNDERSCORE);
    });

    return string.toLower();
}

QString singular(const QString &string)
{
    if (!string.endsWith(QChar('s')))
        return string;

    return string.chopped(1);
}

bool isNumber(const QString &string)
{
    /* Performance boost was amazing after the QRegularExpression has been removed,
       around 50% on the Playground app 👀, from 800ms to 400ms. */
    if (string.isEmpty())
        return false;

    const auto *itBegin = string.cbegin();
    if (string.front() == PLUS || string.front() == MINUS)
        ++itBegin;

    const auto *nonDigit = std::find_if(itBegin, string.cend(),
                                       [](const auto &ch)
    {
        return !std::isdigit(ch.toLatin1());
    });

    return nonDigit == string.cend();
}

} // namespace Orm::Tiny::Utils::String

TINYORM_END_COMMON_NAMESPACE
