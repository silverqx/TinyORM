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

    protected:
        /* Current Tom command */
        /*! Get the currently processed Tom command. */
        static std::optional<QString>
        getCurrentTomCommand(const QString &commandlineArg, QString::size_type cword);

        /* Printing support */
        /*! Get the delimiter for result values (space for bash). */
        QChar getResultDelimiter() const noexcept override;
        /*! Prepare one result value (argument or option) for printing. */
        inline QString
        prepareResultValue(
                QString &&completionText, const std::optional<QString> &listItemText,
                const QString &toolTip) const override;

        /*! Append short verbose options (-v,-vv,-vvv) to the completion result. */
        inline void appendShortVerboseOptions(QStringList &options,
                                              const QString &description) const override;
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

    /* protected */

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
