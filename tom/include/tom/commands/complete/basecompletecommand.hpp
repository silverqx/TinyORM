#pragma once
#ifndef TOM_COMMANDS_COMPLETE_BASECOMPLETECOMMAND_HPP
#define TOM_COMMANDS_COMPLETE_BASECOMPLETECOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"

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
        ~BaseCompleteCommand() override = 0;

        /*! Execute the console command. */
        int run() override;

    protected:
        /* Result output */
        /*! Print all guessed commands. */
        int printGuessedCommands(
                const std::vector<std::shared_ptr<Command>> &commands) const;
        /*! Print all or guessed namespace names for the list command. */
        int printGuessedNamespaces(const QString &wordArg) const;
        /*! Print all or guessed shell names for the integrate command. */
        int printGuessedShells(const QString &wordArg) const;

        /*! Print all or guessed long option parameter names. */
        int printGuessedLongOptions(const std::optional<QString> &currentCommand,
                                    const QString &wordArg) const;
        /*! Print all or guessed short option parameter names. */
        int printGuessedShortOptions(const std::optional<QString> &currentCommand) const;

        /* Printing support */
        /*! Print the completion result. */
        inline void printCompletionResult(const QStringList &result) const;
        /*! Print completion result (used to print one space or block paths complete). */
        inline virtual void printCompletionResult(const QString &result) const;
        /*! Get the delimiter for result values (newline for pwsh and space for bash). */
        virtual QChar getResultDelimiter() const noexcept = 0;

        /*! Prepare one result value (argument or option) for printing (shortcut). */
        inline QString
        prepareResultValue(QString &&completionText, const QString &toolTip) const;
        /*! Prepare one result value (argument or option) for printing (shortcut). */
        inline virtual QString
        prepareResultValue(
                QString &&completionText, const std::optional<QString> &listItemText,
                const QString &toolTip) const = 0;

        /*! Append short verbose options (-v,-vv,-vvv) to the completion result. */
        virtual void appendShortVerboseOptions(QStringList &options,
                                               const QString &description) const = 0;

        /* Option arguments */
        /*! Get an option's default value part. */
        static QString getOptionDefaultValue(const QCommandLineOption &option);

        /*! Get option arguments signature for the given command. */
        QList<CommandLineOption>
        getCommandOptionsSignature(const std::optional<QString> &command) const;

        /*! Option type (long/short). */
        enum struct OptionType : quint8
        {
            /*! Consider/match both long and short option arguments. */
            kAnyOption,
            /*! Long option argument. */
            kLongOption,
            /*! Short option argument. */
            kShortOption,
        };
        /*! Expose the OptionType enum. */
        using enum OptionType;

        /*! Determine whether the given word is an option argument. */
        static bool isOptionArgument(const QString &wordArg,
                                     OptionType type = kAnyOption);
        /*! Determine whether the given word is a long option argument. */
        inline static bool isLongOption(const QString &wordArg);
        /*! Determine whether the given word is a short option argument. */
        inline static bool isShortOption(const QString &wordArg);

        /* Others */
        /*! Determine whether the given value is between min/max (inclusive). */
        inline static bool
        between(ArgumentsSizeType value, ArgumentsSizeType min,
                                         ArgumentsSizeType max) noexcept;
    };

    /* protected */

    /* Printing support */

    void BaseCompleteCommand::printCompletionResult(const QStringList &result) const
    {
        printCompletionResult(result.join(getResultDelimiter()));
    }

    void BaseCompleteCommand::printCompletionResult(const QString &result) const
    {
        note(result, false);
    }

    QString
    BaseCompleteCommand::prepareResultValue(QString &&completionText,
                                            const QString &toolTip) const
    {
        return prepareResultValue(std::move(completionText), std::nullopt, toolTip);
    }

    bool BaseCompleteCommand::isLongOption(const QString &wordArg)
    {
        return isOptionArgument(wordArg, kLongOption);
    }

    bool BaseCompleteCommand::isShortOption(const QString &wordArg)
    {
        return isOptionArgument(wordArg, kShortOption);
    }

    /* Others */

    bool BaseCompleteCommand::between(
            const ArgumentsSizeType value, const ArgumentsSizeType min,
                                           const ArgumentsSizeType max) noexcept
    {
        return value >= min && value <= max;
    }

} // namespace Tom::Commands::Complete

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_COMPLETE_BASECOMPLETECOMMAND_HPP
