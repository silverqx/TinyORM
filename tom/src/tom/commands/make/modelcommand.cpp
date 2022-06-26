#include "tom/commands/make/modelcommand.hpp"

#include <orm/constants.hpp>
#include <orm/tiny/utils/string.hpp>

#include "tom/application.hpp"
#include "tom/exceptions/invalidargumenterror.hpp"
#include "tom/tomconstants.hpp"

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Orm::Constants::NAME;

using StringUtils = Orm::Tiny::Utils::String;

using Tom::Constants::belongs_to;
using Tom::Constants::belongs_to_many;
using Tom::Constants::belongs_to_many_up;
using Tom::Constants::belongs_to_up;
using Tom::Constants::connection_;
using Tom::Constants::connection_up;
using Tom::Constants::disable_timestamps;
using Tom::Constants::fullpath;
using Tom::Constants::one_to_one;
using Tom::Constants::one_to_one_up;
using Tom::Constants::one_to_many;
using Tom::Constants::one_to_many_up;
using Tom::Constants::path_;
using Tom::Constants::path_up;
using Tom::Constants::realpath_;
using Tom::Constants::table_;
using Tom::Constants::table_up;

using CmdOptions = Tom::Commands::Make::Support::ModelCreator::CmdOptions;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make
{

/* public */

ModelCommand::ModelCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
{}

const std::vector<PositionalArgument> &ModelCommand::positionalArguments() const
{
    static const std::vector<PositionalArgument> cached {
        {NAME, QStringLiteral("The name of the model class (required StudlyCase)")},
    };

    return cached;
}

QList<QCommandLineOption> ModelCommand::optionsSignature() const
{
    return {
        {one_to_one,         QStringLiteral("Create one-to-one relation to the given "
                                            "model"), one_to_one_up}, // Value
        {one_to_many,        QStringLiteral("Create one-to-many relation to the given "
                                            "model"), one_to_many_up}, // Value
        {belongs_to,         QStringLiteral("Create belongs-to relation to the given "
                                            "model (inverse relation for oto and otm)"),
                             belongs_to_up}, // Value
        {belongs_to_many,    QStringLiteral("Create many-to-many relation to the "
                                            "given model (also for inverse relation for "
                                            "many-to-many)"),
                             belongs_to_many_up}, // Value

        {table_,             QStringLiteral("The table associated with the model"),
                             table_up}, // Value
        {connection_,        QStringLiteral("The connection name for the model"),
                             connection_up}, // Value
        {disable_timestamps, QStringLiteral("Disable timestamping of the model")},

        {path_,              QStringLiteral("The location where the model file should "
                                            "be created"), path_up}, // Value
        {realpath_,          QStringLiteral("Indicate that any provided model file "
                                            "paths are pre-resolved absolute paths")},
        {fullpath,           QStringLiteral("Output the full path of the created model")},
    };
}

int ModelCommand::run()
{
    Command::run();

    const auto [className, cmdOptions] = prepareModelClassnames(argument(NAME),
                                                                createCmdOptions());

    // Ready to write the model to the disk 🧨✨
    writeModel(className, cmdOptions);

    return EXIT_SUCCESS;
}

/* protected */

std::tuple<QString, CmdOptions>
ModelCommand::prepareModelClassnames(QString &&className, CmdOptions &&cmdOptions)
{
    // Validate the model class names
    throwIfContainsNamespaceOrPath(className, QStringLiteral("argument 'name'"));
    throwIfContainsNamespaceOrPath(cmdOptions.oneToOne,
                                   QStringLiteral("option --one-to-one"));
    throwIfContainsNamespaceOrPath(cmdOptions.oneToMany,
                                   QStringLiteral("option --one-to-many"));

    cmdOptions.oneToOne = StringUtils::studly(std::move(cmdOptions.oneToOne));
    cmdOptions.oneToMany = StringUtils::studly(std::move(cmdOptions.oneToMany));

    return {StringUtils::studly(std::move(className)), std::move(cmdOptions)};
}

void ModelCommand::writeModel(const QString &className, const CmdOptions &cmdOptions)
{
    auto modelFilePath = m_creator.create(className, cmdOptions, getModelPath());

    // make_preferred() returns reference and filename() creates a new fs::path instance
    const auto modelFile = isSet(fullpath) ? modelFilePath.make_preferred()
                                           : modelFilePath.filename();

    info(QStringLiteral("Created Model: "), false);

    note(QString::fromStdString(modelFile.string()));
}

ModelCommand::CmdOptions ModelCommand::createCmdOptions() const
{
    return {
        value(one_to_one),
        value(one_to_many),
        value(connection_),
        value(table_),
        isSet(disable_timestamps),
    };
}

fspath ModelCommand::getModelPath() const
{
    // Default location
    if (!isSet(path_))
        return application().getModelsPath();

    auto targetPath = value(path_).toStdString();

    // The 'path' argument contains an absolute path
    if (isSet(realpath_))
        return {std::move(targetPath)};

    // The 'path' argument contains a relative path
    auto modelsPath = fs::current_path() / std::move(targetPath);

    // Validate
    if (fs::exists(modelsPath) && !fs::is_directory(modelsPath))
        throw Exceptions::InvalidArgumentError(
                QStringLiteral("Models path '%1' exists and it's not a directory.")
                .arg(modelsPath.c_str()));

    return modelsPath;
}

void ModelCommand::throwIfContainsNamespaceOrPath(const QString &className,
                                                  const QString &source)
{
    if (!className.contains(QStringLiteral("::")) && !className.contains(QChar('/')) &&
        !className.contains(QChar('\\'))
    )
        return;

    throw Exceptions::InvalidArgumentError(
                QStringLiteral("Namespace or path is not allowed in the model "
                               "names (%1).")
                .arg(source));
}

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE
