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

    /*! Try to guess the command name by the given name (name can be partial).. */
    class SHAREDLIB_EXPORT GuessCommandName
    {
        Q_DISABLE_COPY(GuessCommandName)

        /*! Alias for the Command. */
        using Command = Commands::Command;

    public:
        /*! Virtual destructor. */
        inline virtual ~GuessCommandName() = default;

    protected:
        /*! Default constructor.*/
        inline GuessCommandName() = default;

        /*! Try to guess the command name by the given name (name can be partial). */
        QString guessCommandName(const QString &name);

        /*! Try to guess the namespaced command. */
        std::vector<std::shared_ptr<Command>>
        guessCommandsWithNamespace(const QString &name);
        /*! Try to guess a command in the global namespace (without namespace). */
        std::vector<std::shared_ptr<Command>>
        guessCommandsWithoutNamespace(const QString &name);

        /*! Print the error wall with ambiguous commands. */
        Q_NORETURN void
        printAmbiguousCommands(
                    const QString &commandName,
                    const std::vector<std::shared_ptr<Command>> &commands) const;

    private:
        /*! Get the tom application. */
        Application &application() noexcept;
        /*! Get the tom application, const version. */
        const Application &application() const noexcept;
    };

} // namespace Concerns
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_CONCERNS_GUESSCOMMANDNAME_HPP
