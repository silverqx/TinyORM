#include "tom/commands/complete/pwshcommand.hpp"

#include <QRegularExpression>

#include <fstream>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>

#include <orm/constants.hpp>
#include <orm/utils/string.hpp>
#include <orm/utils/type.hpp>

#include "tom/application.hpp"
#include "tom/exceptions/invalidargumenterror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace fs = std::filesystem;

using Orm::Constants::NEWLINE;
using Orm::Constants::NOSPACE;
using Orm::Constants::SPACE;

using StringUtils = Orm::Utils::String;

using Tom::Constants::COMMA_C;
using Tom::Constants::EMPTY;
using Tom::Constants::EQ_C;
using Tom::Constants::Env;
using Tom::Constants::LongOptionEq;
using Tom::Constants::TMPL_RESULT2;
using Tom::Constants::commandline;
using Tom::Constants::commandline_up;
using Tom::Constants::connection_;
using Tom::Constants::database_;
using Tom::Constants::only_;
using Tom::Constants::position_;
using Tom::Constants::position_up;
using Tom::Constants::word_;
using Tom::Constants::word_up;

// Environment names for --env= option
using Tom::Constants::Local;
using Tom::Constants::Dev;
using Tom::Constants::Development;
using Tom::Constants::Prod;
using Tom::Constants::Production;
using Tom::Constants::Test;
using Tom::Constants::Testing;
using Tom::Constants::Staging;

using enum Tom::GuessedCommandNameResult;

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
        {word_,       u"The current word that is being completed"_s, word_up}, // Value
        {commandline, u"The entire current command-line"_s, commandline_up}, // Value
        {position_,   u"The current position of the cursor on the command-line"_s,
                      position_up}, // Value (can't have the default value as it's required)
    };
}

int PwshCommand::run()
{
    BaseCompleteCommand::run();

    /* Main logic section */

    // Print all commands after the tom command itself or for the help command
    // Print all guessed commands after tom by word option (context) or for help command
    if (completeAllCommands() || completeCommand())
        return printGuessedCommands();

    // Print all or guessed namespace names for the list command
    if (completeList_NamespacesArgument())
        return printGuessedNamespaces();

    // Print all or guessed shell names for the integrate command
    if (completeIntegrate_ShellsArgument())
        return printGuessedShells();

    // Print all or guessed section names for the --only= option of about command
    if (completeAbout_OnlyOption())
        return printGuessedSectionNamesForAbout();

    // Print all or inferred database connection names for the --database= option
    if (completeDatabaseOption())
        return printGuessedConnectionNames();

    // Print all or guessed environment names for the --env= option
    if (completeEnvOption())
        return printGuessedEnvironmentNames();

    // Print file/dir paths completion for the --path= option
    // if (completePathOption())
    //     return printCompletionResult(<Nothing works here>);

    /* Print long/short options always, the completer is context specific, meaning
       it prints options based on the current command, or prints common options if
       there is no command on the command-line. */

    // Print all or guessed long option parameter names
    if (completeLongOptions())
        return printGuessedLongOptions();

    // Print all or guessed short option parameter names
    if (completeShortOptions())
        return printGuessedShortOptions();

    /* Block file/dir paths completion, our pwsh Register-ArgumentCompleter will Trim()
       this output and returns $null if output is empty (preventing paths completion). */
    printCompletionResult(NEWLINE);

    return EXIT_SUCCESS;
}

/* private */

/* Prepare Context */

CompleteContext PwshCommand::initializeCompletionContext()
{
    // Values from the command-line
    // Both below must be defined as the data member to be available for QStringView-s!
    m_wordArg            = value(word_);
    m_commandlineArg     = value(commandline); // Already trimmed() by pwsh
    m_positionArg        = value(position_).toLongLong();
    m_commandlineArgSize = m_commandlineArg.size(); // pwsh only

    // Validate the required option values (needs m_commandlineArgSize)
    validateInputOptionValues();

    // Common for both (Tom command and option)
    m_commandlineArgSplit   = QStringView(m_commandlineArg) // Already trimmed by pwsh
                              .split(SPACE, Qt::SkipEmptyParts); // CUR1 finish silverqx
    m_isNewArgPositionAtEnd = m_positionArg > m_commandlineArgSize; // !isNewArgPositionAtEnd implies positionArg <= commandlineArgSize

    // Get an option value for the --word= option (with workaround for pwsh)
    const auto [wordArg, multiValueOptionPosition] = getWordArgOptionValue();

    // Currently processed Tom command
//    const auto isCommandLineEndPosition = positionArg == commandlineArgSize;
    const auto argumentsCount    = getArgumentsCount();
          auto guessedTomCommand = getCurrentTomCommand(argumentsCount);
    const auto hasAnyTomCommand  = guessedTomCommand != kNotFound; // kFound || kAmbiguous

    return {
        .guessedTomCommand        = std::move(guessedTomCommand),
        .wordArg                  = wordArg,
        .argumentsCount           = argumentsCount,
        .currentArgumentPosition  = getCurrentArgumentPosition(
                                        getCommadlineBeforeCursor(), wordArg),
        .maxArgumentsCount        = getMaxArgumentsCount(hasAnyTomCommand),
        .hasAnyTomCommand         = hasAnyTomCommand, // kFound || kAmbiguous
        .isNewArgPositionAtEnd    = m_isNewArgPositionAtEnd,
        .multiValueOptionPosition = multiValueOptionPosition,
    };
}

/* Context - Positional arguments */

SizeType
PwshCommand::getCurrentArgumentPosition(const QStringView commandlineArg,
                                        const QStringView wordArg) const
{
    // Cursor is on the long/short option
    if (isOptionArgument(wordArg))
        return kOnOptionArgument;

    SizeType index = kUndefinedPosition;
                                                                      // CUR1 tom FINISH Xyz tom | --ansi ; tom help | --ansi
    for (const auto commandlineArgSplit = commandlineArg.split(SPACE, Qt::SkipEmptyParts); // To avoid Clazy range-loop-detach; Already trimmed by pwsh
         const auto argument : commandlineArgSplit
    ) {
        if (isOptionArgument(argument))
            continue;

        ++index;
    }

    Q_ASSERT(index != kUndefinedPosition);

    /* pwsh truncates the commandlineArg in this case: tom list |,
       it truncates it like this: tom list|, so the index must be incremented manually
       to detect/specify that the cursor is at position 3.
       Also, this condition can be source of bugs because of this wordArg.isEmpty() check,
       there is a chance I didn't catch up all cases. See the blockCompletion()
       at the end of the getWordArgOptionValue() method for more info. */
    if (m_isNewArgPositionAtEnd || wordArg.isEmpty()) // tom list | || tom list | --ansi
        ++index;

    return index;
}

/* Context - Multi-value Option arguments */

MultiValueOptionType
PwshCommand::getWordArgOptionValue() const
{
    /* This method contains a special handling (alternative to getOptionValue() method)
       with workaround for the --word= pwsh option from the Register-ArgumentCompleter.
       It fixes cases when the option value to complete on the command-line contains
       multiple values like --only=version,| in this case, pwsh provides/fills
       the $wordToComplete in the Register-ArgumentCompleter with an empty string so
       there is no way how to correctly complete these values.
       This method workarounds/fixes this behavior and instead of returning an empty
       string returns the correct wordArg value you would expect, like --only=versions,|
       or --only=versions,ma|,env which enables to complete the given partial/ values. */

    /* Nothing to do, cursor is already after an option, eg: --only=env | or --only=env, |
       or somewhere after (pwsh trims the --commandline= option value). */
    if (m_isNewArgPositionAtEnd)
        return {m_wordArg};

    // Get the current word under the cursor (workaround for multi-value options)
    const auto [currentWord, currentWordPostion] = findCurrentWord();

    /* Reason for this is to ensures that our completion will work correctly if this
       will be by any chance fixed in future pwsh versions. 🙃
       I found out later that it's also true in cases like this:
       tom about --jso|n| or tom about --only=mac|ros|
       It virtually means that the current word under the cursor is correct and needs no
       further processing. The wordArg is incorrect only for multi-value options or
       if tab hit happened at the beginning of the current word like tom |about. */
    if (m_wordArg == currentWord)
        return {m_wordArg};

        // Targets --only=macros,|,versio|ns and returns --only=macros,,versions
    if ((m_wordArg.isEmpty() && isLongOption(currentWord) &&
         currentWord.endsWith(COMMA_C)) ||
        // Targets --only=macros,vers|,en| and returns --only=macros,vers,en
        isLongOptionWithArrayValue(currentWord)
    )
        return {currentWord, computeMultiValueOptionPosition(currentWord,
                                                             currentWordPostion)};

    /* tom |about or tom about |--json there may be other cases that I'm not aware of,
       but generally, it gets until here when the pwsh doesn't know what to do, and
       in those cases the value of the --word= option is usually empty. */
    blockCompletion(); // [[noreturn]]
}

