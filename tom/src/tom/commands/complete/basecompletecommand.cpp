#include "tom/commands/complete/basecompletecommand.hpp"

#include <orm/constants.hpp>
#include <orm/macros/likely.hpp>

#include "tom/application.hpp"
#include "tom/tomutils.hpp"

#ifndef TINYTOM_DEBUG
#  include "tom/exceptions/runtimeerror.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::DASH;
using Orm::Constants::EQ_C;
using Orm::Constants::NOSPACE;

using Tom::Constants::DoubleDash;
using Tom::Constants::EMPTY;
using Tom::Constants::LongOption;
using Tom::Constants::LongOptionValue;
using Tom::Constants::ShPwsh;
using Tom::Constants::ShortOption;

#if defined(__linux__) || defined(__MINGW32__)
using Tom::Constants::ShBash;
using Tom::Constants::ShZsh;
#endif

using TomUtils = Tom::Utils;

namespace Tom::Commands::Complete
{

/* Complete command is inspired by winget/dotnet complete:
   https://github.com/microsoft/winget-cli/blob/master/doc/Completion.md */

/* public */

BaseCompleteCommand::BaseCompleteCommand(Application &application,
                                         QCommandLineParser &parser)
    : Command(application, parser)
{}

int BaseCompleteCommand::run()
{
    return Command::run();
}

/* protected */

/* Result output */

int BaseCompleteCommand::printGuessedCommands(
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

        // Exclude hidden commands
        if (Application::isCommandHidden(commandName))
            continue;

        guessedCommands << prepareResultValue(std::move(commandName),
                                              command->description());
    }

    printCompletionResult(guessedCommands);

    return EXIT_SUCCESS;
}

int BaseCompleteCommand::printGuessedNamespaces(const QString &wordArg) const
{
    const auto &allNamespaceNames = Application::namespaceNames();

    // No need to check empty(), there are always some namespaces

    const auto printAll = wordArg.isEmpty();

    QStringList namespaceNames;
    namespaceNames.reserve(static_cast<decltype (namespaceNames)::size_type>(
                               allNamespaceNames.size()));

    for (const QString &namespaceName : allNamespaceNames)
        if (!namespaceName.isEmpty() &&
            !Application::isNamespaceHidden(namespaceName) && // Exclude hidden namespaces
            (printAll || namespaceName.startsWith(wordArg))
        )
            namespaceNames << namespaceName;

    // Want to have it unsorted if printing all namespaces
    if (!printAll)
        namespaceNames.sort(Qt::CaseInsensitive);

    printCompletionResult(namespaceNames);

    return EXIT_SUCCESS;
}

// FUTURE complete, printGuessedNamespaces and printGuessedShells are practically the same methods, if I will implement another method of this simple list type, then create common method and reuse code silverqx
int BaseCompleteCommand::printGuessedShells(const QString &wordArg) const
{
    /* There is no need to complete the bash and zsh for other platforms.
       The pwsh can run on all our supported platforms so always complete it. */
    static const std::array allShellNames {
#if defined(__linux__) || defined(__MINGW32__)
        ShBash,
#endif
        ShPwsh,
#if defined(__linux__) || defined(__MINGW32__)
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

    // Shell names are already sorted

    printCompletionResult(shellNames);

    return EXIT_SUCCESS;
}

int BaseCompleteCommand::printGuessedLongOptions(
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
        Q_ASSERT(between(optionNames.size(), 1, 2));

        // Don't show hidden options
        if (option.hidden())
            continue;

        for (const auto &optionName : std::as_const(optionNames))
            if (optionName.size() > 1 &&
                (printAll || optionName.startsWith(wordToGuess))
            ) {
                // Without a value
                if (const auto valueName = option.valueName();
                    valueName.isEmpty()
                ) {
                    auto longOption = LongOption.arg(optionName);

                    options << prepareResultValue(std::move(longOption),
                                                  option.description());
                }
                // With a value
                else {
                    // Text to be used as the tab-completion result
                    auto longOption = LongOption.arg(optionName).append(EQ_C);
                    // Text to be displayed in the completion list
                    const auto longOptionList = LongOptionValue.arg(optionName,
                                                                    valueName);

                    options << prepareResultValue(
                                   std::move(longOption), longOptionList,
                                   NOSPACE.arg(option.description(),
                                               getOptionDefaultValue(option)));
                }
            }
    }

    printCompletionResult(options);

    return EXIT_SUCCESS;
}

int BaseCompleteCommand::printGuessedShortOptions(
            const std::optional<QString> &currentCommand) const
{
    const auto commandOptions = getCommandOptionsSignature(currentCommand);

    QStringList options;
    options.reserve(commandOptions.size());

    for (const auto &option : commandOptions) {
        auto optionNames = option.names();

        // Some validation
        Q_ASSERT(between(optionNames.size(), 1, 2));

        // Don't show hidden options
        if (option.hidden())
            continue;

        for (auto &optionName : optionNames)
            // Short option
            if (optionName.size() == 1) {
                // All other short options
                if (option.names().constFirst() != u'v') T_LIKELY
                    options << prepareResultValue(ShortOption.arg(optionName),
                                                  option.description());
                // Special handling for the -v options
                else T_UNLIKELY
                    appendShortVerboseOptions(options, option.description());
            }
    }

    printCompletionResult(options);

    return EXIT_SUCCESS;
}

/* Option arguments */

QList<CommandLineOption>
BaseCompleteCommand::getCommandOptionsSignature(
        const std::optional<QString> &command) const
{
    // Nothing to do, no command passed so print only the common options
    if (!command)
        return application().m_options;

    return application().getCommandOptionsSignature(*command) + application().m_options;
}

bool BaseCompleteCommand::isOptionArgument(const QString &wordArg, const OptionType type)
{
    const auto isLong = wordArg.startsWith(DoubleDash);
    const auto isShort = isLong ? false : wordArg.startsWith(DASH);

    // Consider both long and short option arguments
    if (type == kAnyOption)
        return isLong || isShort;

    if (type == kLongOption)
        return isLong;
    if (type == kShortOption)
        return isShort;

#ifndef TINYTOM_DEBUG
    throw Exceptions::RuntimeError(u"Unexpected value for enum struct OptionType."_s);
#else
    Q_UNREACHABLE();
#endif
}

/* private */

/* Option arguments */

QString BaseCompleteCommand::getOptionDefaultValue(const QCommandLineOption &option)
{
    const auto defaultValues = option.defaultValues();

    // Multiple default values are not supported
    Q_ASSERT(defaultValues.size() <= 1);

    return defaultValues.isEmpty() ? EMPTY
                                   : TomUtils::defaultValueText(
                                         defaultValues.constFirst()); // Quotes the string type
}

} // namespace Tom::Commands::Complete

TINYORM_END_COMMON_NAMESPACE
