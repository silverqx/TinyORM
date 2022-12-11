#include "tom/commands/make/makecommand.hpp"

#include <range/v3/view/reverse.hpp>

#include "tom/exceptions/invalidargumenterror.hpp"
#include "tom/tomconstants.hpp"
#include "tom/tomutils.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Tom::Constants::DateTimePrefix;
using Tom::Constants::force;

using TomUtils = Tom::Utils;

namespace Tom::Commands::Make
{

/* public */

MakeCommand::MakeCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
{}

/* protected */

void MakeCommand::prepareFileSystem(
            const QString &type, const fspath &folder, const QString &basename,
            const QString &className) const
{
    throwIfFileAlreadyExists(type, folder, basename, className);

    ensureDirectoryExists(folder);
}

void MakeCommand::throwIfContainsNamespaceOrPath(
            const QString &type, const QString &className, const QString &source)
{
    if (!className.contains(QStringLiteral("::")) &&
        !className.contains(QLatin1Char('/')) &&
        !className.contains(QLatin1Char('\\'))
    )
        return;

    throw Exceptions::InvalidArgumentError(
                QStringLiteral("Namespace or path is not allowed in the %1 name (%2).")
                .arg(type, source));
}

fspath MakeCommand::guessPathForMakeByPwd(
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

/* private */

void MakeCommand::throwIfFileAlreadyExists(
            const QString &type, const fspath &folder, const QString &basename,
            const QString &className) const
{
    // Nothing to check
    if (!fs::exists(folder))
        return;

    auto itemName = className.isEmpty() ? basename : className;

    using options = fs::directory_options;

    for (const auto &entry :
         fs::directory_iterator(folder, options::skip_permission_denied)
    ) {
        // Nothing to do, check only files
        if (!entry.is_regular_file())
            continue;

        // Extract base filename without the extension
        auto entryName = QString::fromStdString(entry.path().stem().string());

        // Migration specific
        if (type == QStringLiteral("migration")) {
            // Nothing to do, check only files with the datetime prefix
            if (!TomUtils::startsWithDatetimePrefix(entryName))
                continue;

            // Also remove the datetime prefix
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            entryName = entryName.sliced(DateTimePrefix.size() + 1);
#else
            entryName = entryName.mid(DateTimePrefix.size() + 1);
#endif
        }

        if (entryName == basename) {
            // Allow overwriting a file using the --force option
            if (!isSet(force))
                throw Exceptions::InvalidArgumentError(
                        QStringLiteral("A '%1' %2 already exists.")
                        .arg(std::move(itemName), type));

            comment(QStringLiteral("Overwriting '%1' already existing %2.")
                    .arg(std::move(itemName), type));
            break;
        }
    }
}

void MakeCommand::ensureDirectoryExists(const fspath &path)
{
    if (fs::exists(path) && fs::is_directory(path))
        return;

    fs::create_directories(path);
}

bool MakeCommand::areParentPathsEqual(QStringList defaultPathList,
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

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE
