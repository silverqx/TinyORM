#include "tom/concerns/guesscommandname.hpp"

#include <orm/constants.hpp>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

#include "tom/application.hpp"
#include "tom/commands/command.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::COLON;
using Orm::Constants::NEWLINE;
using Orm::Constants::SPACE;

using Tom::Commands::Command;
using Tom::Constants::NsAll;
using Tom::Constants::NsGlobal;
using Tom::Constants::NsNamespaced;

namespace Tom::Concerns
{

/* protected */

/* For classic command guesser */

QString GuessCommandName::guessCommandName(const QString &name)
{
    const auto isGlobalCmd = !name.contains(COLON);

    const auto commands = isGlobalCmd ? guessCommandsWithoutNamespace(name)
                                      : guessCommandsWithNamespace(name);

    // Found exactly one command
    if (commands.size() == 1)
        return commands.front()->name();

    // No command found
    if (commands.empty())
        return {};

    // Found more commands (ambiguous)
    printAmbiguousCommands(name, commands);
}

std::vector<std::shared_ptr<Command>>
GuessCommandName::guessCommandsWithNamespace(const QString &name)
{
    const auto namespacedCommands = application().getCommandsInNamespace(NsNamespaced);

    return namespacedCommands
            | ranges::views::filter([&name](const auto &command)
    {
        const auto nameSplitted = name.split(COLON);
        const auto commandNameSplitted = command->name().split(COLON);

        Q_ASSERT(nameSplitted.size() == 2 && commandNameSplitted.size() == 2);

        return commandNameSplitted.constFirst().startsWith(nameSplitted.constFirst(),
                                                           Qt::CaseInsensitive) &&
                commandNameSplitted[1].startsWith(nameSplitted[1], Qt::CaseInsensitive);
    })
            /* I have to materialize this view because namespacedCommands variable is
               destroyed after return, it's good enough 😎. */
            | ranges::to<std::vector<std::shared_ptr<Command>>>();
}

std::vector<std::shared_ptr<Command>>
GuessCommandName::guessCommandsWithoutNamespace(const QString &commandName)
{
    return guessCommandsInNamespace(NsGlobal, commandName);
}

Q_NORETURN void
GuessCommandName::printAmbiguousCommands(
            const QString &commandName,
            const std::vector<std::shared_ptr<Command>> &commands) const
{
    const auto it = std::ranges::max_element(commands, std::less(),
                                             [](const auto &command)
    {
        return command->name().size();
    });
    const auto commandsMaxSize = (*it)->name().size();

    // Prepare commands for the errorWall()
    auto formattedCommands = commands
            | ranges::views::transform([commandsMaxSize](const auto &command)
                                       -> QString
    {
        const auto commandName_ = command->name();
        const auto indent = QString(commandsMaxSize - commandName_.size() + 1, SPACE);

        return QStringLiteral("    %1%2%3").arg(commandName_, indent,
                                                command->description());
    })
            | ranges::to<QStringList>();

    application().errorWall(
                QStringLiteral(
                    "Command \"%1\" is ambiguous.\n\nDid you mean one of these?\n%2")
                .arg(commandName, formattedCommands.join(NEWLINE)));

    Application::exitApplication(EXIT_FAILURE);
}

/* For the complete command */

std::vector<std::shared_ptr<Command>>
GuessCommandName::guessCommandNamesForComplete(const QString &name)
{
    const auto isNamespacedCmd = name.contains(COLON);

    return isNamespacedCmd ? guessCommandsWithNamespace(name)
                           : guessCommandsInAllNamespaces(name);
}

std::vector<std::shared_ptr<Command>>
GuessCommandName::guessCommandsInAllNamespaces(const QString &commandName)
{
    return guessCommandsInNamespace(NsAll, commandName);
}

/* private */

/* Common */

std::vector<std::shared_ptr<GuessCommandName::Command>>
GuessCommandName::guessCommandsInNamespace(const QString &namespaceName,
                                           const QString &commandName)
{
    const auto allCommands = application().getCommandsInNamespace(namespaceName);

    return allCommands
            | ranges::views::filter([&commandName](const auto &command)
    {
        return commandName.isEmpty() ||
                command->name().startsWith(commandName, Qt::CaseInsensitive);
    })
            | ranges::to<std::vector<std::shared_ptr<Command>>>();
}

Application &GuessCommandName::application() noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
    return static_cast<Application &>(*this);
}

const Application &GuessCommandName::application() const noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
    return static_cast<const Application &>(*this);
}

} // namespace Tom::Concerns

TINYORM_END_COMMON_NAMESPACE
