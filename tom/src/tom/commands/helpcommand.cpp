#include "tom/commands/helpcommand.hpp"

#include <QCommandLineOption>

#include <orm/constants.hpp>

#include "tom/application.hpp"
#include "tom/tomconstants.hpp"
#include "tom/tomutils.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::SPACE;

using Tom::Constants::command_name;

using TomUtils = Tom::Utils;

namespace Tom::Commands
{

/* public */

HelpCommand::HelpCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
    , Concerns::PrintsOptions(application.m_options, *this)
{}

const std::vector<PositionalArgument> &HelpCommand::positionalArguments() const
{
    static const std::vector<PositionalArgument> cached {
        {command_name, QStringLiteral("The command name"), {}, true, Constants::help},
    };

    return cached;
}

QString HelpCommand::help() const
{
    return QStringLiteral(
R"(  The <info>help</info> command displays help for a given command:

    <info>tom</info> help list

  To display the list of available commands, please use the <info>list</info> command.)");
}

int HelpCommand::run()
{
    Command::run();

    const auto commandNameArg = argument(command_name);

    const auto command = createCommand(commandNameArg);
    const auto &arguments = command->positionalArguments();

    if (!validateRequiredArguments(arguments))
        return EXIT_FAILURE;

    printDescriptionSection(*command);
    printUsageSection(*command, arguments);

    printArgumentsSection(arguments);
    printOptionsSection(*command);

    printHelpSection(*command);

    return EXIT_SUCCESS;
}

/* protected */

std::unique_ptr<Command> HelpCommand::createCommand(const QString &name) const
{
    /* Get a command name on the base of a passed command name, guess logic will be
       applied if needed, if the command name is only a partial name, eg. ma:mig. */
    const auto commandName = application().getCommandName(name,
                                                          Application::ShowErrorWall);

    return application().createCommand(commandName, std::nullopt, false);
}

bool HelpCommand::validateRequiredArguments(
        const std::vector<PositionalArgument> &arguments) const
{
    // Fail on required argument after optional argument
    for (std::vector<PositionalArgument>::size_type i = 1;
         i < arguments.size() ; ++i
    ) {
        const auto &left = arguments.at(i - 1);
        const auto &right = arguments.at(i);

        if (left.optional && !right.optional) {
            errorWall(QStringLiteral("Cannot add a required argument '%1' after "
                                     "an optional one '%2'.")
                      .arg(right.name, left.name));

            return false;
        }
    }

    // Fail when required argument has a default value
    return std::ranges::none_of(arguments, [this](const auto &argument)
    {
        const auto requiredWithDefault = !argument.optional &&
                                         !argument.defaultValue.isEmpty();

        if (requiredWithDefault)
            errorWall(QStringLiteral("The required argument '%1' has a default value "
                                     "'%2'.")
                      .arg(argument.name, argument.defaultValue));

        return requiredWithDefault;
    });
}

void HelpCommand::printDescriptionSection(const Command &command) const
{
    comment(QStringLiteral("Description:"));

    note(QStringLiteral("  %1").arg(command.description()));
}

void HelpCommand::printUsageSection(
        const Command &command, const std::vector<PositionalArgument> &arguments) const
{
    /* Everything after the option -- (double dash) is treated as positional arguments.
       [] means optional, <> means positional argument. If an argument is optional,
       all arguments after have to be optional. */

    newLine();

    comment(QStringLiteral("Usage:"));

    auto commandName = command.name();
    const auto hasOptions = command.hasOptions();
    const auto hasPositionalArguments = command.hasPositionalArguments();

    QString usage;
    usage.reserve(2 + commandName.size() +
                  (hasOptions ? 10 : 0) +
                  (hasPositionalArguments
                   ? 5 + (argumentsMaxSize(arguments) + 5) *
                     static_cast<QString::size_type>(arguments.size())
                   : 0) +
                  // Some reserve, important if a command has only one argument
                  10);

    usage.fill(SPACE, 2);
    usage += std::move(commandName);

    if (hasOptions)
        usage += QLatin1String(" [options]");

    if (hasPositionalArguments) {
        usage += QLatin1String(" [--]");

        auto optionalCounter = 0;

        for (const auto &argument : arguments) {
            auto syntax = argument.syntax.isEmpty() ? argument.name : argument.syntax;

            if (argument.optional) {
                usage += QStringLiteral(" [<%1>").arg(std::move(syntax));
                ++optionalCounter;
            }
            else
                usage += QStringLiteral(" <%1>").arg(std::move(syntax));
        }

        usage += QString(optionalCounter, QLatin1Char(']'));
    }

    note(usage);
}

void HelpCommand::printArgumentsSection(
        const std::vector<PositionalArgument> &arguments) const
{
    if (arguments.empty())
        return;

    newLine();

    comment(QStringLiteral("Arguments:"));

    for (const auto argumentsMaxSize = HelpCommand::argumentsMaxSize(arguments);
         const auto &argument : arguments
    ) {
        // Compute indent
        auto indent = QString(argumentsMaxSize - argument.name.size(), SPACE);

        info(QStringLiteral("  %1%2  ").arg(argument.name, std::move(indent)), false);

        note(argument.description, false);

        printArgumentDefaultValue(argument);
    }
}

int HelpCommand::argumentsMaxSize(const std::vector<PositionalArgument> &arguments)
{
    static const auto cached = [&arguments]
    {
        const auto it = std::ranges::max_element(arguments, std::less {},
                                                 [](const auto &argument)
        {
            return argument.name.size();
        });

        return static_cast<int>((*it).name.size());
    }();

    return cached;
}

void HelpCommand::printArgumentDefaultValue(const PositionalArgument &argument) const
{
    if (const auto &defaultValue = argument.defaultValue;
        !defaultValue.isEmpty()
    )
        comment(TomUtils::defaultValueText(defaultValue), false);

    newLine();
}

int HelpCommand::printOptionsSection(const Command &command) const
{
    // Prepare the m_options list, commands' options will be obtained from it
    application().prependOptions(command.optionsSignature());

    return PrintsOptions::printOptionsSection(false);
}

void HelpCommand::printHelpSection(const Command &command) const
{
    const auto help = command.help();

    if (help.isEmpty())
        return;

    newLine();

    comment(QStringLiteral("Help:"));

    note(help);
}

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE
