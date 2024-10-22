#include "tom/commands/complete/bashcommand.hpp"

#include <orm/constants.hpp>
#include <orm/utils/type.hpp>

#include "tom/application.hpp"
#include "tom/exceptions/invalidargumenterror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::SPACE;

using Tom::Constants::EMPTY;
using Tom::Constants::commandline;
using Tom::Constants::commandline_up;
using Tom::Constants::cargs_;
using Tom::Constants::cargs_up;
using Tom::Constants::word_;
using Tom::Constants::word_up;

using enum Tom::GuessedCommandNameResult;

namespace Tom::Commands::Complete
{

/* Complete command is inspired by winget/dotnet complete:
   https://github.com/microsoft/winget-cli/blob/master/doc/Completion.md */

/* public */

BashCommand::BashCommand(Application &application, QCommandLineParser &parser)
    : BaseCompleteCommand(application, parser)
{}

QList<CommandLineOption> BashCommand::optionsSignature() const
{
    return {
        {word_,       u"The current word that is being completed"_s, word_up}, // Value
        {commandline, u"The entire current command-line"_s, commandline_up}, // Value
        {cargs_,      u"The number of positional arguments before the current word "
                       "excluding options"_s, cargs_up}, // Value (can't have the default value as it's required)
    };
}

int BashCommand::run()
{
    BaseCompleteCommand::run();

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

    /* Bash has it's own guess logic in the tom.bash bash-completion file
       for the following cases:
       Print all or guessed section names for the about command --only= option
       Print all or inferred database connection names for the --database= option
       Print environment names for the --env= option
    */

    /* Print long/short options always, the completer is context specific, meaning
       it prints options based on the current command or prints common options if
       there is no command on the command-line. */

    // Print all or guessed long option parameter names
    if (completeLongOptions())
        return printGuessedLongOptions();

    // Print all or guessed short option parameter names
    if (completeShortOptions())
        return printGuessedShortOptions();

    /* Don't need to block directory paths completion on bash, the _filedir like function
       must be called explicitly to invoke directory paths completion, it's done
       based on the command name in the __tom_complete_dir function. */

    return EXIT_SUCCESS;
}

/* private */

/* Prepare Context */

CompleteContext BashCommand::initializeCompletionContext()
{
    // Values from the command-line
    // Both below must be defined as the data member to be available for QStringView-s!
    m_wordArg        = value(word_); // Trimmed in tom.bash
    m_commandlineArg = value(commandline).trimmed(); // Our logic is optimized for the trimmed command-line because pwsh depends on it
    m_cargsArg       = value(cargs_).toLongLong();

    // Validate the required option values (doesn't need m_commandlineArgSize)
    validateInputOptionValues();

    // Common for both (Tom command and options)
    m_commandlineArgSplit = QStringView(m_commandlineArg)
                            .split(SPACE, Qt::SkipEmptyParts); // CUR1 finish silverqx

    // Currently processed Tom command and positional arguments
    const auto argumentsCount    = getArgumentsCount();
          auto guessedTomCommand = getCurrentTomCommand(argumentsCount);
    const auto hasAnyTomCommand  = guessedTomCommand != kNotFound; // kFound || kAmbiguous

    return {
        .guessedTomCommand          = std::move(guessedTomCommand),
        .wordArg                    = std::move(m_wordArg),
        .argumentsCount             = argumentsCount,
        .currentArgumentPosition    = getCurrentArgumentPosition(),
        .maxArgumentsCount          = getMaxArgumentsCount(hasAnyTomCommand),
        .hasAnyTomCommand           = hasAnyTomCommand, // kFound || kAmbiguous
    };
}

/* Others */

void BashCommand::validateInputOptions() const
{
    // Validate the required common options for all complete:xyz commands (checks isSet())
    BaseCompleteCommand::validateInputOptions();

    // Bash specific
    constexpr static auto optionsToValidate = std::to_array<
                                              std::reference_wrapper<const QString>>({
        cargs_,
    });

    // TODO parser, add support for required positional arguments and options silverqx
    for (const auto &optionName : optionsToValidate)
        if (!isSet(optionName))
            throw Exceptions::InvalidArgumentError(
                    u"The --%1= option must be set for the complete:bash command "
                     "in %2()."_s
                    .arg(optionName, __tiny_func__));
}

void BashCommand::validateInputOptionValues() const
{
    // Validate the required common option values for all complete:xyz commands
    BaseCompleteCommand::validateInputOptionValues();

    // Bash specific
    // Nothing to do
    if (m_cargsArg > 0)
        return;

    throw Exceptions::InvalidArgumentError(
                u"The --cargs= option value must be >0 for complete:bash command "
                 "in %2()."_s
                .arg(__tiny_func__));
}

} // namespace Tom::Commands::Complete

TINYORM_END_COMMON_NAMESPACE
