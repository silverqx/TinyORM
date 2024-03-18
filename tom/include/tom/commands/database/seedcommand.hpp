#pragma once
#ifndef TOM_COMMANDS_DATABASE_SEEDCOMMAND_HPP
#define TOM_COMMANDS_DATABASE_SEEDCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"
#include "tom/concerns/confirmable.hpp"
#include "tom/concerns/usingconnection.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{
    class Seeder;

namespace Commands::Database
{

    /*! Drop all tables, views, and types. */
    class SeedCommand : public Command,
                        public Concerns::Confirmable,
                        public Concerns::UsingConnection
    {
        Q_DISABLE_COPY(SeedCommand)

        /*! Alias for the ConnectionResolverInterface. */
        using ConnectionResolverInterface = Orm::ConnectionResolverInterface;

    public:
        /*! Constructor. */
        SeedCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        ~SeedCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! The console command positional arguments signature. */
        const std::vector<PositionalArgument> &positionalArguments() const override;
        /*! The signature of the console command. */
        QList<CommandLineOption> optionsSignature() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! Result of the getSeeder(). */
        struct GetSeederResult
        {
            /*! Root seeder name. */
            QString name;
            /*! Reference to the root seeder. */
            std::reference_wrapper<Seeder> seeder;
        };

        /*! Get a seeder instance. */
        GetSeederResult getSeeder() const;

        /*! Seed the application's database, execute a root seeder. */
        static void runRootSeeder(Seeder &seeder);

    private:
        /*! Default name of the root database seeder. */
        static const QString DatabaseSeeder;

        /*! Throw if the root seeder is not defined. */
        void throwIfDoesntContainSeeder(const QString &seederClass) const;
        /*! Get a reference to the all seeder instances. */
        const std::vector<std::shared_ptr<Seeder>> &seeders() const noexcept;
    };

    /* public */

    QString SeedCommand::name() const
    {
        return Constants::DbSeed;
    }

    QString SeedCommand::description() const
    {
        return QStringLiteral("Seed the database with records");
    }

} // namespace Commands::Database
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_DATABASE_SEEDCOMMAND_HPP
