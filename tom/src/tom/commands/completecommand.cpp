#include "tom/commands/completecommand.hpp"

#include <QRegularExpression>

#include <fstream>

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

#include <orm/constants.hpp>
#include <orm/macros/likely.hpp>
#include <orm/utils/string.hpp>

#include "tom/application.hpp"
#include "tom/tomutils.hpp"

#ifndef sl
/*! Alias for the QStringLiteral(). */
#  define sl(str) QStringLiteral(str)
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace fs = std::filesystem;

using Orm::Constants::COMMA_C;
using Orm::Constants::DASH;
using Orm::Constants::EMPTY;
using Orm::Constants::EQ_C;
using Orm::Constants::NEWLINE;
using Orm::Constants::NOSPACE;
using Orm::Constants::SPACE;
using Orm::Constants::database_;

using StringUtils = Orm::Utils::String;

using Tom::Constants::About;
using Tom::Constants::DoubleDash;
using Tom::Constants::Env;
using Tom::Constants::Help;
using Tom::Constants::Integrate;
using Tom::Constants::List;
using Tom::Constants::LongOption;
using Tom::Constants::ShPwsh;
using Tom::Constants::commandline;
using Tom::Constants::commandline_up;
using Tom::Constants::only_;
using Tom::Constants::word_;
using Tom::Constants::word_up;

#ifdef _MSC_VER
using Tom::Constants::LongOptionValue;
using Tom::Constants::position_;
using Tom::Constants::position_up;
#else
using Tom::Constants::ShBash;
using Tom::Constants::ShZsh;
using Tom::Constants::cword_;
using Tom::Constants::cword_up;
#endif

using TomUtils = Tom::Utils;

namespace Tom::Commands
{

/* Complete command is inspired by winget/dotnet complete:
   https://github.com/microsoft/winget-cli/blob/master/doc/Completion.md */

/* public */

CompleteCommand::CompleteCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
{}

QList<CommandLineOption> CompleteCommand::optionsSignature() const
{
    return {
        {word_,       sl("The current word that is being completed"), word_up},
        {commandline, sl("The entire current command-line"), commandline_up},
#ifdef _MSC_VER
        {position_,   sl("The current position of the cursor on the command-line"),
                      position_up, sl("0")},
#else
        {cword_,      sl("Position of the current word on the command-line that is "
                         "being completed"), cword_up},
#endif
    };
}

int CompleteCommand::run() // NOLINT(readability-function-cognitive-complexity)
{
    Command::run();

    /* Initialization section */
    const auto commandlineArg = value(commandline);

#ifdef _MSC_VER
#  if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const auto positionArg = value(position_).toLongLong();
#  else
    const auto positionArg = value(position_).toInt();
#  endif

    // Currently processed tom command
    const auto commandlineArgSplitted = commandlineArg.split(SPACE);
    Q_ASSERT(!commandlineArgSplitted.isEmpty());

    const auto currentCommandArg = getCurrentTomCommand(commandlineArgSplitted);
    const auto tomCommandSize    = commandlineArgSplitted.constFirst().size();

    // Get the command-line option value for --word= option (with workaround for pwsh)
    const auto commandlineArgSize = commandlineArg.size();
    const auto wordArg = getWordOptionValue(commandlineArgSplitted, positionArg,
                                            commandlineArgSize);
#else
    const auto wordArg = value(word_);

#  if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const auto cwordArg = value(cword_).toLongLong();
#  else
    const auto cwordArg = value(cword_).toInt();
#  endif

    // Currently processed tom command
    const auto currentCommandArg = getCurrentTomCommand(commandlineArg, cwordArg);
#endif

    /* Main logic section */

    /* Print all commands after tom command itself or for the help command
       --- */
#ifdef _MSC_VER
    if (wordArg.isEmpty() &&
        (positionArg == tomCommandSize + 1 ||
         (positionArg >= commandlineArgSize &&
          (!commandlineArg.contains(SPACE) || currentCommandArg == Help)))
    )
#else
    if (wordArg.isEmpty() && (!currentCommandArg || currentCommandArg == Help))
#endif
        return printGuessedCommands(
                    application().guessCommandNamesForComplete({}));

    /* Print all guessed commands by the word argument after tom or for the help command
       --- */
#ifdef _MSC_VER
    if (!isOptionArgument(wordArg) && !wordArg.isEmpty() &&
        (StringUtils::countBefore(commandlineArg, SPACE, positionArg) == 1 ||
         currentCommandArg == Help)
    )
#else
    if (!isOptionArgument(wordArg) && !wordArg.isEmpty() &&
        (!currentCommandArg || currentCommandArg == Help)
    )
#endif
        return printGuessedCommands(
                    application().guessCommandNamesForComplete(wordArg));

    /* Print all or guessed namespace names for the list command
       --- */
#ifdef _MSC_VER
    if (!isOptionArgument(wordArg) && currentCommandArg == List &&
        positionArg >= commandlineArgSize &&
        ((wordArg.isEmpty() && commandlineArg.count(SPACE) == 1) ||
         (!wordArg.isEmpty() && commandlineArg.count(SPACE) == 2))
    )
#else
    if (!isOptionArgument(wordArg) && currentCommandArg == List)
#endif
        return printGuessedNamespaces(wordArg);

    /* Print all or guessed shell names for the integrate command
       --- */
#ifdef _MSC_VER
    if (!isOptionArgument(wordArg) && currentCommandArg == Integrate &&
        positionArg >= commandlineArgSize &&
        ((wordArg.isEmpty() && commandlineArg.count(SPACE) == 1) ||
         (!wordArg.isEmpty() && commandlineArg.count(SPACE) == 2))
    )
#else
    if (!isOptionArgument(wordArg) && currentCommandArg == Integrate)
#endif
        return printGuessedShells(wordArg);

    /* Print all or guessed section names for the about command --only= option
       --- */
    // Bash has it's own guess logic in the tom.bash complete file
#ifdef _MSC_VER
    if (currentCommandArg == About && wordArg.startsWith(LongOption.arg(only_)) &&
        positionArg >= commandlineArgSize
    )
        return printGuessedSectionNamesForAbout(getOptionValue(wordArg));
#endif

    /* Print all or inferred database connection names for the --database= option
       --- */
    // Bash has it's own guess logic in the tom.bash complete file
#ifdef _MSC_VER
    if (wordArg.startsWith(LongOption.arg(database_)) &&
        currentCommandArg && commandHasLongOption(*currentCommandArg, database_) &&
        positionArg >= commandlineArgSize
    )
        return printGuessedConnectionNames(getOptionValue(wordArg));
#endif

    /* Print environment names for the --env= option
       --- */
    // Bash has it's own guess logic in the tom.bash complete file
#ifdef _MSC_VER
    if (wordArg.startsWith(LongOption.arg(Env)) &&
        currentCommandArg && // All commands has the --env= option
        positionArg >= commandlineArgSize
    )
        return printGuessedEnvironmentNames(getOptionValue(wordArg));
#endif

    /* Print all or guessed long option parameter names
       --- */
#ifdef _MSC_VER
    if (isLongOption(wordArg) && commandlineArg.contains(SPACE))
#else
    if (isLongOption(wordArg) && cwordArg >= 1)
#endif
        return printGuessedLongOptions(currentCommandArg, wordArg);

    /* Print all or guessed short option parameter names
       --- */
#ifdef _MSC_VER
    if (isShortOption(wordArg) && commandlineArg.contains(SPACE))
#else
    if (isShortOption(wordArg) && cwordArg >= 1)
#endif
        return printGuessedShortOptions(currentCommandArg);

    return EXIT_SUCCESS;
}

/* protected */

#ifdef _MSC_VER
std::optional<QString>
CompleteCommand::getCurrentTomCommand(const QStringList &commandlineArgSplitted)
{
    // It's not a command name
    if (commandlineArgSplitted.size() < 2 || isLongOption(commandlineArgSplitted[1]))
        return std::nullopt;

    return std::make_optional(commandlineArgSplitted[1]);
}
#else
std::optional<QString>
CompleteCommand::getCurrentTomCommand(const QString &commandlineArg,
                                      const QString::size_type cword)
{
    const auto commandlineArgSplitted = commandlineArg.trimmed().split(SPACE);
    const auto currentSplittedSize = commandlineArgSplitted.size();

    if (currentSplittedSize <= 1)
        return std::nullopt;

    for (auto i = 1; i < currentSplittedSize; ++i)
        if (const auto &currentCommand = commandlineArgSplitted[i];
            !isOptionArgument(currentCommand)
        ) {
            // Little weird, but the current tom command is actually completing now
            if (i == cword)
                return std::nullopt;

            return currentCommand;
        }

    return std::nullopt;
}
#endif

QString CompleteCommand::getWordOptionValue(
        const QStringList &commandlineArgSplitted,
        const QString::size_type positionArg, const QString::size_type commandlineArgSize)
{
    /* This method contains a special handling (alternative to getOptionValue() method)
       with workaround for the --word= pwsh option from the Register-ArgumentCompleter.
       It fixes cases when the option value on the command-line to complete contains
       array value like --only=version,| in this case, pwsh provides/fills
       the $wordToComplete in the Register-ArgumentCompleter with an empty string so
       there is no way how to correctly complete these values.
       This method workarounds/fixes this behavior and instead of an empty string provides
       the correct value you would expect like --only=version,| or --only=version,ma|
       which enables to complete the given partial array values. */

    auto wordArg = value(word_);

    /* Nothing to do, cursor is already after an option, eg: --only=env | or --only=env, |
       or somewhere before. */
    if (positionArg != commandlineArgSize)
        return wordArg;

    const auto &lastArg = commandlineArgSplitted.constLast();

    /* Reason for this is to ensures that our completion will work correctly if this
       will be by any chance fixed in future pwsh versions. 🙃
       I found out later that it's also true in cases like this:
       tom about --json| or tom about --only=macros|
       So when the last word is already completed and there is nothing to complete. */
    if (wordArg == lastArg)
        return wordArg;

    const auto isLongOption = CompleteCommand::isLongOption(lastArg);
    const auto isWordArgEmpty = wordArg.isEmpty();

        // Targets --only=macros,| and returns --only=macros,
    if ((isLongOption && isWordArgEmpty && lastArg.endsWith(COMMA_C)) ||
        // Targets --only=macros,versions,en| and returns --only=macros,versions,en
        isLongOptionWithArrayValue(lastArg)
    ) T_UNLIKELY
        return lastArg;

    else T_LIKELY
        return wordArg;
}

int CompleteCommand::printGuessedCommands(
        const std::vector<std::shared_ptr<Command>> &commands) const
{
    QStringList guessedCommands;
    guessedCommands.reserve(static_cast<decltype (guessedCommands)::size_type>(
                                commands.size()));

    for (const auto &command : commands) {
        auto commandName = command->name();

        if (m_dontList.contains(commandName))
            continue;

#ifdef _MSC_VER
        guessedCommands << QStringLiteral("%1;%2;%3").arg(commandName, commandName,
                                                          command->description());
#else
#  if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        guessedCommands << std::move(commandName);
#  else
        guessedCommands << commandName;
#  endif
#endif
    }

    note(guessedCommands.join(NEWLINE));

    return EXIT_SUCCESS;
}

int CompleteCommand::printGuessedNamespaces(const QString &word) const
{
    const auto &allNamespaceNames = Application::namespaceNames();

    const auto printAll = word.isEmpty();

    QStringList namespaceNames;
    namespaceNames.reserve(static_cast<decltype (namespaceNames)::size_type>(
                               allNamespaceNames.size()));

    for (const QString &namespaceName : allNamespaceNames)
        if (!namespaceName.isEmpty() &&
            (printAll || namespaceName.startsWith(word))
        )
            namespaceNames << namespaceName;

    // Want to have un-sorted if printing all namespaces
    if (!printAll)
        namespaceNames.sort(Qt::CaseInsensitive);

    note(namespaceNames.join(NEWLINE));

    return EXIT_SUCCESS;
}

// FUTURE complete, printGuessedNamespaces and printGuessedShells are practically the same methods, if I will implement another method of this simple list type, then create common method and reuse code silverqx
int CompleteCommand::printGuessedShells(const QString &word) const
{
    static const std::array allShellNames {
#ifndef _MSC_VER
        ShBash,
#endif
        ShPwsh,
#ifndef _MSC_VER
        ShZsh,
#endif
    };

    const auto printAll = word.isEmpty();

    QStringList shellNames;
    shellNames.reserve(static_cast<decltype (shellNames)::size_type>(
                           allShellNames.size()));

    for (const QString &shellName : allShellNames)
        if (!shellName.isEmpty() && (printAll || shellName.startsWith(word)))
            shellNames << shellName;

    // Want to have un-sorted if printing all namespaces
    if (!printAll)
        shellNames.sort(Qt::CaseInsensitive);

    note(shellNames.join(NEWLINE));

    return EXIT_SUCCESS;
}

#ifdef _MSC_VER
int
CompleteCommand::printGuessedSectionNamesForAbout(const QStringView sectionNamesArg) const
{
    static const QStringList allSectionNames {
        sl("environment"), sl("macros"), sl("versions"), sl("connections"),
    };

    // Initialize local variables
    auto [sectionNameArg,
          allSectionNamesFiltered,
          isFirstSectionNameArg,
          printAllSectionNames
    ] = initializePrintArrayOptionValues(sectionNamesArg, allSectionNames);

    /* Print only one space if all array option values have already been entered,
       it prevents printing basic files completion. */
    if (allSectionNamesFiltered.isEmpty()){
        note(sl(" ; "));
        return EXIT_SUCCESS;
    }

    QStringList sectionNames;
    sectionNames.reserve(allSectionNamesFiltered.size());

    /* It also evaluates to true if the given sectionNamesArg is an empty string "",
       it prints all section names in this case.
       isFirstSectionNameArg note:
       For the first section name the --only= has to be prepended because pwsh
       overwrites the whole option, for the next sections we don't have to, this is
       because for the following sections, the wordArg is empty, so pwsh doesn't overwrite
       the entire text of the --only= option, so we only need to print a section name. */
    for (const auto allSectionName : allSectionNamesFiltered)
        if (printAllSectionNames || allSectionName.startsWith(sectionNameArg))
            sectionNames << sl("%1;%2").arg(
                                isFirstSectionNameArg
                                ? NOSPACE.arg(LongOption.arg(only_).append(EQ_C),
                                              allSectionName)
                                : allSectionName,
                                allSectionName);

    // Print
    note(sectionNames.join(NEWLINE));

    return EXIT_SUCCESS;
}

int CompleteCommand::printGuessedConnectionNames(const QString &connectionNamesArg) const
{
    const auto allConnectionNames = getConnectionNamesFromFile();

    // Nothing to guess, no database connections where defined for the tom application
    if (allConnectionNames.isEmpty())
        return EXIT_SUCCESS;

    // Initialize local variables
    auto [connectionNameArg,
          allConnectionNamesFiltered,
          isFirstConnectionNameArg,
          printAllConnectionNames
    ] = initializePrintArrayOptionValues(connectionNamesArg, allConnectionNames);

    /* Print only one space if all array option values have already been entered,
       it prevents printing basic files completion. */
    if (allConnectionNamesFiltered.isEmpty()){
        note(sl(" ; "));
        return EXIT_SUCCESS;
    }

    QStringList connectionNames;
    connectionNames.reserve(allConnectionNamesFiltered.size());

    /* It also evaluates to true if the given connectionNamesArg is an empty string "",
       it prints all connection names in this case.
       isFirstConnectionNameArg note:
       For the first connection name the --database= has to be prepended because pwsh
       overwrites the whole option, for the next connections we don't have to, this is
       because for the following connections, the wordArg is empty, so pwsh doesn't
       overwrite the entire text of the --database= option, so we only need to print
       the connection name. */
    for (const auto allConnectionName : allConnectionNamesFiltered)
        if (printAllConnectionNames || allConnectionName.startsWith(connectionNameArg))
            connectionNames << sl("%1;%2").arg(
                                   isFirstConnectionNameArg
                                   ? NOSPACE.arg(LongOption.arg(database_).append(EQ_C),
                                                 allConnectionName)
                                   : allConnectionName,
                                   allConnectionName);

    // Print
    note(connectionNames.join(NEWLINE));

    return EXIT_SUCCESS;
}

int CompleteCommand::printGuessedEnvironmentNames(const QString &environmentName) const
{
    static const QStringList allEnvironmentNames {
        QStringLiteral("dev"),     QStringLiteral("development"), QStringLiteral("local"),
        QStringLiteral("prod"),    QStringLiteral("production"),  QStringLiteral("test"),
        QStringLiteral("testing"), QStringLiteral("staging")
    };

    QStringList environmentNames;
    environmentNames.reserve(allEnvironmentNames.size());

    for (const auto &environment : allEnvironmentNames)
        /* It also evaluates to true if the given environmentName is an empty string "",
           so it prints all environment names in this case.
           Also --env= has to be prepended because pwsh overwrites whole option. */
        if (environment.startsWith(environmentName))
            environmentNames
                    << QStringLiteral("%1;%2").arg(
                           NOSPACE.arg(LongOption.arg(Env).append(EQ_C), environment),
                           environment);

    // Print
    note(environmentNames.join(NEWLINE));

    return EXIT_SUCCESS;
}
#endif

int CompleteCommand::printGuessedLongOptions(
            const std::optional<QString> &currentCommand, const QString &word) const
{
    const auto commandOptions = getCommandOptionsSignature(currentCommand);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // The -- is guaranteed by the isLongOption(wordArg)
    const auto wordToGuess = word.sliced(2);
#else
    const auto wordToGuess = word.mid(2);
#endif
    const auto printAll = wordToGuess.isEmpty();

    QStringList options;
    options.reserve(commandOptions.size());

    for (const auto &option : commandOptions) {
        auto optionNames = option.names();

        // Some validation
        const auto optionNamesSize = optionNames.size();
        Q_ASSERT(optionNamesSize == 1 || optionNamesSize == 2);

        // Don't show hidden options
        if (option.hidden())
            continue;

        for (const auto &optionName : optionNames)
            if (optionName.size() > 1 &&
                (printAll || optionName.startsWith(wordToGuess))
            ) {
                // W/o a value
                if (const auto valueName = option.valueName();
                    valueName.isEmpty()
                ) {
                    auto longOption = LongOption.arg(optionName);

#ifdef _MSC_VER
                    options << QStringLiteral("%1;%2;%3").arg(longOption, longOption,
                                                              option.description());
#else
#  if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                    options << std::move(longOption);
#  else
                    options << longOption;
#  endif
#endif
                }
                // With a value
                else {
                    // Text to be used as the auto completion result
                    auto longOption = LongOption.arg(optionName).append(EQ_C);
#ifdef _MSC_VER
                    // Text to be displayed in a list
                    const auto longOptionList = LongOptionValue.arg(optionName,
                                                                    valueName);

                    options << QStringLiteral("%1;%2;%3%4")
                               .arg(longOption, longOptionList,
                                    option.description(),
                                    getOptionDefaultValue(option));
#else
#  if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                    options << std::move(longOption);
#  else
                    options << longOption;
#  endif
#endif
                }
            }
    }

    note(options.join(NEWLINE));

    return EXIT_SUCCESS;
}

int CompleteCommand::printGuessedShortOptions(
            const std::optional<QString> &currentCommand) const
{
    const auto commandOptions = getCommandOptionsSignature(currentCommand);

    QStringList options;
    options.reserve(commandOptions.size());

    for (const auto &option : commandOptions) {
        const auto optionNames = option.names();

        // Some validation
        Q_ASSERT(optionNames.size() == 1 || optionNames.size() == 2);

        // Don't show hidden options
        if (option.hidden())
            continue;

        for (const auto &optionName : optionNames)
            // Short option
            if (optionName.size() == 1) {
                // All other short options
                if (option.names().constFirst() != "v")
#ifdef _MSC_VER
                    options << QStringLiteral("-%1;-%2;%3").arg(optionName, optionName,
                                                                option.description());
#else
                    options << QStringLiteral("-%1").arg(optionName);
#endif
                // Special handling of the -v options, good enough 😎
                else {
                    /* Has to be v because pwsh parameter names are case-insensitive so
                       it collides with the -V parameter. */
#ifdef _MSC_VER
                    options << QStringLiteral("-v;v;%1").arg(option.description())
                            << QStringLiteral("-vv;-vv;%1").arg(option.description())
                            << QStringLiteral("-vvv;-vvv;%1").arg(option.description());
#else
                    options << QStringLiteral("-v")
                            << QStringLiteral("-vv")
                            << QStringLiteral("-vvv");
#endif
                }
            }
    }

    note(options.join(NEWLINE));

    return EXIT_SUCCESS;
}

QString CompleteCommand::getOptionDefaultValue(const QCommandLineOption &option)
{
    const auto defaultValues = option.defaultValues();

    // More default values is not supported
    Q_ASSERT(defaultValues.size() <= 1);

    return defaultValues.isEmpty()
            ? QString("")
            : TomUtils::defaultValueText(defaultValues.constFirst());
}

QList<CommandLineOption>
CompleteCommand::getCommandOptionsSignature(const std::optional<QString> &command) const
{
    if (!command)
        return application().m_options;

    return application().getCommandOptionsSignature(*command) + application().m_options;
}

bool CompleteCommand::isOptionArgument(const QString &wordArg, const OptionType type)
{
    const auto isLong = wordArg.startsWith(DoubleDash);
    const auto isShort = isLong ? false : wordArg.startsWith(DASH);

    if (type == UNDEFINED)
        return isLong || isShort;

    if (type == LONG)
        return isLong;
    if (type == SHORT)
        return isShort;

    Q_UNREACHABLE();
}

bool CompleteCommand::isLongOptionWithArrayValue(const QString &wordArg)
{
    // Nothing to check, not a long option
    if (!isLongOption(wordArg))
        return false;

    const auto wordArgSplitted = StringUtils::splitAtFirst(wordArg, EQ_C,
                                                           Qt::KeepEmptyParts);

    // Checks --only=macros, or --only=macros,en
    return wordArgSplitted.size() == 2 && wordArgSplitted.constLast().contains(COMMA_C);
}

QString CompleteCommand::getOptionValue(const QString &wordArg)
{
    Q_ASSERT(wordArg.contains(EQ_C));

    const auto wordArgSplitted = wordArg.split(EQ_C);

    Q_ASSERT(wordArgSplitted.size() <= 2);

    return wordArgSplitted.size() == 1 ? QString() : wordArgSplitted.last();
}

bool CompleteCommand::commandHasLongOption(const QString &command, const QString &option)
{
    /* Currently used for the --database= option only, so checking for hidden options
       doesn't make sense. */
    return ranges::contains(getCommandOptionsSignature(command), true,
                            [&option](const QCommandLineOption &optionItem)
    {
        Q_ASSERT(!optionItem.names().isEmpty());

        return optionItem.names().constFirst() == option;
    });
}

QStringList CompleteCommand::getConnectionNamesFromFile()
{
    // Are we in the tom project folder?
    if (!fs::exists("database/migrations") || !fs::exists("main.cpp"))
        return {};

    QStringList connectionNames;
    connectionNames.reserve(16);

    std::ifstream mainFileStream("main.cpp");

    if (mainFileStream.fail())
        return {};

    std::string line;
    line.reserve(256);

    static const QRegularExpression regex(R"T("(\w+)".*// shell:connection$)T");

    while (getline(mainFileStream, line)) {
        const auto match = regex.match(QString::fromStdString(line));

        if (match.hasMatch())
            connectionNames << match.captured(1);
    }

    mainFileStream.close();

    return connectionNames;
}

#ifdef _MSC_VER
CompleteCommand::PrintArrayOptionValuesType
CompleteCommand::initializePrintArrayOptionValues(const QStringView optionValuesArg,
                                                  const QStringList &allValues)
{
    // Nothing to do, option values are empty, return right away as we know the resut
    if (optionValuesArg.isEmpty())
        return {EMPTY, ranges::to<QList<QStringView>>(allValues), true, true};

    // Option values already displayed on the command-line (from getOptionValue())
    auto optionValuesArgSplitted = optionValuesArg.split(COMMA_C, Qt::KeepEmptyParts);
    // Needed for pwsh, determines an output format
    const auto isFirstOptionValue = optionValuesArgSplitted.size() == 1;
    /* The currently completing option value needs to be removed, so that this option
       value is not filtered out in the ranges::views::filter() algorithm below. */
    const auto lastOptionValueArg = optionValuesArgSplitted.takeLast();
    const auto printAllValues = lastOptionValueArg.isEmpty();

    // Remove all empty and null strings (it would print all option values w/o this)
    optionValuesArgSplitted.removeAll({});

    // Filter out option values that are already displayed on the command-line
    auto allValuesFiltered = allValues
            | ranges::views::filter([&optionValuesArgSplitted]
                                    (const QString &allValue)
    {
        // Include all option values that aren't already on the command-line
        return std::ranges::none_of(optionValuesArgSplitted,
                                    [&allValue](const QStringView optionValueArg)
        {
            return allValue.startsWith(optionValueArg);
        });
    })
            | ranges::to<QList<QStringView>>();

    return {lastOptionValueArg.toString(), std::move(allValuesFiltered),
            isFirstOptionValue, printAllValues};
}
#endif

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE
