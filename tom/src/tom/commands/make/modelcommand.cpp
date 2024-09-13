#include "tom/commands/make/modelcommand.hpp"

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/move.hpp>
#include <range/v3/view/transform.hpp>

#include <orm/constants.hpp>
#include <orm/utils/container.hpp>
#include <orm/utils/string.hpp>

#include "tom/application.hpp"
#include "tom/exceptions/invalidargumenterror.hpp"

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
using Tom::Constants::accessors;
using Tom::Constants::accessors_up;
using Tom::Constants::appends;
using Tom::Constants::appends_up;
using Tom::Constants::as_;
using Tom::Constants::as_up;
using Tom::Constants::belongs_to;
using Tom::Constants::belongs_to_many;
using Tom::Constants::belongs_to_many_up;
using Tom::Constants::belongs_to_up;
using Tom::Constants::casts_example;
using Tom::Constants::create_;
using Tom::Constants::dateformat;
using Tom::Constants::dateformat_up;
using Tom::Constants::dates;
using Tom::Constants::dates_up;
using Tom::Constants::connection_;
using Tom::Constants::connection_up;
using Tom::Constants::disable_incrementing;
using Tom::Constants::disable_snake_attributes;
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
using Tom::Constants::hidden;
using Tom::Constants::hidden_up;
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
using Tom::Constants::snake_attributes;
using Tom::Constants::table_;
using Tom::Constants::table_up;
using Tom::Constants::touches;
using Tom::Constants::touches_up;
using Tom::Constants::visible;
using Tom::Constants::visible_up;
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
        {NAME, u"The name of the model class (required StudlyCase)"_s},
    };

    return cached;
}

QList<CommandLineOption> ModelCommand::optionsSignature() const
{
    return {
        // Call other commands
        {{QChar('m'),
          migration_},         u"Create a new migration file for the model"_s},
        {{QChar('s'),
          seeder},             u"Create a new seeder for the model"_s},

        // Relationship methods
        {one_to_one,           u"Create one-to-one relation to the given model "
                                "<comment>(multiple options allowed)</comment>"_s,
                               one_to_one_up}, // Value
        {one_to_many,          u"Create one-to-many relation to the given model "
                                "<comment>(multiple options allowed)</comment>"_s,
                               one_to_many_up}, // Value
        {belongs_to,           u"Create belongs-to relation to the given model "
                                "<comment>(multiple options allowed)</comment>"_s,
                               belongs_to_up}, // Value
        {belongs_to_many,      u"Create many-to-many relation to the given model "
                                "<comment>(multiple options allowed)</comment>"_s,
                               belongs_to_many_up}, // Value

        // Common for all relations
        {foreign_key,          u"The foreign key name <comment>(two values allowed "
                                "for btm)</comment>"_s, foreign_key_up},

        // Belongs-to-many related
        {pivot_table,          u"The pivot table name"_s, pivot_table_up}, // Value
        {pivot_,               u"The class name of the pivot class for the "
                                "belongs-to-many relationship"_s, pivot_up}, // Value
        {pivot_inverse,        u"The class name of the pivot class for the "
                                "belongs-to-many inverse relationship "
                                "<comment>(multiple values allowed)</comment>"_s,
                               pivot_inverse_up}, // Value
        {as_,                  u"The name for the pivot relation"_s, as_up}, // Value
        {with_pivot,           u"Extra attributes for the pivot model "
                                "<comment>(multiple values allowed)</comment>"_s,
                               with_pivot_up}, // Value
        {with_timestamps,      u"Pivot table with timestamps"_s},

        // Model related attributes in the private section
        {table_,               u"The table associated with the model"_s, table_up}, // Value
        {primary_key,          u"The primary key associated with the table"_s,
                               primary_key_up}, // Value
        {incrementing,         u"Enable auto-incrementing for the model's primary key "
                                "<comment>(default)</comment>"_s},
        {disable_incrementing, u"Disable auto-incrementing for the model's primary "
                                "key"_s},
        {connection_,          u"The connection name for the model"_s, connection_up}, // Value
        {with_,                u"The relations to eager load on every query "
                                "<comment>(multiple values allowed)</comment>"_s,
                               with_up}, // Value
        {fillable,             u"The attributes that are mass assignable <comment>"
                                "(multiple values allowed)</comment>"_s, fillable_up}, // Value
        {guarded,              u"The guarded attributes that aren't mass assignable "
                                "<comment>(multiple values allowed) </comment>"_s,
                               guarded_up}, // Value
        {disable_timestamps,   u"Disable timestamping of the model"_s},
        {dateformat,           u"The storage format of the model's date columns"_s,
                               dateformat_up}, // Value
        {dates,                u"The attributes that should be mutated to dates "
                                "<comment>(multiple values allowed)</comment>"_s,
                               dates_up}, // Value
        {touches,              u"All of the relationships to be touched <comment>"
                                "(multiple values allowed)</comment>"_s, touches_up}, // Value
        {casts_example,        u"Create the u_casts map example"_s},
        {snake_attributes,     u"Enable snake_cased attributes during serialization "
                                "<comment>(default)</comment>"_s},
        {disable_snake_attributes,
                               u"Disable snake_cased attributes during serialization"_s},
        {visible,              u"The attributes that should be visible during "
                                "serialization <comment>(multiple values allowed)"
                                "</comment>"_s, visible_up}, // Value
        {hidden,               u"The attributes that should be hidden during "
                                "serialization <comment>(multiple values allowed)"
                                "</comment>"_s, hidden_up}, // Value
        {accessors,            u"Create accessor methods (merged with appends) "
                                "<comment>(multiple values allowed)</comment>"_s,
                               accessors_up}, // Value
        {appends,              u"The attributes that should be appended during "
                                "serialization <comment>(multiple values allowed)"
                                "</comment>"_s, appends_up}, // Value
        {pivot_model,          u"Generate a custom pivot model class"_s},

        // Others
        {{QChar('o'),
          preserve_order},     u"Preserve relations order defined on the "
                                "command-line"_s},

        // Paths related
        {path_,                u"The location where the model file should be created"_s,
                               path_up}, // Value
        {realpath_,            u"Indicate that any provided model file paths are "
                                "pre-resolved absolute paths"_s},
        {fullpath,             u"Output the full path of the created model"_s},
        {{QChar('f'),
          force},              u"Overwrite the model class if already exists"_s},
    };
}

