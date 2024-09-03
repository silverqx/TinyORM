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
        {NAME, u"The name of the seeder class (required StudlyCase)"_s},
    };

    return cached;
}

QList<CommandLineOption> SeederCommand::optionsSignature() const
{
    return {
        {path_,        u"The location where the seeder file should be created"_s,
                       path_up}, // Value
        {realpath_,    u"Indicate that any provided seeder file paths are "
                        "pre-resolved absolute paths"_s},
        {fullpath,     u"Output the full path of the created seeder"_s},

        {{QChar('f'),
          force},      u"Overwrite the seeder class if already exists"_s},
        // Hidden option, used in the special case when called from the make:model
        {from_model,   u"Internal argument used when guessing a path"_s, true},
    };
}

int SeederCommand::run()
{
    Command::run();

    const auto className = prepareSeederClassName(argument(NAME));

    const auto seedersPath = getSeedersPath();

    // Check whether a seeder file already exists and create parent folder if needed
    prepareFileSystem(seeder, seedersPath, className.toLower(), className);

    // Ready to write the seeder to the disk 🧨✨
    writeSeeder(className, seedersPath);

    return EXIT_SUCCESS;
}

/* protected */

QString SeederCommand::prepareSeederClassName(QString &&className)
{
    // Validate the seeder name
    throwIfContainsNamespaceOrPath(u"seeder"_s, className, u"argument 'name'"_s);

    static const auto Seeder    = u"Seeder"_s;
    static const auto Seeder_lc = u"seeder"_s;

    static const auto Seeder_lc_size = Seeder_lc.size();

    className = StringUtils::studly(className);

    if (className.endsWith(Seeder))
        return std::move(className);

    // Append Seeder
    if (!className.endsWith(Seeder_lc))
        return std::move(className.append(Seeder));

    // Change Xyzseeder to XyzSeeder
    className[className.size() - Seeder_lc_size] = u'S';

    return std::move(className);
}

void SeederCommand::writeSeeder(const QString &className,
                                const fspath &seedersPath) const
{
    auto seederFilePath = Support::SeederCreator::create(className, seedersPath);

    // make_preferred() returns reference and filename() creates a new fs::path instance
    const auto seederFile = isSet(fullpath) ? seederFilePath.make_preferred()
                                            : seederFilePath.filename();

    info(u"Created Seeder: "_s, false);

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
                u"Seeders path '%1' exists and it's not a directory."_s
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
            : fs::current_path() / targetPath;
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
