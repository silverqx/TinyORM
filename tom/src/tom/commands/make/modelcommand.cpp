#include "tom/commands/make/modelcommand.hpp"

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/move.hpp>
#include <range/v3/view/transform.hpp>

#include <orm/constants.hpp>
#include <orm/utils/container.hpp>
#include <orm/utils/string.hpp>

#include "tom/application.hpp"
#include "tom/exceptions/invalidargumenterror.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Orm::Constants::NAME;
using Orm::Constants::NOSPACE;
using Orm::Constants::pivot_;

using ContainerUtils = Orm::Utils::Container;
using StringUtils = Orm::Utils::String;

using Tom::Constants::MakeMigration;
using Tom::Constants::MakeSeeder;
using Tom::Constants::as_;
using Tom::Constants::as_up;
using Tom::Constants::belongs_to;
using Tom::Constants::belongs_to_many;
using Tom::Constants::belongs_to_many_up;
using Tom::Constants::belongs_to_up;
using Tom::Constants::create_;
using Tom::Constants::dateformat;
using Tom::Constants::dateformat_up;
using Tom::Constants::dates;
using Tom::Constants::dates_up;
using Tom::Constants::connection_;
using Tom::Constants::connection_up;
using Tom::Constants::disable_incrementing;
using Tom::Constants::disable_timestamps;
using Tom::Constants::fillable;
using Tom::Constants::fillable_up;
using Tom::Constants::force;
using Tom::Constants::foreign_key;
using Tom::Constants::foreign_key_up;
using Tom::Constants::from_model;
using Tom::Constants::fullpath;
using Tom::Constants::guarded;
using Tom::Constants::guarded_up;
using Tom::Constants::incrementing;
using Tom::Constants::migration_;
using Tom::Constants::one_to_one;
using Tom::Constants::one_to_one_up;
using Tom::Constants::one_to_many;
using Tom::Constants::one_to_many_up;
using Tom::Constants::path_;
using Tom::Constants::path_up;
using Tom::Constants::pivot_inverse;
using Tom::Constants::pivot_inverse_up;
using Tom::Constants::pivot_model;
using Tom::Constants::pivot_table;
using Tom::Constants::pivot_table_up;
using Tom::Constants::pivot_up;
using Tom::Constants::preserve_order;
using Tom::Constants::primary_key;
using Tom::Constants::primary_key_up;
using Tom::Constants::realpath_;
using Tom::Constants::seeder;
using Tom::Constants::table_;
using Tom::Constants::table_up;
using Tom::Constants::touches;
using Tom::Constants::touches_up;
using Tom::Constants::with_;
using Tom::Constants::with_up;
using Tom::Constants::with_pivot;
using Tom::Constants::with_pivot_up;
using Tom::Constants::with_timestamps;

namespace Tom::Commands::Make
{

/* public */

ModelCommand::ModelCommand(Application &application, QCommandLineParser &parser)
    : MakeCommand(application, parser)
{}

const std::vector<PositionalArgument> &ModelCommand::positionalArguments() const
{
    static const std::vector<PositionalArgument> cached {
        {NAME, QStringLiteral("The name of the model class (required StudlyCase)")},
    };

    return cached;
}

QList<CommandLineOption> ModelCommand::optionsSignature() const
{
    return {
        // Call other commands
        {{QChar('m'),
          migration_},         QStringLiteral("Create a new migration file for the "
                                              "model")},
        {{QChar('s'),
          seeder},             QStringLiteral("Create a new seeder for the model")},

        // Relationship methods
        {one_to_one,           QStringLiteral("Create one-to-one relation to the given "
                                              "model <comment>(multiple options allowed)"
                                              "</comment>"), one_to_one_up}, // Value
        {one_to_many,          QStringLiteral("Create one-to-many relation to the given "
                                              "model <comment>(multiple options allowed)"
                                              "</comment>"), one_to_many_up}, // Value
        {belongs_to,           QStringLiteral("Create belongs-to relation to the given "
                                              "model <comment>(multiple options allowed)"
                                              "</comment>"), belongs_to_up}, // Value
        {belongs_to_many,      QStringLiteral("Create many-to-many relation to the "
                                              "given model <comment>(multiple options "
                                              "allowed)</comment>"), belongs_to_many_up}, // Value

        // Common for all relations
        {foreign_key,          QStringLiteral("The foreign key name <comment>(two values "
                                              "allowed for btm)</comment>"),
                               foreign_key_up},

        // Belongs-to-many related
        {pivot_table,          QStringLiteral("The pivot table name"), pivot_table_up}, // Value
        {pivot_,               QStringLiteral("The class name of the pivot class for the "
                                              "belongs-to-many relationship"),
                               pivot_up}, // Value
        {pivot_inverse,        QStringLiteral("The class name of the pivot class for the "
                                              "belongs-to-many inverse relationship"
                                              "<comment>(multiple values allowed)"
                                              "</comment>"),
                               pivot_inverse_up}, // Value
        {as_,                  QStringLiteral("The name for the pivot relation"),
                               as_up}, // Value
        {with_pivot,           QStringLiteral("Extra attributes for the pivot model "
                                              "<comment>(multiple values allowed)"
                                              "</comment>"),
                               with_pivot_up}, // Value
        {with_timestamps,      QStringLiteral("Pivot table with timestamps")},

        // Model related attributes in the private section
        {table_,               QStringLiteral("The table associated with the model"),
                               table_up}, // Value
        {primary_key,          QStringLiteral("The primary key associated with the "
                                              "table"), primary_key_up}, // Value
        {incrementing,         QStringLiteral("Enable auto-incrementing for the model's "
                                              "primary key")},
        {disable_incrementing, QStringLiteral("Disable auto-incrementing for the model's "
                                              "primary key")},
        {connection_,          QStringLiteral("The connection name for the model"),
                               connection_up}, // Value
        {with_,                QStringLiteral("The relations to eager load on every "
                                              "query <comment>(multiple values allowed)"
                                              "</comment>"), with_up}, // Value
        {fillable,             QStringLiteral("The attributes that are mass assignable "
                                              "<comment>(multiple values allowed)"
                                              "</comment>"), fillable_up}, // Value
        {guarded,              QStringLiteral("The guarded attributes that aren't mass "
                                              "assignable <comment>(multiple values "
                                              "allowed) </comment>"), guarded_up}, // Value
        {disable_timestamps,   QStringLiteral("Disable timestamping of the model")},
        {dateformat,           QStringLiteral("The storage format of the model's date "
                                              "columns"), dateformat_up}, // Value
        {dates,                QStringLiteral("The attributes that should be mutated to "
                                              "dates <comment>(multiple values allowed)"
                                              "</comment>"), dates_up}, // Value
        {touches,              QStringLiteral("All of the relationships to be touched "
                                              "<comment>(multiple values allowed)"
                                              "</comment>"), touches_up}, // Value
        {pivot_model,          QStringLiteral("Genarate a custom pivot model class")},

        // Others
        {{QChar('o'),
          preserve_order},     QStringLiteral("Preserve relations order defined on the "
                                              "command-line")},

        // Paths related
        {path_,                QStringLiteral("The location where the model file should "
                                              "be created"), path_up}, // Value
        {realpath_,            QStringLiteral("Indicate that any provided model file "
                                              "paths are pre-resolved absolute paths")},
        {fullpath,             QStringLiteral("Output the full path of the created "
                                              "model")},
        {{QChar('f'),
          force},              QStringLiteral("Overwrite the model class if already "
                                              "exists")},
    };
}

QString ModelCommand::help() const
{
    return QStringLiteral(
R"(  The <info>belongs-to</info> option is inverse relation for the <info>one-to-one</info>, and <info>one-to-many</info> relationships. The <info>belongs-to-many</info> can be used to define <comment>many-to-many</comment> relationship and also to define the inverse of a <comment>many-to-many</comment> relationship.

  The <info>one-to-one</info>, <info>one-to-many</info>, <info>belongs-to</info>, and <info>belongs-to-many</info> options can be defined more than once:

    <info>tom make:model User --one-to-many=Posts --one-to-many=Comments</info>

  The <info>foreign-key</info> option is common for all relation types, it must follow after the relation option and it should be defined max. one time for the <info>one-to-one</info>, <info>one-to-many</info>, and <info>belongs-to</info> relationships:

    <info>tom make:model User --one-to-many=Posts --foreign-key=post_id --one-to-many=Comments --foreign-key=comment_id</info>

  And max. two times for the <info>belongs-to-many</info> relation, if only one value was given then set the <comment>related pivot key</comment> first. If two values were given then they follow the <gray>Model::belongsToMany()</gray> parameters order, a first value will be the <comment>foreign pivot key</comment>, and a second value the <comment>related pivot key</comment>. Two values can be passed using one <info>foreign-key</info> option separated by the , character or by two separate <info>foreign-key</info> options <gray>(this is also true for all other options that accept multiple values)</gray>:

    <info>tom make:model User --belongs-to-many=Tags --foreign-key=tag_id</info>
    <info>tom make:model User --belongs-to-many=Tags --foreign-key=user_id,tag_id</info>
    <info>tom make:model User --belongs-to-many=Tags --foreign-key=user_id --foreign-key=tag_id</info>

  The <info>pivot-table</info>, <info>pivot</info>, <info>as</info>, <info>with-pivot</info>, and <info>with-timestamps</info> options can be given only after the <info>belongs-to-many</info> relationship.

  The <info>table</info>, <info>primary-key</info>, <info>incrementing</info>, <info>disable-incrementing</info>, <info>connection</info>, <info>with</info>, <info>fillable</info>, <info>guarded</info>, <info>disable-timestamps</info>, <info>dateformat</info>, <info>dates</info>, and <info>touches</info> options relate to the <blue>Model</blue> class itself, they have nothing to do with relationships and can be passed anywhere, best before relationship options:

    <info>tom make:model User --table=users --connection=tinyorm_connection_name --one-to-many=Posts</info>

 The order of the generated relation methods will be <info>one-to-one</info>, <info>one-to-many</info>, <info>belongs-to</info>, and <info>belongs-to-many</info> until you pass the <info>preserve-order</info> option then the order will be preserved.

  The <info>pivot-inverse</info> option adds the pivot type to the Model's <gray>AllRelations</gray> template parameter pack but beware this template parameter is needed by the inverse belongs-to-many relation! It has nothing to do with relationships in the currently generated model. Closer explanation in the documentation. https://bit.ly/3PjlN2R
)");
}

int ModelCommand::run()
{
    Command::run();

    const auto [className, cmdOptions] = prepareModelClassNames(argument(NAME),
                                                                createCmdOptions());

    // Unused warnings
    showUnusedOptionsWarnings(cmdOptions);

    if (!m_unusedBtmOptions.empty() || m_shownUnusedForeignKey ||
        !m_unusedPivotModelOptions.empty() || m_shownUnusedIncrementing
    )
        newLine();

    const auto modelsPath = getModelsPath();

    // Check whether a model file already exists and create parent folder if needed
    prepareFileSystem(QStringLiteral("model"), modelsPath, className.toLower(),
                      className);

    // Ready to write the model to the disk 🧨✨
    writeModel(className, cmdOptions, modelsPath);

    // Call other commands
    if (isSet(migration_))
        createMigration(className);

    if (isSet(seeder))
        createSeeder(className);

    return EXIT_SUCCESS;
}

/* protected */

namespace
{
    /*! Studly all string lists in the given container. */
    const auto studlyContainer = [](auto &&container)
    {
        return ranges::views::move(container)
                | ranges::views::transform([](auto &&classNamesList)
        {
            return StringUtils::studly(
                        std::forward<decltype (classNamesList)>(classNamesList));
        })
                | ranges::to<std::vector<QStringList>>();
    };
} // namespace

std::tuple<QString, CmdOptions>
ModelCommand::prepareModelClassNames(QString &&className, CmdOptions &&cmdOptions)
{
    auto &&[
            _1,
            oneToOneList, oneToManyList, belongsToList, belongsToManyList,
            _2, _3, pivotClasses, pivotInverseClasses, _4, _5, _6, _7, _8, _9, _10, _11,
            _12, _13, _14, _15, _16, _17, _18, _19
    ] = cmdOptions;

    // Validate the model class names
    MakeCommand::throwIfContainsNamespaceOrPath(QStringLiteral("model"), className,
                                                QStringLiteral("argument 'name'"));
    throwIfContainsNamespaceOrPath(oneToOneList,
                                   QStringLiteral("option --one-to-one"));
    throwIfContainsNamespaceOrPath(oneToManyList,
                                   QStringLiteral("option --one-to-many"));
    throwIfContainsNamespaceOrPath(belongsToList,
                                   QStringLiteral("option --belongs-to"));
    throwIfContainsNamespaceOrPath(belongsToManyList,
                                   QStringLiteral("option --belongs-to-many"));
    throwIfContainsNamespaceOrPath(pivotClasses,
                                   QStringLiteral("option --pivot"),
                                   QStringLiteral("pivot model"));
    throwIfContainsNamespaceOrPath(pivotInverseClasses,
                                   QStringLiteral("option --pivot-inverse"),
                                   QStringLiteral("pivot model"));

    oneToOneList        = StringUtils::studly(std::move(oneToOneList));
    oneToManyList       = StringUtils::studly(std::move(oneToManyList));
    belongsToList       = StringUtils::studly(std::move(belongsToList));
    belongsToManyList   = StringUtils::studly(std::move(belongsToManyList));
    pivotClasses        = StringUtils::studly(std::move(pivotClasses));
    pivotInverseClasses = studlyContainer(std::move(pivotInverseClasses));

    return {StringUtils::studly(std::move(className)), std::move(cmdOptions)};
}

void ModelCommand::showUnusedOptionsWarnings(const CmdOptions &cmdOptions)
{
    if (m_isSetPivotModel)
        showUnusedPivotModelOptionsWarnings();
    else
        showUnusedBtmOptionsWarnings(cmdOptions);

    // Show unused disable-incremening option if passed also incrementing option
    showUnusedIncrementingWarning();
}

void ModelCommand::showUnusedBtmOptionsWarnings(const CmdOptions &cmdOptions)
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