QString ModelCommand::help() const
{
    return
uR"(  The <info>belongs-to</info> option is inverse relation for the <info>one-to-one</info>, and <info>one-to-many</info> relationships. The <info>belongs-to-many</info> can be used to define <comment>many-to-many</comment> relationship and also to define the inverse of a <comment>many-to-many</comment> relationship.

  The <info>one-to-one</info>, <info>one-to-many</info>, <info>belongs-to</info>, and <info>belongs-to-many</info> options can be defined more than once:

    <info>tom make:model User --one-to-many=Posts --one-to-many=Comments</info>

  The <info>foreign-key</info> option is common for all relation types, it must follow after the relation option and it should be defined max. one time for the <info>one-to-one</info>, <info>one-to-many</info>, and <info>belongs-to</info> relationships:

    <info>tom make:model User --one-to-many=Posts --foreign-key=post_id --one-to-many=Comments --foreign-key=comment_id</info>

  And max. two times for the <info>belongs-to-many</info> relation, if only one value was given then set the <comment>related pivot key</comment> first. If two values were given then they follow the <gray>Model::belongsToMany()</gray> parameters order, a first value will be the <comment>foreign pivot key</comment>, and a second value the <comment>related pivot key</comment>. Two values can be passed using one <info>foreign-key</info> option separated by the , character or by two separate <info>foreign-key</info> options <gray>(this is also true for all other options that accept multiple values)</gray>:

    <info>tom make:model User --belongs-to-many=Tags --foreign-key=tag_id</info>
    <info>tom make:model User --belongs-to-many=Tags --foreign-key=user_id,tag_id</info>
    <info>tom make:model User --belongs-to-many=Tags --foreign-key=user_id --foreign-key=tag_id</info>

  The <info>pivot-table</info>, <info>pivot</info>, <info>as</info>, <info>with-pivot</info>, and <info>with-timestamps</info> options can be given only after the <info>belongs-to-many</info> relationship.

  The <info>table</info>, <info>primary-key</info>, <info>incrementing</info>, <info>disable-incrementing</info>, <info>connection</info>, <info>with</info>, <info>fillable</info>, <info>guarded</info>, <info>disable-timestamps</info>, <info>dateformat</info>, <info>dates</info>,  <info>touches</info>, <info>casts-example</info>, <info>snake-attributes</info>, <info>disable-snake-attributes</info>, <info>visible</info>, <info>hidden</info>, <info>accessors</info>, and <info>appends</info> options relate to the <blue>Model</blue> class itself, they have nothing to do with relationships and can be passed anywhere, best before relationship options:

    <info>tom make:model User --table=users --connection=tinyorm_connection_name --one-to-many=Posts</info>

 The order of the generated relation methods will be <info>one-to-one</info>, <info>one-to-many</info>, <info>belongs-to</info>, and <info>belongs-to-many</info> until you pass the <info>preserve-order</info> option then the order will be preserved.

  The <info>pivot-inverse</info> option adds the pivot type to the Model's <gray>AllRelations</gray> template parameter pack but beware this template parameter is needed by the inverse belongs-to-many relation! It has nothing to do with relationships in the currently generated model. Closer explanation in the documentation. https://bit.ly/44Kk3aC

  The <info>appends</info> values are internally merged into the <info>accessors</info>, which ensures that the accessor methods for <info>appends</info> will be generated automatically.
)"_s;
}

int ModelCommand::run()
{
    Command::run();

    const auto [className, cmdOptions] = prepareModelClassNames(argument(NAME),
                                                                createCmdOptions());

    // Unused warnings
    showUnusedOptionsWarnings(cmdOptions);

    if (!m_unusedBtmOptions.empty()        || m_shownUnusedForeignKey   ||
        !m_unusedPivotModelOptions.empty() || m_shownUnusedIncrementing ||
        m_shownUnusedSnakeAttribtues
    )
        newLine();

    const auto modelsPath = getModelsPath();

    // Check whether a model file already exists and create parent folder if needed
    prepareFileSystem(u"model"_s, modelsPath, className.toLower(), className);

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
    /*! Studly all string lists in the given container (std::vector<QStringList>). */
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
            _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26
    ] = cmdOptions;

    // Validate the model class names
    MakeCommand::throwIfContainsNamespaceOrPath(u"model"_s, className,
                                                u"argument 'name'"_s);
    throwIfContainsNamespaceOrPath(oneToOneList,
                                   u"option --one-to-one"_s);
    throwIfContainsNamespaceOrPath(oneToManyList,
                                   u"option --one-to-many"_s);
    throwIfContainsNamespaceOrPath(belongsToList,
                                   u"option --belongs-to"_s);
    throwIfContainsNamespaceOrPath(belongsToManyList,
                                   u"option --belongs-to-many"_s);
    throwIfContainsNamespaceOrPath(pivotClasses,
                                   u"option --pivot"_s,
                                   u"pivot model"_s);
    throwIfContainsNamespaceOrPath(pivotInverseClasses,
                                   u"option --pivot-inverse"_s,
                                   u"pivot model"_s);

    oneToOneList        = StringUtils::studly(std::move(oneToOneList)); // QStringList
    oneToManyList       = StringUtils::studly(std::move(oneToManyList));
    belongsToList       = StringUtils::studly(std::move(belongsToList));
    belongsToManyList   = StringUtils::studly(std::move(belongsToManyList));
    pivotClasses        = StringUtils::studly(std::move(pivotClasses));
    pivotInverseClasses = studlyContainer(std::move(pivotInverseClasses)); // std::vector<QStringList>

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
    /* Show unused disable-snake-attributes option if passed also the snake-attributes
       option. */
    showUnusedSnakeAttributesWarning();
}

void ModelCommand::showUnusedBtmOptionsWarnings(const CmdOptions &cmdOptions)
{
    findUnusedBtmOptions(cmdOptions);

    // Nothing to show
    if (m_unusedBtmOptions.empty())
        return;

    // Warning message templates
    static const auto singular = u"Unused option %1; it depends on the "
                                  "--belongs-to-many= option."_s;
    static const auto plural =   u"Unused options %1; they depend on the "
                                  "--belongs-to-many= option."_s;

    comment((m_unusedBtmOptions.size() == 1 ? singular : plural)
            .arg(ContainerUtils::join(m_unusedBtmOptions)));
}

void ModelCommand::findUnusedBtmOptions(const CmdOptions &cmdOptions)
{
    // Nothing to find, in this case algorithm in the btmValues() searches unused options
    if (!cmdOptions.belongsToManyList.isEmpty())
        return;

    if (isSet(pivot_table))
        m_unusedBtmOptions.emplace(u"--pivot-table"_s);

    if (isSet(pivot_))
        m_unusedBtmOptions.emplace(u"--pivot"_s);

    if (isSet(pivot_inverse))
        m_unusedBtmOptions.emplace(u"--pivot-inverse"_s);

    if (isSet(as_))
        m_unusedBtmOptions.emplace(u"--as"_s);

    if (isSet(with_pivot))
        m_unusedBtmOptions.emplace(u"--with-pivot"_s);

    if (isSet(with_timestamps))
        m_unusedBtmOptions.emplace(u"--with-timestamps"_s);
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
    static const auto singular = u"Unused option %1; it's not supported along with "
                                  "the --pivot-model option."_s;
    static const auto plural =   u"Unused options %1; they are not supported along "
                                  "with the --pivot-model option."_s;

    comment((m_unusedPivotModelOptions.size() == 1 ? singular : plural)
            .arg(ContainerUtils::join(m_unusedPivotModelOptions)));
}

void ModelCommand::showUnusedIncrementingWarning()
{
    // Nothing to show
    if (!(isSet(incrementing) && isSet(disable_incrementing)))
        return;

    comment(u"Unused --disable-incrementing option; the --incrementing option "
             "has always precedence if both options were given."_s);

    m_shownUnusedIncrementing = true;
}

void ModelCommand::showUnusedSnakeAttributesWarning()
{
    // Nothing to show
    if (!(isSet(snake_attributes) && isSet(disable_snake_attributes)))
        return;

    comment(u"Unused --disable-snake-attributes option; the --snake-attributes option "
             "has always precedence if both options were given."_s);

    m_shownUnusedSnakeAttribtues = true;
}

void ModelCommand::writeModel(const QString &className, const CmdOptions &cmdOptions,
                              const fspath &modelsPath)
{
    auto modelFilePath = m_creator.create(className, cmdOptions, modelsPath,
                                          isSet(preserve_order));

    // make_preferred() returns reference and filename() creates a new fs::path instance
    const auto modelFile = isSet(fullpath) ? modelFilePath.make_preferred()
                                           : modelFilePath.filename();

    info(u"Created Model: "_s, false);

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
            value(table_),       value(primary_key), value(connection_), {},
            values(fillable),    values(guarded),
            value(dateformat),   values(dates),      values(touches),
            values(visible),     values(hidden),
            values(accessors),   values(appends),
            isSet(incrementing), isSet(disable_incrementing), isSet(disable_timestamps),
            isSet(casts_example),
            isSet(snake_attributes), isSet(disable_snake_attributes),
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
        value(table_),       value(primary_key), value(connection_), values(with_),
        values(fillable),    values(guarded),
        value(dateformat),   values(dates),      values(touches),
        values(visible),     values(hidden),
        values(accessors),   values(appends),
        isSet(incrementing), isSet(disable_incrementing), isSet(disable_timestamps),
        isSet(casts_example),
        isSet(snake_attributes), isSet(disable_snake_attributes),
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
                      // User-defined path
                      ? getUserModelsPath()
                      // Try to guess or use the default path
                      : guessModelsPath();

    // Validate
    if (fs::exists(modelsPath) && !fs::is_directory(modelsPath))
        throw Exceptions::InvalidArgumentError(
                u"Models path '%1' exists and it's not a directory."_s
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
        table += u's';

    call(MakeMigration, {longOption(create_, table),
                         boolCmd(force),
                         u"create_%1_table"_s.arg(table),
                         // Inform the make:migration that it's called from the make:model
                         longOption(from_model),
                         // Proxy path to the make:seeder
                         valueCmd(path_)});
}

void ModelCommand::createSeeder(const QString &className) const
{
    auto seederClassName = NOSPACE.arg(className, u"Seeder"_s);

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
