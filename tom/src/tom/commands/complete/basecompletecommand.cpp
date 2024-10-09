#include "tom/commands/complete/basecompletecommand.hpp"

#include <range/v3/algorithm/contains.hpp>

#include <orm/constants.hpp>
#include <orm/macros/likely.hpp>
#include <orm/utils/type.hpp>

#include "tom/application.hpp"
#include "tom/exceptions/invalidargumenterror.hpp"
#include "tom/tomutils.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::DASH;
using Orm::Constants::NOSPACE;

using Tom::Constants::DoubleDash;
using Tom::Constants::EMPTY;
using Tom::Constants::LongOption;
using Tom::Constants::LongOptionEq;
using Tom::Constants::LongOptionValue;
using Tom::Constants::ShPwsh;
using Tom::Constants::ShortOption;
using Tom::Constants::commandline;
using Tom::Constants::word_;

#if defined(__linux__) || defined(__MINGW32__)
using Tom::Constants::ShBash;
using Tom::Constants::ShZsh;
#endif

using TomUtils = Tom::Utils;

using enum Tom::GuessedCommandNameResult;

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
    const auto exitCode = Command::run();

    // Validate the required option arguments (checks isSet())
    validateInputOptions();

    // Initialize context for tab-completion
    m_context = initializeCompletionContext();

    return exitCode;
}

/* protected */

/* Current Tom command */

GuessedCommandName
BaseCompleteCommand::getCurrentTomCommand(const SizeType argumentsCount) const
{
    // Nothing to do, no Tom command on the command-line (only the tom executable name)
    if (argumentsCount <= 1)
        return {kNotFound, std::nullopt};

    // Try to guess one Tom command name (detects kFound, kNotFound, kAmbiguous)
    return application().guessCommandNameForComplete(getRawTomCommandName());
}

QStringView BaseCompleteCommand::getRawTomCommandName() const
{
    for (SizeType index = kUndefinedPosition;
         const auto argument : m_commandlineArgSplit
    ) {
        if (isOptionArgument(argument))
            continue;

        if (++index == ArgTomCommand)
            return argument;
    }

#ifndef TINYTOM_DEBUG
    throw Exceptions::RuntimeError(
                u"Unexpected return value, it can't be empty if argumentsCount > 1 "
                 "in %1()."_s.arg(__tiny_func__));
#else
    // Guaranteed by the argumentsCount <= 1 above, there are always 2 commands
    Q_UNREACHABLE();
#endif
}

SizeType BaseCompleteCommand::getMaxArgumentsCount(const bool hasAnyTomCommand) const
{
    using CommandType = std::shared_ptr<Application::Command>;

    // +1 for tom.exe and +1 for any known/our or ambiguous Tom command
    return 1 + (hasAnyTomCommand ? 1 : 0) +
            static_cast<SizeType>(
                (*std::ranges::max_element(application().createCommandsVector(),
                                           std::less(), [](const CommandType &command)
    {
        return command->positionalArguments().size();
    }))
        ->positionalArguments().size());
}

SizeType
BaseCompleteCommand::getMaxArgumentsCount(const QString &command,
                                          const bool hasAnyTomCommand) const
{
    // +1 for tom.exe and +1 for any known/our or ambiguous Tom command
    return 1 + (hasAnyTomCommand ? 1 : 0) +
            static_cast<SizeType>(
                application().createCommand(command, std::nullopt, false)
                            ->positionalArguments().size());
}

/* Result output */

int BaseCompleteCommand::printGuessedCommands() const
{
    const auto commands = application().guessCommandsForComplete(context().wordArg);
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

    // Want to have them always unsorted, they are sorted by namespaces

    printCompletionResult(guessedCommands);

    return EXIT_SUCCESS;
}

int BaseCompleteCommand::printGuessedNamespaces() const
{
    const auto &allNamespaceNames = Application::namespaceNames();

    // No need to check empty(), there are always some namespaces

    const auto wordArg = context().wordArg;
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

    // Want to have them unsorted if printing all namespaces
    if (!printAll)
        namespaceNames.sort(Qt::CaseInsensitive);

    printCompletionResult(namespaceNames);

    return EXIT_SUCCESS;
}

// FUTURE complete, printGuessedNamespaces and printGuessedShells are practically the same methods, if I will implement another method of this simple list type, then create common method and reuse code silverqx
int BaseCompleteCommand::printGuessedShells() const
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

    const auto wordArg = context().wordArg;
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

