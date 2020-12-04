#ifndef UTILS_TYPE_H
#define UTILS_TYPE_H

#include <QRegularExpression>

namespace Utils
{
    namespace Type
    {
        /*! Class name without a namespace and template parameters. */
        template<typename Type>
        inline QString classPureBasename()
        {
            QRegularExpression re(
                        QStringLiteral("(?:(?<=^struct )\\w+|(?<=^class )\\w+|(?<=::)\\w+)"
                                       "(?=<.*>| |$)"));

            const auto match = re.match(typeid (Type).name());

            // This should never happen, but who knows 🤔
            Q_ASSERT_X(match.hasMatch(),
                       "regex match", "Can not get class base name in getForeignKey().");

            return match.captured();
        }
    }
}

#endif // UTILS_TYPE_H
