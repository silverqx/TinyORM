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
namespace Types
{
    struct GuessCommandNameType;
}

namespace Concerns
{

    /*! Try to guess the Tom command name by the given name (name can be partial). */
    class TINYORM_EXPORT GuessCommandName
    {
        Q_DISABLE_COPY_MOVE(GuessCommandName)

        /*! Alias for the Command. */
        using Command = Commands::Command;
        /*! Alias for the GuessCommandNameType. */
        using GuessCommandNameType = Types::GuessCommandNameType;

    public:
        /*! Pure virtual destructor. */
        inline virtual ~GuessCommandName() = 0;

    protected:
        /*! Default constructor. */
        GuessCommandName() = default;

        /* For classic command guesser */
        /*! Try to guess the command name by the given name (to create/invoke command). */
        QString guessCommandName(const QString &name); // The name can be partial

        /* For the complete command */
        /*! Try to guess one Tom command name by the given name (for complete logic). */
        GuessCommandNameType
        guessCommandNameForComplete(const QString &name); // The name can be partial

        /*! Try to guess all Tom commands by the given name (for printing to the cout). */
        std::vector<std::shared_ptr<Command>>
        guessCommandsForComplete(const QString &name); // The name can be partial

    private:
        /* For classic command guesser */
        /*! Print the error wall with ambiguous commands. */
        [[noreturn]] void
        printAmbiguousCommands(
                const QString &commandName,
                const std::vector<std::shared_ptr<Command>> &commands) const;

        /* For the complete command */
        /*! Try to guess all commands in all namespaces (also in the global namespace). */
        std::vector<std::shared_ptr<Command>>
        guessCommandsInAllNamespaces(const QString &commandName);

        /* Common for both */
        /*! Try to guess the namespaced Tom command. */
        std::vector<std::shared_ptr<Command>>
        guessCommandsWithNamespace(const QString &name);
        /*! Try to guess a Tom command in the global namespace (without namespace). */
        std::vector<std::shared_ptr<Command>>
        guessCommandsWithoutNamespace(const QString &commandName);

        /*! Try to guess a Tom command in the given namespace. */
        std::vector<std::shared_ptr<Command>>
        guessCommandsInNamespace(const QString &namespaceName,
                                 const QString &commandName);

        /*! Try to guess the Tom command name by the given name (common logic). */
        std::vector<std::shared_ptr<Command>>
        guessCommandsInternal(const QString &name);

        /* Others */
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
