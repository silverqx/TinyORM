#pragma once
#ifndef TOM_COMMANDS_COMPLETECOMMAND_HPP
#define TOM_COMMANDS_COMPLETECOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <unordered_set>

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
        /*! Currently proccessed tom command. */
        static std::optional<QString>
        getCurrentTomCommand(const QString &commandlineArg, QString::size_type cword);

        /*! Print all guessed commands. */
        int printGuessedCommands(std::vector<std::shared_ptr<Command>> &&commands) const;
        /*! Print all or guessed namespace names for the list command. */
        int printGuessedNamespaces(const QString &word) const;
        /*! Print all or guessed shell names for the integrate command. */
        int printGuessedShells(const QString &word) const;
        /*! Print all or guessed long option parameter names. */
        int printGuessedLongOptions(const std::optional<QString> &currentCommand,
                                    const QString &word) const;
        /*! Print all or guessed short option parameter names. */
        int printGuessedShortOptions(const std::optional<QString> &currentCommand) const;

        /*! Get an option's default value part. */
        static QString getOptionDefaultValue(const QCommandLineOption &option);

        /*! Get options signature for the given command. */
        QList<QCommandLineOption>
        getCommandOptionsSignature(const std::optional<QString> &command) const;

        /*! Option type (long/short). */
        enum struct OptionType
        {
            UNDEFINED = -1,
            LONG,
            SHORT,
        };
        constexpr static auto UNDEFINED = OptionType::UNDEFINED;
        constexpr static auto LONG      = OptionType::LONG;
        constexpr static auto SHORT     = OptionType::SHORT;

        /*! Determine whether the given word is an option argument. */
        static bool isOptionArgument(const QString &wordArg, OptionType type = UNDEFINED);
        /*! Determine whether the given word is a long option argument. */
        inline static bool isLongOption(const QString &wordArg);
        /*! Determine whether the given word is a short option argument. */
        inline static bool isShortOption(const QString &wordArg);

        /*! Commands to exclude from the list. */
        const std::unordered_set<QString> m_dontList {
            Tom::Constants::Complete,
        };
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

    /* protected */

    bool CompleteCommand::isLongOption(const QString &wordArg)
    {
        return isOptionArgument(wordArg, LONG);
    }

    bool CompleteCommand::isShortOption(const QString &wordArg)
    {
        return isOptionArgument(wordArg, SHORT);
    }

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_COMPLETECOMMAND_HPP
