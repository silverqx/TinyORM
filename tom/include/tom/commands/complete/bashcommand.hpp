#pragma once
#ifndef TOM_COMMANDS_COMPLETE_BASHCOMMAND_HPP
#define TOM_COMMANDS_COMPLETE_BASHCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/complete/basecompletecommand.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Complete
{

    /*! Complete command names and options in the bash shell (tab-completion only). */
    class BashCommand : public BaseCompleteCommand
    {
        Q_DISABLE_COPY_MOVE(BashCommand)

    public:
        /*! Constructor. */
        BashCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        ~BashCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! Options signature of the console command. */
        QList<CommandLineOption> optionsSignature() const override;

        /*! Execute the console command. */
        int run() override;

    private:
        /*! Initialize context for tab-completion. */
        CompleteContext initializeCompletionContext() override;

        /* Printing support */
        /*! Prepare one result value (argument or option) for printing. */
        inline QString
        prepareResultValue(
                QString &&completionText, const std::optional<QString> &listItemText,
                const QString &toolTip) const override;

        /*! Append short verbose options (-v,-vv,-vvv) to the tab-completion result. */
        inline void appendShortVerboseOptions(QStringList &options,
                                              const QString &description) const override;

        /* Others */
        /*! Validate the required option arguments (check isSet()). */
        void validateInputOptions() const override;
        /*! Validate the required option values. */
        void validateInputOptionValues() const override;

        /* Data members */
        /*! Index of the current word under the cursor on the command-line (0-based). */
        SizeType m_cwordArg = 0;
    };

    /* public */

    QString BashCommand::name() const
    {
        return Constants::CompleteBash;
    }

    QString BashCommand::description() const
    {
        return u"Complete command names and options in the bash shell (tab-completion)"_s;
    }

    /* private */

    /* Printing support */

    QString
    BashCommand::prepareResultValue(
            QString &&completionText, const std::optional<QString> &/*unused*/,
            const QString &/*unused*/) const
    {
        // This is the reason why the completionText parameter is rvalue
        return std::move(completionText);
    }

    void BashCommand::appendShortVerboseOptions(QStringList &options,
                                                const QString &/*unused*/) const
    {
        options << u"-v"_s
                << u"-vv"_s
                << u"-vvv"_s;
    }

} // namespace Tom::Commands::Complete

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_COMPLETE_BASHCOMMAND_HPP