int BaseCompleteCommand::printGuessedLongOptions() const
{
    const auto commandOptions = getCommandOptionsSignature();

    // The -- is guaranteed by the isLongOption(wordArg)
    const auto wordToGuess = context().wordArg.sliced(2);
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
                // Without value (eg. --ansi)
                if (const auto valueName = option.valueName();
                    valueName.isEmpty()
                ) T_LIKELY
                    options << prepareResultValue(LongOption.arg(optionName),
                                                  option.description());
                // With value (eg. --only=)
                else T_UNLIKELY
                    options << prepareResultValue(
                                   // Text to use as the tab-completion result eg. --only=
                                   LongOptionEq.arg(optionName),
                                   // Text that appears in the completion list
                                   LongOptionValue.arg(optionName, valueName),
                                   // Description that appears in the completion list
                                   NOSPACE.arg(option.description(),
                                               getOptionDefaultValue(option)));
            }
    }

    // Want to have them always unsorted (sorted as they are defined in the source code)

    printCompletionResult(options);

    return EXIT_SUCCESS;
}

int BaseCompleteCommand::printGuessedShortOptions() const
{
    const auto commandOptions = getCommandOptionsSignature();

    QStringList options;
    options.reserve(commandOptions.size());

    for (const auto &option : commandOptions) {
        const auto optionNames = option.names();

        // Some validation
        Q_ASSERT(between(optionNames.size(), 1, 2));

        // Don't show hidden options
        if (option.hidden())
            continue;

        for (const auto &optionName : optionNames)
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

    // Want to have them always unsorted

    printCompletionResult(options);

    return EXIT_SUCCESS;
}

/* Option arguments */

QList<CommandLineOption> BaseCompleteCommand::getCommandOptionsSignature() const
{
    /* There's no case where we need to pass a custom command name, so always use
       the value from the context for a nicer/shorter syntax, even if it's a little
       confusing. */
    const auto &currentCommandArg = context().currentCommandArg;
    auto &application = this->application();

    // Nothing to do, no Tom command passed/guessed so print only the common options
    if (!currentCommandArg)
        return application.m_options;

    return application.getCommandOptionsSignature(*currentCommandArg) +
           application.m_options;
}

bool
BaseCompleteCommand::isOptionArgument(const QStringView wordArg, const OptionType type)
{
    const auto isLongOption  = wordArg.startsWith(DoubleDash);
    const auto isShortOption = isLongOption ? false : wordArg.startsWith(DASH);

    // Consider both long and short option arguments
    if (type == kAnyOption)
        return isLongOption || isShortOption;

    if (type == kLongOption)
        return isLongOption;
    if (type == kShortOption)
        return isShortOption;

#ifndef TINYTOM_DEBUG
    throw Exceptions::RuntimeError(
                u"Unexpected value for enum struct OptionType in %1()."_s
                .arg(__tiny_func__));
#else
    Q_UNREACHABLE();
#endif
}

bool BaseCompleteCommand::commandHasLongOption(const QString &optionName) const
{
    /* Currently, used for the --database= option only, so checking for hidden options
       doesn't make sense. */
    return ranges::contains(getCommandOptionsSignature(), true,
                            [&optionName](const QCommandLineOption &option)
    {
        Q_ASSERT(!option.names().isEmpty());

        return option.names().constFirst() == optionName;
    });
}

void BaseCompleteCommand::validateInputOptions() const
{
    constexpr static auto optionsToValidate = std::to_array<
                                              std::reference_wrapper<const QString>>({
        commandline,
        word_,
    });

    // TODO parser, add support for required positional arguments and options silverqx
    for (const auto &optionName : optionsToValidate)
        if (!isSet(optionName))
            throw Exceptions::InvalidArgumentError(
                    u"The --%1= option must be set for complete:bash/pwsh commands "
                     "in %2()."_s
                    .arg(optionName, __tiny_func__));
}

void BaseCompleteCommand::validateInputOptionValues() const
{
    // Nothing to do
    if (!m_commandlineArg.isEmpty())
        return;

    throw Exceptions::InvalidArgumentError(
                u"The --commandline= option value cannot be empty for complete:bash/pwsh "
                 "commands in %1()."_s
                .arg(__tiny_func__));
}

/* private */

/* Option arguments */

QString BaseCompleteCommand::getOptionDefaultValue(const QCommandLineOption &option)
{
    const auto defaultValues = option.defaultValues();

    // Multiple default values are not supported
    Q_ASSERT(defaultValues.size() <= 1);

    return defaultValues.isEmpty() ? EMPTY
                                   : TomUtils::defaultValueText( // Quote the string type
                                         defaultValues.constFirst());
}

} // namespace Tom::Commands::Complete

TINYORM_END_COMMON_NAMESPACE
