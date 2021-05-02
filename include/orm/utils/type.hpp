#ifndef UTILS_TYPE_H
#define UTILS_TYPE_H

#include <QRegularExpression>

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Utils::Type
{
    /*! Class name without a namespace and template parameters. */
    template<typename Type>
    inline QString classPureBasename(const bool withNamespace = false)
    {
        const auto matchRegEx = withNamespace ? "[\\w:]+" : "\\w+|(?<=::)\\w+";

        // FUTURE ask at some forum if this regexp is correctly written silverqx
        QRegularExpression re(QStringLiteral("(?:(?<=^struct |^class )%1)(?=<.*>| |$)")
                              .arg(matchRegEx));

        const auto match = re.match(typeid (Type).name());

        // This should never happen, but who knows 🤔
        Q_ASSERT_X(match.hasMatch(),
                   "regex match", "Can not get the class base name in "
                                  "Utils::Type::classPureBasename().");

        return match.captured();
    }

    /*! Class name without a namespace and template parameters. */
    template<typename Type>
    inline QString classPureBasename(const Type &type, const bool withNamespace = false)
    {
        const auto matchRegEx = withNamespace ? "[\\w:]+" : "\\w+|(?<=::)\\w+";

        QRegularExpression re(QStringLiteral("(?:(?<=^struct |^class )%1)(?=<.*>| |$)")
                              .arg(matchRegEx));

        /* If you want to obtain a name for the polymorphic type, take care to pass
            a glvalue as the 'type' argument, the 'this' pointer is a prvalue! */
        const auto match = re.match(typeid (type).name());

        // This should never happen, but who knows 🤔
        Q_ASSERT_X(match.hasMatch(),
                   "regex match", "Can not get the class base name in "
                                  "Utils::Type::classPureBasename().");

        return match.captured();
    }

} // namespace Orm::Utils::Type
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // UTILS_TYPE_H
