#pragma once
#ifndef TOM_CONCERNS_GUESSCOMMANDNAME_HPP
#define TOM_CONCERNS_GUESSCOMMANDNAME_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QtGlobal>

#include <memory>
#include <vector>

#include <orm/macros/commonnamespace.hpp>
#include <orm/macros/export.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{
    class Application;

namespace Commands
{
    class Command;
}

namespace Concerns
{

    /*! Try to guess the command name by the given name (name can be partial). */
    class SHAREDLIB_EXPORT GuessCommandName
    {
        Q_DISABLE_COPY_MOVE(GuessCommandName)

        /*! Alias for the Command. */
        using Command = Commands::Command;

    public:
        /*! Pure virtual destructor. */
        inline virtual ~GuessCommandName() = 0;

    protected:
        /*! Default constructor. */
        GuessCommandName() = default;

        /* For classic command guesser */
        /*! Try to guess the command name by the given name (name can be partial). */
        QString guessCommandName(const QString &name);

        /*! Try to guess the namespaced command. */
        std::vector<std::shared_ptr<Command>>
        guessCommandsWithNamespace(const QString &name);
        /*! Try to guess a command in the global namespace (without namespace). */
        std::vector<std::shared_ptr<Command>>
        guessCommandsWithoutNamespace(const QString &commandName);

        /*! Print the error wall with ambiguous commands. */
        Q_NORETURN void
        printAmbiguousCommands(
                    const QString &commandName,
                    const std::vector<std::shared_ptr<Command>> &commands) const;

        /* For the complete command */
        /*! Try to guess all command names by the given name (name can be partial). */
        std::vector<std::shared_ptr<Command>>
        guessCommandNamesForComplete(const QString &name);

        /*! Try to guess all commands in all namespaces (also in the global namespace). */
        std::vector<std::shared_ptr<Command>>
        guessCommandsInAllNamespaces(const QString &commandName);

    private:
        /* Common */
        /*! Try to guess a command in the given namespace. */
        std::vector<std::shared_ptr<Command>>
        guessCommandsInNamespace(const QString &namespaceName,
                                 const QString &commandName);

        /*! Get the tom application. */
        Application &application() noexcept;
        /*! Get the tom application, const version. */
        const Application &application() const noexcept;
    };

    /* public */

    GuessCommandName::~GuessCommandName() = default;

} // namespace Concerns
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_CONCERNS_GUESSCOMMANDNAME_HPP
