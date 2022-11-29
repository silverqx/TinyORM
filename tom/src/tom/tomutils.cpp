#include "tom/tomutils.hpp"

#include <QList>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/transform.hpp>

#include <orm/constants.hpp>
#include <orm/utils/string.hpp>

#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

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
    auto defaultValue = StringUtils::isNumber(value, true, true)
                        ? value
                        : QStringLiteral("\"%1\"").arg(value);

    return QStringLiteral(" [default: %1]").arg(std::move(defaultValue));
}

fspath Utils::guessPathForMakeByPwd(
        const fspath &defaultPath,
        std::optional<std::reference_wrapper<const fspath>> &&defaultModelsPath)
{
    /* Method is commented on heavily because the logic is sketchy and unclear. 😵‍💫🙃
       It tries to guess a path based on the pwd and the defaultPath (can be relative
       or absolute; it can be migrations, seeders, or models default path) for "make:"
       commands.
       The comments below describe how it works, so I won't describe it here, just
       a quick summary. If the defaultPath is relative and the pwd is somewhere inside
       this defaultPath, then it appends the rest of the path from the defaultPath
       to the pwd, eg. the defaultPath = "aa/bb/cc/dd" and
       the pwd = "E:/<some path>/aa/bb", then the guessed result path will be
       "E:/<some path>/aa/bb/cc/dd", the cc/dd was appended. */

    // Nothing to do, the user passed an absolute path
    if (defaultPath.is_absolute())
        return defaultPath;

    // Initialize
    auto pwd = fs::current_path();
    const auto pwdStr = QString::fromStdString(pwd.string());
    const auto defaultPathStr = QString::fromStdString(defaultPath.string());

    // Nothing to do, the pwd is already the defaultPath
    if (pwdStr.endsWith(defaultPathStr))
        return pwd;

    auto defaultPathList = defaultPathStr.split(fspath::preferred_separator,
                                                Qt::SkipEmptyParts,
                                                Qt::CaseInsensitive);

    QStringList guessedPathList;
    // + 1 because also using takeLast() part; * 2 because of prepend()
    guessedPathList.reserve(
                (defaultPathStr.count(fspath::preferred_separator) + 1) * 2);

    /* Special case when the make:migration/seeder commands are called
       from the make:model and the pwd is inside the default models path
       (TINYTOM_MODELS_DIR), in this case the guessed path will be ../migrations or
       ../seeders. The areParentPathsEqual() condition restricts this special case
       because an user can set up absolutely different paths for migrations/seeders. */
    if (defaultModelsPath &&
        pwdStr.endsWith(QString::fromStdString(defaultModelsPath->get().string())) &&
        areParentPathsEqual(defaultPathList, *defaultModelsPath)
    )
        return pwd.parent_path() / defaultPathList.constLast().toStdString();

    // Exclude the last folder, it's already processed by the previous if condition
    guessedPathList << defaultPathList.takeLast();

    /* Nothing to do, no folders left, special case, it can happen when
       the defaultPath has only one folder name (without this check, it would work ok
       and it would end up with totally different path result). */
    if (defaultPathList.isEmpty())
        return pwd / defaultPath;

    // 1. stage = false, 2. stage = true
    auto validationStage = false;
    // Cleared in the 1. stage and used in the 2. stage to verify the rest of the path
    QString verifyRemainingPath;
    // * 2 because of prepend()
    const auto verifyRemainingPathReserve = defaultPathStr.size() * 2;
    verifyRemainingPath.reserve(verifyRemainingPathReserve);

    /* Loop over all folder names and compute the guessed path based on the pwd.
       It has two stages, in the first stage, the algorithm will collect folder names
       until the pwd ends with some folder name.
       In the second stage, all the remaining folder names have to be found in the pwd,
       so it validates that we are somewhere in the defaultPath. */
    for (const auto &folder : ranges::reverse_view(defaultPathList)) {
        verifyRemainingPath.prepend(folder).prepend(fspath::preferred_separator);

        const auto pwdEndWithTmp = pwdStr.endsWith(verifyRemainingPath);

        /* First part of the algorithm, collecting the folder names until the pwd will
           end with some folder name, then the collected folder names will be appended
           to the pwd (at the end of the method). */
        if (!validationStage && !pwdEndWithTmp) {
            // This is the point of this method, collect the folder names ℹ️
            guessedPathList.prepend(defaultPathList.takeLast());

            verifyRemainingPath.clear();
            // Clear resets also capacity, reserve again
            verifyRemainingPath.reserve(verifyRemainingPathReserve);

            continue;
        }

        // Detected different folder name, the pwd doesn't contain this folder
        if (validationStage && !pwdEndWithTmp)
            break;

        /* Enable the second part of the algorithm, detect if the remaining folder names
           are in the pwd. */
        if (!validationStage)
            validationStage = true;
    }

    /* The pwd doesn't contain any of the defaultPath folder names (different path),
       the validation stage didn't even start. */
    if (!validationStage)
        return std::move(pwd) / defaultPath;

    // Append the computed/guessed (from the defaultPath) to the pwd 🙏
    return std::move(pwd) / guessedPathList.join(fspath::preferred_separator)
                                           .toStdString();
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
Utils::convertToQCommandLineOptionList(QList<CommandLineOption> &&options)
{
    QList<QCommandLineOption> result;
    result.reserve(options.size());

    for (auto &&option : options)
        result << std::move(option);

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

/* private */

bool Utils::areParentPathsEqual(QStringList defaultPathList,
                                const fspath &defaultModelsPath)
{
    // Prepare
    auto defaultModelsPathList = QString::fromStdString(defaultModelsPath.string())
                                 .split(fspath::preferred_separator,
                                        Qt::SkipEmptyParts,
                                        Qt::CaseInsensitive);
    // Remove last folder names
    defaultPathList.removeLast();
    defaultModelsPathList.removeLast();

    // Remaining path is equal
    return defaultPathList == defaultModelsPathList;
}

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE
