#include "tom/commands/make/seedercommand.hpp"

#include <orm/constants.hpp>
#include <orm/tiny/utils/string.hpp>

#include "tom/application.hpp"
#include "tom/exceptions/invalidargumenterror.hpp"
#include "tom/tomconstants.hpp"

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Orm::Constants::NAME;

using StringUtils = Orm::Tiny::Utils::String;

using Tom::Constants::fullpath;
using Tom::Constants::path_;
using Tom::Constants::path_up;
using Tom::Constants::realpath_;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make
{

/* public */

SeederCommand::SeederCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
{}

const std::vector<PositionalArgument> &SeederCommand::positionalArguments() const
{
    static const std::vector<PositionalArgument> cached {
        {NAME, QStringLiteral("The name of the seeder class (required StudlyCase)")},
    };

    return cached;
}

QList<QCommandLineOption> SeederCommand::optionsSignature() const
{
    return {
        {path_,     QStringLiteral("The location where the seeder file should be "
                                   "created"), path_up}, // Value
        {realpath_, QStringLiteral("Indicate that any provided seeder file paths are "
                                   "pre-resolved absolute paths")},
        {fullpath,  QStringLiteral("Output the full path of the created seeder")},
    };
}

int SeederCommand::run()
{
    Command::run();

    const auto className = prepareSeederClassname(argument(NAME));

    // Ready to write the seeder to the disk 🧨✨
    writeSeeder(className);

    return EXIT_SUCCESS;
}

/* protected */

QString SeederCommand::prepareSeederClassname(QString &&className)
{
    // Validate the seeder name
    throwIfContainsNamespaceOrPath(className);

    static const auto Seeder = QStringLiteral("Seeder");
    static const auto Seeder_lc = QStringLiteral("seeder");

    static const auto Seeder_lc_size = Seeder_lc.size();

    className = StringUtils::studly(className);

    if (className.endsWith(Seeder))
        return std::move(className);

    // Append Seeder
    if (!className.endsWith(Seeder_lc))
        return className.append(Seeder);

    // Change Xyzseeder to XyzSeeder
    className[className.size() - Seeder_lc_size] = QChar('S');

    // CUR stackoverflow, still confused about this moves, when I assign a new value silverqx
    return std::move(className);
}

void SeederCommand::writeSeeder(const QString &className) const
{
    auto seederFilePath = m_creator.create(className, getSeederPath());

    // make_preferred() returns reference and filename() creates a new fs::path instance
    const auto seederFile = isSet(fullpath) ? seederFilePath.make_preferred()
                                            : seederFilePath.filename();

    info(QStringLiteral("Created Seeder: "), false);

    note(QString::fromStdString(seederFile.string()));
}

fspath SeederCommand::getSeederPath() const
{
    // Default location
    if (!isSet(path_))
        return application().getSeedersPath();

    auto targetPath = value(path_).toStdString();

    // The 'path' argument contains an absolute path
    if (isSet(realpath_))
        return {std::move(targetPath)};

    // The 'path' argument contains a relative path
    auto seedersPath = fs::current_path() / std::move(targetPath);

    // Validate
    if (fs::exists(seedersPath) && !fs::is_directory(seedersPath))
        throw Exceptions::InvalidArgumentError(
                QStringLiteral("Seeders path '%1' exists and it's not a directory.")
                .arg(seedersPath.c_str()));

    return seedersPath;
}

void SeederCommand::throwIfContainsNamespaceOrPath(const QString &className)
{
    if (!className.contains(QStringLiteral("::")) && !className.contains(QChar('/')) &&
        !className.contains(QChar('\\'))
    )
        return;

    throw Exceptions::InvalidArgumentError(
                "Namespace or path is not allowed in the seeder name");
}

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE
