#pragma once
#ifndef TOM_CONCERNS_CONFIRMABLE_HPP
#define TOM_CONCERNS_CONFIRMABLE_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <functional>

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

    /*! Prints alert and asks for the confirmation (Y/N). */
    class Confirmable
    {
        Q_DISABLE_COPY(Confirmable)

        /*! Alias for the Command. */
        using Command = Commands::Command;

    public:
        /*! Constructor (int param. to avoid interpret it as copy ctor). */
        Confirmable(Command &command, int /*unused*/);
        /*! Virtual destructor. */
        inline virtual ~Confirmable() = default;

        /*! Confirm before proceeding with the action (only in production environment). */
        bool confirmToProceed(
                    const QString &warning = QStringLiteral("Application In Production!"),
                    const std::function<bool()> &callback = nullptr) const;

    protected:
        /*! Get the default confirmation callback. */
        std::function<bool()> defaultConfirmCallback() const;

        /*! Reference to a command that should be confimable. */
        std::reference_wrapper<Command> m_command;
    };

} // namespace Concerns
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_CONCERNS_CONFIRMABLE_HPP
