#pragma once
#ifndef TOM_COMMANDS_COMPLETE_BASECOMPLETECOMMAND_HPP
#define TOM_COMMANDS_COMPLETE_BASECOMPLETECOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/complete/completetypes.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Complete
{

    /*! Complete command names and options (for tab-completion only). */
    class BaseCompleteCommand : public Command
    {
        Q_DISABLE_COPY_MOVE(BaseCompleteCommand)

    public:
        /*! Constructor. */
        BaseCompleteCommand(Application &application, QCommandLineParser &parser);
        /*! Pure virtual destructor. */
        inline ~BaseCompleteCommand() override = 0;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! Get context for tab-completion. */
        inline const CompleteContext &context() const;

        /* Current Tom command */
        /*! Alias for the GuessedCommandName. */
        using GuessedCommandName = Types::GuessedCommandName;
        /*! Expose the TomCursorPostion enum. */
        using enum TomCursorPostion;

        /*! Get the currently processed Tom command. */
        GuessedCommandName getCurrentTomCommand(SizeType argumentsCount) const;
        /*! Get the raw Tom command name (positional argument number 1). */
        QStringView getRawTomCommandName() const;

        /* Positional arguments */
        /*! Get the number of positional arguments on the command-line. */
        inline SizeType getArgumentsCount() const;

        /*! Get max. number of positional arguments for all commands (from signature). */
        SizeType getMaxArgumentsCount(bool hasAnyTomCommand) const;
        /*! Get number of positional arguments for the given command (from signature). */
        SizeType getMaxArgumentsCount(const QString &command,
                                      bool hasAnyTomCommand = true) const;

        /* Complete conditions */
        /*! Print all commands after the tom command itself or for the help command. */
        inline bool completeAllCommands() const;
        /*! Print all guessed commands after tom.exe by the --word= option (in context) or
            for the help command. */
        inline bool completeCommand() const;
        /*! Print all or guessed namespace names for the list command. */
        inline bool completeList_NamespacesArgument() const;
        /*! Print all or guessed shell names for the integrate command. */
        inline bool completeIntegrate_ShellsArgument() const;
        /*! Print all or guessed section names for --only= option of about command . */
        inline bool completeAbout_OnlyOption() const;
        /*! Print all or inferred database connection names for the --database= option. */
        inline bool completeDatabaseOption() const;
        /*! Print all or guessed environment names for the --env= option. */
        inline bool completeEnvOption() const;
        /*! Print file/dir paths completion for the --path= option. */
        // inline bool completePathOption() const;
        /*! Print all or guessed long option parameter names. */
        inline bool completeLongOptions() const;
        /*! Print all or guessed short option parameter names. */
        inline bool completeShortOptions() const;

        /* Result output */
        /*! Print all guessed commands. */
        int printGuessedCommands() const;
        /*! Print all or guessed namespace names for the list command. */
        int printGuessedNamespaces() const;
        /*! Print all or guessed shell names for the integrate command. */
        int printGuessedShells() const;

        /*! Print all or guessed long option parameter names. */
        int printGuessedLongOptions() const;
        /*! Print all or guessed short option parameter names. */
        int printGuessedShortOptions() const;

        /* Printing support */
        /*! Print completion result (used to print one space or block paths complete). */
        inline virtual int printCompletionResult(const QString &result) const;

        /*! Prepare one result value (argument or option) for printing (shortcut). */
        inline virtual QString
        prepareResultValue(
                QString &&completionText, const std::optional<QString> &listItemText,
                const QString &toolTip) const = 0;

        /*! Append short verbose options (-v,-vv,-vvv) to the tab-completion result. */
        virtual void appendShortVerboseOptions(QStringList &options,
                                               const QString &description) const = 0;

        /* Option arguments */
        /*! Get option arguments signature for the given command. */
        QList<CommandLineOption> getCommandOptionsSignature() const;

        /*! Expose the OptionType enum. */
        using enum OptionType;

        /*! Determine whether the given word is an option argument. */
        static bool isOptionArgument(QStringView wordArg,
                                     OptionType type = kAnyOption);
        /*! Determine whether the given word is a long option argument. */
        inline static bool isLongOption(QStringView wordArg);
        /*! Determine whether the given word is a short option argument. */
        inline static bool isShortOption(QStringView wordArg);
        /*! Determine whether the word is a long option argument with the given name
            (like --only= (with equal sign!), used by multi-value options logic only). */
        inline static bool isLongOptionName(QStringView wordArg, const QString &name);

        /*! Determine if the given command contains the given option (in signature). */
        bool commandHasLongOption(const QString &optionName) const;

        /* Others */
        /*! Determine whether the given value is between min/max (inclusive). */
        inline static bool between(SizeType value, SizeType min, SizeType max) noexcept;
        /*! Determine whether the given value is between min/max (inclusive) (alias). */
        inline static bool bw(SizeType value, SizeType min, SizeType max) noexcept;

        /*! Validate the required option arguments (checks isSet()). */
        virtual void validateInputOptions() const;
        /*! Validate the required option values. */
        virtual void validateInputOptionValues() const;

        /* Data members */
        /*! The entire current command-line (to be able to use QStringView). */
        QString m_commandlineArg;
        /*! The current word that is being completed (to be able to use QStringView). */
        QString m_wordArg;

        /*! The entire current command-line split using the SPACE character. */
        QList<QStringView> m_commandlineArgSplit;

    private:
        /*! Initialize context for tab-completion. */
        virtual CompleteContext initializeCompletionContext() = 0;

        /* Printing support */
        /*! Print the tab-completion result. */
        inline int printCompletionResult(const QStringList &result) const;

        /*! Prepare one result value (argument or option) for printing (shortcut). */
        inline QString
        prepareResultValue(QString &&completionText, const QString &toolTip) const;

        /* Option arguments */
        /*! Get an option's default value part. */
        static QString getOptionDefaultValue(const QCommandLineOption &option);

        /* Data members */
        /*! Context for tab-completion. */
        std::optional<CompleteContext> m_context = std::nullopt;
    };

    /* public */

    BaseCompleteCommand::~BaseCompleteCommand() = default;

    /* protected */

    const CompleteContext &BaseCompleteCommand::context() const
    {
        return m_context.value(); // NOLINT(bugprone-unchecked-optional-access)
    }

    /* Positional arguments */

    SizeType BaseCompleteCommand::getArgumentsCount() const
    {
        return std::ranges::count_if(m_commandlineArgSplit,
                                     [](const QStringView argument)
        {
            return !isOptionArgument(argument);
        });
    }

    /* Complete conditions */

    bool BaseCompleteCommand::completeAllCommands() const
    {
        const auto &[
                guessedTomCommand, wordArg, argumentsCount, currentArgumentPosition,
                _1, _2, _3, _4
        ] = context();

        const auto helpMaxArguments = getMaxArgumentsCount(Constants::Help);

        return wordArg.isEmpty() &&
                ((currentArgumentPosition == ArgTomCommand &&
                  bw(argumentsCount, 1, helpMaxArguments)) || // tom | ; tom | xyz
                 (guessedTomCommand == Constants::Help &&
                  argumentsCount == 2 && currentArgumentPosition == Arg2)); // tom help |
    }

    bool BaseCompleteCommand::completeCommand() const
    {
        const auto &[
                guessedTomCommand, wordArg, argumentsCount, currentArgumentPosition,
                maxArgumentsCount, _1, _2, _3
        ] = context();

        return !wordArg.isEmpty() && !isOptionArgument(wordArg) &&
                ((bw(argumentsCount, 2, maxArgumentsCount) &&  // tom db:se|           ; tom db:see|d           ; tom db:seed|
                  currentArgumentPosition == ArgTomCommand) || // tom db:se| XyzSeeder ; tom db:see|d XyzSeeder ; tom db:seed| XyzSeeder
                 (guessedTomCommand == Constants::Help && argumentsCount == 3 &&
                  (currentArgumentPosition == ArgTomCommand || // tom he| a   ; tom hel|p a ; tom help| a
                   currentArgumentPosition == Arg2)));         // tom help a| ; tom help ab|out
    }

    bool BaseCompleteCommand::completeList_NamespacesArgument() const
    {
        const auto &[
                guessedTomCommand, wordArg, argumentsCount, currentArgumentPosition,
                _1, _2, _3, _4
        ] = context();

        const auto listMaxArguments = getMaxArgumentsCount(Constants::List);

        return guessedTomCommand == Constants::List && !isOptionArgument(wordArg) &&
                currentArgumentPosition == Arg2 &&
                (( wordArg.isEmpty() && argumentsCount == 2) ||               // tom list |
                 (!wordArg.isEmpty() && argumentsCount == listMaxArguments)); // tom list g| ; tom list gl|obal
    }

    bool BaseCompleteCommand::completeIntegrate_ShellsArgument() const
    {
        const auto &[
                guessedTomCommand, wordArg, argumentsCount, currentArgumentPosition,
                _1, _2, _3, _4
        ] = context();

        const auto integrateMaxArguments = getMaxArgumentsCount(Constants::Integrate);

        return guessedTomCommand == Constants::Integrate && !isOptionArgument(wordArg) &&
                currentArgumentPosition == Arg2 &&
                (( wordArg.isEmpty() && argumentsCount == 2) ||                    // tom integrate |
                 (!wordArg.isEmpty() && argumentsCount == integrateMaxArguments)); // tom integrate p| ; tom integrate p|wsh
    }

    bool BaseCompleteCommand::completeAbout_OnlyOption() const
    {
        const auto &[
                guessedTomCommand, wordArg, argumentsCount, currentArgumentPosition,
                _1, _2, isNewArgPositionAtEnd, _3
        ] = context();

        return guessedTomCommand == Constants::About &&
                isLongOptionName(wordArg, Constants::only_) &&
                argumentsCount == 2 && currentArgumentPosition == kOnOptionArgument && // tom about --only=m| ; tom about --only=m|acros
                !isNewArgPositionAtEnd; // < : tom about --only=| --ansi ; = : tom about --only=|
    }

    bool BaseCompleteCommand::completeDatabaseOption() const
    {
        const auto &[
                guessedTomCommand, wordArg, argumentsCount, currentArgumentPosition,
                maxArgumentsCount, hasAnyTomCommand, isNewArgPositionAtEnd, _1
        ] = context();

        using enum Tom::GuessedCommandNameResult;

        const auto dbSeedMaxArgumentsCount = getMaxArgumentsCount(Constants::DbSeed,
                                                                  hasAnyTomCommand);

        return isLongOptionName(wordArg, Constants::database_) &&
                ((guessedTomCommand == kFound &&
                  bw(argumentsCount, 2, maxArgumentsCount)) ||
                 /* db:seed is the only command that has positional argument,
                    all other commands with the --database= option don't have any. */
                 (guessedTomCommand == Constants::DbSeed && // tom db:seed Xyz --database=|
                  bw(argumentsCount, 2, dbSeedMaxArgumentsCount))) &&
                currentArgumentPosition == kOnOptionArgument && // tom migrate --database=t| ; tom migrate --database=tiny|orm_tom_mysql
                !isNewArgPositionAtEnd && // < : tom migrate --database=| --ansi ; = : tom migrate --database=|
                guessedTomCommand && commandHasLongOption(Constants::database_); // All migrate/: and db: commands have the --database= option
    }

    bool BaseCompleteCommand::completeEnvOption() const
    {
        const auto &[
                _1, wordArg, argumentsCount, currentArgumentPosition, maxArgumentsCount,
                hasAnyTomCommand, isNewArgPositionAtEnd, _2
        ] = context();

        return isLongOptionName(wordArg, Constants::Env) &&
                currentArgumentPosition == kOnOptionArgument &&
                !isNewArgPositionAtEnd &&
                (argumentsCount == 1 || // tom --env=| ; tom --env=d| ; tom --env=d|ev (argumentsCount == 1 implies the kNotFound aka !guessedTomCommand)
                  // Don't print/complete the --env= option for unknown commands eg. tom xyz --|
                 (hasAnyTomCommand && bw(argumentsCount, 2, maxArgumentsCount))); // kFound : tom migrate --env=| ; tom migrate --env=d| ; tom db:seed --env=d|ev Xyz
                                                                                  // kAmbiguous : tom migrate:re --env=| (migrate:refresh or migrate:reset)
    }

//    bool BaseCompleteCommand::completePathOption() const
//    {
//        const auto &[
//                guessedTomCommand, wordArg, argumentsCount, currentArgumentPosition,
//                maxArgumentsCount, _1, isNewArgPositionAtEnd, _2
//        ] = context();

//        /* This could provide directory path completion for all commands with the --path=
//           option, but pwsh isn't able to complete file/dir paths after --path=| (after
//           long options that end with the = character). I leave it here, maybe it will be
//           supported in future versions. */
//        return (guessedTomCommand == Integrate || guessedTomCommand == MakeMigration ||
//                guessedTomCommand == MakeModel || guessedTomCommand == MakeSeeder) &&
//                isLongOptionName(wordArg, path_) &&
//                currentArgumentPosition == kOnOptionArgument && // tom integrate --path=|
//                !isNewArgPositionAtEnd && // < : tom integrate --path=| --ansi ; = : tom integrate --path=|
//                bw(argumentsCount, 2, maxArgumentsCount);
//    }

    bool BaseCompleteCommand::completeLongOptions() const
    {
        const auto &[
                _1, wordArg, argumentsCount, currentArgumentPosition, maxArgumentsCount,
                hasAnyTomCommand, isNewArgPositionAtEnd, _2
        ] = context();

        return isLongOption(wordArg) && currentArgumentPosition == kOnOptionArgument &&
                !isNewArgPositionAtEnd &&
                (argumentsCount == 1 || // tom --| ; tom --e| (argumentsCount == 1 implies the kNotFound aka !guessedTomCommand)
                  // Don't print/complete options for unknown commands eg. tom xyz --|
                 (hasAnyTomCommand && bw(argumentsCount, 2, maxArgumentsCount))); // kFound : tom list --| ; tom list --r| ; tom list --r|aw
                                                                                  // kAmbiguous : tom i --| ; tom i --a| (inspire or integrate)
    }

    bool BaseCompleteCommand::completeShortOptions() const
    {
        const auto &[
                _1, wordArg, argumentsCount, currentArgumentPosition, maxArgumentsCount,
                hasAnyTomCommand, isNewArgPositionAtEnd, _2
        ] = context();

        return isShortOption(wordArg) && currentArgumentPosition == kOnOptionArgument &&
                !isNewArgPositionAtEnd &&
                (argumentsCount == 1 || // tom -| ; tom -e| (argumentsCount == 1 implies the kNotFound aka !guessedTomCommand)
                  // Don't print/complete options for unknown commands eg. tom xyz -|
                 (hasAnyTomCommand && bw(argumentsCount, 2, maxArgumentsCount))); // kFound : tom list -| ; tom list -h|
                                                                                  // kAmbiguous : tom i -| ; tom i -h| (inspire or integrate)
    }

    /* Printing support */

    int BaseCompleteCommand::printCompletionResult(const QString &result) const
    {
        note(result, false);

        // For nicer/shorter if() conditions, to be able return right away
        return EXIT_SUCCESS;
    }

    /* Option arguments */

    bool BaseCompleteCommand::isLongOption(const QStringView wordArg)
    {
        return isOptionArgument(wordArg, kLongOption);
    }

    bool BaseCompleteCommand::isShortOption(const QStringView wordArg)
    {
        return isOptionArgument(wordArg, kShortOption);
    }

    bool
    BaseCompleteCommand::isLongOptionName(const QStringView wordArg, const QString &name)
    {
        return wordArg.startsWith(Constants::LongOptionEq.arg(name));
    }

    /* Others */

    bool BaseCompleteCommand::between(const SizeType value, const SizeType min,
                                                            const SizeType max) noexcept
    {
        return value >= min && value <= max;
    }

    /* private */

    /* Printing support */

    int BaseCompleteCommand::printCompletionResult(const QStringList &result) const
    {
        return printCompletionResult(result.join(QChar::LineFeed));
    }

    QString
    BaseCompleteCommand::prepareResultValue(QString &&completionText,
                                            const QString &toolTip) const
    {
        return prepareResultValue(std::move(completionText), std::nullopt, toolTip);
    }

    bool BaseCompleteCommand::bw(const SizeType value, const SizeType min,
                                                       const SizeType max) noexcept
    {
        return between(value, min, max);
    }

} // namespace Tom::Commands::Complete

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_COMPLETE_BASECOMPLETECOMMAND_HPP
