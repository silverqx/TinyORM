#pragma once
#ifndef TOM_COMMANDS_COMPLETE_PWSHCOMMAND_HPP
#define TOM_COMMANDS_COMPLETE_PWSHCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/complete/basecompletecommand.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{
namespace Types
{
    struct GuessCommandNameType;
}

namespace Commands::Complete
{

    /*! Complete command names and options in the pwsh shell (tab-completion only). */
    class PwshCommand : public BaseCompleteCommand
    {
        Q_DISABLE_COPY_MOVE(PwshCommand)

        /*! Alias for the GuessCommandNameType. */
        using GuessCommandNameType = Types::GuessCommandNameType;

    public:
        /*! Constructor. */
        PwshCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        ~PwshCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! Options signature of the console command. */
        QList<CommandLineOption> optionsSignature() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /* Current Tom command */
        /*! Cursor positions on the command-line. */
        enum TomCursorPostion : qint8
        {
            /*! Undefined cursor position (used as an initial value). */
            kUndefinedPosition = -1, // Must be -1!
            /*! Cursor is on the long/short option. */
            kOnOptionArgument  = -2,
            /*! Cursor is on the multi-value long option. */
//            kOnMultiValueOptionArgument = -3, // CUR1 finish, search all kOnOptionArgument, comments too silverqx
        };
        /*! Tom command positional argument position. */
        constexpr static qint8 TomCommandPosition = 1;

        /*! Get the currently processed Tom command. */
        GuessCommandNameType
        getCurrentTomCommand(const QStringList &commandlineArgSplitted,
                             ArgumentsSizeType argumentsCount) const;
        /*! Get the raw Tom command name (positional argument number 1). */
        static QString getRawTomCommandName(const QStringList &commandlineArgSplitted);

        /*! Get the Tom command (positional argument) position under the cursor (0-based)
            or kOnOptionArgument if the cursor is on the long/short option. */
        static ArgumentsSizeType
        getCurrentArgumentPosition(QStringView commandlineArg, const QString &wordArg,
                                   bool isNewArgumentPositionAtEnd);
        /*! Get positional arguments count on the command-line. */
        inline static ArgumentsSizeType
        getArgumentsCount(const QStringList &commandlineArgSplitted);

        /*! Get positional arguments count for all commands (from signature). */
        ArgumentsSizeType getMaxArgumentsCount();
        /*! Get positional arguments count for the given command (from signature). */
        ArgumentsSizeType getMaxArgumentsCount(const QString &command);

        /* Result output */
        /*! Print all or guessed section names for the --only= option of about command. */
        int printGuessedSectionNamesForAbout(QStringView sectionNamesArg) const;
        /*! Print all or guessed connection names for the --database= option. */
        int printGuessedConnectionNames(const QString &connectionNamesArg) const;
        /*! Print all or guessed environment names for the --env= option. */
        int printGuessedEnvironmentNames(const QString &environmentNameArg) const;

        /* Printing support */
        /*! Print the completion result. */
        inline void printCompletionResult(const QStringList &result) const;
        /*! Print completion result (used to print one space or block paths complete). */
        inline void printCompletionResult(const QString &result) const override;

        /*! Get the delimiter for result values (newline for pwsh). */
        QChar getResultDelimiter() const noexcept override;
        /*! Prepare one result value (argument or option) for printing. */
        inline QString
        prepareResultValue(
                QString &&completionText, const std::optional<QString> &listItemText,
                const QString &toolTip) const override;

        /*! Append short verbose options (-v,-vv,-vvv) to the completion result. */
        void appendShortVerboseOptions(QStringList &options,
                                       const QString &description) const override;

        /* Option arguments */
        /*! Get an option value for the --word= option (with workaround for pwsh). */
        QString getWordOptionValue(
                const QStringList &commandlineArgSplitted, QString::size_type positionArg,
                ArgumentsSizeType commandlineArgSize) const;

        /*! Return type for the initializePrintArrayOptionValues() method. */
        struct PrintArrayOptionValuesType
        {
            /*! Option value to guess/complete (given on the command-line). */
            QString lastOptionValueArg;
            /*! All values for printing (excluding already printed/completed values). */
            QList<QStringView> allValuesFiltered;
            /*! Determine whether guessing/printing the first value (need by pwsh). */
            bool isFirstOptionValue;
            /*! Print all option values? (if the current option value is empty) */
            bool printAllValues;
        };

        /*! Initialize local variables for printing and guessing multi-value options. */
        static PrintArrayOptionValuesType
        initializePrintArrayOptionValues(QStringView optionValuesArg,
                                         const QStringList &allValues);

        /*! Determine if the given word is a long option argument with multi-value. */
        static bool isLongOptionWithArrayValue(const QString &wordArg);
        /*! Determine whether the word is a long option argument with the given name. */
        inline static bool isLongOptionName(const QString &wordArg, const QString &name);
        /*! Get the value of the option argument (eg. --database=value). */
        static QString getOptionValue(const QString &wordArg);

        /*! Determine if the given command contains the given option (in signature). */
        bool commandHasLongOption(const QString &command, const QString &option);

        /*! Obtain all connection names tagged with // shell:connection comment
            (in the main.cpp). */
        static QStringList getConnectionNamesFromFile();

        /* Others */
        /*! Determine whether the given value is between min/max (inclusive) (alias). */
        inline static bool bw(ArgumentsSizeType value, ArgumentsSizeType min,
                                                       ArgumentsSizeType max) noexcept;

        /* Data members */
        /*! Is known/our or ambiguous Tom command on the command-line? (!kNotFound). */
        bool m_hasAnyTomCommand = false;
    };

    /* public */

    QString PwshCommand::name() const
    {
        return Constants::CompletePwsh;
    }

    QString PwshCommand::description() const
    {
        return u"Complete command names and options in the pwsh shell (tab-completion)"_s;
    }

    /* protected */

    /* Current Tom command */

    PwshCommand::ArgumentsSizeType
    PwshCommand::getArgumentsCount(const QStringList &commandlineArgSplitted)
    {
        return std::ranges::count_if(commandlineArgSplitted, [](const QString &argument)
        {
            return !isOptionArgument(argument);
        });
    }

    /* Printing support */

    void PwshCommand::printCompletionResult(const QStringList &result) const
    {
        printCompletionResult(result.join(getResultDelimiter()));
    }

    void PwshCommand::printCompletionResult(const QString &result) const
    {
        // Print a newline if the result is empty to block file/dir paths completion
        note(result, result.isEmpty());
    }

    QString
    PwshCommand::prepareResultValue(
            QString &&completionText, const std::optional<QString> &listItemText, // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
            const QString &toolTip) const
    {
        return Constants::TMPL_RESULT3
                .arg(completionText, listItemText.value_or(completionText), toolTip);
    }

    /* Option arguments */

    bool PwshCommand::isLongOptionName(const QString &wordArg, const QString &name)
    {
        return wordArg.startsWith(Constants::LongOptionEq.arg(name));
    }

    /* Others */

    bool PwshCommand::bw(
            const ArgumentsSizeType value, const ArgumentsSizeType min,
                                           const ArgumentsSizeType max) noexcept
    {
        return between(value, min, max);
    }

} // namespace Commands::Complete
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_COMPLETE_PWSHCOMMAND_HPP
