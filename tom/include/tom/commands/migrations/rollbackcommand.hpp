#pragma once
#ifndef TOM_COMMANDS_MIGRATIONS_ROLLBACKCOMMAND_HPP
#define TOM_COMMANDS_MIGRATIONS_ROLLBACKCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"
#include "tom/concerns/confirmable.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{
    class Migrator;

namespace Commands::Migrations
{

    /*! Rollback the last database migration. */
    class RollbackCommand : public Command,
                            public Concerns::Confirmable
    {
        Q_DISABLE_COPY(RollbackCommand)

        /*! Alias for the Command. */
        using Command = Commands::Command;

    public:
        /*! Constructor. */
        RollbackCommand(Application &application, QCommandLineParser &parser,
                        std::shared_ptr<Migrator> migrator);
        /*! Virtual destructor. */
        inline ~RollbackCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! The signature of the console command. */
        QList<QCommandLineOption> optionsSignature() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! The migrator service instance. */
        std::shared_ptr<Migrator> m_migrator;
    };

    /* public */

    QString RollbackCommand::name() const
    {
        return Constants::MigrateRollback;
    }

    QString RollbackCommand::description() const
    {
        return QStringLiteral("Rollback the last database migration");
    }

} // namespace Commands::Migrations
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MIGRATIONS_ROLLBACKCOMMAND_HPP
