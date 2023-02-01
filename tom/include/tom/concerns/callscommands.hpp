#pragma once
#ifndef TOM_CONCERNS_CALLSCOMMANDS_HPP
#define TOM_CONCERNS_CALLSCOMMANDS_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QStringList>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{
namespace Commands
{
    class Command;
}

namespace Concerns
{

    /*! Invoke another command by name and passed arguments. */
    class CallsCommands
    {
        Q_DISABLE_COPY(CallsCommands)

    public:
        /*! Default constructor. */
        inline CallsCommands() = default;
        /*! Pure virtual destructor. */
        inline virtual ~CallsCommands() = 0;

        /*! Call another console command. */
        inline int call(const QString &command, QStringList &&arguments = {}) const;

    protected:
        /*! Run the given console command. */
        int runCommand(const QString &command, QStringList &&arguments) const;

        /*! Create command-line arguments from the given arguments. */
        static QStringList
        createCommandLineArguments(const QString &command, QStringList &&arguments,
                                   QStringList &&currentArguments);

        /*! Get common command-line arguments from current command-line arguments. */
        static QStringList getCommonArguments(QStringList &&arguments);

    private:
        /*! Static cast *this to the Command & derived type, const version. */
        const Commands::Command &command() const noexcept;
    };

    /* public */

    CallsCommands::~CallsCommands() = default;

    int CallsCommands::call(const QString &command, QStringList &&arguments) const
    {
        return runCommand(command, std::move(arguments));
    }

} // namespace Concerns
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_CONCERNS_CALLSCOMMANDS_HPP
