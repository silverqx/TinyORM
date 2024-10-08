#pragma once
#ifndef TOM_COMMANDS_MIGRATIONS_INSTALLCOMMAND_HPP
#define TOM_COMMANDS_MIGRATIONS_INSTALLCOMMAND_HPP

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

    /*! Create the migration database repository. */
    class InstallCommand : public Command,
                           public Concerns::Confirmable,
                           public Concerns::Pretendable,
                           public Concerns::UsingConnection
    {
        Q_DISABLE_COPY_MOVE(InstallCommand)

        /*! Alias for the Command. */
        using Command = Commands::Command;

    public:
        /*! Constructor. */
        InstallCommand(Application &application, QCommandLineParser &parser,
                       std::shared_ptr<MigrationRepository> repository);
        /*! Virtual destructor. */
        ~InstallCommand() override = default;

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

    QString InstallCommand::name() const
    {
        return Constants::MigrateInstall;
    }

    QString InstallCommand::description() const
    {
        return u"Create the migration repository"_s;
    }

} // namespace Commands::Migrations
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MIGRATIONS_INSTALLCOMMAND_HPP
