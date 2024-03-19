#pragma once
#ifndef TOM_COMMANDS_MIGRATIONS_RESETCOMMAND_HPP
#define TOM_COMMANDS_MIGRATIONS_RESETCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"
#include "tom/concerns/confirmable.hpp"
#include "tom/concerns/usingconnection.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{
    class Migrator;

namespace Commands::Migrations
{

    /*! Rollback all database migrations. */
    class ResetCommand : public Command,
                         public Concerns::Confirmable,
                         public Concerns::UsingConnection
    {
        Q_DISABLE_COPY_MOVE(ResetCommand)

    public:
        /*! Constructor. */
        ResetCommand(Application &application, QCommandLineParser &parser,
                     std::shared_ptr<Migrator> migrator);
        /*! Virtual destructor. */
        ~ResetCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! The signature of the console command. */
        QList<CommandLineOption> optionsSignature() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! The migrator service instance. */
        std::shared_ptr<Migrator> m_migrator;
    };

    /* public */

    QString ResetCommand::name() const
    {
        return Constants::MigrateReset;
    }

    QString ResetCommand::description() const
    {
        return QStringLiteral("Rollback all database migrations");
    }

} // namespace Commands::Migrations
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MIGRATIONS_RESETCOMMAND_HPP
