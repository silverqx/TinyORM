#pragma once
#ifndef TOM_COMMANDS_INTEGRATECOMMAND_HPP
#define TOM_COMMANDS_INTEGRATECOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

class QTextStream;

namespace Tom::Commands
{

    /*! Enable tab-completion for the given shell. */
    class IntegrateCommand : public Command
    {
        Q_DISABLE_COPY(IntegrateCommand)

    public:
        /*! Constructor. */
        IntegrateCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        inline ~IntegrateCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! The console command positional arguments signature. */
        const std::vector<PositionalArgument> &positionalArguments() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /* pwsh profile related */
        /*! Integrate tab-completion for the pwsh shell. */
        int integratePwsh() const;

        /*! Get system dependant Documents folder. */
        static QString getPwshProfileFolder();
        /*! Write the TinyORM tom tab-completion code to the pwsh profile. */
        static bool writeToPwshProfile(
                const QString &pwshProfileFolder, const QString &profileFileRelative,
                const QString &pwshProfile);
        /*! Detect whether the tom tab-completion is already registered. */
        static bool isPwshCompletionRegistered(const QString &pwshProfile,
                                               QTextStream &pwshProfileStream);
        /*! Get the Register-ArgumentCompleter command string. */
        static QString getRegisterArgumentCompleter();
    };

    /* public */

    QString IntegrateCommand::name() const
    {
        return Constants::Integrate;
    }

    QString IntegrateCommand::description() const
    {
        return QStringLiteral("Enable tab-completion for the given shell");
    }

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_INTEGRATECOMMAND_HPP