MultiValueOptionType PwshCommand::findCurrentWord() const
{
    constexpr static SizeType Position0 = 0; // Needed by the std::max()

    const auto firstCharIdx  = m_commandlineArg.lastIndexOf(
                                   SPACE, std::max(Position0, m_positionArg - 1) // The lowest must always be 0
                               ) + 1; // +1 because it finds the first space position before, so +1 will be the first character after this space
    // The last character index must point after the last character for the QStringView
    const auto spaceAfterIdx = m_commandlineArg.indexOf(SPACE, m_positionArg);
    const auto lastCharIdx   = spaceAfterIdx == -1 ? m_commandlineArgSize : spaceAfterIdx;

    const auto *const commandlineArgData = m_commandlineArg.constData();

    return {
        {commandlineArgData + firstCharIdx, commandlineArgData + lastCharIdx}, // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        firstCharIdx,
    };
}

bool PwshCommand::isLongOptionWithArrayValue(const QStringView wordArg)
{
    // Nothing to check, not a long option
    if (!isLongOption(wordArg) || !wordArg.contains(EQ_C))
        return false;

    const auto wordArgSplit = StringUtils::splitAtFirst(wordArg, EQ_C,
                                                        Qt::KeepEmptyParts);
    Q_ASSERT(wordArgSplit.size() == 2);

    // Checks --only=macros, or --only=macros,en
    return wordArgSplit.constLast().contains(COMMA_C);
}

/* Result output */

int PwshCommand::printGuessedSectionNamesForAbout() const
{
    static const QStringList allSectionNames {
        u"environment"_s, u"macros"_s, u"versions"_s, u"connections"_s,
    };

    // Initialize local variables
    const auto [sectionNameArg,
                allSectionNamesFiltered,
                isFirstSectionNameArg,
                printAllSectionNames,
                appendComma,
                appendSpace
    ] = initializePrintMultiValueOption(allSectionNames);

    // Nothing to do
    if (allSectionNamesFiltered.isEmpty() && !printAllSectionNames)
        blockCompletion(); // [[noreturn]]

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
            sectionNames << prepareMultiValueResult(
                                only_, allSectionName, isFirstSectionNameArg,
                                appendComma, appendSpace);

    printCompletionResult(sectionNames);

    return EXIT_SUCCESS;
}

int PwshCommand::printGuessedConnectionNames() const
{
    const auto allConnectionNames = getConnectionNamesFromFile();

    // Nothing to guess, no database connection tags where defined for Tom application
    if (allConnectionNames.isEmpty())
        return EXIT_SUCCESS;

    // Initialize local variables
    const auto [connectionNameArg,
                allConnectionNamesFiltered,
                isFirstConnectionNameArg,
                printAllConnectionNames,
                appendComma,
                appendSpace
    ] = initializePrintMultiValueOption(allConnectionNames);

    // Nothing to do
    if (allConnectionNamesFiltered.isEmpty() && !printAllConnectionNames)
        blockCompletion(); // [[noreturn]]

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
            connectionNames << prepareMultiValueResult(
                                   database_, allConnectionName,
                                   isFirstConnectionNameArg, appendComma, appendSpace);

    printCompletionResult(connectionNames);

    return EXIT_SUCCESS;
}

int PwshCommand::printGuessedEnvironmentNames() const
{
    static const QStringList allEnvironmentNames {
        Dev,     Development, Local,
        Prod,    Production,
        Test,    Testing,
        Staging,
    };

    const auto environmentNameArg = getOptionValue(context().wordArg);

    QStringList environmentNames;
    environmentNames.reserve(allEnvironmentNames.size());

    /* It also evaluates to true if the given environmentNameArg is an empty string "",
       so it prints all environment names in this case.
       Also, --env= has to be prepended because pwsh overwrites whole option. */
    for (const auto &environment : allEnvironmentNames)
        if (environment.startsWith(environmentNameArg))
            environmentNames << TMPL_RESULT2.arg(
                                    NOSPACE.arg(LongOptionEq.arg(Env), environment),
                                    environment);

    printCompletionResult(environmentNames);

    return EXIT_SUCCESS;
}

/* Printing support */

/* These method exits immediately, the previous logic correctly saved the state,
   all the completion logic was invoked, and based on that state it blocked or printed
   a single space at the end of the run() method (as the last thing).
   But, I had to adapt all the logic (it had to handle these edge cases properly) and
   it also needed a lot more if() conditions, so I decided to quit immediately when
   possible, this is a worse way to do this though as it doesn't allow to invoke
   any post actions, but I don't have any now, so it's OK. 😁 */

void PwshCommand::blockCompletion() const
{
    printCompletionResult(EMPTY);

    Application::exitApplication(EXIT_SUCCESS);
}

void PwshCommand::appendShortVerboseOptions(QStringList &options,
                                            const QString &description) const
{
    options << u"-v;-v;%1"_s.arg(description)
            << u"-vv;-vv;%1"_s.arg(description)
            << u"-vvv;-vvv;%1"_s.arg(description);
}

/* Printing - Multi-value Option arguments */

PrintMultiValueOptionType
PwshCommand::initializePrintMultiValueOption(const QStringList &allValues) const
{
    /* This method is a real porn 😎, it covers so many cases, it took me days/weeks
       to debug it. */

    // Get the value of the option argument (eg. --only=macros)
    const auto optionValuesArg = getOptionValue(context().wordArg);

    /* Nothing to do, option value/s are empty like --only=|, complete/print all values
       for the first option value. */
    if (optionValuesArg.isEmpty())
        return {
            .optionValues       = ranges::to<QList<QStringView>>(allValues),
            .isFirstOptionValue = true,
            .printAllValues     = true,
        };

    // Get the value/s before the cursor of the multi-value option argument
    const auto optionValuesBeforeCursor = getOptionValuesBeforeCursor(optionValuesArg);
    // Get the current value index (0-based) among other values
    const auto currentOptionValueIndex = optionValuesBeforeCursor.count(COMMA_C);

    // For appendComma and Register-ArgumentCompleter (determines an output format)
    const auto isFirstOptionValue = currentOptionValueIndex == 0;
    // Option values that are already on the command-line (from getOptionValue())
    auto optionValuesArgSplit = optionValuesArg.split(COMMA_C, Qt::KeepEmptyParts);

    // For currentOptionValue and appendComma
    const auto optionValuesArgSize = optionValuesArg.size();
    // For appendComma and appendSpace (must be before optionValuesArgSplit.takeAt())
    const auto optionValuesArgSplitSize = optionValuesArgSplit.size();

    /* This is for one special case like --only=environment,|macros,versions, or
       --only=environment,|macros,,versions, (with an extra/double comma).
       In this case set the currentOptionValue to an empty string and don't take/remove
       the current word under the cursor (macros in our example), to prevent/avoid
       this current 'macros' word from being removed from the completion results,
       otherwise, it would be filtered out using the optionValuesArgSplit.takeAt() below.
       After the tab hit it prints:
       --only=environment,connections,¦macros,versions, (instead of)
       --only=environment,macros,¦macros,versions, */
    const auto printAllRemaining = m_wordArg.isEmpty() &&
                                   optionValuesBeforeCursor.endsWith(COMMA_C) &&
                                   optionValuesArgSize > optionValuesBeforeCursor.size();
    /* The currently completing option value needs to be removed, so that this option
       value is not filtered out in the filterOptionValues() method below. */
    const auto currentOptionValue = !printAllRemaining
                                    ? optionValuesArgSplit.takeAt(currentOptionValueIndex)
                                    : EMPTY;
    const auto printAllValues = currentOptionValue.isEmpty();

    // For appendComma and appendSpace
    const auto lastOptionValueIndex = optionValuesArgSplitSize - 1;
    const auto isLastOptionValue = !printAllRemaining && // This can't be !printAllValues
                                   currentOptionValueIndex == lastOptionValueIndex;

    // Determine whether to append a comma after the multi-value option value
    const auto appendComma = shouldAppendComma(
                                 optionValuesArgSplitSize,
                                 isCommaUnderCursor(optionValuesArg, optionValuesArgSize),
                                 isFirstOptionValue, isLastOptionValue);

    // Remove all empty and null strings (it would print all option values without this)
    optionValuesArgSplit.removeAll({});

    // Filter out option values that are already completed on the command-line
    const auto optionValues = filterOptionValues(allValues, optionValuesArgSplit);
    /* If the last option value on the command-line is being completed and there is only
       one value left to complete. */
    const auto appendSpace = isLastOptionValue && optionValues.size() == 1;

    return {
        .currentOptionValue = currentOptionValue,
        .optionValues       = optionValues,
        .isFirstOptionValue = isFirstOptionValue,
        .printAllValues     = printAllValues,
        // For middle values only (edge case)
        .appendComma        = appendComma,
        // For the last value only, if all values have already been completed (edge case)
        .appendSpace        = appendSpace,
    };
}

SizeType
PwshCommand::getOptionValuesLastPosition(const QStringView optionValuesArg) const
{
    const auto multiValueOptionPosition = context().multiValueOptionPosition;

    /* Tab hit happened inside the multi-value option value like
       --only=envi|ronment,macro| or --only=envi|,macr|os. */
    if (multiValueOptionPosition >= 0)
        return multiValueOptionPosition;

    /* Tab hit happened before the = character inside the option name itself, both cases
       below happen only for multi-value options. */

    // With multiple option values like -|-on|ly|=environment,versions
    if (const auto firstCommaPosition = optionValuesArg.indexOf(COMMA_C);
        firstCommaPosition != -1
    )
        return firstCommaPosition;

    // With one option value like -|-on|ly|=environment
    return optionValuesArg.size();
}

bool PwshCommand::isCommaUnderCursor(const QStringView optionValuesArg,
                                     const SizeType optionValuesArgSize) const
{
    // This method works only in the context (if called from) of the shouldAppendComma()

    const auto multiValueOptionPosition = context().multiValueOptionPosition;
    constexpr static SizeType Position1 = 1;

    /* This applies to cases where we know the result in advance:
       - the cursor is at the end of the command-line
       - the isFirstOptionValue == true (position is always kUndefinedPosition(-1)),
         in this case the result is irrelevant
       - minimum value is 0, in this case the result is also irrelevant because
         the true value for this method can only happen if the minimum value is 1,
         it looks like the following (both have minimum value 1):
       tom about --only=,|, (return true)
       tom about --only=,|macros (return false) */
    if (!between(multiValueOptionPosition, Position1, optionValuesArgSize - 1))
        return false;

    return optionValuesArg.at(multiValueOptionPosition) == COMMA_C;
}

QList<QStringView>
PwshCommand::filterOptionValues(const QStringList &allValues,
                                const QList<QStringView> &optionValues)
{
    // Filter out option values that are already completed on the command-line
    return allValues
            | ranges::views::filter([&optionValues]
                                    (const QString &allValue)
    {
        // Include all option values that aren't already on the command-line
        return std::ranges::none_of(optionValues,
                                    [&allValue](const QStringView optionValueArg)
        {
            return allValue.startsWith(optionValueArg);
        });
    })
            | ranges::to<QList<QStringView>>();;
}

QString
PwshCommand::prepareMultiValueResult(
        const QString &optionName, const QStringView value,
        const bool isFirstOptionValue, const bool appendComma, const bool appendSpace)
{
    // TMPL_RESULT2 format: completionText;listItemText

    /* The first option value must be in the following format for pwsh to work properly:
       --only=xyz */
    if (isFirstOptionValue)
        return TMPL_RESULT2.arg(NOSPACE.arg(LongOptionEq.arg(optionName), value), value);

    // Edge case if values in the middle are being completed
    if (appendComma)
        return TMPL_RESULT2.arg(NOSPACE.arg(value, COMMA_C), value);

    /* If the last option value on the command-line is being completed and there is only
       one value left to complete. */
    if (appendSpace)
        return TMPL_RESULT2.arg(NOSPACE.arg(value, SPACE), value);

    return TMPL_RESULT2.arg(/*completionText*/ value, /*listItemText*/ value);
}

QStringView PwshCommand::getOptionValue(const QStringView wordArg)
{
    Q_ASSERT(wordArg.contains(EQ_C));

    return StringUtils::splitAtFirst(wordArg, EQ_C, Qt::KeepEmptyParts).constLast();
}

QStringList PwshCommand::getConnectionNamesFromFile()
{
    // Are we in the Tom project folder?
    if (!fs::exists("database/migrations") || !fs::exists("main.cpp"))
        return {};

    QStringList connectionNames;
    connectionNames.reserve(16);

    std::ifstream mainFileStream("main.cpp");

    // Nothing to do, file opening failed
    if (mainFileStream.fail())
        return {};

    // CUR1 test utf8 silverqx
    std::string line;
    line.reserve(256);

    static const QRegularExpression
    regex(uR"T("(?<connection>[\w\.-]+)".*// shell:connection$)T"_s);

    // No need to worry about \r\n at the end
    while (getline(mainFileStream, line))

        if (const auto match = regex.match(QString::fromStdString(line));
            match.hasMatch()
        )
            connectionNames << match.captured(connection_);

    mainFileStream.close();

    return connectionNames;
}

/* Others */

void PwshCommand::validateInputOptions() const
{
    // Validate the required common options for all complete:xyz commands (checks isSet())
    BaseCompleteCommand::validateInputOptions();

    // Pwsh specific
    constexpr static auto optionsToValidate = std::to_array<
                                              std::reference_wrapper<const QString>>({
        position_,
    });

    // TODO parser, add support for required positional arguments and options silverqx
    for (const auto &optionName : optionsToValidate)
        if (!isSet(optionName))
            throw Exceptions::InvalidArgumentError(
                    u"The --%1= option must be set for the complete:pwsh command "
                     "in %2()."_s
                    .arg(optionName, __tiny_func__));
}

void PwshCommand::validateInputOptionValues() const
{
    // Validate the required common option values for all complete:xyz commands
    BaseCompleteCommand::validateInputOptionValues();

    // Pwsh specific
    // Get cursor position after the tom executable including the space (0-based)
    const auto minRequiredPosition = getCursorPositionAfterExecutable();

    // Nothing to do
    if (m_positionArg >= minRequiredPosition)
        return;

    throw Exceptions::InvalidArgumentError(
                u"The --position= option value must be >=%1 for complete:pwsh command "
                 "in %2()."_s
                .arg(minRequiredPosition).arg(__tiny_func__));
}

SizeType PwshCommand::getCursorPositionAfterExecutable() const
{
    const auto firstSpacePosition = m_commandlineArg.indexOf(SPACE);

    // +1 to include the space after the tom executable
    return (firstSpacePosition == -1 ? m_commandlineArgSize : firstSpacePosition) + 1;
}

} // namespace Tom::Commands::Complete

TINYORM_END_COMMON_NAMESPACE
