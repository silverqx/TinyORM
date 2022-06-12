#include "tom/commands/completecommand.hpp"

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

#include <orm/constants.hpp>

#include "tom/application.hpp"
#include "tom/tomutils.hpp"

using Orm::Constants::DASH;
using Orm::Constants::EMPTY;
using Orm::Constants::EQ_C;
using Orm::Constants::NEWLINE;
using Orm::Constants::SPACE;

using Tom::Constants::DoubleDash;
using Tom::Constants::Help;
using Tom::Constants::Integrate;
using Tom::Constants::List;
using Tom::Constants::LongOption;
using Tom::Constants::LongOptionValue;
//using Tom::Constants::ShBash;
using Tom::Constants::ShPwsh;
//using Tom::Constants::ShZsh;
using Tom::Constants::commandline;
using Tom::Constants::commandline_up;
using Tom::Constants::cword_;
using Tom::Constants::cword_up;
using Tom::Constants::position;
using Tom::Constants::position_up;
using Tom::Constants::word_;
using Tom::Constants::word_up;

using TomUtils = Tom::Utils;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands
{

/* Complete command is inspired by winget/dotnet complete:
   https://github.com/microsoft/winget-cli/blob/master/doc/Completion.md */

/* public */

CompleteCommand::CompleteCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
{}

QList<QCommandLineOption> CompleteCommand::optionsSignature() const
{
    return {
        {word_,       QStringLiteral("The current word that is being "
                                     "completed"), word_up},
        {commandline, QStringLiteral("The entire current command-line"), commandline_up},
#ifdef _MSC_VER
        {position,    QStringLiteral("The current position of the cursor on the "
                                     "command line"), position_up, "0"},
#else
        {cword_,      QStringLiteral("Position of the current word on the command-line "
                                     "that is being completed"), cword_up},
#endif
    };
}

int CompleteCommand::run()
{
    Command::run();

    /* Initialization section */
    const auto wordArg = value(word_);
    const auto commandlineArg = value(commandline);

#ifdef _MSC_VER
    const auto positionArg = static_cast<QString::size_type>(
                                 value(position).toLongLong());

    const auto commandlineArgSize = commandlineArg.size();
#else
    const auto cwordArg = static_cast<QString::size_type>(value(cword_).toLongLong());
#endif

    // Currently proccessed tom command
    const auto currentCommandArg = getCurrentTomCommand(commandlineArg, cwordArg);

    /* Main logic section */

    /* Print all commands after tom command itself or for the help command
       --- */
#ifdef _MSC_VER
    if (!isOptionArgument(wordArg) && wordArg.isEmpty() &&
        positionArg >= commandlineArgSize &&
        (!commandlineArg.contains(SPACE) || currentCommandArg == Help)
    )
#else
    if (!isOptionArgument(wordArg) && wordArg.isEmpty() &&
        (!currentCommandArg || currentCommandArg == Help)
    )
#endif
        return printGuessedCommands(
                    application().guessCommandNamesForComplete({}));

    /* Print all guessed commands by the word argument after tom or for the help command
       --- */
#ifdef _MSC_VER
    if (!isOptionArgument(wordArg) && !wordArg.isEmpty() &&
        positionArg >= commandlineArgSize &&
        (commandlineArg.count(SPACE) == 1 || currentCommandArg == Help)
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

std::optional<QString>
CompleteCommand::getCurrentTomCommand(const QString &commandlineArg,
                                      const QString::size_type cword)
{
    const auto currentCommandSplitted = commandlineArg.trimmed().split(SPACE);
    const auto currentSplittedSize = currentCommandSplitted.size();

    if (currentSplittedSize <= 1)
        return std::nullopt;

    for (auto i = 1; i < currentSplittedSize; ++i)
        if (const auto &currentCommand = currentCommandSplitted[i];
            !isOptionArgument(currentCommand)
        ) {
            // Little weird, but the current tom command is actually completing now
            if (i == cword)
                return std::nullopt;
            else
                return currentCommand;
        }

    return std::nullopt;
}

int CompleteCommand::printGuessedCommands(
            std::vector<std::shared_ptr<Command>> &&commands) const
{
    QStringList guessedCommands;
    guessedCommands.reserve(static_cast<QStringList::size_type>(commands.size()));

    for (const auto &command : commands) {
        auto commandName = command->name();

        if (m_dontList.contains(commandName))
            continue;

#ifdef _MSC_VER
        guessedCommands << QStringLiteral("%1;%2;%3").arg(commandName, commandName,
                                                          command->description());
#else
        guessedCommands << std::move(commandName);
#endif
    }

    note(guessedCommands.join(NEWLINE));

    return EXIT_SUCCESS;
}

int CompleteCommand::printGuessedNamespaces(const QString &word) const
{
    const auto &allNamespaceNames = application().namespaceNames();

    const auto printAll = word.isEmpty();

    QStringList namespaceNames;
    namespaceNames.reserve(
                static_cast<QStringList::size_type>(allNamespaceNames.size()));

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
    static const std::vector allShellNames {
//        ShBash,
        ShPwsh,
//        ShZsh,
    };

    const auto printAll = word.isEmpty();

    QStringList shellNames;
    shellNames.reserve(static_cast<QStringList::size_type>(allShellNames.size()));

    for (const QString &shellName : allShellNames)
        if (!shellName.isEmpty() && (printAll || shellName.startsWith(word)))
            shellNames << shellName;

    // Want to have un-sorted if printing all namespaces
    if (!printAll)
        shellNames.sort(Qt::CaseInsensitive);

    note(shellNames.join(NEWLINE));

    return EXIT_SUCCESS;
}

int CompleteCommand::printGuessedLongOptions(
            const std::optional<QString> &currentCommand, const QString &word) const
{
    const auto commandOptions = getCommandOptionsSignature(currentCommand);

    const auto wordToGuess = word.mid(2);
    const auto printAll = wordToGuess.isEmpty();

    QStringList options;
    options.reserve(commandOptions.size());

    for (const auto &option : commandOptions) {
        auto optionNames = option.names();

        // Some validation
        const auto optionNamesSize = optionNames.size();
        Q_ASSERT(optionNamesSize == 1 || optionNamesSize == 2);

        for (auto &&optionName : optionNames)
            if (optionName.size() > 1 &&
                (printAll || optionName.startsWith(wordToGuess))
            ) {
                // W/o a value
                if (const auto valueName = option.valueName();
                    valueName.isEmpty()
                ) {
                    auto longOption = LongOption.arg(std::move(optionName));

#ifdef _MSC_VER
                    options << QStringLiteral("%1;%2;%3").arg(longOption, longOption,
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
                    // Text to be displayed in a list
                    auto longOptionList = LongOptionValue.arg(std::move(optionName),
                                                              valueName);

                    options << QStringLiteral("%1;%2;%3%4")
                               .arg(std::move(longOption), std::move(longOptionList),
                                    option.description(),
                                    getOptionDefaultValue(option));
#else
                    options << std::move(longOption);
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

    return defaultValues.isEmpty() ? EMPTY
                                   : TomUtils::defaultValueText(defaultValues[0]);
}

QList<QCommandLineOption>
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

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE
