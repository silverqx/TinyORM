#include "orm/utils/type.hpp"

#include <QRegularExpression>

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Utils
{

QString
Type::classPureBasenameInternal(const std::type_info &typeInfo, const bool withNamespace)
{
    // FUTURE ask at some forum if this regexps are correctly written silverqx
#ifdef _MSC_VER
    const auto matchRegEx = withNamespace ? "[\\w:]+" : "\\w+|(?<=::)\\w+";

    QRegularExpression re(QStringLiteral("(?:(?<=^struct |^class )%1)(?=<.*>| |$)")
                          .arg(matchRegEx));

    const auto match = re.match(typeInfo.name());
#elif __GNUG__
    const auto matchRegEx = withNamespace ? "^[\\w:]+" : "^\\w+|(?<=::)\\w+";

    QRegularExpression re(QStringLiteral("(?:%1)(?=<.*>|$| |\\*)").arg(matchRegEx));

    int status;
    const auto typeName_ = abi::__cxa_demangle(typeInfo.name(), nullptr, nullptr,
                                               &status);
    const QString typeName(typeName_);
    // CUR check by valgrind silverqx
    free(typeName_);

    // CUR throw on status != 0 silverqx

    const auto match = re.match(typeName);
#endif

    // This should never happen, but who knows 🤔
    Q_ASSERT_X(match.hasMatch(), "regex match",
               "Can not get the class base name in Utils::Type::classPureBasename().");

    return match.captured();
}

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif
