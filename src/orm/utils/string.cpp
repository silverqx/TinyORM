#include "orm/utils/string.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Utils::String
{

QString toSnake(QString string)
{
    // RegExp not used for performance reasons
    std::vector<unsigned int> positions;
    positions.reserve(string.size() / 2 + 2);

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
        string.insert(pos, QChar('_'));
    });

    return string.toLower();
}

QString singular(const QString &string)
{
    if (!string.endsWith(QChar('s')))
        return string;

    return string.chopped(1);
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
