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
#include "tom/types/guesscommandnametype.hpp"

#ifndef TINYTOM_DEBUG
#  include "tom/exceptions/runtimeerror.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace fs = std::filesystem;

using Orm::Constants::COMMA_C;
using Orm::Constants::DASH;
using Orm::Constants::EQ_C;
using Orm::Constants::NEWLINE;
using Orm::Constants::SPACE;

#ifdef _MSC_VER
using Orm::Constants::NOSPACE;
using Orm::Constants::database_;
#endif

using StringUtils = Orm::Utils::String;

using Tom::Constants::DbSeed;
using Tom::Constants::DoubleDash;
using Tom::Constants::EMPTY;
using Tom::Constants::Help;
using Tom::Constants::Integrate;
using Tom::Constants::List;
using Tom::Constants::LongOption;
using Tom::Constants::ShPwsh;
using Tom::Constants::commandline;
using Tom::Constants::commandline_up;
using Tom::Constants::word_;
using Tom::Constants::word_up;

#ifdef _MSC_VER
using Tom::Constants::About;
using Tom::Constants::Env;
using Tom::Constants::LongOptionValue;
using Tom::Constants::only_;
using Tom::Constants::position_;
using Tom::Constants::position_up;
#else
using Tom::Constants::ShBash;
using Tom::Constants::ShZsh;
using Tom::Constants::cword_;
using Tom::Constants::cword_up;
#endif

using TomUtils = Tom::Utils;

using enum Tom::GuessCommandNameResult;

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
        {word_,       u"The current word that is being completed"_s, word_up},
        {commandline, u"The entire current command-line"_s, commandline_up},
#ifdef _MSC_VER
        {position_,   u"The current position of the cursor on the command-line"_s,
                      position_up, u"0"_s},
#else
        {cword_,      u"Position of the current word on the command-line that is "
                       "being completed"_s, cword_up},
#endif
    };
}

int CompleteCommand::run() // NOLINT(readability-function-cognitive-complexity)
{
    Command::run();

    /* Initialization section */
    const auto commandlineArg = value(commandline);

#ifdef _MSC_VER
    const auto positionArg = value(position_).toLongLong();

    // Common for both (command and option)
    const auto commandlineArgSize     = commandlineArg.size();      // xyz
    const auto commandlineArgSplitted = commandlineArg.split(SPACE, Qt::SkipEmptyParts);
    Q_ASSERT(!commandlineArgSplitted.isEmpty());

    // Get the command-line option value for --xyz= option (with workaround for pwsh)
    const auto wordArg = getWordOptionValue(commandlineArgSplitted, positionArg,
                                            commandlineArgSize);

    // Currently processed Tom command
//    const auto isCommandLineEndPosition   = positionArg == commandlineArgSize;
    const auto isNewArgumentPositionAtEnd = positionArg > commandlineArgSize; // !isNewArgumentPositionAtEnd implies positionArg <= commandlineArgSize
    const auto argumentsCount             = getArgumentsCount(commandlineArgSplitted);
    const auto currentCommandArg          = getCurrentTomCommand(commandlineArgSplitted,
                                                                 argumentsCount);
    const auto currentArgumentPosition    = getCurrentArgumentPosition(
                                                {commandlineArg.constBegin(),
                                                 isNewArgumentPositionAtEnd
                                                 ? positionArg - 1 : positionArg},
                                                wordArg, isNewArgumentPositionAtEnd);
    m_hasAnyTomCommand = currentCommandArg != kNotFound; // kFound || kAmbiguous
    const auto maxArgumentsCount = getPositionalArgumentsCount();
#else
    const auto wordArg  = value(word_);
    const auto cwordArg = value(cword_).toLongLong();

    // Currently processed Tom command
    const auto currentCommandArg = getCurrentTomCommand(commandlineArg, cwordArg);
#endif

    /* Main logic section */

    /* Print all commands after tom command itself or for the help command
       --- */
#ifdef _MSC_VER
    if (wordArg.isEmpty() &&
        ((currentArgumentPosition == TomCommandPosition && bw(argumentsCount, 1, 2)) || // tom | ; tom | a
         (currentCommandArg == Help && argumentsCount == 2 &&
                                       currentArgumentPosition == 2)) // tom help |
    )
#else
    if (wordArg.isEmpty() && (!currentCommandArg || currentCommandArg == Help))
#endif
        return printGuessedCommands(application().guessCommandsForComplete({}));

    /* Print all guessed commands by the word argument after tom or for the help command
       --- */
#ifdef _MSC_VER
    if (!wordArg.isEmpty() && !isOptionArgument(wordArg) &&
        ((bw(argumentsCount, 2, maxArgumentsCount) &&       // tom db:se|           ; tom db:see|d           ; tom db:seed|
          currentArgumentPosition == TomCommandPosition) || // tom db:se| XyzSeeder ; tom db:see|d XyzSeeder ; tom db:seed| XyzSeeder
         (currentCommandArg == Help && argumentsCount == 3 &&
          (currentArgumentPosition == TomCommandPosition || // tom he| a   ; tom hel|p a ; tom help| a
           currentArgumentPosition == 2)))                  // tom help a| ; tom help ab|out
    )
#else
    if (!isOptionArgument(wordArg) && !wordArg.isEmpty() &&
        (!currentCommandArg || currentCommandArg == Help)
    )
#endif
        return printGuessedCommands(application().guessCommandsForComplete(wordArg));

    /* Print all or guessed namespace names for the list command
       --- */
#ifdef _MSC_VER
    if (!isOptionArgument(wordArg) && currentCommandArg == List &&
        currentArgumentPosition == 2 &&
        ((argumentsCount == 2 && wordArg.isEmpty()) || // tom list |
         (argumentsCount == 3 && !wordArg.isEmpty()))  // tom list g| ; tom list gl|obal
    )
#else
    if (!isOptionArgument(wordArg) && currentCommandArg == List)
#endif
        return printGuessedNamespaces(wordArg);

    /* Print all or guessed shell names for the integrate command
       --- */
#ifdef _MSC_VER
    if (!isOptionArgument(wordArg) && currentCommandArg == Integrate &&
        currentArgumentPosition == 2 &&
        ((argumentsCount == 2 && wordArg.isEmpty()) || // tom integrate |
         (argumentsCount == 3 && !wordArg.isEmpty()))  // tom integrate p| ; tom integrate p|wsh
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
        argumentsCount == 2 && currentArgumentPosition == kOnOptionArgument && // tom about --only=m| ; tom about --only=m|acros
        !isNewArgumentPositionAtEnd // < : tom about --only=| --ansi ; = : tom about --only=|
    )
        return printGuessedSectionNamesForAbout(getOptionValue(wordArg));
#endif

    /* Print all or inferred database connection names for the --database= option
       --- */
    // Bash has it's own guess logic in the tom.bash complete file
#ifdef _MSC_VER
    if (wordArg.startsWith(LongOption.arg(database_)) &&
        ((currentCommandArg == kFound && argumentsCount == 2) ||
         /* db:seed is the only command that has positional argument,
            all other commands with the --database= option don't have any. */
         (currentCommandArg == DbSeed && // tom db:seed Xyz --database=|
          argumentsCount == getPositionalArgumentsCount(DbSeed))) &&
        currentArgumentPosition == kOnOptionArgument && // tom migrate --database=t| ; tom migrate --database=tiny|orm_tom_mysql
        !isNewArgumentPositionAtEnd && // < : tom migrate --database=| --ansi ; = : tom migrate --database=|
        currentCommandArg && commandHasLongOption(*currentCommandArg, database_) // All migrate/: and db: commands have the --database= option
    )
        return printGuessedConnectionNames(getOptionValue(wordArg));
#endif

    /* Print environment names for the --env= option
       --- */
    // Bash has it's own guess logic in the tom.bash complete file
#ifdef _MSC_VER
    if (wordArg.startsWith(LongOption.arg(Env)) &&
        currentArgumentPosition == kOnOptionArgument && !isNewArgumentPositionAtEnd &&
        (argumentsCount == 1 || // tom --env=| ; tom --env=d| ; tom --env=d|ev (argumentsCount == 1 implies the kNotFound aka !currentCommandArg)
          // Don't print/complete the --env= option for unknown commands eg. tom xyz --|
         (m_hasAnyTomCommand && bw(argumentsCount, 2, maxArgumentsCount))) // kFound : tom migrate --env=| ; tom migrate --env=d| ; tom db:seed --env=d|ev Xyz
                                                                           // kAmbiguous : tom migrate:re --env=| (migrate:refresh or migrate:reset)
    )
        return printGuessedEnvironmentNames(getOptionValue(wordArg));
#endif

    /* Print long/short options always, the completer is context specific, meaning
       it prints options based on the current command, or prints common options if
       there is no command on the command-line. */

    /* Print all or guessed long option parameter names
       --- */
#ifdef _MSC_VER
    if (isLongOption(wordArg) &&
        currentArgumentPosition == kOnOptionArgument && !isNewArgumentPositionAtEnd &&
        (argumentsCount == 1 || // tom --| ; tom --e| (argumentsCount == 1 implies the kNotFound aka !currentCommandArg)
          // Don't print/complete options for unknown commands eg. tom xyz --|
         (m_hasAnyTomCommand && bw(argumentsCount, 2, maxArgumentsCount))) // kFound : tom list --| ; tom list --r| ; tom list --r|aw
                                                                           // kAmbiguous : tom i --| ; tom i --a| (inspire or integrate)
    )
        return printGuessedLongOptions(currentCommandArg.commandName, wordArg);
#else
    if (isLongOption(wordArg) && cwordArg >= 1)
        return printGuessedLongOptions(currentCommandArg, wordArg);
#endif

    /* Print all or guessed short option parameter names
       --- */
#ifdef _MSC_VER
    if (isShortOption(wordArg) &&
        currentArgumentPosition == kOnOptionArgument && !isNewArgumentPositionAtEnd &&
        (argumentsCount == 1 || // tom -| ; tom -e| (argumentsCount == 1 implies the kNotFound aka !currentCommandArg)
          // Don't print/complete options for unknown commands eg. tom xyz -|
         (m_hasAnyTomCommand && bw(argumentsCount, 2, maxArgumentsCount))) // kFound : tom list -| ; tom list -h|
                                                                           // kAmbiguous : tom i -| ; tom i -h| (inspire or integrate)
    )
        return printGuessedShortOptions(currentCommandArg.commandName);
#else
    if (isShortOption(wordArg) && cwordArg >= 1)
        return printGuessedShortOptions(currentCommandArg);
#endif

    /* Block paths completion, pwsh Register-ArgumentCompleter will Trim()
       the output and returns $null if empty (preventing paths completion). */
    note(EMPTY, false);

    return EXIT_SUCCESS;
}

/* protected */

#ifdef _MSC_VER
GuessCommandNameType
CompleteCommand::getCurrentTomCommand(const QStringList &commandlineArgSplitted,
                                      const ArgumentsSizeType argumentsCount) const
{
    // No Tom command on the command-line (only the tom.exe executable name)
    if (argumentsCount <= 1)
        return {kNotFound, std::nullopt};

    // Try to guess one Tom command name (detects kFound, kNotFound, kAmbiguous)
    return application().guessCommandNameForComplete(
                             getRawTomCommandName(commandlineArgSplitted));
}

QString
CompleteCommand::getRawTomCommandName(const QStringList &commandlineArgSplitted)
{
    for (ArgumentsSizeType index = kUndefinedPosition;
         const auto &argument : commandlineArgSplitted
    ) {
        if (isOptionArgument(argument))
            continue;

        if (++index == TomCommandPosition)
            return argument;
    }

    Q_UNREACHABLE();
    return {};
}

Command::ArgumentsSizeType
CompleteCommand::getCurrentArgumentPosition(
        const QStringView commandlineArg, const QString &wordArg,
        const bool isNewArgumentPositionAtEnd)
{
    // Cursor is on the long/short option
    if (isOptionArgument(wordArg))
        return kOnOptionArgument;

    ArgumentsSizeType index = kUndefinedPosition;
                                                                      // CUR1 tom FINISH Xyz tom | --ansi ; tom help | --ansi
    for (const auto commandlineSplitted = commandlineArg.split(SPACE, Qt::SkipEmptyParts); // To avoid Clazy range-loop-detach
         const auto argument : commandlineSplitted
    ) {
        // CUR1 tom FINISH QStringView silverqx
        if (isOptionArgument(argument.toString()))
            continue;

        ++index;
    }

    Q_ASSERT(index != kUndefinedPosition);

    /* pwsh truncates the commandlineArg in this case: tom list |,
       it truncates it like this: tom list|, so the index must be incremented manually
       to detect that the cursor is at position 3. */
    if (isNewArgumentPositionAtEnd || wordArg.isEmpty())
        ++index;

    return index;
}

CompleteCommand::ArgumentsSizeType
CompleteCommand::getPositionalArgumentsCount()
{
    // +1 for tom.exe and +1 for any known/our or ambiguous Tom command
    return 1 + (m_hasAnyTomCommand ? 1 : 0) +
            static_cast<ArgumentsSizeType>(
                (*std::ranges::max_element(application().createCommandsVector(),
                                           std::less(), [](const auto &command)
    {
        return command->positionalArguments().size();
    }))
        ->positionalArguments().size());
}

CompleteCommand::ArgumentsSizeType
CompleteCommand::getPositionalArgumentsCount(const QString &command)
{
    // +1 for tom.exe and +1 for any known/our or ambiguous Tom command
    return 1 + (m_hasAnyTomCommand ? 1 : 0) +
            static_cast<ArgumentsSizeType>(
                application().createCommand(command, std::nullopt, false)
                            ->positionalArguments().size());
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
    // Nothing to print
    if (commands.empty())
        return EXIT_SUCCESS;

    QStringList guessedCommands;
    guessedCommands.reserve(static_cast<decltype (guessedCommands)::size_type>(
                                commands.size()));

    for (const auto &command : commands) {
        auto commandName = command->name();

        if (m_dontList.contains(commandName))
            continue;

#ifdef _MSC_VER
        guessedCommands << u"%1;%2;%3"_s.arg(commandName, commandName,
                                             command->description());
#else
        guessedCommands << std::move(commandName);
#endif
    }

    note(guessedCommands.join(NEWLINE), false);

    return EXIT_SUCCESS;
}

int CompleteCommand::printGuessedNamespaces(const QString &wordArg) const
{
    const auto &allNamespaceNames = Application::namespaceNames();

    // Nothing to print
    if (allNamespaceNames.empty())
        return EXIT_SUCCESS;

    const auto printAll = wordArg.isEmpty();

    QStringList namespaceNames;
    namespaceNames.reserve(static_cast<decltype (namespaceNames)::size_type>(
                               allNamespaceNames.size()));

    for (const QString &namespaceName : allNamespaceNames)
        if (!namespaceName.isEmpty() &&
            (printAll || namespaceName.startsWith(wordArg))
        )
            namespaceNames << namespaceName;

    // Want to have unsorted if printing all namespaces
    if (!printAll)
        namespaceNames.sort(Qt::CaseInsensitive);

    note(namespaceNames.join(NEWLINE), false);

    return EXIT_SUCCESS;
}

// FUTURE complete, printGuessedNamespaces and printGuessedShells are practically the same methods, if I will implement another method of this simple list type, then create common method and reuse code silverqx
int CompleteCommand::printGuessedShells(const QString &wordArg) const
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

    const auto printAll = wordArg.isEmpty();

    QStringList shellNames;
    shellNames.reserve(static_cast<decltype (shellNames)::size_type>(
                           allShellNames.size()));

    for (const QString &shellName : allShellNames)
        if (!shellName.isEmpty() && (printAll || shellName.startsWith(wordArg)))
            shellNames << shellName;

    // Want to have unsorted if printing all namespaces
    if (!printAll)
        shellNames.sort(Qt::CaseInsensitive);

    note(shellNames.join(NEWLINE), false);

    return EXIT_SUCCESS;
}

#ifdef _MSC_VER
int
CompleteCommand::printGuessedSectionNamesForAbout(const QStringView sectionNamesArg) const
{
    static const QStringList allSectionNames {
        u"environment"_s, u"macros"_s, u"versions"_s, u"connections"_s,
    };

    // Initialize local variables
    const auto [sectionNameArg,
                allSectionNamesFiltered,
                isFirstSectionNameArg,
                printAllSectionNames
    ] = initializePrintArrayOptionValues(sectionNamesArg, allSectionNames);

    /* Print only one space if all array option values have already been entered,
       it prevents printing basic files completion. */
    if (allSectionNamesFiltered.isEmpty()){
        note(u" ; "_s, false);
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
            sectionNames << u"%1;%2"_s.arg(
                                isFirstSectionNameArg
                                ? NOSPACE.arg(LongOption.arg(only_).append(EQ_C),
                                              allSectionName)
                                : allSectionName,
                                allSectionName);

    // Print
    note(sectionNames.join(NEWLINE), false);

    return EXIT_SUCCESS;
}

int CompleteCommand::printGuessedConnectionNames(const QString &connectionNamesArg) const
{
    const auto allConnectionNames = getConnectionNamesFromFile();

    // Nothing to guess, no database connections where defined for the tom application
    if (allConnectionNames.isEmpty())
        return EXIT_SUCCESS;

    // Initialize local variables
    const auto [connectionNameArg,
                allConnectionNamesFiltered,
                isFirstConnectionNameArg,
                printAllConnectionNames
    ] = initializePrintArrayOptionValues(connectionNamesArg, allConnectionNames);

    /* Print only one space if all array option values have already been entered,
       it prevents printing basic files completion. */
    if (allConnectionNamesFiltered.isEmpty()){
        note(u" ; "_s, false);
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
            connectionNames << u"%1;%2"_s.arg(
                                   isFirstConnectionNameArg
                                   ? NOSPACE.arg(LongOption.arg(database_).append(EQ_C),
                                                 allConnectionName)
                                   : allConnectionName,
                                   allConnectionName);

    // Print
    note(connectionNames.join(NEWLINE), false);

    return EXIT_SUCCESS;
}

int CompleteCommand::printGuessedEnvironmentNames(const QString &environmentNameArg) const
{
    static const QStringList allEnvironmentNames {
        u"dev"_s,     u"development"_s, u"local"_s,
        u"prod"_s,    u"production"_s,
        u"test"_s,    u"testing"_s,
        u"staging"_s,
    };

    QStringList environmentNames;
    environmentNames.reserve(allEnvironmentNames.size());

    /* It also evaluates to true if the given environmentNameArg is an empty string "",
       so it prints all environment names in this case.
       Also --env= has to be prepended because pwsh overwrites whole option. */
    for (const auto &environment : allEnvironmentNames)
        if (environment.startsWith(environmentNameArg))
            environmentNames
                    << u"%1;%2"_s.arg(
                           NOSPACE.arg(LongOption.arg(Env).append(EQ_C), environment),
                           environment);

    // Print
    note(environmentNames.join(NEWLINE), false);

    return EXIT_SUCCESS;
}
#endif

int CompleteCommand::printGuessedLongOptions(
        const std::optional<QString> &currentCommand, const QString &wordArg) const
{
    const auto commandOptions = getCommandOptionsSignature(currentCommand);

    // The -- is guaranteed by the isLongOption(wordArg)
    const auto wordToGuess = wordArg.sliced(2);
    const auto printAll = wordToGuess.isEmpty();

    QStringList options;
    options.reserve(commandOptions.size());

    for (const auto &option : commandOptions) {
        auto optionNames = option.names();

        // Some validation
        Q_ASSERT(optionNames.size() == 1 || optionNames.size() == 2);

        // Don't show hidden options
        if (option.hidden())
            continue;

        for (const auto &optionName : std::as_const(optionNames))
            if (optionName.size() > 1 &&
                (printAll || optionName.startsWith(wordToGuess))
            ) {
                // W/o a value
                if (const auto valueName = option.valueName();
                    valueName.isEmpty()
                ) {
                    auto longOption = LongOption.arg(optionName);

#ifdef _MSC_VER
                    options << u"%1;%2;%3"_s.arg(longOption, longOption,
                                                 option.description());
#else
                    options << std::move(longOption);
#endif
                }
                // With a value
                else {
                    // Text to be used as the auto completion result
                    auto longOption = LongOption.arg(optionName).append(EQ_C);
#ifdef _MSC_VER
                    // Text to be displayed in the list
                    const auto longOptionList = LongOptionValue.arg(optionName,
                                                                    valueName);

                    options << u"%1;%2;%3%4"_s
                               .arg(longOption, longOptionList, option.description(),
                                    getOptionDefaultValue(option));
#else
                    options << std::move(longOption);
#endif
                }
            }
    }

    note(options.join(NEWLINE), false);

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
                    options << u"-%1;-%2;%3"_s.arg(optionName, optionName,
                                                   option.description());
#else
                    options << u"-%1"_s.arg(optionName);
#endif
                // Special handling of the -v options, good enough 😎
                else {
#ifdef _MSC_VER
                    options << u"-v;-v;%1"_s.arg(option.description())
                            << u"-vv;-vv;%1"_s.arg(option.description())
                            << u"-vvv;-vvv;%1"_s.arg(option.description());
#else
                    options << u"-v"_s
                            << u"-vv"_s
                            << u"-vvv"_s;
#endif
                }
            }
    }

    note(options.join(NEWLINE), false);

    return EXIT_SUCCESS;
}

QString CompleteCommand::getOptionDefaultValue(const QCommandLineOption &option)
{
    const auto defaultValues = option.defaultValues();

    // More default values is not supported
    Q_ASSERT(defaultValues.size() <= 1);

    return defaultValues.isEmpty()
            ? EMPTY
            : TomUtils::defaultValueText(defaultValues.constFirst());
}

QList<CommandLineOption>
CompleteCommand::getCommandOptionsSignature(const std::optional<QString> &command) const
{
    // Nothing to do, no command passed so print the common options
    if (!command)
        return application().m_options;

    return application().getCommandOptionsSignature(*command) + application().m_options;
}

bool CompleteCommand::isOptionArgument(const QString &wordArg, const OptionType type)
{
    const auto isLong = wordArg.startsWith(DoubleDash);
    const auto isShort = isLong ? false : wordArg.startsWith(DASH);

    // Consider both long and short option arguments
    if (type == ANY)
        return isLong || isShort;

    if (type == LONG)
        return isLong;
    if (type == SHORT)
        return isShort;

#ifndef TINYTOM_DEBUG
    throw Exceptions::RuntimeError(u"Unexpected value for enum struct OptionType."_s);
#else
    Q_UNREACHABLE();
#endif
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

    static const QRegularExpression
    regex(uR"T("(?<connection>[\w\.-]+)".*// shell:connection$)T"_s);

    // No need to care about \r\n at the end
    while (getline(mainFileStream, line))

        if (const auto match = regex.match(QString::fromStdString(line));
            match.hasMatch()
        )
            connectionNames << match.captured(u"connection"_s);

    mainFileStream.close();

    return connectionNames;
}

#ifdef _MSC_VER
CompleteCommand::PrintArrayOptionValuesType
CompleteCommand::initializePrintArrayOptionValues(const QStringView optionValuesArg,
                                                  const QStringList &allValues)
{
    // Nothing to do, option values are empty, return right away as we know the result
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
