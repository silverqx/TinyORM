#pragma once
#ifndef TOM_COMMANDS_MIGRATIONS_MIGRATECOMMAND_HPP
#define TOM_COMMANDS_MIGRATIONS_MIGRATECOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"
#include "tom/concerns/confirmable.hpp"
#include "tom/concerns/usingconnection.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{
    class Migrator;

namespace Commands::Migrations
{

    /*! Run the database migrations up/down. */
    class MigrateCommand : public Command,
                           public Concerns::Confirmable,
                           public Concerns::UsingConnection
    {
        Q_DISABLE_COPY_MOVE(MigrateCommand)

    public:
        /*! Constructor. */
        MigrateCommand(Application &application, QCommandLineParser &parser,
                       std::shared_ptr<Migrator> migrator);
        /*! Virtual destructor. */
        ~MigrateCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! Options signature of the console command. */
        QList<CommandLineOption> optionsSignature() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! Prepare the migration database for running. */
        void prepareDatabase(const QString &database) const;
        /*! Load the schema state to seed the initial database schema structure. */
//        void loadSchemaState() const;

        /*! Run the database seeder command. */
        int runSeeder(const QString &database) const;

        /*! The migrator service instance. */
        std::shared_ptr<Migrator> m_migrator;
    };

    /* public */

    QString MigrateCommand::name() const
    {
        return Constants::Migrate;
    }

    QString MigrateCommand::description() const
    {
        return u"Run the database migrations"_s;
    }

} // namespace Commands::Migrations
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MIGRATIONS_MIGRATECOMMAND_HPP