    if (isSet(pivot_inverse))
        m_unusedBtmOptions.emplace(QStringLiteral("--pivot-inverse"));

    if (isSet(as_))
        m_unusedBtmOptions.emplace(QStringLiteral("--as"));

    if (isSet(with_pivot))
        m_unusedBtmOptions.emplace(QStringLiteral("--with-pivot"));

    if (isSet(with_timestamps))
        m_unusedBtmOptions.emplace(QStringLiteral("--with-timestamps"));
}

void ModelCommand::showUnusedPivotModelOptionsWarnings()
{
    static const std::unordered_set unsupportedOptions {
        one_to_one, one_to_many, belongs_to, belongs_to_many,
        foreign_key,
        pivot_table, pivot_, pivot_inverse, as_, with_pivot, with_timestamps,
        with_,
        preserve_order
    };

    // Find unused options
    for (auto &&option : parser().optionNames())
        if (unsupportedOptions.contains(option))
            m_unusedPivotModelOptions.insert(option);

    // Nothing to show
    if (m_unusedPivotModelOptions.empty())
        return;

    // Warning message templates
    static const auto singular = QStringLiteral(
                                     "Unused option %1; it's not supported along with "
                                     "the --pivot-model option.");
    static const auto plural =   QStringLiteral(
                                     "Unused options %1; they are not supported along "
                                     "with the --pivot-model option.");

    comment((m_unusedPivotModelOptions.size() == 1 ? singular : plural)
            .arg(ContainerUtils::join(m_unusedPivotModelOptions)));
}

void ModelCommand::showUnusedIncrementingWarning()
{
    // Nothing to show
    if (!(isSet(incrementing) && isSet(disable_incrementing)))
        return;

    comment(QStringLiteral(
                "Unused the --disable-incrementing option; the --incrementing option "
                "has always precedence if both options were given."));

    m_shownUnusedIncrementing = true;
}

void ModelCommand::writeModel(const QString &className, const CmdOptions &cmdOptions,
                              const fspath &modelsPath)
{
    auto modelFilePath = m_creator.create(className, cmdOptions, modelsPath,
                                          isSet(preserve_order));

    // make_preferred() returns reference and filename() creates a new fs::path instance
    const auto modelFile = isSet(fullpath) ? modelFilePath.make_preferred()
                                           : modelFilePath.filename();

    info(QStringLiteral("Created Model: "), false);

    note(QString::fromStdString(modelFile.string()));
}

CmdOptions ModelCommand::createCmdOptions()
{
    // Cache it as it's used multiple times
    m_isSetPivotModel = isSet(pivot_model);

    // Pivot models don't support relations
    if (m_isSetPivotModel)
        return {
            // Relationship methods
            {},

            {}, {}, {}, {},

            // Common for all relationship methods
            {},

            // Belongs-to-many related
            {}, {}, {}, {}, {}, {},

            // Model related
            value(table_),       value(primary_key),          value(connection_),
            {},                  values(fillable),            values(guarded),
            value(dateformat),   values(dates),               values(touches),
            isSet(incrementing), isSet(disable_incrementing), isSet(disable_timestamps),
            m_isSetPivotModel
        };

    return {
        // Relationship methods
        relationsOrder(),

        values(one_to_one), values(one_to_many), values(belongs_to),
        values(belongs_to_many),

        // Common for all relationship methods
        foreignKeyValues(),

        // Belongs-to-many related
        btmValues(pivot_table), btmValues(pivot_), btmMultiValues(pivot_inverse),
        btmValues(as_),         btmMultiValues(with_pivot),
        btmBoolValues(with_timestamps),

        // Model related
        value(table_),       value(primary_key),          value(connection_),
        values(with_),       values(fillable),            values(guarded),
        value(dateformat),   values(dates),               values(touches),
        isSet(incrementing), isSet(disable_incrementing), isSet(disable_timestamps),
        m_isSetPivotModel
    };
}

RelationsOrder ModelCommand::relationsOrder()
{
    RelationsOrder preparedOrders;
    std::size_t relationOrderIndex = 0;

    for (auto &&option : optionNames())
        if (option == one_to_one)
            preparedOrders.oneToOne.push_back(relationOrderIndex++);
        else if (option == one_to_many)
            preparedOrders.oneToMany.push_back(relationOrderIndex++);
        else if (option == belongs_to)
            preparedOrders.belongsTo.push_back(relationOrderIndex++);
        else if (option == belongs_to_many)
            preparedOrders.belongsToMany.push_back(relationOrderIndex++);

    return preparedOrders;
}

/* Others */

fspath ModelCommand::getModelsPath() const
{
    /* If a user passes the --path parameter use it, otherwise try to guess models
       path based on the pwd and if not found use the default path which is set
       by the TINYTOM_MODELS_DIR macro. */
    auto modelsPath = isSet(path_)
                      // User defined path
                      ? getUserModelsPath()
                      // Try to guess or use the default path
                      : guessModelsPath();

    // Validate
    if (fs::exists(modelsPath) && !fs::is_directory(modelsPath))
        throw Exceptions::InvalidArgumentError(
                QStringLiteral("Models path '%1' exists and it's not a directory.")
                .arg(modelsPath.c_str()));

    return modelsPath;
}

fspath ModelCommand::getUserModelsPath() const
{
    auto targetPath = fspath(value(path_).toStdString()).lexically_normal();

    return isSet(realpath_)
            // The 'path' argument contains an absolute path
            ? std::move(targetPath)
            // The 'path' argument contains a relative path
            : fs::current_path() / targetPath;
}

fspath ModelCommand::guessModelsPath() const
{
    return guessPathForMakeByPwd(application().getModelsPath());
}

const std::unordered_set<QString> &ModelCommand::relationNames()
{
    static const std::unordered_set cached {
        Tom::Constants::one_to_one, Tom::Constants::one_to_many,
        Tom::Constants::belongs_to, Tom::Constants::belongs_to_many
    };

    return cached;
}

void ModelCommand::createMigration(const QString &className) const
{
    auto table = StringUtils::snake(className);

    // Plural for a non-pivot models
    if (!isSet(pivot_model))
        table += QLatin1Char('s');

    call(MakeMigration, {longOption(create_, table),
                         boolCmd(force),
                         QStringLiteral("create_%1_table").arg(table),
                         // Inform the make:migration that it's called from the make:model
                         longOption(from_model),
                         // Proxy path to the make:seeder
                         valueCmd(path_)});
}

void ModelCommand::createSeeder(const QString &className) const
{
    auto seederClassName = NOSPACE.arg(className, QStringLiteral("Seeder"));

    // FUTURE tom, add --table option for the make:seeder command and pass singular table name for pivot models because currently the make:seeder command generates eg. taggeds table name (even if this table name is in the commented code), command to reproduce: tom make:model Tagged --pivot-model --seeder silverqx

    call(MakeSeeder, {boolCmd(force),
                      std::move(seederClassName),
                      // Inform the make:seeder that it's called from the make:model
                      longOption(from_model),
                      // Proxy path to the make:seeder
                      valueCmd(path_)});
}

/* private */

void ModelCommand::throwIfContainsNamespaceOrPath(
        const std::vector<QStringList> &classNames, const QString &source,
        const QString &commandType)
{
    for (const auto &classNameList : classNames)
        throwIfContainsNamespaceOrPath(classNameList, source, commandType);
}

void ModelCommand::throwIfContainsNamespaceOrPath(
        const QStringList &classNames, const QString &source, const QString &commandType)
{
    for (const auto &className : classNames)
        MakeCommand::throwIfContainsNamespaceOrPath(commandType, className, source);
}

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE
