#pragma once
#ifndef TOM_COMMANDS_MIGRATIONS_FRESHCOMMAND_HPP
#define TOM_COMMANDS_MIGRATIONS_FRESHCOMMAND_HPP

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

    /*! Rollback the last database migration/s. */
    class FreshCommand : public Command,
                         public Concerns::Confirmable
    {
        Q_DISABLE_COPY(FreshCommand)

        /*! Alias for the Command. */
        using Command = Commands::Command;

    public:
        /*! Constructor. */
        FreshCommand(Application &application, QCommandLineParser &parser,
                     std::shared_ptr<Migrator> migrator);
        /*! Virtual destructor. */
        inline ~FreshCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! The signature of the console command. */
        QList<QCommandLineOption> optionsSignature() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! Determine if the developer has requested database seeding. */
//        bool needsSeeding() const;
        /*! Run the database seeder command. */
//        void runSeeder(QString &&databaseCmd) const;

        /*! The migrator service instance. */
        std::shared_ptr<Migrator> m_migrator;
    };

    /* public */

    QString FreshCommand::name() const
    {
        return Constants::MigrateFresh;
    }

    QString FreshCommand::description() const
    {
        return QLatin1String("Drop all tables and re-run all migrations");
    }

} // namespace Commands::Migrations
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MIGRATIONS_FRESHCOMMAND_HPP
