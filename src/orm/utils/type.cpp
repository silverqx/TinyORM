#include "orm/utils/type.hpp"

#include <QRegularExpression>

#if !defined(_MSC_VER)
#  include <memory>
#endif

#include "orm/constants.hpp"
#if !defined(_MSC_VER)
#  include "orm/exceptions/runtimeerror.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::ASTERISK_C;
using Orm::Constants::LT_C;
using Orm::Constants::SPACE;

#if !defined(_MSC_VER)
using Orm::Exceptions::RuntimeError;
#endif

namespace Orm::Utils
{

/* public */

QString
Type::classPureBasename(const std::type_index typeIndex, const bool withNamespace)
{
    return classPureBasenameInternal(typeIndex.name(), withNamespace);
}

QString Type::prettyFunction(const QString &function)
{
    // CUR regex doesn't catch main, ::main, run<int>, ::run<int>, functions without NS, fixed regex (?(?:.*::)?(\\w+)(?:<.*>)?::)?(\\w+)(?:<.*>)?(?:$|::<lambda) silverqx
    // TODO perf, rewrite w/o the QRegularExpression silverqx
    /* I can leave RegEx here because this function is used only during throwing
       exceptions, so there would not be any performance benefit. */
#if defined(__GNUG__) || defined(__clang__)
    static const QRegularExpression regex(
                QLatin1String(R"((?:.* )?(?:.*::)?(\w+)(?:<.*>)?::(\w+)\(.*\))"));
#elif _MSC_VER
    static const QRegularExpression regex(
                QLatin1String(R"((?:.*::)?(\w+)(?:<.*>)?::(\w+)(?:$|::<lambda))"));
#else
    throw RuntimeError(
                "Unsupported compiler in Utils::Type::prettyFunction().");
#endif

    Q_ASSERT_X(!function.isEmpty(), "empty string",
               "The function name can't be empty in Utils::Type::prettyFunction().");

    const auto match = regex.match(function);

    // This should never happen, but who knows 🤔
    Q_ASSERT_X(match.hasMatch(), "regex match",
               "Can not get the function name in Utils::Type::prettyFunction().");
    Q_ASSERT_X(regex.captureCount() == 2, "regex match",
               "Can not get the function name in Utils::Type::prettyFunction().");

    return QStringLiteral("%1::%2").arg(match.captured(1), match.captured(2));
}

bool Type::isTrue(const QString &value)
{
    return !value.isEmpty() &&
            value != QLatin1Char('0') &&
            value != QStringLiteral("false") &&
            // QVariant::value<bool>() doesn't check conditions below
                     value != QStringLiteral("off");
}

bool Type::isTrue(const QVariant &value)
{
    return value.canConvert<QString>() && isTrue(value.value<QString>());
}

QString
Type::classPureBasenameInternal(const std::type_info &typeInfo, const bool withNamespace)
{
    return classPureBasenameInternal(typeInfo.name(), withNamespace);
}

#ifdef __GNUG__
namespace
{
    /*! Throw when abi::__cxa_demangle() status < 0. */
    void throwIfDemangleStatusFailed(const int status)
    {
        if (status == 0)
            return;

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
            Q_UNREACHABLE();
        }
    }
} // namespace
#endif

QString
Type::classPureBasenameInternal(const char *typeName, const bool withNamespace)
{
#ifdef _MSC_VER
    return classPureBasenameMsvc(typeName, withNamespace);
#elif __GNUG__
    // Demangle a type name
    int status = 0;
    const std::unique_ptr<char, decltype (std::free) &> typeNameDemangled_(
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
    Q_ASSERT_X(!className.isEmpty(), "empty string",
               "The class name can't be empty in Utils::Type::classPureBasenameMsvc().");

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
        if (const qptrdiff toBegin = className.lastIndexOf(QStringLiteral("::"));
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
    Q_ASSERT_X(!className.isEmpty(), "empty string",
               "The class name can't be empty in Utils::Type::classPureBasenameGcc().");

    // Find the beginning of the class name
    const auto *itBegin = className.cbegin();

    if (!withNamespace)
        // Have the namespace and :: found, +2 to point after
        if (const qptrdiff toBegin = className.lastIndexOf(QStringLiteral("::"));
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

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE
