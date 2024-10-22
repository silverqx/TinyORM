#pragma once
#ifndef TOM_COMMANDS_COMPLETE_PWSHCOMMAND_HPP
#define TOM_COMMANDS_COMPLETE_PWSHCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/complete/basecompletecommand.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Complete
{

    /*! Complete command names and options in the pwsh shell (tab-completion only). */
    class PwshCommand : public BaseCompleteCommand
    {
        Q_DISABLE_COPY_MOVE(PwshCommand)

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

    private:
        /* Prepare Context */
        /*! Initialize context for tab-completion. */
        CompleteContext initializeCompletionContext() override;

        /* Context - Positional arguments */
        /*! Get the Tom command (positional argument) position under the cursor (0-based)
            or kOnOptionArgument if the cursor is on the long/short option. */
        SizeType
        getCurrentArgumentPosition(QStringView commandlineArg, QStringView wordArg) const;

        /*! Get the entire command-line before the cursor. */
        inline QStringView getCommadlineBeforeCursor() const;

        /* Context - Multi-value Option arguments */
        /*! Get an option value for the --word= option (with workaround for pwsh). */
        MultiValueOptionType getWordArgOptionValue() const;

        /*! Find a current word under the cursor (workaround for multi-value options). */
        MultiValueOptionType findCurrentWord() const;
        /*! Compute a multi-value option value position without the long option prefix,
            eg. for --only=env,mac|ros,versions the position will be 7 (0-based). */
        inline SizeType
        computeMultiValueOptionPosition(QStringView currentWord,
                                        SizeType currentWordPostion) const;

        /*! Determine if the given word is a long option argument with multi-value. */
        static bool isLongOptionWithArrayValue(QStringView wordArg);

        /* Result output */
        /*! Print all or guessed section names for the --only= option of about command. */
        int printGuessedSectionNamesForAbout() const;
        /*! Print all or guessed connection names for the --database= option. */
        int printGuessedConnectionNames() const;
        /*! Print all or guessed environment names for the --env= option. */
        int printGuessedEnvironmentNames() const;

        /* Printing support */
        /*! Print completion result (used to print one space or block paths complete). */
        inline int printCompletionResult(const QString &result) const override;
        /*! Print the tab-completion result. */
        inline int printCompletionResult(const QStringList &result) const;

        /*! Print one space (used by multi-value options). */
        [[noreturn]] void printOneSpace() const;
        /*! Block the tab-completion and print nothing. */
        [[noreturn]] void blockCompletion() const;

        /*! Prepare one result value (argument or option) for printing. */
        inline QString
        prepareResultValue(
                QString &&completionText, const std::optional<QString> &listItemText,
                const QString &toolTip) const override;

        /*! Append short verbose options (-v,-vv,-vvv) to the tab-completion result. */
        void appendShortVerboseOptions(QStringList &options,
                                       const QString &description) const override;

        /* Printing - Multi-value Option arguments */
        /*! Initialize local variables for printing and guessing multi-value options. */
        PrintMultiValueOptionType
        initializePrintMultiValueOption(const QStringList &allValues) const;

        /*! Get the value/s before the cursor of the multi-value option argument. */
        inline QStringView
        getOptionValuesBeforeCursor(QStringView optionValuesArg) const;
        /*! Get the last character position for the value/s before the cursor (multi). */
        SizeType
        getOptionValuesLastPosition(QStringView optionValuesArg) const;

        /*! Determine whether to append a comma after the multi-value option value. */
        inline bool
        shouldAppendComma(
                SizeType optionValuesArgSplitSize, bool isCommaUnderCursor,
                bool isFirstOptionValue, bool isLastOptionValue) const;
        /*! Determine if there is a comma under the cursor (multi). */
        bool isCommaUnderCursor(QStringView optionValuesArg,
                                SizeType optionValuesArgSize) const;

        /*! Filter out option values that are already completed on the command-line. */
        static QList<QStringView>
        filterOptionValues(const QStringList &allValues,
                           const QList<QStringView> &optionValues);

        /*! Prepare one completion result value for a multi-value option. */
        static QString
        prepareMultiValueResult(
                const QString &optionName, QStringView value, bool isFirstOptionValue,
                bool appendComma, bool appendSpace);

        /* Option arguments */
        /*! Get the value of the option argument (eg. --database=value). */
        static QStringView getOptionValue(QStringView wordArg);

        /*! Obtain all connection names tagged with // shell:connection comment
            (in the main.cpp). */
        static QStringList getConnectionNamesFromFile();

        /* Others */
        /*! Validate the required option arguments (check isSet()). */
        void validateInputOptions() const override;
        /*! Validate the required option values. */
        void validateInputOptionValues() const override;

        /*! Get cursor position after the tom executable including the space (0-based). */
        SizeType getCursorPositionAfterExecutable() const;

        /* Data members */
        /*! The current position of the cursor on the command-line (0-based). */
        SizeType m_positionArg = kUndefinedPosition;
        /*! Size of the --commandline= option value. */
        SizeType m_commandlineArgSize = -1;
        /*! Determine if the cursor is at the end on the command-line, in this case
            positionArg > commandlineArgSize (this is pwsh specific). */
        bool m_isNewArgPositionAtEnd = false;
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

    /* private */

    /* Context - Positional arguments */

    QStringView PwshCommand::getCommadlineBeforeCursor() const
    {
        return {m_commandlineArg.constData(),
                m_isNewArgPositionAtEnd ? m_positionArg -
                                         (m_positionArg - m_commandlineArgSize)
                                        : m_positionArg};
    }

    /* Context - Option arguments */

    SizeType
    PwshCommand::computeMultiValueOptionPosition(const QStringView currentWord,
                                                 const SizeType currentWordPostion) const
    {
        /* Find the first character position after the =; it's the same like:
           m_positionArg - currentWordPostion - indexOf(EQ_C) - 1. */
        const auto equalSignPosition = currentWord.indexOf(Constants::EQ_C) + 1;
        const auto position = m_positionArg - currentWordPostion - equalSignPosition;

        /* Fix a special case where a tab hit happened at the beginning of a multi-value
           option like |--only=versions, (in this case complete nothing). */
        if (position == equalSignPosition * -1)
            blockCompletion(); // [[noreturn]]

        return position;
    }

    /* Printing support */

    int PwshCommand::printCompletionResult(const QString &result) const
    {
        // Print a newline if the result is empty to block file/dir paths completion
        note(result, result.isEmpty());

        // For nicer/shorter if() conditions, to be able return right away
        return EXIT_SUCCESS;
    }

    int PwshCommand::printCompletionResult(const QStringList &result) const
    {
        return printCompletionResult(result.join(QChar::LineFeed));
    }

    QString
    PwshCommand::prepareResultValue(
            QString &&completionText, const std::optional<QString> &listItemText, // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
            const QString &toolTip) const
    {
        return Constants::TMPL_RESULT3
                .arg(completionText, listItemText.value_or(completionText), toolTip);
    }

    /* Printing - Multi-value Option arguments */

    QStringView
    PwshCommand::getOptionValuesBeforeCursor(const QStringView optionValuesArg) const
    {
        const auto *const optionValuesArgData = optionValuesArg.constData();

        // The last character index must point after the last character for QStringView
        return {optionValuesArgData,
                optionValuesArgData + getOptionValuesLastPosition(optionValuesArg)}; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }

    bool PwshCommand::shouldAppendComma(
            const SizeType optionValuesArgSplitSize, const bool isCommaUnderCursor,
            const bool isFirstOptionValue, const bool isLastOptionValue) const
    {
        /* Append only in the following special case (in this case the m_word is empty):
           tom about --only=env,|versions,macros
                     --only=env,connections,¦versions,macros
           Also, don't use the context().wordArg here because it's already processed,
           the m_wordArg is the raw/original value passed on the command-line. */
        return m_wordArg.isEmpty() &&
               /* Should only append for values in the Middle, so there must be at least
                  two values. */
               optionValuesArgSplitSize >= 2 &&
               // Not First and not Last
               !isFirstOptionValue && !isLastOptionValue &&
               // Avoid multiple commas
               !isCommaUnderCursor;
    }

} // namespace Tom::Commands::Complete

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_COMPLETE_PWSHCOMMAND_HPP
