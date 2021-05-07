#ifndef UTILS_TYPE_H
#define UTILS_TYPE_H

#include <QRegularExpression>

#ifdef __GNUG__
#include <cxxabi.h>
#endif

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Utils::Type
{
    // CUR rewrite to library class to get rid of duplicate code silverqx
    /*! Class name without a namespace and template parameters. */
    template<typename Type>
    inline QString classPureBasename(const bool withNamespace = false)
    {
        // FUTURE ask at some forum if this regexps are correctly written silverqx
#ifdef _MSC_VER
        const auto matchRegEx = withNamespace ? "[\\w:]+" : "\\w+|(?<=::)\\w+";

        QRegularExpression re(QStringLiteral("(?:(?<=^struct |^class )%1)(?=<.*>| |$)")
                              .arg(matchRegEx));

        const auto match = re.match(typeid (Type).name());
#elif __GNUG__
        const auto matchRegEx = withNamespace ? "^[\\w:]+" : "^\\w+|(?<=::)\\w+";

        QRegularExpression re(QStringLiteral("(?:%1)(?=<.*>|$| |\\*)")
                              .arg(matchRegEx));

        int status;
        const auto typeName_ = abi::__cxa_demangle(typeid (Type).name(), nullptr, nullptr,
                                                   &status);
        const QString typeName(typeName_);
        // CUR check by valgrind silverqx
        free(typeName_);

        // CUR throw on status != 0 silverqx

        const auto match = re.match(typeName);
#endif

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
        /* If you want to obtain a name for the polymorphic type, take care to pass
            a glvalue as the 'type' argument, the 'this' pointer is a prvalue! */

#ifdef _MSC_VER
        const auto matchRegEx = withNamespace ? "[\\w:]+" : "\\w+|(?<=::)\\w+";

        QRegularExpression re(QStringLiteral("(?:(?<=^struct |^class )%1)(?=<.*>| |$)")
                              .arg(matchRegEx));

        const auto match = re.match(typeid (type).name());
#elif __GNUG__
        const auto matchRegEx = withNamespace ? "^[\\w:]+" : "^\\w+|(?<=::)\\w+";

        QRegularExpression re(QStringLiteral("(?:%1)(?=<.*>|$| |\\*)")
                              .arg(matchRegEx));

        int status;
        const auto typeName_ = abi::__cxa_demangle(typeid (type).name(), nullptr, nullptr,
                                                   &status);
        const QString typeName(typeName_);
        free(typeName_);

        const auto match = re.match(typeName);
#endif

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
