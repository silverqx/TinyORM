#include "tom/concerns/guesscommandname.hpp"

#include <orm/constants.hpp>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

#include "tom/application.hpp"
#include "tom/commands/command.hpp"
#include "tom/types/guesscommandnametype.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::COLON;
using Orm::Constants::NEWLINE_C;
using Orm::Constants::SPACE;

using Tom::Constants::NsAll;
using Tom::Constants::NsGlobal;
using Tom::Constants::NsNamespaced;

using Tom::Commands::Command;

using enum Tom::GuessCommandNameResult;

namespace Tom::Concerns
{

/* protected */

/* For classic command guesser */

QString GuessCommandName::guessCommandName(const QString &name)
{
    const auto commands = guessCommandsInternal(name);

    // Found exactly one Tom command
    if (commands.size() == 1)
        return commands.front()->name();

    // No Tom command found
    if (commands.empty())
        return {};

    // Found more Tom commands (ambiguous)
    printAmbiguousCommands(name, commands);
}

/* For the complete command */

GuessCommandNameType
GuessCommandName::guessCommandNameForComplete(const QString &name)
{
    const auto commands = guessCommandsInternal(name);

    // Found exactly one Tom command
    if (commands.size() == 1)
        return {kFound, commands.front()->name()};

    // No Tom command found
    if (commands.empty())
        return {kNotFound, std::nullopt};

    // More Tom commands found
    return {kAmbiguous, std::nullopt};
}

std::vector<std::shared_ptr<Command>>
GuessCommandName::guessCommandsForComplete(const QString &name)
{
    const auto isNamespacedCmd = name.contains(COLON);

    // Here we need to print all Tom commands in all namespaces after <TAB> pressed
    return isNamespacedCmd ? guessCommandsWithNamespace(name)
                           : guessCommandsInAllNamespaces(name);
}

/* private */

/* For classic command guesser */

void GuessCommandName::printAmbiguousCommands(
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
    const auto formattedCommands = commands
            | ranges::views::transform([commandsMaxSize](const auto &command) -> QString
    {
        const auto commandName_ = command->name();
        const auto indent = QString(commandsMaxSize - commandName_.size() + 1, SPACE);

        return u"    %1%2%3"_s.arg(commandName_, indent, command->description());
    })
            | ranges::to<QStringList>();

    application().io().errorWall(
                u"Command \"%1\" is ambiguous.\n\nDid you mean one of these?\n%2"_s
                .arg(commandName, formattedCommands.join(NEWLINE_C)));

    Application::exitApplication(EXIT_FAILURE);
}

/* For the complete command */

std::vector<std::shared_ptr<Command>>
GuessCommandName::guessCommandsInAllNamespaces(const QString &commandName)
{
    return guessCommandsInNamespace(NsAll, commandName);
}

/* Common for both */

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

std::vector<std::shared_ptr<Command>>
GuessCommandName::guessCommandsInternal(const QString &name)
{
    const auto isGlobalCmd = !name.contains(COLON);

    /* The reason why the first is the guessCommandsWithoutNamespace() and not
       the guessCommandsInAllNamespaces() is that eg. the tom mig must match only
       the tom migrate, with the guessCommandsInAllNamespaces() it would match
       all migrate:xyz and it would be ambiguous. */
    return isGlobalCmd ? guessCommandsWithoutNamespace(name)
                       : guessCommandsWithNamespace(name);
}

/* Others */

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
