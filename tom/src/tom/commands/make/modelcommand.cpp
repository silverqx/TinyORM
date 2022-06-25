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

using Tom::Constants::connection_;
using Tom::Constants::connection_up;
using Tom::Constants::fullpath;
using Tom::Constants::path_;
using Tom::Constants::path_up;
using Tom::Constants::realpath_;
using Tom::Constants::table_;
using Tom::Constants::table_up;
using Tom::Constants::disable_timestamps;

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
        {connection_,        QStringLiteral("The connection name for the model"),
                             connection_up}, // Value
        {table_,             QStringLiteral("The table associated with the model"),
                             table_up}, // Value
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

    const auto className = prepareModelClassname(argument(NAME));

    // Ready to write the model to the disk 🧨✨
    writeModel(className);

    return EXIT_SUCCESS;
}

/* protected */

QString ModelCommand::prepareModelClassname(QString &&className)
{
    // Validate the model name
    throwIfContainsNamespaceOrPath(className);

    return StringUtils::studly(std::move(className));
}

void ModelCommand::writeModel(const QString &className) const
{
    auto modelFilePath = m_creator.create(className, createCmdOptions(), getModelPath());

    // make_preferred() returns reference and filename() creates a new fs::path instance
    const auto modelFile = isSet(fullpath) ? modelFilePath.make_preferred()
                                           : modelFilePath.filename();

    info(QStringLiteral("Created Model: "), false);

    note(QString::fromStdString(modelFile.string()));
}

ModelCommand::CmdOptions ModelCommand::createCmdOptions() const
{
    return {
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

void ModelCommand::throwIfContainsNamespaceOrPath(const QString &className)
{
    if (!className.contains(QStringLiteral("::")) && !className.contains(QChar('/')) &&
        !className.contains(QChar('\\'))
    )
        return;

    throw Exceptions::InvalidArgumentError(
                "Namespace or path is not allowed in the model name");
}

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE
