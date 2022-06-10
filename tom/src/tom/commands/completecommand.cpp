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
using Tom::Constants::position;
using Tom::Constants::position_up;
using Tom::Constants::word_;
using Tom::Constants::word_up;

using TomUtils = Tom::Utils;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands
{

/* public */

CompleteCommand::CompleteCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
{}

QList<QCommandLineOption> CompleteCommand::optionsSignature() const
{
    return {
        {word_,       QStringLiteral("The current word that is being "
                                     "completed"), word_up},
        {commandline, QStringLiteral("The entire current command line"), commandline_up},
        {position,    QStringLiteral("The current position of the cursor on the "
                                     "command line"), position_up, "0"},
    };
}

int CompleteCommand::run()
{
    Command::run();

    const auto wordArg = value(word_);
    const auto commandlineArg = value(commandline);
    const auto positionArg = static_cast<QString::size_type>(
                                 value(position).toLongLong());

    const auto currentCommandSplitted = commandlineArg.split(SPACE);
    const auto currentCommandArg = currentCommandSplitted.size() >= 2
                                   ? std::make_optional(currentCommandSplitted[1])
                                   : std::nullopt;

    const auto commandlineArgSize = commandlineArg.size();

    // Print all commands after tom command itself or for the help command
    if (wordArg.isEmpty() && positionArg >= commandlineArgSize &&
        (!commandlineArg.contains(SPACE) || currentCommandArg == Help)
    )
        return printGuessedCommands(
                    application().guessCommandNamesForComplete({}));

    // Print all guessed commands by the word argument after tom or for the help command
    if (!wordArg.isEmpty() && positionArg >= commandlineArgSize &&
        (commandlineArg.count(SPACE) == 1 || currentCommandArg == Help)
    )
        return printGuessedCommands(
                    application().guessCommandNamesForComplete(wordArg));

    // Print all or guessed namespace names for the list command
    if (!wordArg.startsWith(QLatin1String("--")) && !wordArg.startsWith(DASH) &&
        currentCommandArg == List && positionArg >= commandlineArgSize &&
        ((wordArg.isEmpty() && commandlineArg.count(SPACE) == 1) ||
         (!wordArg.isEmpty() && commandlineArg.count(SPACE) == 2))
    )
        return printGuessedNamespaces(wordArg);

    // Print all or guessed shell names for the integrate command
    if (!wordArg.startsWith(QLatin1String("--")) && !wordArg.startsWith(DASH) &&
        currentCommandArg == Integrate && positionArg >= commandlineArgSize &&
        ((wordArg.isEmpty() && commandlineArg.count(SPACE) == 1) ||
         (!wordArg.isEmpty() && commandlineArg.count(SPACE) == 2))
    )
        return printGuessedShells(wordArg);

    // Print all or guessed long option parameter names
    if (wordArg.startsWith(QLatin1String("--")) && commandlineArg.contains(SPACE))
        return printGuessedLongOptions(*currentCommandArg, wordArg);

    // Print all or guessed short option parameter names
    if (wordArg.startsWith(DASH) && commandlineArg.contains(SPACE))
        return printGuessedShortOptions(*currentCommandArg);

    return EXIT_SUCCESS;
}

/* protected */

int CompleteCommand::printGuessedCommands(
            std::vector<std::shared_ptr<Command>> &&commands) const
{
    for (const auto &command : commands) {
        const auto commandName = command->name();

        note(QStringLiteral("%1;%2;%3").arg(commandName, commandName,
                                            command->description()));
    }

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

int CompleteCommand::printGuessedLongOptions(const QString &currentCommand,
                                             const QString &word) const
{
    // Ownership of a unique_ptr()
    const auto command = application().createCommand(
                             currentCommand, std::nullopt, false);

    const auto commandOptions = command->optionsSignature() + application().m_options;

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
                    const auto longOption = LongOption.arg(std::move(optionName));

                    options << QStringLiteral("%1;%2;%3").arg(longOption, longOption,
                                                              option.description());
                }
                // With a value
                else {
                    // Text to be used as the auto completion result
                    auto longOption = LongOption.arg(optionName).append(EQ_C);
                    // Text to be displayed in a list
                    auto longOptionList = LongOptionValue.arg(std::move(optionName),
                                                              valueName);

                    options << QStringLiteral("%1;%2;%3%4")
                               .arg(std::move(longOption), std::move(longOptionList),
                                    option.description(),
                                    getOptionDefaultValue(option));
                }
            }
    }

    note(options.join(NEWLINE));

    return EXIT_SUCCESS;
}

int CompleteCommand::printGuessedShortOptions(const QString &currentCommand) const
{
    // Ownership of a unique_ptr()
    const auto command = application().createCommand(
                             currentCommand, std::nullopt, false);

    const auto commandOptions = command->optionsSignature() + application().m_options;

    QStringList options;
    options.reserve(commandOptions.size());

    for (const auto &option : commandOptions) {
        const auto optionNames = option.names();

        // Some validation
        const auto optionNamesSize = optionNames.size();
        Q_ASSERT(optionNamesSize == 1 || optionNamesSize == 2);

        for (const auto &optionName : optionNames)
            // Short option
            if (optionName.size() == 1) {
                // All other short options
                if (option.names()[0] != "v")
                    options << QStringLiteral("-%1;-%2;%3").arg(optionName, optionName,
                                                                option.description());
                // Special handling of the -v options, good enough 😎
                else {
                    /* Has to be v because pwsh parameter names are case-insensitive so
                       it collides with the -V parameter. */
                    options << QStringLiteral("-v;v;%1").arg(option.description());
                    options << QStringLiteral("-vv;-vv;%1").arg(option.description());
                    options << QStringLiteral("-vvv;-vvv;%1").arg(option.description());
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

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE
