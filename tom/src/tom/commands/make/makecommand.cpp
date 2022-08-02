#include "tom/commands/make/makecommand.hpp"

#include "tom/exceptions/invalidargumenterror.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Tom::Constants::DateTimePrefix;
using Tom::Constants::force;

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
    throwIfModelAlreadyExists(type, folder, basename, className);

    ensureDirectoryExists(folder);
}

void MakeCommand::throwIfContainsNamespaceOrPath(
            const QString &type, const QString &className, const QString &source)
{
    if (!className.contains(QStringLiteral("::")) && !className.contains(QChar('/')) &&
        !className.contains(QChar('\\'))
    )
        return;

    throw Exceptions::InvalidArgumentError(
                QStringLiteral("Namespace or path is not allowed in the %1 name (%2).")
                .arg(type, source));
}

/* private */

void MakeCommand::throwIfModelAlreadyExists(
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
        // Check only files
        if (!entry.is_regular_file())
            continue;

        // Extract base filename without the extension
        auto entryName = QString::fromStdString(entry.path().stem().string());

        // If checking a migration then also remove the datetime prefix
        if (type == QStringLiteral("migration"))
            entryName = entryName.mid(DateTimePrefix.size() + 1);

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

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE
