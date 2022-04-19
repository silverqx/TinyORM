#pragma once
#ifndef TOM_COMMANDS_ENVIRONMENTCOMMAND_HPP
#define TOM_COMMANDS_ENVIRONMENTCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands
{

    /*! Display the current environment. */
    class EnvironmentCommand : public Command
    {
        Q_DISABLE_COPY(EnvironmentCommand)

    public:
        /*! Constructor. */
        EnvironmentCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        inline ~EnvironmentCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! Execute the console command. */
        int run() override;
    };

    /* public */

    QString EnvironmentCommand::name() const
    {
        return QStringLiteral("env");
    }

    QString EnvironmentCommand::description() const
    {
        return QLatin1String("Display the current framework environment");
    }

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_ENVIRONMENTCOMMAND_HPP
