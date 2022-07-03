#include "tom/commands/make/modelcommand.hpp"

#include <orm/constants.hpp>
#include <orm/tiny/utils/string.hpp>
#include <orm/utils/container.hpp>

#include "tom/application.hpp"
#include "tom/exceptions/invalidargumenterror.hpp"
#include "tom/tomconstants.hpp"

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Orm::Constants::NAME;

using ContainerUtils = Orm::Utils::Container;
using StringUtils = Orm::Tiny::Utils::String;

using Tom::Constants::as_;
using Tom::Constants::as_up;
using Tom::Constants::belongs_to;
using Tom::Constants::belongs_to_many;
using Tom::Constants::belongs_to_many_up;
using Tom::Constants::belongs_to_up;
using Tom::Constants::connection_;
using Tom::Constants::connection_up;
using Tom::Constants::disable_timestamps;
using Tom::Constants::foreign_key;
using Tom::Constants::foreign_key_up;
using Tom::Constants::fullpath;
using Tom::Constants::one_to_one;
using Tom::Constants::one_to_one_up;
using Tom::Constants::one_to_many;
using Tom::Constants::one_to_many_up;
using Tom::Constants::path_;
using Tom::Constants::path_up;
using Tom::Constants::pivot_;
using Tom::Constants::pivot_table;
using Tom::Constants::pivot_table_up;
using Tom::Constants::pivot_up;
using Tom::Constants::realpath_;
using Tom::Constants::table_;
using Tom::Constants::table_up;
using Tom::Constants::with_pivot;
using Tom::Constants::with_pivot_up;
using Tom::Constants::with_timestamps;

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
        // Relationship methods
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

        // Common for all relations
        {foreign_key,        QStringLiteral("The foreign key name, every relation can "
                                            "have one foreign key <comment>(multiple "
                                            "values allowed) </comment>"),
                             foreign_key_up},

        // Belongs-to-many related
        {pivot_table,        QStringLiteral("The pivot table name"), pivot_table_up}, // Value
        {pivot_,             QStringLiteral("The class name of the pivot class for the "
                                            "belongs-to-many relationship"),
                             pivot_up}, // Value
        {as_,                QStringLiteral("The name for the pivot relation"),
                             as_up}, // Value
        {with_pivot,         QStringLiteral("Extra attributes for the pivot model "
                                            "<comment>(multiple values allowed)"
                                            "</comment>"),
                             with_pivot_up}, // Value
        {with_timestamps,    QStringLiteral("Pivot table with timestamps")},

        // Attributes in the private section
        {table_,             QStringLiteral("The table associated with the model"),
                             table_up}, // Value
        {connection_,        QStringLiteral("The connection name for the model"),
                             connection_up}, // Value
        {disable_timestamps, QStringLiteral("Disable timestamping of the model")},

        // Paths related
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

    showUnusedOptionsWarnings(cmdOptions);

    if (!m_unusedBtmOptions.empty() || m_shownUnusedForeignKey)
        newLine();

    // Ready to write the model to the disk 🧨✨
    writeModel(className, cmdOptions);

    return EXIT_SUCCESS;
}

/* protected */

std::tuple<QString, CmdOptions>
ModelCommand::prepareModelClassnames(QString &&className, CmdOptions &&cmdOptions)
{
    auto &&[
            oneToOneList, oneToManyList, belongsToList, belongsToManyList,
            _1, _2, pivotClasses, _3, _4, _5, _6, _7, _8
    ] = cmdOptions;

    // Validate the model class names
    throwIfContainsNamespaceOrPath(className, QStringLiteral("argument 'name'"));
    throwIfContainsNamespaceOrPath(oneToOneList,
                                   QStringLiteral("option --one-to-one"));
    throwIfContainsNamespaceOrPath(oneToManyList,
                                   QStringLiteral("option --one-to-many"));
    throwIfContainsNamespaceOrPath(belongsToList,
                                   QStringLiteral("option --belongs-to"));
    throwIfContainsNamespaceOrPath(belongsToManyList,
                                   QStringLiteral("option --belongs-to-many"));
    throwIfContainsNamespaceOrPath(pivotClasses,
                                   QStringLiteral("option --pivot"));

    oneToOneList      = StringUtils::studly(std::move(oneToOneList));
    oneToManyList     = StringUtils::studly(std::move(oneToManyList));
    belongsToList     = StringUtils::studly(std::move(belongsToList));
    belongsToManyList = StringUtils::studly(std::move(belongsToManyList));
    pivotClasses      = StringUtils::studly(std::move(pivotClasses));

    return {StringUtils::studly(std::move(className)), std::move(cmdOptions)};
}

void ModelCommand::showUnusedOptionsWarnings(const CmdOptions &cmdOptions)
{
    findUnusedBtmOptions(cmdOptions);

    // Nothing to show
    if (m_unusedBtmOptions.empty())
        return;

    // Warning message templates
    static const auto singular = QStringLiteral("Unused option %1; it depends on the "
                                                "--belongs-to-many= option.");
    static const auto plural =   QStringLiteral("Unused options %1; they depend on the "
                                                "--belongs-to-many= option.");

    comment((m_unusedBtmOptions.size() == 1 ? singular : plural)
            .arg(ContainerUtils::join(m_unusedBtmOptions)));
}

void ModelCommand::findUnusedBtmOptions(const CmdOptions &cmdOptions)
{
    // Nothing to find, in this case algorithm in the btmValues() searches unused options
    if (!cmdOptions.belongsToManyList.isEmpty())
        return;

    if (isSet(pivot_table))
        m_unusedBtmOptions.emplace(QStringLiteral("--pivot-table"));

    if (isSet(pivot_))
        m_unusedBtmOptions.emplace(QStringLiteral("--pivot"));

    if (isSet(as_))
        m_unusedBtmOptions.emplace(QStringLiteral("--as"));

    if (isSet(with_pivot))
        m_unusedBtmOptions.emplace(QStringLiteral("--with-pivot"));

    if (isSet(with_timestamps))
        m_unusedBtmOptions.emplace(QStringLiteral("--with-timestamps"));
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

CmdOptions ModelCommand::createCmdOptions()
{
    return {
        // Relationship methods
        values(one_to_one), values(one_to_many), values(belongs_to),
        values(belongs_to_many),

        // Common for all relationship methods
        foreignKeyValues(),

        // Belongs-to-many related
        btmValues(pivot_table),      btmValues(pivot_), btmValues(as_),
        btmMultiValues(with_pivot),  btmBoolValues(with_timestamps),

        // Model related
        value(connection_), value(table_), isSet(disable_timestamps),
    };
}

/* Others */

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

/* private */

const std::unordered_set<QString> &ModelCommand::relationNames()
{
    static const std::unordered_set cached {
        Tom::Constants::one_to_one, Tom::Constants::one_to_many,
        Tom::Constants::belongs_to, Tom::Constants::belongs_to_many
    };

    return cached;
}

void ModelCommand::throwIfContainsNamespaceOrPath(const QStringList &classNames,
                                                  const QString &source)
{
    for (const auto &className : classNames)
        throwIfContainsNamespaceOrPath(className, source);
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
