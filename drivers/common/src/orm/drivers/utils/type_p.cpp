#include "orm/drivers/utils/type_p.hpp"

#include <QRegularExpression>

// CUR drivers Orm::Exceptions::RuntimeError silverqx
#if !defined(_MSC_VER)
#  include <orm/exceptions/runtimeerror.hpp>
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

#if !defined(_MSC_VER)
using Orm::Exceptions::RuntimeError;
#endif

namespace Orm::Drivers::Utils
{

/* public */

QString Type::prettyFunction(const QString &function)
{
    // CUR regex doesn't catch main, ::main, run<int>, ::run<int>, functions without NS, fixed regex (?(?:.*::)?(\\w+)(?:<.*>)?::)?(\\w+)(?:<.*>)?(?:$|::<lambda) silverqx
    // TODO perf, rewrite w/o the QRegularExpression silverqx
    /* I can leave RegEx here because this function is used only during throwing
       exceptions, so there would not be any performance benefit. */
#if defined(__GNUG__) || defined(__clang__)
    static const QRegularExpression
            regex(R"((?:.* )?(?:.*::)?(\w+)(?:<.*>)?::(\w+)\(.*\))"_L1);
#elif _MSC_VER
    static const QRegularExpression
            regex(R"((?:.*::)?(\w+)(?:<.*>)?::(\w+)(?:$|::<lambda))"_L1);
#else
    throw RuntimeError(
                "Unsupported compiler in Drivers::Utils::Type::prettyFunction().");
#endif

    Q_ASSERT_X(!function.isEmpty(), "empty string",
               "The function name can't be empty "
               "in Drivers::Utils::Type::prettyFunction().");

    const auto match = regex.match(function);

    // This should never happen, but who knows 🤔
    Q_ASSERT_X(match.hasMatch(), "regex match",
               "Can not get the function name "
               "in Drivers::Utils::Type::prettyFunction().");
    Q_ASSERT_X(regex.captureCount() == 2, "regex match",
               "Can not get the function name "
               "in Drivers::Utils::Type::prettyFunction().");

    return u"%1::%2"_s.arg(match.captured(1), match.captured(2));
}

} // namespace Orm::Drivers::Utils

TINYORM_END_COMMON_NAMESPACE
