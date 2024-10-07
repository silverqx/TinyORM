#include "tom/commands/complete/bashcommand.hpp"

#include <orm/constants.hpp>

#include "tom/application.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::SPACE;

using Tom::Constants::EMPTY;
using Tom::Constants::Help;
using Tom::Constants::Integrate;
using Tom::Constants::List;
using Tom::Constants::commandline;
using Tom::Constants::commandline_up;
using Tom::Constants::cword_;
using Tom::Constants::cword_up;
using Tom::Constants::word_;
using Tom::Constants::word_up;

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
        {cword_,      u"Position of the current word on the command-line that is being "
                       "completed"_s, cword_up, u"0"_s}, // Value
    };
}

int BashCommand::run()
{
    BaseCompleteCommand::run();

    /* Initialization section */
    const auto wordArg        = value(word_);
    const auto commandlineArg = value(commandline);
    const auto cwordArg       = value(cword_).toLongLong();

    // Currently processed Tom command
    const auto currentCommandArg = getCurrentTomCommand(commandlineArg, cwordArg);

    /* Main logic section */

    // Print all commands after tom command itself or for the help command
    if (wordArg.isEmpty() && (!currentCommandArg || currentCommandArg == Help))
        return printGuessedCommands(application().guessCommandsForComplete({}));

    // Print all guessed commands by the word argument after tom or for the help command
    if (!isOptionArgument(wordArg) && !wordArg.isEmpty() &&
        (!currentCommandArg || currentCommandArg == Help)
    )
        return printGuessedCommands(application().guessCommandsForComplete(wordArg));

    // Print all or guessed namespace names for the list command
    if (!isOptionArgument(wordArg) && currentCommandArg == List)
        return printGuessedNamespaces(wordArg);

    // Print all or guessed shell names for the integrate command
    if (!isOptionArgument(wordArg) && currentCommandArg == Integrate)
        return printGuessedShells(wordArg);

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
    if (isLongOption(wordArg) && cwordArg >= 1)
        return printGuessedLongOptions(currentCommandArg, wordArg);

    // Print all or guessed short option parameter names
    if (isShortOption(wordArg) && cwordArg >= 1)
        return printGuessedShortOptions(currentCommandArg);

    /* This doesn't block directory paths completion on bash, the _filedir like function
       must be called explicitly to invoke directory paths completion, it's done
       based on the command name in the __tom_complete_dir function. */
    printCompletionResult(EMPTY);

    return EXIT_SUCCESS;
}

/* private */

/* Current Tom command */

std::optional<QString>
BashCommand::getCurrentTomCommand(const QString &commandlineArg,
                                  const QString::size_type cword)
{
    const auto commandlineArgSplitted = commandlineArg.trimmed()
                                                      // CUR1 finish silverqx
                                        .split(SPACE, Qt::KeepEmptyParts);
    const auto currentSplittedSize = commandlineArgSplitted.size();
    Q_ASSERT(!commandlineArgSplitted.isEmpty());

    // Nothing to do, no Tom command (only the tom executable)
    if (currentSplittedSize <= 1)
        return std::nullopt;

    for (ArgumentsSizeType i = 1; i < currentSplittedSize; ++i) {
        const auto &currentCommand = commandlineArgSplitted[i];

        if (isOptionArgument(currentCommand))
            continue;

        // Tom command is being completed, return the nullopt to invoke the guess logic
        if (i == cword)
            return std::nullopt;

        return currentCommand;
    }

    return std::nullopt;
}

/* Printing support */

QChar BashCommand::getResultDelimiter() const noexcept
{
    return SPACE;
}

} // namespace Tom::Commands::Complete

TINYORM_END_COMMON_NAMESPACE
