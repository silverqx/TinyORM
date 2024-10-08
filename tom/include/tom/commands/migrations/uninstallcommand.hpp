#pragma once
#ifndef TOM_COMMANDS_MIGRATIONS_UNINSTALLCOMMAND_HPP
#define TOM_COMMANDS_MIGRATIONS_UNINSTALLCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"
#include "tom/concerns/confirmable.hpp"
#include "tom/concerns/pretendable.hpp"
#include "tom/concerns/usingconnection.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{
    class MigrationRepository;

namespace Commands::Migrations
{

    /*! Drop the migration database repository with an optional reset. */
    class UninstallCommand : public Command,
                             public Concerns::Confirmable,
                             public Concerns::Pretendable,
                             public Concerns::UsingConnection
    {
        Q_DISABLE_COPY_MOVE(UninstallCommand)

        /*! Alias for the Command. */
        using Command = Commands::Command;

    public:
        /*! Constructor. */
        UninstallCommand(Application &application, QCommandLineParser &parser,
                         std::shared_ptr<MigrationRepository> repository);
        /*! Virtual destructor. */
        ~UninstallCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! Options signature of the console command. */
        QList<CommandLineOption> optionsSignature() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! The repository instance. */
        std::shared_ptr<MigrationRepository> m_repository;
    };

    /* public */

    QString UninstallCommand::name() const
    {
        return Constants::MigrateUninstall;
    }

    QString UninstallCommand::description() const
    {
        return u"Drop the migration repository with an optional reset"_s;
    }

} // namespace Commands::Migrations
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MIGRATIONS_UNINSTALLCOMMAND_HPP
