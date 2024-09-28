#include "tom/commands/complete/pwshcommand.hpp"

#include <QRegularExpression>

#include <fstream>

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>

#include <orm/constants.hpp>
#include <orm/macros/likely.hpp>
#include <orm/utils/string.hpp>

#include "tom/application.hpp"
#include "tom/types/guesscommandnametype.hpp"

#ifndef TINYTOM_DEBUG
#  include "tom/exceptions/runtimeerror.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace fs = std::filesystem;

using Orm::Constants::COMMA_C;
using Orm::Constants::EQ_C;
using Orm::Constants::NEWLINE;
using Orm::Constants::NEWLINE_C;
using Orm::Constants::NOSPACE;
using Orm::Constants::SPACE;
using Orm::Constants::database_;

using StringUtils = Orm::Utils::String;

using Tom::Constants::About;
using Tom::Constants::DbSeed;
using Tom::Constants::EMPTY;
using Tom::Constants::Env;
using Tom::Constants::Help;
using Tom::Constants::Integrate;
using Tom::Constants::List;
using Tom::Constants::LongOption;
using Tom::Constants::LongOptionEq;
using Tom::Constants::TMPL_RESULT2;
using Tom::Constants::commandline;
using Tom::Constants::commandline_up;
using Tom::Constants::connection_;
using Tom::Constants::only_;
using Tom::Constants::position_;
using Tom::Constants::position_up;
using Tom::Constants::word_;
using Tom::Constants::word_up;

using enum Tom::GuessCommandNameResult;

namespace Tom::Commands::Complete
{

/* Complete command is inspired by winget/dotnet complete:
   https://github.com/microsoft/winget-cli/blob/master/doc/Completion.md */

/* public */

PwshCommand::PwshCommand(Application &application, QCommandLineParser &parser)
    : BaseCompleteCommand(application, parser)
{}

QList<CommandLineOption> PwshCommand::optionsSignature() const
{
    return {
        {word_,       u"The current word that is being completed"_s, word_up},
        {commandline, u"The entire current command-line"_s, commandline_up},
        {position_,   u"The current position of the cursor on the command-line"_s,
                      position_up, u"0"_s},
    };
}

int PwshCommand::run() // NOLINT(readability-function-cognitive-complexity)
{
    BaseCompleteCommand::run();

    /* Initialization section */
    const auto commandlineArg = value(commandline);
    const auto positionArg    = value(position_).toLongLong();

    // Common for both (Tom command and option)
    const auto commandlineArgSize     = commandlineArg.size();      // CUR1 finish silverqx
    const auto commandlineArgSplitted = commandlineArg.split(SPACE, Qt::SkipEmptyParts); // Already trimmed by pwsh
    Q_ASSERT(!commandlineArgSplitted.isEmpty());

    // Get an option value for the --word= option (with workaround for pwsh)
    const auto wordArg = getWordOptionValue(commandlineArgSplitted, positionArg,
                                            commandlineArgSize);

    // Currently processed Tom command
//    const auto isCommandLineEndPosition   = positionArg == commandlineArgSize;
    const auto isNewArgumentPositionAtEnd = positionArg > commandlineArgSize; // !isNewArgumentPositionAtEnd implies positionArg <= commandlineArgSize
    const auto argumentsCount             = getArgumentsCount(commandlineArgSplitted);
    const auto currentCommandArg          = getCurrentTomCommand(commandlineArgSplitted,
                                                                 argumentsCount);
    const auto currentArgumentPosition    = getCurrentArgumentPosition(
                                                {commandlineArg.constData(),
                                                 isNewArgumentPositionAtEnd
                                                 ? positionArg - 1 : positionArg},
                                                wordArg, isNewArgumentPositionAtEnd);
    m_hasAnyTomCommand = currentCommandArg != kNotFound; // kFound || kAmbiguous
    const auto maxArgumentsCount = getMaxArgumentsCount();

    /* Main logic section */

    // Print all commands after tom command itself or for the help command
    if (wordArg.isEmpty() &&
        ((currentArgumentPosition == TomCommandPosition && bw(argumentsCount, 1, 2)) || // tom | ; tom | a
         (currentCommandArg == Help && argumentsCount == 2 &&
                                       currentArgumentPosition == 2)) // tom help |
    )
        return printGuessedCommands(application().guessCommandsForComplete({}));

    // Print all guessed commands by the word argument after tom or for the help command
    if (!wordArg.isEmpty() && !isOptionArgument(wordArg) &&
        ((bw(argumentsCount, 2, maxArgumentsCount) &&       // tom db:se|           ; tom db:see|d           ; tom db:seed|
          currentArgumentPosition == TomCommandPosition) || // tom db:se| XyzSeeder ; tom db:see|d XyzSeeder ; tom db:seed| XyzSeeder
         (currentCommandArg == Help && argumentsCount == 3 &&
          (currentArgumentPosition == TomCommandPosition || // tom he| a   ; tom hel|p a ; tom help| a
           currentArgumentPosition == 2)))                  // tom help a| ; tom help ab|out
    )
        return printGuessedCommands(application().guessCommandsForComplete(wordArg));

    // Print all or guessed namespace names for the list command
    if (!isOptionArgument(wordArg) && currentCommandArg == List &&
        currentArgumentPosition == 2 &&
        ((argumentsCount == 2 && wordArg.isEmpty()) || // tom list |
         (argumentsCount == 3 && !wordArg.isEmpty()))  // tom list g| ; tom list gl|obal
    )
        return printGuessedNamespaces(wordArg);

    // Print all or guessed shell names for the integrate command
    if (!isOptionArgument(wordArg) && currentCommandArg == Integrate &&
        currentArgumentPosition == 2 &&
        ((argumentsCount == 2 && wordArg.isEmpty()) || // tom integrate |
         (argumentsCount == 3 && !wordArg.isEmpty()))  // tom integrate p| ; tom integrate p|wsh
    )
        return printGuessedShells(wordArg);

    // Print all or guessed section names for the about command --only= option
    if (currentCommandArg == About && isLongOptionName(wordArg, only_) &&
        argumentsCount == 2 && currentArgumentPosition == kOnOptionArgument && // tom about --only=m| ; tom about --only=m|acros
        !isNewArgumentPositionAtEnd // < : tom about --only=| --ansi ; = : tom about --only=|
    )
        return printGuessedSectionNamesForAbout(getOptionValue(wordArg));

    // Print all or inferred database connection names for the --database= option
    if (isLongOptionName(wordArg, database_) &&
        ((currentCommandArg == kFound && argumentsCount == 2) ||
         /* db:seed is the only command that has positional argument,
            all other commands with the --database= option don't have any. */
         (currentCommandArg == DbSeed && // tom db:seed Xyz --database=|
          argumentsCount == getMaxArgumentsCount(DbSeed))) &&
        currentArgumentPosition == kOnOptionArgument && // tom migrate --database=t| ; tom migrate --database=tiny|orm_tom_mysql
        !isNewArgumentPositionAtEnd && // < : tom migrate --database=| --ansi ; = : tom migrate --database=|
        currentCommandArg && commandHasLongOption(*currentCommandArg, database_) // All migrate/: and db: commands have the --database= option
    )
        return printGuessedConnectionNames(getOptionValue(wordArg));

    // Print environment names for the --env= option
    if (isLongOptionName(wordArg, Env) &&
        currentArgumentPosition == kOnOptionArgument && !isNewArgumentPositionAtEnd &&
        (argumentsCount == 1 || // tom --env=| ; tom --env=d| ; tom --env=d|ev (argumentsCount == 1 implies the kNotFound aka !currentCommandArg)
          // Don't print/complete the --env= option for unknown commands eg. tom xyz --|
         (m_hasAnyTomCommand && bw(argumentsCount, 2, maxArgumentsCount))) // kFound : tom migrate --env=| ; tom migrate --env=d| ; tom db:seed --env=d|ev Xyz
                                                                           // kAmbiguous : tom migrate:re --env=| (migrate:refresh or migrate:reset)
    )
        return printGuessedEnvironmentNames(getOptionValue(wordArg));

    /* Print long/short options always, the completer is context specific, meaning
       it prints options based on the current command, or prints common options if
       there is no command on the command-line. */

    // Print all or guessed long option parameter names
    if (isLongOption(wordArg) &&
        currentArgumentPosition == kOnOptionArgument && !isNewArgumentPositionAtEnd &&
        (argumentsCount == 1 || // tom --| ; tom --e| (argumentsCount == 1 implies the kNotFound aka !currentCommandArg)
          // Don't print/complete options for unknown commands eg. tom xyz --|
         (m_hasAnyTomCommand && bw(argumentsCount, 2, maxArgumentsCount))) // kFound : tom list --| ; tom list --r| ; tom list --r|aw
                                                                           // kAmbiguous : tom i --| ; tom i --a| (inspire or integrate)
    )
        return printGuessedLongOptions(currentCommandArg.commandName, wordArg);

    // Print all or guessed short option parameter names
    if (isShortOption(wordArg) &&
        currentArgumentPosition == kOnOptionArgument && !isNewArgumentPositionAtEnd &&
        (argumentsCount == 1 || // tom -| ; tom -e| (argumentsCount == 1 implies the kNotFound aka !currentCommandArg)
          // Don't print/complete options for unknown commands eg. tom xyz -|
         (m_hasAnyTomCommand && bw(argumentsCount, 2, maxArgumentsCount))) // kFound : tom list -| ; tom list -h|
                                                                           // kAmbiguous : tom i -| ; tom i -h| (inspire or integrate)
    )
        return printGuessedShortOptions(currentCommandArg.commandName);

    /* Block paths completion, pwsh Register-ArgumentCompleter will Trim()
       the output and returns $null if empty (preventing paths completion). */
    printCompletionResult(NEWLINE);

    return EXIT_SUCCESS;
}

/* private */

/* Current Tom command */

GuessCommandNameType
PwshCommand::getCurrentTomCommand(const QStringList &commandlineArgSplitted,
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
PwshCommand::getRawTomCommandName(const QStringList &commandlineArgSplitted)
{
    for (ArgumentsSizeType index = kUndefinedPosition;
         const auto &argument : commandlineArgSplitted
    ) {
        if (isOptionArgument(argument))
            continue;

        if (++index == TomCommandPosition)
            return argument;
    }

#ifndef TINYTOM_DEBUG
    throw Exceptions::RuntimeError(
                u"Unexpected return value, it can't be empty if argumentsCount > 1."_s);
#else
    // Guaranteed by the argumentsCount <= 1 above, there are always 2 commands
    Q_UNREACHABLE();
#endif
}

PwshCommand::ArgumentsSizeType
PwshCommand::getCurrentArgumentPosition(
        const QStringView commandlineArg, const QString &wordArg,
        const bool isNewArgumentPositionAtEnd)
{
    // Cursor is on the long/short option
    if (isOptionArgument(wordArg))
        return kOnOptionArgument;

    ArgumentsSizeType index = kUndefinedPosition;
                                                                      // CUR1 tom FINISH Xyz tom | --ansi ; tom help | --ansi
    for (const auto commandlineSplitted = commandlineArg.split(SPACE, Qt::SkipEmptyParts); // To avoid Clazy range-loop-detach; Already trimmed by pwsh
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
       to detect/specify that the cursor is at position 3. */
    if (isNewArgumentPositionAtEnd || wordArg.isEmpty()) // tom list | || tom list | --ansi
        ++index;

    return index;
}

PwshCommand::ArgumentsSizeType
PwshCommand::getMaxArgumentsCount()
{
    using CommandType = std::shared_ptr<Application::Command>;

    // +1 for tom.exe and +1 for any known/our or ambiguous Tom command
    return 1 + (m_hasAnyTomCommand ? 1 : 0) +
            static_cast<ArgumentsSizeType>(
                (*std::ranges::max_element(application().createCommandsVector(),
                                           std::less(), [](const CommandType &command)
    {
        return command->positionalArguments().size();
    }))
        ->positionalArguments().size());
}

PwshCommand::ArgumentsSizeType
PwshCommand::getMaxArgumentsCount(const QString &command)
{
    // +1 for tom.exe and +1 for any known/our or ambiguous Tom command
    return 1 + (m_hasAnyTomCommand ? 1 : 0) +
            static_cast<ArgumentsSizeType>(
                application().createCommand(command, std::nullopt, false)
                            ->positionalArguments().size());
}

/* Result output */

int PwshCommand::printGuessedSectionNamesForAbout(
        const QStringView sectionNamesArg) const
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
       it also prevents printing file/dir paths completion. */
    if (allSectionNamesFiltered.isEmpty()){
        printCompletionResult(u" ; "_s);
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
            sectionNames << TMPL_RESULT2.arg(
                                isFirstSectionNameArg
                                ? NOSPACE.arg(LongOption.arg(only_).append(EQ_C),
                                              allSectionName)
                                : allSectionName,
                                allSectionName);

    printCompletionResult(sectionNames);

    return EXIT_SUCCESS;
}

int PwshCommand::printGuessedConnectionNames(const QString &connectionNamesArg) const
{
    const auto allConnectionNames = getConnectionNamesFromFile();

    // Nothing to guess, no database connection tags where defined for Tom application
    if (allConnectionNames.isEmpty())
        return EXIT_SUCCESS;

    // Initialize local variables
    const auto [connectionNameArg,
                allConnectionNamesFiltered,
                isFirstConnectionNameArg,
                printAllConnectionNames
    ] = initializePrintArrayOptionValues(connectionNamesArg, allConnectionNames);

    /* Print only one space if all array option values have already been entered,
       it also prevents printing file/dir paths completion. */
    if (allConnectionNamesFiltered.isEmpty()){
        printCompletionResult(u" ; "_s);
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
            connectionNames << TMPL_RESULT2.arg(
                                   isFirstConnectionNameArg
                                   ? NOSPACE.arg(LongOption.arg(database_).append(EQ_C),
                                                 allConnectionName)
                                   : allConnectionName,
                                   allConnectionName);

    printCompletionResult(connectionNames);

    return EXIT_SUCCESS;
}

int PwshCommand::printGuessedEnvironmentNames(const QString &environmentNameArg) const
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
       Also, --env= has to be prepended because pwsh overwrites whole option. */
    for (const auto &environment : allEnvironmentNames)
        if (environment.startsWith(environmentNameArg))
            environmentNames
                    << TMPL_RESULT2.arg(
                           NOSPACE.arg(LongOption.arg(Env).append(EQ_C), environment),
                           environment);

    printCompletionResult(environmentNames);

    return EXIT_SUCCESS;
}

/* Printing support */

QChar PwshCommand::getResultDelimiter() const noexcept
{
    return NEWLINE_C;
}

void PwshCommand::appendShortVerboseOptions(QStringList &options,
                                            const QString &description) const
{
    options << u"-v;-v;%1"_s.arg(description)
            << u"-vv;-vv;%1"_s.arg(description)
            << u"-vvv;-vvv;%1"_s.arg(description);
}

/* Option arguments */

QString
PwshCommand::getWordOptionValue(
        const QStringList &commandlineArgSplitted, const QString::size_type positionArg,
        const ArgumentsSizeType commandlineArgSize) const
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
    if (positionArg > commandlineArgSize)
        return wordArg;

    const auto &lastArg = commandlineArgSplitted.constLast();

    /* Reason for this is to ensures that our completion will work correctly if this
       will be by any chance fixed in future pwsh versions. 🙃
       I found out later that it's also true in cases like this:
       tom about --json| or tom about --only=macros|
       So when the last word is already completed and there is nothing to complete. */
    if (wordArg == lastArg)
        return wordArg;

    const auto isLongOption = BaseCompleteCommand::isLongOption(lastArg);
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

PwshCommand::PrintArrayOptionValuesType
PwshCommand::initializePrintArrayOptionValues(const QStringView optionValuesArg,
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

bool PwshCommand::isLongOptionWithArrayValue(const QString &wordArg)
{
    // Nothing to check, not a long option
    if (!isLongOption(wordArg))
        return false;

    const auto wordArgSplitted = StringUtils::splitAtFirst(wordArg, EQ_C,
                                                           Qt::KeepEmptyParts);

    // Checks --only=macros, or --only=macros,en
    return wordArgSplitted.size() == 2 && wordArgSplitted.constLast().contains(COMMA_C);
}

QString PwshCommand::getOptionValue(const QString &wordArg)
{
    Q_ASSERT(wordArg.contains(EQ_C));

    const auto wordArgSplitted = wordArg.split(EQ_C);

    Q_ASSERT(wordArgSplitted.size() <= 2);

    return wordArgSplitted.size() == 1 ? QString() : wordArgSplitted.last();
}

bool PwshCommand::commandHasLongOption(const QString &command, const QString &option)
{
    /* Currently, used for the --database= option only, so checking for hidden options
       doesn't make sense. */
    return ranges::contains(getCommandOptionsSignature(command), true,
                            [&option](const QCommandLineOption &optionItem)
    {
        Q_ASSERT(!optionItem.names().isEmpty());

        return optionItem.names().constFirst() == option;
    });
}

QStringList PwshCommand::getConnectionNamesFromFile()
{
    // Are we in the Tom project folder?
    if (!fs::exists("database/migrations") || !fs::exists("main.cpp"))
        return {};

    QStringList connectionNames;
    connectionNames.reserve(16);

    std::ifstream mainFileStream("main.cpp");

    if (mainFileStream.fail())
        return {};

    // CUR1 test utf8 silverqx
    std::string line;
    line.reserve(256);

    static const QRegularExpression
    regex(uR"T("(?<connection>[\w\.-]+)".*// shell:connection$)T"_s);

    // No need to care about \r\n at the end
    while (getline(mainFileStream, line))

        if (const auto match = regex.match(QString::fromStdString(line));
            match.hasMatch()
        )
            connectionNames << match.captured(connection_);

    mainFileStream.close();

    return connectionNames;
}

} // namespace Tom::Commands::Complete

TINYORM_END_COMMON_NAMESPACE
