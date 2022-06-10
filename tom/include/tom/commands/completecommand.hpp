#pragma once
#ifndef TOM_COMMANDS_COMPLETECOMMAND_HPP
#define TOM_COMMANDS_COMPLETECOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands
{

    /*! Complete command names and parameters (for tab-completion only). */
    class CompleteCommand : public Command
    {
        Q_DISABLE_COPY(CompleteCommand)

        /*! Alias for the Command. */
        using Command = Commands::Command;

    public:
        /*! Constructor. */
        CompleteCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        inline ~CompleteCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! The signature of the console command. */
        QList<QCommandLineOption> optionsSignature() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! Print all guessed commands. */
        int printGuessedCommands(std::vector<std::shared_ptr<Command>> &&commands) const;
        /*! Print all or guessed namespace names for the list command. */
        int printGuessedNamespaces(const QString &word) const;
        /*! Print all or guessed shell names for the integrate command. */
        int printGuessedShells(const QString &word) const;
        /*! Print all or guessed long option parameter names. */
        int printGuessedLongOptions(const QString &currentCommand,
                                    const QString &word) const;
        /*! Print all or guessed short option parameter names. */
        int printGuessedShortOptions(const QString &currentCommand) const;

        /*! Get an option's default value part. */
        static QString getOptionDefaultValue(const QCommandLineOption &option);
    };

    /* public */

    QString CompleteCommand::name() const
    {
        return Constants::Complete;
    }

    QString CompleteCommand::description() const
    {
        return QStringLiteral(
                    "Complete command names and parameters (for tab-completion only)");
    }

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_COMPLETECOMMAND_HPP
