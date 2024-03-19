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

    public:
        /*! Constructor. */
        CompleteCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        ~CompleteCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! The signature of the console command. */
        QList<CommandLineOption> optionsSignature() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
#ifdef _MSC_VER
        /*! Currently processed tom command. */
        static std::optional<QString>
        getCurrentTomCommand(const QStringList &currentCommandSplitted);
#else
        /*! Currently processed tom command. */
        static std::optional<QString>
        getCurrentTomCommand(const QString &commandlineArg, QString::size_type cword);
#endif

        /*! Print all guessed commands. */
        int printGuessedCommands(
                const std::vector<std::shared_ptr<Command>> &commands) const;
        /*! Print all or guessed namespace names for the list command. */
        int printGuessedNamespaces(const QString &word) const;
        /*! Print all or guessed shell names for the integrate command. */
        int printGuessedShells(const QString &word) const;
        /*! Print all or guessed connection names for the --database= option. */
        int printAndGuessConnectionNames(const QString &connectionName) const;
        /*! Print all or guessed environment names for the --env= option. */
        int printEnvironmentNames(const QString &environmentName) const;
        /*! Print all or guessed long option parameter names. */
        int printGuessedLongOptions(const std::optional<QString> &currentCommand,
                                    const QString &word) const;
        /*! Print all or guessed short option parameter names. */
        int printGuessedShortOptions(const std::optional<QString> &currentCommand) const;

        /*! Get an option's default value part. */
        static QString getOptionDefaultValue(const QCommandLineOption &option);

        /*! Get options signature for the given command. */
        QList<CommandLineOption>
        getCommandOptionsSignature(const std::optional<QString> &command) const;

        /*! Option type (long/short). */
        enum struct OptionType : qint8
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
        /*! Get the command-line option value (--database=value). */
        static QString getOptionValue(const QString &wordArg);

        /*! Determine whether the given command contains the given option. */
        bool commandHasLongOption(const QString &command, const QString &option);

        /*! Obtain all connection names tagged with shell:connection comment from
            the main.cpp file. */
        static QStringList getConnectionNamesFromFile();

        /*! Commands to exclude from the list. */
        /*const*/ std::unordered_set<QString> m_dontList {
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
