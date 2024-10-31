#include "orm/utils/type.hpp"

#include <QRegularExpression>

#include <typeindex>

#ifndef _MSC_VER
#  include <memory>
#endif

#ifdef __GNUG__
#  include <cxxabi.h>
#endif

#include "orm/constants.hpp"

#ifndef _MSC_VER
#  include "orm/exceptions/runtimeerror.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::ASTERISK_C;
using Orm::Constants::LT_C;
using Orm::Constants::OFF;
using Orm::Constants::ON;
using Orm::Constants::SPACE;
using Orm::Constants::off;

#ifndef _MSC_VER
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
    static const QRegularExpression RegEx(
                uR"((?:.* )?(?:.*::)?(\w+)(?:<.*>)?::(\w+)\(.*\))"_s);
#elif defined(_MSC_VER)
    static const QRegularExpression RegEx(
                uR"((?:.*::)?(\w+)(?:<.*>)?::(\w+)(?:$|::<lambda))"_s);
#else
#  error Unsupported compiler in Orm::Utils::Type::prettyFunction().
#endif

    Q_ASSERT_X(!function.isEmpty(), "empty string",
               "The function name can't be empty in Orm::Utils::Type::prettyFunction().");

    const auto match = RegEx.match(function);

    // This should never happen, but who knows 🤔
    Q_ASSERT_X(match.hasMatch(), "RegEx match",
               "Can not get the function name in Orm::Utils::Type::prettyFunction().");
    Q_ASSERT_X(RegEx.captureCount() == 2, "RegEx match",
               "Can not get the function name in Orm::Utils::Type::prettyFunction().");

    return u"%1::%2"_s.arg(match.captured(1), match.captured(2));
}

bool Type::isTrue(const QString &value)
{
    return !value.isEmpty() &&
           value != u'0' &&
           value.compare("false"_L1, Qt::CaseInsensitive) != 0 &&
           // QVariant::value<bool>() doesn't check conditions below
           value.compare(off, Qt::CaseInsensitive) != 0;
}

bool Type::isTrue(const QVariant &value)
{
    return value.canConvert<QString>() && isTrue(value.value<QString>());
}

QString Type::normalizeCMakeTriStateBool(const QString &value)
{
    static const auto NotFoundSuffix = u"-NOTFOUND"_s;
    static const auto NotFound       = u"NOTFOUND"_s;

    /* Is NOTFOUND or xyz-NOTFOUND (just/only return it).
       The NOTFOUND and xyz-NOTFOUND are case-sensitive even if CMake documentation
       states that the named boolean constants are case-insensitive.
       We have to take this into account because notfound or xyz-notfound are
       considered TRUE. */
    if (value.compare(NotFound, Qt::CaseSensitive) == 0 ||
        value.endsWith(NotFoundSuffix, Qt::CaseSensitive)
    )
        return value;

    // Classic bool TRUE/FALSE
    return normalizeCMakeBool(value);
}

QString Type::normalizeCMakeBool(const QString &value)
{
    // Classic bool TRUE/FALSE
    return isCMakeTrue(value) ? ON : OFF;
}

bool Type::isCMakeTrue(const QString &value)
{
    /* Is simpler to compare to the false-type values because comparing the true-type
       values involves numbers, especially floating point numbers and +- sign before
       a number is more complicated. */
    return !value.isEmpty() &&
           value != u'0' &&
           value.compare(off,              Qt::CaseInsensitive) != 0 &&
           value.compare("no"_L1,          Qt::CaseInsensitive) != 0 &&
           value.compare("n"_L1,           Qt::CaseInsensitive) != 0 &&
           value.compare("false"_L1,       Qt::CaseInsensitive) != 0 &&
           value.compare("ignore"_L1,      Qt::CaseInsensitive) != 0 &&
           /* The NOTFOUND and xyz-NOTFOUND are case-sensitive even if CMake documentation
              states that the named boolean constants are case-insensitive.
              We have to take this into account because notfound or xyz-notfound are
              considered TRUE. */
           value.compare("NOTFOUND"_L1,    Qt::CaseSensitive)   != 0 &&
           !value.endsWith("-NOTFOUND"_L1, Qt::CaseSensitive);
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
                        u"A memory allocation failure occurred "
                         "in abi::__cxa_demangle()."_s);
        case -2:
            throw RuntimeError(
                        u"The mangled_name argument for abi::__cxa_demangle() is not "
                         "a valid name under the C++ ABI mangling rules."_s);
        case -3:
            throw RuntimeError(
                        u"One of the arguments for abi::__cxa_demangle() is invalid."_s);

        default:
            throw RuntimeError(
                        u"Unexpected status code '%1' returned "
                         "from abi::__cxa_demangle() in class Orm::Utils::Type."_s
                        .arg(status));
        }
    }
} // namespace

