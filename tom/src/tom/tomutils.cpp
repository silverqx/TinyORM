#include "tom/tomutils.hpp"

#include <QList>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>

#include <orm/constants.hpp>
#include <orm/utils/string.hpp>

#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::TMPL_DQUOTES;
using Orm::Constants::UNDERSCORE;

using StringUtils = Orm::Utils::String;

using Tom::Constants::DateTimePrefix;

namespace Tom
{

/* public */

bool Utils::startsWithDatetimePrefix(const QString &migrationName)
{
    /* Datetime prefix 2022_02_02_011255_, the size has to be >18, has to have 4 parts
       after the split(_), every part has specific size and all parts has to be numbers.
       I want to avoid the RegEx where it's possible. */

    static const auto datetimePrefixSize = DateTimePrefix.size();

    /* 17 chars datetime prefix, 1 char the last _ character after the datetime prefix,
       and at least one character for the migration name; >18. */
    if (migrationName.size() <= datetimePrefixSize + 1)
        return false;

    const auto datetime = QStringView(migrationName.constBegin(), datetimePrefixSize)
                          .split(UNDERSCORE);

    // 4 parts
    if (datetime.size() != 4)
        return false;

    // The size of every part has to be equal
    if (!areDatetimePartsEqual(datetime))
        return false;

    // All parts are numbers
    return std::ranges::all_of(datetime, [](const auto datetimePart)
    {
        return StringUtils::isNumber(datetimePart);
    });
}

QString Utils::defaultValueText(const QString &value)
{
    // Quote the string type
    const auto defaultValue = StringUtils::isNumber(value, true, true)
                              ? value
                              : TMPL_DQUOTES.arg(value);

    return QStringLiteral(" [default: %1]").arg(defaultValue);
}

QList<QCommandLineOption>
Utils::convertToQCommandLineOptionList(const QList<CommandLineOption> &options)
{
    QList<QCommandLineOption> result;
    result.reserve(options.size());

    for (const auto &option : options)
        result << option;

    return result;
}

QList<QCommandLineOption>
Utils::convertToQCommandLineOptionList(QList<CommandLineOption> &&options) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
    QList<QCommandLineOption> result;
    result.reserve(options.size());

    for (auto &&option : options)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        result << option;
#else
        result << std::move(option);
#endif

    return result;
}

/* private */

bool Utils::areDatetimePartsEqual(const QList<QStringView> &prefixParts)
{
    using SizeType = QList<QStringView>::size_type;

    /*! Cached the datetime prefix parts sizes. */
    static const auto prefixSizes = []
    {
        const auto prefixSplitted = DateTimePrefix.split(UNDERSCORE);

        return prefixSplitted
                | ranges::views::transform([](const auto &datetimePart)
        {
            return datetimePart.size();
        })
                | ranges::to<std::vector<SizeType>>();
    }();

    /*! Compute the current datetime prefix parts sizes. */
    const auto prefixPartsSizes = [&prefixParts]
    {
        return prefixParts
                | ranges::views::transform([](const auto datetimePart)
        {
            return datetimePart.size();
        })
                | ranges::to<std::vector<SizeType>>();
    };

    // The size of every part has to be equal
    return prefixSizes == prefixPartsSizes();
}

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE
