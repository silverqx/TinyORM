#include "tom/commands/make/modelcommand.hpp"

#include <orm/constants.hpp>
#include <orm/tiny/utils/string.hpp>
#include <orm/utils/container.hpp>

#include "tom/application.hpp"
#include "tom/exceptions/invalidargumenterror.hpp"
#include "tom/tomconstants.hpp"

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Orm::Constants::COLON;
using Orm::Constants::NAME;
using Orm::Constants::PLUS;

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

    if (m_shownUnusedOption || m_shownUnusedForeign)
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
            oneToOneList, oneToManyList, belongsToList, belongsToMany,
            _1, _2, pivot, _3, _4, _5, _6, _7, _8
    ] = cmdOptions;

    // Validate the model class names
    throwIfContainsNamespaceOrPath(className, QStringLiteral("argument 'name'"));
    throwIfContainsNamespaceOrPath(oneToOneList,
                                   QStringLiteral("option --one-to-one"));
    throwIfContainsNamespaceOrPath(oneToManyList,
                                   QStringLiteral("option --one-to-many"));
    throwIfContainsNamespaceOrPath(belongsToList,
                                   QStringLiteral("option --belongs-to"));
    throwIfContainsNamespaceOrPath(belongsToMany,
                                   QStringLiteral("option --belongs-to-many"));
    throwIfContainsNamespaceOrPath(pivot,
                                   QStringLiteral("option --pivot"));

    oneToOneList  = StringUtils::studly(std::move(oneToOneList));
    oneToManyList = StringUtils::studly(std::move(oneToManyList));
    belongsToList = StringUtils::studly(std::move(belongsToList));
    belongsToMany = StringUtils::studly(std::move(belongsToMany));
    pivot         = StringUtils::studly(std::move(pivot));

    return {StringUtils::studly(std::move(className)), std::move(cmdOptions)};
}

void ModelCommand::showUnusedOptionsWarnings(const CmdOptions &cmdOptions)
{
    const auto &[
            _1, _2, _3, belongsToMany,
            _4, pivotTable, pivot, as, withPivot, withTimestamps,
            _5, _6, _7
    ] = cmdOptions;

    if (!belongsToMany.isEmpty())
        return;

    std::set<QString> unusedOptions;

    if (!pivotTable.isEmpty())
        unusedOptions.emplace(QStringLiteral("--pivot-table"));

    if (!pivot.isEmpty())
        unusedOptions.emplace(QStringLiteral("--pivot"));

    if (!as.isEmpty())
        unusedOptions.emplace(QStringLiteral("--as"));

    if (!withPivot.isEmpty())
        unusedOptions.emplace(QStringLiteral("--with-pivot"));

    if (withTimestamps)
        unusedOptions.emplace(QStringLiteral("--with-timestamps"));

    // Warning message templates
    const auto singular = QStringLiteral("Unused option %1; it depends on the "
                                         "--belongs-to-many= option.");
    const auto plural =   QStringLiteral("Unused options %1; they depend on the "
                                         "--belongs-to-many= option.");

    comment((unusedOptions.size() == 1 ? singular : plural)
            .arg(ContainerUtils::join(unusedOptions)));

    m_shownUnusedOption = true;
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

ModelCommand::CmdOptions ModelCommand::createCmdOptions()
{
    return {
        values(one_to_one), values(one_to_many), values(belongs_to),
        value(belongs_to_many),

        prepareForeignKeys(values(foreign_key)),

        value(pivot_table),        value(pivot_),      value(as_),    values(with_pivot),
        isSet(with_timestamps),    value(connection_), value(table_),
        isSet(disable_timestamps),
    };
}

/* Foreign key names */

// TODO tom, make model, detect passing more values to --foreign-key=post_id,xx, only one option I can think of to get whole command line and parse/detect it manually, Qt doesn't offer any API for this silverqx
ModelCommand::ForeignKeys
ModelCommand::prepareForeignKeys(const QStringList &foreignKeyValues)
{
    static const std::unordered_set relationNames {
        one_to_one, one_to_many, belongs_to, belongs_to_many
    };

    auto optionsNames = parser().optionNames();

    QString currentRelation;
    ForeignKeys foreignKeys;
    QStringList::size_type foreignIndex = 0;
    bool wasForeignKeySet = false;
    // belongs-to-many related, it allows to set two foreign keys for the pivot table
    bool wasForeignKeySetPartial = false;

    for (auto &&option : optionsNames) {
        // Try to start a new relation during foreign key names search
        if (startNewRelation(relationNames, option, currentRelation, foreignKeys,
                             wasForeignKeySet, wasForeignKeySetPartial))
            continue;

        // Searching a foreign key name after the relation option on the cmd. line
        // Nothing to do
        if (option != foreign_key)
            continue;

        // Foreign key name defined before a relation or more options set for one relation
        if (currentRelation.isEmpty() || wasForeignKeySet) {
            showUnusedForeignKeyWarning();

            // Skip the value, one the first option's value is used
            if (wasForeignKeySet)
                ++foreignIndex;

            continue;
        }

        // Foreign key name found, assign it to the correct relation type
        insertForeignKeyName(currentRelation, foreignKeys, foreignKeyValues, foreignIndex,
                             wasForeignKeySet, wasForeignKeySetPartial);
    }

    // Handle the last relation
    // No foreign key name was passed on the cmd. line so insert the default value
    if (!currentRelation.isEmpty() && !wasForeignKeySet)
        insertEmptyForeignList(currentRelation, foreignKeys);

    return foreignKeys;
}

bool ModelCommand::startNewRelation(
            const std::unordered_set<QString> &relationNames, QString &option,
            QString &currentRelation, ForeignKeys &foreignKeys,
            bool &wasForeignKeySet, bool &wasForeignKeySetPartial)
{
    // Nothing to start
    if (!relationNames.contains(option))
        return false;

    // Relation related option passed on the cmd. line, start (found relation option)
    // Parsing a first relation or foreign key name was already set
    if (currentRelation.isEmpty() || wasForeignKeySet) {
        currentRelation = std::move(option);
        // Reset to defaults
        wasForeignKeySet = false;
        wasForeignKeySetPartial = false;

        return true;
    }

    // No foreign key name was passed on the cmd. line so insert the default value
    insertEmptyForeignList(currentRelation, foreignKeys);

    currentRelation = std::move(option);

    return true;
}

void ModelCommand::insertEmptyForeignList(const QString &currentRelation,
                                          ForeignKeys &foreignKeys)
{
    if (currentRelation == one_to_one)
        foreignKeys.oneToOne.push_back({});

    else if (currentRelation == one_to_many)
        foreignKeys.oneToMany.push_back({});

    else if (currentRelation == belongs_to)
        foreignKeys.belongsTo.push_back({});

    else if (currentRelation == belongs_to_many)
        foreignKeys.belongsToMany.push_back({});

    else
        Q_UNREACHABLE();
}

void ModelCommand::insertForeignKeyName(
            const QString &currentRelation, ForeignKeys &foreignKeys,
            const QStringList &foreignKeyValues, QStringList::size_type &foreignIndex,
            bool &wasForeignKeySet, bool &wasForeignKeySetPartial)
{
    if (currentRelation == one_to_one) {
        foreignKeys.oneToOne << foreignKeyValues.at(foreignIndex++);
        wasForeignKeySet = true;
    }
    else if (currentRelation == one_to_many) {
        foreignKeys.oneToMany << foreignKeyValues.at(foreignIndex++);
        wasForeignKeySet = true;
    }
    else if (currentRelation == belongs_to) {
        foreignKeys.belongsTo << foreignKeyValues.at(foreignIndex++);
        wasForeignKeySet = true;
    }
    else if (currentRelation == belongs_to_many)
        insertForeignKeyNameBtm(foreignKeys, foreignKeyValues, foreignIndex,
                                wasForeignKeySet, wasForeignKeySetPartial);
    else
        Q_UNREACHABLE();
}

void ModelCommand::insertForeignKeyNameBtm(
            ForeignKeys &foreignKeys,
            const QStringList &foreignKeyValues, QStringList::size_type &foreignIndex,
            bool &wasForeignKeySet, bool &wasForeignKeySetPartial)
{
    /* Both foreign key names can be assigned using one --foreign-key= option, but they
       must be divided by the colon character. */
    if (const auto &keyValue = foreignKeyValues.at(foreignIndex++);
        keyValue.contains(COLON)
    ) {
        auto foreignKeyValuesSplitted = keyValue.split(COLON, Qt::KeepEmptyParts);
        Q_ASSERT(foreignKeyValuesSplitted.size() == 2);

#ifdef __clang__
        foreignKeys.belongsToMany.push_back(
                    {std::move(foreignKeyValuesSplitted.first()),
                     std::move(foreignKeyValuesSplitted.last())});
#else
        foreignKeys.belongsToMany.emplace_back(
                    std::move(foreignKeyValuesSplitted.first()),
                    std::move(foreignKeyValuesSplitted.last()));
#endif

        wasForeignKeySet = true;
    }
    /* Or they can be assigned using two --foreign-key= options or one option and the
       given value must be divided using the comma character (Qt specific behavior), in
       this case the first value has to start with the + character, this is only one
       option how to detect that there is also second value ready in the list.
       If only one value was passed then set the related pivot key first. */
    else
        // First value
        if (!wasForeignKeySetPartial) {
            // Set the related pivot key name (one value was passed)
            if (!keyValue.startsWith(PLUS)) {
#ifdef __clang__
                foreignKeys.belongsToMany.push_back({{}, keyValue});
#else
                foreignKeys.belongsToMany.emplace_back<QString>({}, keyValue);
#endif

                wasForeignKeySetPartial = true;
            }
            /* Set both values at once, a foreign and related pivot key names (two
               values have been passed and the first value began with the + char.). */
            else {
#ifdef __clang__
                foreignKeys.belongsToMany.push_back(
                            {keyValue.mid(1), foreignKeyValues.at(foreignIndex++)});
#else
                foreignKeys.belongsToMany.emplace_back(
                            keyValue.mid(1), foreignKeyValues.at(foreignIndex++));
#endif

                wasForeignKeySet = true;
            }
        }
        // Second value (one value was passed)
        else {
            // Swap pivot keys, so it follows belongsToMany() parameters order
            auto &last = foreignKeys.belongsToMany.back();
            last.foreignPivotKey.swap(last.relatedPivotKey);
            last.relatedPivotKey = keyValue;

            wasForeignKeySet = true;
        }
}

void ModelCommand::showUnusedForeignKeyWarning()
{
    // Already shown
    if (m_shownUnusedForeign)
        return;

    comment(QStringLiteral(
                "Unused --foreign-key= option, it has to follow after any relation "
                "option and should be defined only once for every relation option."));

    m_shownUnusedForeign = true;
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