QString Type::demangleTypeNameGnuG(const char *const typeName)
{
    int status = 0;

    const std::unique_ptr<char, decltype (std::free) &> typeNameDemangled(
        abi::__cxa_demangle(typeName, nullptr, nullptr, &status), std::free);

    // Throw if the abi::__cxa_demangle() status < 0
    throwIfDemangleStatusFailed(status);

    return QString::fromUtf8(typeNameDemangled.get());
}
#endif

/* private */

QString
Type::classPureBasenameInternal(const std::type_info &typeInfo, const bool withNamespace)
{
    return classPureBasenameInternal(typeInfo.name(), withNamespace);
}

QString
Type::classPureBasenameInternal(const char *const typeName, const bool withNamespace)
{
#ifdef _MSC_VER
    return classPureBasenameMsvc(typeName, withNamespace);
#elif defined(__GNUG__)
    return classPureBasenameGcc(demangleTypeNameGnuG(typeName), withNamespace);
#else
#  error Unsupported compiler in Orm::Utils::Type::classPureBasenameInternal().
#endif
}

// BUG both msvc and gcc implementation doesn't return correct name for nested classes with namespace, eg. for these types struct A1<int>::N1, struct X::Abc1<int>::N2 it returns A1, X::Abc1 instead of N1, N2; w/o namespaces returns the correct result silverqx

QString
Type::classPureBasenameMsvc(const QString &className, const bool withNamespace)
{
    Q_ASSERT_X(!className.isEmpty(), "empty string",
               "The class name can't be empty "
               "in Orm::Utils::Type::classPureBasenameMsvc().");

    auto findBeginWithoutNS = [&className]
    {
        return className.indexOf(SPACE) + 1;
    };

    // Find the beginning of the class name
    const auto *itBegin = className.cbegin();

    // Include the namespace in the result
    if (withNamespace)
        itBegin += findBeginWithoutNS(); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    // Do not include the namespace in the result
    // Block needed to pass Clang-Tidy bugprone-branch-clone
    else {
        // Doesn't contain the namespace
        if (const qptrdiff toBegin = className.lastIndexOf(u"::"_s);
             toBegin == -1
        )
            itBegin += findBeginWithoutNS(); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

        // Have the namespace (:: found), +2 to point after
        else
            itBegin += toBegin + 2; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }

    // Find the end of the class name
    const auto *itEnd = std::find_if(itBegin, className.cend(),
                                     [](const QChar ch)
    {
        // The class name can end with < or space (anything else can't be at the end)
        return ch == LT_C || ch == SPACE;
    });

    return QStringView(itBegin, itEnd).toString();
}

QString
Type::classPureBasenameGcc(const QString &className, const bool withNamespace)
{
    Q_ASSERT_X(!className.isEmpty(), "empty string",
               "The class name can't be empty "
               "in Orm::Utils::Type::classPureBasenameGcc().");

    // Find the beginning of the class name
    const auto *itBegin = className.cbegin();

    if (!withNamespace)
        // Have the namespace and :: found, +2 to point after
        if (const qptrdiff toBegin = className.lastIndexOf(u"::"_s);
            toBegin != -1
        )
            itBegin += toBegin + 2; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

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
