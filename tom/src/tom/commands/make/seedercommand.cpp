#include "tom/commands/make/seedercommand.hpp"

#include <orm/constants.hpp>
#include <orm/utils/string.hpp>

#include "tom/application.hpp"
#include "tom/commands/make/support/seedercreator.hpp"
#include "tom/exceptions/invalidargumenterror.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Orm::Constants::NAME;

using StringUtils = Orm::Utils::String;

using Tom::Constants::force;
using Tom::Constants::from_model;
using Tom::Constants::fullpath;
using Tom::Constants::path_;
using Tom::Constants::path_up;
using Tom::Constants::realpath_;
using Tom::Constants::seeder;

namespace Tom::Commands::Make
{

/* public */

SeederCommand::SeederCommand(Application &application, QCommandLineParser &parser)
    : MakeCommand(application, parser)
{}

const std::vector<PositionalArgument> &SeederCommand::positionalArguments() const
{
    static const std::vector<PositionalArgument> cached {
        {NAME, QStringLiteral("The name of the seeder class (required StudlyCase)")},
    };

    return cached;
}

QList<CommandLineOption> SeederCommand::optionsSignature() const
{
    return {
        {path_,        QStringLiteral("The location where the seeder file should be "
                                      "created"), path_up}, // Value
        {realpath_,    QStringLiteral("Indicate that any provided seeder file paths are "
                                      "pre-resolved absolute paths")},
        {fullpath,     QStringLiteral("Output the full path of the created seeder")},

        {{QChar('f'),
          force},      QStringLiteral("Overwrite the seeder class if already exists")},
        // Hidden option, used in the special case when called from the make:model
        {from_model,   QStringLiteral("Internal argument used when guessing a path"),
                       true},
    };
}

int SeederCommand::run()
{
    Command::run();

    const auto className = prepareSeederClassName(argument(NAME));

    auto seedersPath = getSeedersPath();

    // Check whether a seeder file already exists and create parent folder if needed
    prepareFileSystem(seeder, seedersPath, className.toLower(), className);

    // Ready to write the seeder to the disk 🧨✨
    writeSeeder(className, std::move(seedersPath));

    return EXIT_SUCCESS;
}

/* protected */

QString SeederCommand::prepareSeederClassName(QString &&className)
{
    // Validate the seeder name
    throwIfContainsNamespaceOrPath(QStringLiteral("seeder"), className,
                                   QStringLiteral("argument 'name'"));

    static const auto Seeder = QStringLiteral("Seeder");
    static const auto Seeder_lc = QStringLiteral("seeder");

    static const auto Seeder_lc_size = Seeder_lc.size();

    className = StringUtils::studly(className);

    if (className.endsWith(Seeder))
        return std::move(className);

    // Append Seeder
    if (!className.endsWith(Seeder_lc))
        return std::move(className.append(Seeder));

    // Change Xyzseeder to XyzSeeder
    className[className.size() - Seeder_lc_size] = QLatin1Char('S');

    return std::move(className);
}

void SeederCommand::writeSeeder(const QString &className, fspath &&seedersPath) const
{
    auto seederFilePath = Support::SeederCreator::create(className,
                                                         std::move(seedersPath));

    // make_preferred() returns reference and filename() creates a new fs::path instance
    const auto seederFile = isSet(fullpath) ? seederFilePath.make_preferred()
                                            : seederFilePath.filename();

    info(QStringLiteral("Created Seeder: "), false);

    note(QString::fromStdString(seederFile.string()));
}

fspath SeederCommand::getSeedersPath() const
{
    /* If a user passes the --path parameter use it, otherwise try to guess seeders
       path based on the pwd and if not found use the default path which is set
       by the TINYTOM_SEEDERS_DIR macro. */
    auto seedersPath = isSet(path_)
                       // User defined path
                       ? getUserSeedersPath()
                       // Try to guess or use the default path
                       : guessSeedersPath();

    // Validate
    if (fs::exists(seedersPath) && !fs::is_directory(seedersPath))
        throw Exceptions::InvalidArgumentError(
                QStringLiteral("Seeders path '%1' exists and it's not a directory.")
                .arg(seedersPath.c_str()));

    return seedersPath;
}

fspath SeederCommand::getUserSeedersPath() const
{
    auto targetPath = fspath(value(path_).toStdString()).lexically_normal();

    return isSet(realpath_)
            // The 'path' argument contains an absolute path
            ? std::move(targetPath)
            // The 'path' argument contains a relative path
            : fs::current_path() / std::move(targetPath);
}

fspath SeederCommand::guessSeedersPath() const
{
    return guessPathForMakeByPwd(
                application().getSeedersPath(),
                /* Models path needed to correctly guess the path in one special case,
                   when this command is called from the make:model. */
                isSet(from_model) ? std::make_optional(
                                        std::cref(application().getModelsPath()))
                                  : std::nullopt);
}

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE
