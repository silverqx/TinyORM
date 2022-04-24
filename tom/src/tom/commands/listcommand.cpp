#include "tom/commands/listcommand.hpp"

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

#include <orm/constants.hpp>

#include "tom/application.hpp"

using Orm::Constants::COLON;
using Orm::Constants::NEWLINE;
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
R"(  The <info>list</info> command lists all commands:

    <info>tom list</info>

  You can also display the commands for a specific namespace:

    <info>tom list test</info>

  It's also possible to get raw list of commands (useful for embedding command runner):

    <info>tom list --raw</info>)");
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
    printCommandsSection(getNamespaceName(namespaceArg), optionsMaxSize);

    return EXIT_SUCCESS;
}

int ListCommand::raw(const QString &namespaceArg)
{
    const auto &commands = getCommandsByNamespace(getNamespaceName(namespaceArg));

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

/* Guess namespace name section */

QString ListCommand::getNamespaceName(const QString &namespaceArg) const
{
    // Namespace positional argument was not defined, show all commands list
    if (namespaceArg.isNull())
        return {};

    // Show commands for the global namespace if empty string
    if (namespaceArg.isEmpty())
        return "global";

    // Try to find a full command name to avoid the guess logic
    if (auto namespaceArg_ = namespaceArg.toLower();
        ranges::contains(application().namespaceNames(), namespaceArg_)
    )
        return namespaceArg_;

    // Invoke the guess logic 👌
    const auto namespaces = guessNamespace(namespaceArg);

    // Found exactly one namespace
    if (namespaces.size() == 1)
        return namespaces.front();

    // No namespace found
    if (namespaces.empty()) {
        application().errorWall(
                    QLatin1String(
                        "There are no commands defined in the \"%1\" namespace.")
                    .arg(namespaceArg));

        application().exitApplication(EXIT_FAILURE);
    }

    // Found more namespaces (ambiguous)
    printAmbiguousNamespaces(namespaceArg, namespaces);
}

std::vector<QString> ListCommand::guessNamespace(const QString &namespaceArg) const
{
    return application().namespaceNames()
            | ranges::views::filter([&namespaceArg](const QString &namespaceName)
    {
        return namespaceName.startsWith(namespaceArg, Qt::CaseInsensitive);
    })
            | ranges::to<std::vector<QString>>();
}

Q_NORETURN void
ListCommand::printAmbiguousNamespaces(const QString &namespaceName,
                                      const std::vector<QString> &namespaces) const
{
    // Prepare namespaces for the errorWall()
    const auto formattedNamespaces = namespaces
            | ranges::views::transform([](const auto &ambiguousNsName) -> QString
    {
        return QLatin1String("    %1").arg(ambiguousNsName);
    })
            | ranges::to<QStringList>();

    application().errorWall(
                QLatin1String("The namespace \"%1\" is ambiguous.\n\n"
                              "Did you mean one of these?\n%2")
                .arg(namespaceName, formattedNamespaces.join(NEWLINE)));

    application().exitApplication(EXIT_FAILURE);
}

/* Commands section */

void ListCommand::printCommandsSection(const QString &namespaceName,
                                       const int optionsMaxSize) const
{
    const auto hasNamespaceName = !namespaceName.isNull();

    const auto &commands = getCommandsByNamespace(namespaceName);

    newLine();

    // Specific namespace
    if (hasNamespaceName)
                // Custom message for the namespaced argument
        comment(namespaceName == QLatin1String("namespaced")
                ? QLatin1String("Commands with the namespace prefix:")
                : QLatin1String("Available commands for the '%1' namespace:")
                  .arg(namespaceName.isEmpty() ? QLatin1String("global") : namespaceName));
    // All commands
    else
        comment(QLatin1String("Available commands:"));

    // Get max. command size in all command names
    const auto commandsMaxSize = this->commandsMaxSize(commands, optionsMaxSize);
    // Print commands to the console
    printCommands(commands, commandsMaxSize, hasNamespaceName);
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
        const int commandsMaxSize, const bool hasNamespaceName) const
{
    // Currently rendering NS
    QString renderingNamespace;

    for (const auto &command : commands) {
        auto commandName = command->name();

        // Begin a new namespace section
        tryBeginNsSection(renderingNamespace, commandName, hasNamespaceName);

        auto indent = QString(commandsMaxSize - commandName.size(), SPACE);

        info(QStringLiteral("  %1%2").arg(std::move(commandName), std::move(indent)),
             false);

        note(QStringLiteral("  %1").arg(command->description()));
    }
}

void ListCommand::tryBeginNsSection(
        QString &renderingNamespace, const QString &commandName,
        const bool hasNamespaceName) const
{
    const auto commandNamespace = this->commandNamespace(commandName);

    if (hasNamespaceName || commandNamespace == renderingNamespace)
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
    /* Obtain all commands and isNull() needed because still able to return the global
       namespace for an empty string. */
    if (name.isNull())
        return application().createCommandsVector();

    /* This avoids one copy that would be done if commands would be returned by a value,
       key thing is that it can be returned as a const reference, believe it it works. */
    static std::vector<std::shared_ptr<Command>> cached;

    return cached = application().getCommandsInNamespace(name)
                    | ranges::to<std::vector<std::shared_ptr<Command>>>();
}

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE
