#include "tom/commands/listcommand.hpp"

#include <QCommandLineOption>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/slice.hpp>

#include <orm/constants.hpp>

#include "tom/application.hpp"

using Orm::Constants::COLON;
using Orm::Constants::SPACE;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands
{

/* public */

ListCommand::ListCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
    , Concerns::PrintsOptions(*this, 0)
{}

const std::vector<PositionalArgument> &ListCommand::positionalArguments() const
{
    static const std::vector<PositionalArgument> cached {
        {"namespace", "The namespace name", {}, true},
    };

    return cached;
}

QList<QCommandLineOption> ListCommand::optionsSignature() const
{
    return {
        {"raw", "To output raw command list"},
    };
}

QString ListCommand::help() const
{
    return QLatin1String(
"  The <info>list</info> command lists all commands:\n\n"
"    <info>tom list</info>\n\n"
"  You can also display the commands for a specific namespace:\n\n"
"    <info>tom list test</info>\n\n"
"  It's also possible to get raw list of commands (useful for embedding command "
  "runner):\n\n"
"    <info>tom list --raw</info>");
}

int ListCommand::run()
{
    Command::run();

    const auto namespaceArg = argument("namespace");

    return isSet("raw") ? raw(namespaceArg) : full(namespaceArg);
}

/* protected */

int ListCommand::full(const QString &namespaceArg)
{
    application().printVersion();

    newLine();
    comment(QLatin1String("Usage:"));
    note(QLatin1String("  command [options] [--] [arguments]"));

    // Options section
    const auto optionsMaxSize = printOptionsSection(true);

    // Commands section
    printCommandsSection(namespaceArg, optionsMaxSize);

    return EXIT_SUCCESS;
}

int ListCommand::raw(const QString &namespaceArg)
{
    const auto &commands = getCommandsByNamespace(namespaceArg);

    const auto it = std::ranges::max_element(commands, std::less {},
                                             [](const auto &command)
    {
        return command->name().size();
    });

    const auto commandMaxSize = static_cast<int>((*it)->name().size());

    for (const auto &command : commands) {
        auto commandName = command->name();
        auto indent = QString(commandMaxSize - commandName.size(), SPACE);

        note(QStringLiteral("%1%2   %3").arg(std::move(commandName), std::move(indent),
                                             command->description()));
    }

    return EXIT_SUCCESS;
}

/* Commands section */

void ListCommand::printCommandsSection(const QString &namespaceArg,
                                       const int optionsMaxSize) const
{
    const auto hasNamespaceArg = !namespaceArg.isNull();

    newLine();

    if (hasNamespaceArg)
        comment(QLatin1String("Available commands for the '%1' namespace:")
                .arg(namespaceArg.isEmpty() ? QLatin1String("global") : namespaceArg));
    else
        comment(QLatin1String("Available commands:"));

    const auto &commands = getCommandsByNamespace(namespaceArg);

    // Get max. command size in all command names
    const auto commandsMaxSize = this->commandsMaxSize(commands, optionsMaxSize);
    // Print commands to the console
    printCommands(commands, commandsMaxSize, hasNamespaceArg);
}

int ListCommand::commandsMaxSize(
        const std::vector<std::shared_ptr<Command>> &commands,
        const int optionsMaxSize) const
{
    const auto it = std::ranges::max_element(commands, std::less {},
                                             [](const auto &command)
    {
        return command->name().size();
    });

    auto commandsMaxSize = static_cast<int>((*it)->name().size());

    // Align commands' description to the same level as options' description
    if (commandsMaxSize < optionsMaxSize)
        commandsMaxSize = optionsMaxSize;

    return commandsMaxSize;
}

void ListCommand::printCommands(
        const std::vector<std::shared_ptr<Command>> &commands,
        const int commandsMaxSize, const bool hasNamespaceArg) const
{
    // Currently rendering NS
    QString renderingNamespace;

    for (const auto &command : commands) {
        auto commandName = command->name();

        // Begin a new namespace section
        tryBeginNsSection(renderingNamespace, commandName, hasNamespaceArg);

        auto indent = QString(commandsMaxSize - commandName.size(), SPACE);

        info(QStringLiteral("  %1%2").arg(std::move(commandName), std::move(indent)),
             false);

        note(QStringLiteral("  %1").arg(command->description()));
    }
}

void ListCommand::tryBeginNsSection(
        QString &renderingNamespace, const QString &commandName,
        const bool hasNamespaceArg) const
{
    const auto commandNamespace = this->commandNamespace(commandName);

    if (hasNamespaceArg || commandNamespace == renderingNamespace)
        return;

    // Update currently rendering NS section
    renderingNamespace = commandNamespace;

    comment(QStringLiteral(" %1").arg(renderingNamespace));
}

QString ListCommand::commandNamespace(const QString &commandName) const
{
    if (!commandName.contains(COLON))
        return {};

    return commandName.split(COLON).at(0);
}

const std::vector<std::shared_ptr<Command>> &
ListCommand::getCommandsByNamespace(const QString &name) const
{
    // isNull() needed because still able to return global namespace for an empty string
    if (name.isNull())
        return application().createCommandsVector();

    /* This avoids one copy that would be done if commands would be returned by a value,
       key thing is that it can be returned as a const reference, believe it it works. */
    static std::vector<std::shared_ptr<Command>> cached;

    return cached = getCommandsInNamespace(name);
}

std::vector<std::shared_ptr<Command>>
ListCommand::getCommandsInNamespace(const QString &name) const
{
    /* First number is index where it starts (0-based), second the number where it ends
       (it's like iterator's end so should point after).
       Look to the Application::commandNames() to understand this indexes.
       tuple is forwarded as args to the ranges::views::slice(). */
    static const std::unordered_map<QString, std::tuple<int, int>> cached {
        {"",        std::make_tuple(0, 5)},
        {"global",  std::make_tuple(0, 5)},
        {"db",      std::make_tuple(5, 6)},
        {"make",    std::make_tuple(6, 7)},
        {"migrate", std::make_tuple(7, 13)},
    };

    if (!cached.contains(name)) {
        errorWall(QLatin1String("There are no commands defined in the '%1' namespace.")
                  .arg(name));

        application().exitApplication(EXIT_FAILURE);
    }

    return application().createCommandsVector()
            | std::apply(ranges::views::slice, cached.at(name))
            | ranges::to<std::vector<std::shared_ptr<Command>>>();
}

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE
