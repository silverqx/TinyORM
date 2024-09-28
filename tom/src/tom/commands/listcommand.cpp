#include "tom/commands/listcommand.hpp"

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

#include <orm/constants.hpp>

#include "tom/application.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::COLON;
using Orm::Constants::NEWLINE;
using Orm::Constants::SPACE;

using Tom::Constants::NsGlobal;
using Tom::Constants::NsNamespaced;
using Tom::Constants::namespace_;
using Tom::Constants::raw_;

namespace Tom::Commands
{

/* public */

ListCommand::ListCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
    , Concerns::PrintsOptions(application.m_options, *this)
{}

const std::vector<PositionalArgument> &ListCommand::positionalArguments() const
{
    static const std::vector<PositionalArgument> cached {
        {namespace_, u"The namespace name"_s, {}, true},
    };

    return cached;
}

QList<CommandLineOption> ListCommand::optionsSignature() const
{
    return {
        {raw_, u"To output raw command list"_s},
    };
}

QString ListCommand::help() const
{
    return
uR"(  The <info>list</info> command lists all commands:

    <info>tom list</info>

  You can also display the commands for a specific namespace:

    <info>tom list test</info>

  It's also possible to get raw list of commands (useful for embedding command runner):

    <info>tom list --raw</info>)"_s;
}

int ListCommand::run()
{
    Command::run();

    const auto namespaceArg = argument(namespace_);

    return isSet(raw_) ? raw(namespaceArg) : full(namespaceArg);
}

/* protected */

int ListCommand::full(const QString &namespaceArg)
{
    application().printVersion();

    newLine();
    comment(u"Usage:"_s);
    note(u"  command [options] [--] [arguments]"_s);

    // Options section
    const auto optionsMaxSize = printOptionsSection(true);

    // Commands section
    printCommandsSection(getNamespaceName(namespaceArg), optionsMaxSize);

    return EXIT_SUCCESS;
}

int ListCommand::raw(const QString &namespaceArg)
{
    const auto &commands = getCommandsByNamespace(getNamespaceName(namespaceArg));

    const auto it = std::ranges::max_element(commands, std::less(),
                                             [](const auto &command)
    {
        return command->name().size();
    });

    const auto commandMaxSize = (*it)->name().size();

    for (const auto &command : commands) {
        const auto commandName = command->name();

        // Exclude hidden commands
        if (m_dontList.contains(commandName))
            continue;

        const auto indent = QString(commandMaxSize - commandName.size(), SPACE);

        note(u"%1%2   %3"_s.arg(commandName, indent, command->description()));
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
        return NsGlobal;

    // Try to find a full command name to avoid the guess logic
    if (auto namespaceArg_ = namespaceArg.toLower();
        ranges::contains(Application::namespaceNames(), namespaceArg_)
    )
        return namespaceArg_;

    // Invoke the guess logic 👌
    const auto namespaces = guessNamespace(namespaceArg);

    // Found exactly one namespace
    if (namespaces.size() == 1)
        return namespaces.front();

    // No namespace found
    if (namespaces.empty()) {
        errorWall(uR"(There are no commands defined in the "%1" namespace.)"_s
                  .arg(namespaceArg));

        Application::exitApplication(EXIT_FAILURE);
    }

    // Found more namespaces (ambiguous)
    printAmbiguousNamespaces(namespaceArg, namespaces);
}

std::vector<QString> ListCommand::guessNamespace(const QString &namespaceArg)
{
    return Application::namespaceNames()
            // CUR1 ranges, check all pred and proj, now I understand where I have to use auto & or auto &&, note in bash_or_cmd c++ sheet silverqx
            | ranges::views::filter([&namespaceArg](const QString &namespaceName)
    {
        return namespaceName.startsWith(namespaceArg, Qt::CaseInsensitive);
    })
            | ranges::to<std::vector<QString>>();
}

void ListCommand::printAmbiguousNamespaces(const QString &namespaceName,
                                           const std::vector<QString> &namespaces) const
{
    // Prepare namespaces for the errorWall()
    const auto formattedNamespaces = namespaces
            | ranges::views::transform([](const auto &ambiguousNsName)
    {
        return u"    %1"_s.arg(ambiguousNsName);
    })
            | ranges::to<QStringList>();

    errorWall(u"The namespace \"%1\" is ambiguous.\n\nDid you mean one of these?\n%2"_s
              .arg(namespaceName, formattedNamespaces.join(NEWLINE)));

    Application::exitApplication(EXIT_FAILURE);
}

/* Commands section */

void ListCommand::printCommandsSection(const QString &namespaceName,
                                       const PrintsOptions::SizeType optionsMaxSize) const
{
    const auto hasNamespaceName = !namespaceName.isNull();

    const auto &commands = getCommandsByNamespace(namespaceName);

    newLine();

    // Specific namespace
    if (hasNamespaceName)
                // Custom message for the namespaced argument
        comment(namespaceName == NsNamespaced
                ? u"Commands with the namespace prefix:"_s
                : u"Available commands for the '%1' namespace:"_s
                  .arg(namespaceName.isEmpty() ? NsGlobal : namespaceName));
    // All commands
    else
        comment(u"Available commands:"_s);

    // Get max. command size in all command names
    const auto commandsMaxSize = ListCommand::commandsMaxSize(commands, optionsMaxSize);
    // Print commands to the console
    printCommands(commands, commandsMaxSize, hasNamespaceName);
}

Concerns::PrintsOptions::SizeType
ListCommand::commandsMaxSize(const std::vector<std::shared_ptr<Command>> &commands,
                             const PrintsOptions::SizeType optionsMaxSize)
{
    const auto it = std::ranges::max_element(commands, std::less(),
                                             [](const auto &command)
    {
        return command->name().size();
    });

    const auto commandsMaxSize = (*it)->name().size();

    // Align commands' descriptions to the same level as options' descriptions
    return std::max(commandsMaxSize, optionsMaxSize);
}

void ListCommand::printCommands(
        const std::vector<std::shared_ptr<Command>> &commands,
        const PrintsOptions::SizeType commandsMaxSize, const bool hasNamespaceName) const
{
    // Currently rendering NS
    QString renderingNamespace;

    for (const auto &command : commands) {
        const auto commandName = command->name();

        // Exclude hidden commands
        if (m_dontList.contains(commandName))
            continue;

        // Begin a new namespace section
        tryBeginNsSection(renderingNamespace, commandName, hasNamespaceName);

        const auto indent = QString(commandsMaxSize - commandName.size(), SPACE);

        info(u"  %1%2"_s.arg(commandName, indent), false);

        note(u"  %1"_s.arg(command->description()));
    }
}

void ListCommand::tryBeginNsSection(
        QString &renderingNamespace, const QString &commandName,
        const bool hasNamespaceName) const
{
    const auto commandNamespace = ListCommand::commandNamespace(commandName);

    if (hasNamespaceName || commandNamespace == renderingNamespace)
        return;

    // Update currently rendering NS section
    renderingNamespace = commandNamespace;

    comment(u" %1"_s.arg(renderingNamespace));
}

QString ListCommand::commandNamespace(const QString &commandName)
{
    if (!commandName.contains(COLON))
        return {};

    return commandName.split(COLON).constFirst();
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
