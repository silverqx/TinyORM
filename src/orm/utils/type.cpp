#include "orm/utils/type.hpp"

#include <QRegularExpression>

#if !defined(_MSC_VER)
#include <memory>
#endif

#include "orm/constants.hpp"
#if !defined(_MSC_VER)
#include "orm/exceptions/runtimeerror.hpp"
#endif

using Orm::Constants::ASTERISK_C;
using Orm::Constants::LT_C;
using Orm::Constants::SPACE;

#if !defined(_MSC_VER)
using Orm::Exceptions::RuntimeError;
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Utils
{

QString
Type::classPureBasename(const std::type_index typeIndex, const bool withNamespace)
{
    return classPureBasenameInternal(typeIndex.name(), withNamespace);
}

QString Type::prettyFunction(const QString &function)
{
    /* I can leave RegEx here because this function is used only during throwing
       exceptions, so there would not be any performance benefit. */
#ifdef __GNUG__
    QRegularExpression re(QStringLiteral(
                              "(?:.* )?(?:.*::)?(\\w+)(?:<.*>)?::(\\w+)\\(.*\\)"));
#elif _MSC_VER
    QRegularExpression re(QStringLiteral(
                              "(?:.*::)?(\\w+)(?:<.*>)?::(\\w+)(?:$|::<lambda)"));
#else
    throw RuntimeError(
                "Unsupported compiler in Utils::Type::prettyFunction().");
#endif

    const auto match = re.match(function);

    // This should never happen, but who knows 🤔
    Q_ASSERT_X(match.hasMatch(), "regex match",
               "Can not get the function name in Utils::Type::prettyFunction().");
    Q_ASSERT_X(re.captureCount() == 2, "regex match",
               "Can not get the function name in Utils::Type::prettyFunction().");

    return QStringLiteral("%1::%2").arg(match.captured(1), match.captured(2));
}

QString
Type::classPureBasenameInternal(const std::type_info &typeInfo, const bool withNamespace)
{
    return classPureBasenameInternal(typeInfo.name(), withNamespace);
}

QString
Type::classPureBasenameInternal(const char *typeName, const bool withNamespace)
{
#ifdef _MSC_VER
    return classPureBasenameMsvc(typeName, withNamespace);
#elif __GNUG__
    // Demangle a type name
    int status = 0;
    std::unique_ptr<char, decltype (std::free) &> typeNameDemangled_(
        abi::__cxa_demangle(typeName, nullptr, nullptr, &status), std::free);

    // Throw when abi::__cxa_demangle() status < 0
    throwIfDemangleStatusFailed(status);

    const QString typeNameDemangled(typeNameDemangled_.get());

    return classPureBasenameGcc(typeNameDemangled, withNamespace);
#else
    throw RuntimeError(
                "Unsupported compiler in Utils::Type::classPureBasenameInternal().");
#endif
}

QString
Type::classPureBasenameMsvc(const QString &className, const bool withNamespace)
{
    auto findBeginWithoutNS = [&className]
    {
        return className.indexOf(SPACE) + 1;
    };

    // Find the beginning of the class name
    const auto *itBegin = className.cbegin();

    // Include the namespace in the result
    if (withNamespace)
        itBegin += findBeginWithoutNS();

    // Do not include the namespace in the result
    // Block needed to pass clang-tidy bugprone-branch-clone
    else {
        // Doesn't contain the namespace
        if (qptrdiff toBegin = className.lastIndexOf(QStringLiteral("::"));
             toBegin == -1
        )
            itBegin += findBeginWithoutNS();

        // Have the namespace (:: found), +2 to point after
        else
            itBegin += toBegin + 2;
    }

    // Find the end of the class name
    const auto *itEnd = std::find_if(itBegin, className.cend(),
                              [](const QChar ch)
    {
        // The class name can end with < or space, anything else
        return ch == LT_C || ch == SPACE;
    });

    return QStringView(itBegin, itEnd).toString();
}

QString
Type::classPureBasenameGcc(const QString &className, const bool withNamespace)
{
    // Find the beginning of the class name
    const auto *itBegin = className.cbegin();

    if (!withNamespace)
        // Have the namespace and :: found, +2 to point after
        if (qptrdiff toBegin = className.lastIndexOf(QStringLiteral("::"));
            toBegin != -1
        )
            itBegin += toBegin + 2;

    // Find the end of the class name
    const auto *itEnd = std::find_if(itBegin, className.cend(),
                                     [](const QChar ch)
    {
        // The class name can end with <, * or space, anything else
        return ch == LT_C || ch == SPACE || ch == ASTERISK_C;
    });

    return QStringView(itBegin, itEnd).toString();
}

void Type::throwIfDemangleStatusFailed(const int status)
{
    switch (status) {
    case -1:
        throw RuntimeError(
                    "A memory allocation failure occurred in abi::__cxa_demangle().");
    case -2:
        throw RuntimeError(
                    "mangled_name argument for abi::__cxa_demangle() is not a valid "
                    "name under the C++ ABI mangling rules.");
    case -3:
        throw RuntimeError(
                    "One of the arguments for abi::__cxa_demangle() is invalid.");
    default:
        break;
    }
}

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE
