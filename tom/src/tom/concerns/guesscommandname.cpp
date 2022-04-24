#include "tom/concerns/guesscommandname.hpp"

#include <orm/constants.hpp>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

#include "tom/application.hpp"
#include "tom/commands/command.hpp"

using Orm::Constants::COLON;
using Orm::Constants::NEWLINE;
using Orm::Constants::SPACE;

using Tom::Commands::Command;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Concerns
{

/* protected */

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
    const auto namespacedCommands = application().getCommandsInNamespace("namespaced");

    return namespacedCommands
            | ranges::views::filter([&name](const auto &command)
    {
        const auto nameSplitted = name.split(COLON);
        const auto commandNameSplitted = command->name().split(COLON);

        Q_ASSERT(nameSplitted.size() == 2 && commandNameSplitted.size() == 2);

        return commandNameSplitted[0].startsWith(nameSplitted[0], Qt::CaseInsensitive) &&
                commandNameSplitted[1].startsWith(nameSplitted[1], Qt::CaseInsensitive);
    })
            /* I have to materialize this view because namespacedCommands variable is
               destroyed after return, it's good enough 😎. */
            | ranges::to<std::vector<std::shared_ptr<Command>>>();
}

std::vector<std::shared_ptr<Command>>
GuessCommandName::guessCommandsWithoutNamespace(const QString &name)
{
    const auto globalCommands = application().getCommandsInNamespace("global");

    return globalCommands
            | ranges::views::filter([&name](const auto &command)
    {
        return command->name().startsWith(name, Qt::CaseInsensitive);
    })
            | ranges::to<std::vector<std::shared_ptr<Command>>>();
}

Q_NORETURN void
GuessCommandName::printAmbiguousCommands(
            const QString &commandName,
            const std::vector<std::shared_ptr<Command>> &commands) const
{
    const auto it = std::ranges::max_element(commands, std::less {},
                                             [](const auto &command)
    {
        return command->name().size();
    });
    const auto commandsMaxSize = static_cast<int>((*it)->name().size());

    // Prepare commands for the errorWall()
    auto formattedCommands =
            commands | ranges::views::transform([commandsMaxSize](const auto &command)
                                                -> QString
    {
        auto commandName = command->name();
        auto indent = QString(commandsMaxSize - commandName.size() + 1, SPACE);

        return QLatin1String("    %1%2%3")
                .arg(std::move(commandName), std::move(indent), command->description());
    })
            | ranges::to<QStringList>();

    application().errorWall(
                QLatin1String(
                    "Command \"%1\" is ambiguous.\n\nDid you mean one of these?\n%2")
                .arg(commandName, formattedCommands.join(NEWLINE)));

    application().exitApplication(EXIT_FAILURE);
}

/* private */

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
