#include "tom/commands/migrations/migratecommand.hpp"

#include <QCommandLineParser>

#include <orm/constants.hpp>

#include "tom/migrator.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::database_;

using Tom::Constants::database_up;
using Tom::Constants::force;
using Tom::Constants::pretend;
using Tom::Constants::seed;
using Tom::Constants::step_;
using Tom::Constants::DbSeed;
using Tom::Constants::MigrateInstall;

namespace Tom::Commands::Migrations
{

/* public */

MigrateCommand::MigrateCommand(
        Application &application, QCommandLineParser &parser,
        std::shared_ptr<Migrator> migrator
)
    : Command(application, parser)
    , Concerns::UsingConnection(connectionResolver())
    , m_migrator(std::move(migrator))
{}

QList<CommandLineOption> MigrateCommand::optionsSignature() const
{
    return {
        {database_,     QStringLiteral("The database connection to use "
                                       "<comment>(multiple values allowed)</comment>"),
                        database_up}, // Value
        {{QChar('f'),
          force},       QStringLiteral("Force the operation to run when in production")},
        {pretend,       QStringLiteral("Dump the SQL queries that would be run")},
//        {"schema-path", QStringLiteral("The path to a schema dump file")}, // Value
        {seed,          QStringLiteral("Indicates if the seed task should be re-run")},
        {step_,         QStringLiteral("Force the migrations to be run so they can be "
                                       "rolled back individually")},
    };
}

int MigrateCommand::run()
{
    Command::run();

    // Ask for confirmation in the production environment
    if (!confirmToProceed())
        return EXIT_FAILURE;

    // Database connection to use (multiple connections supported)
    return usingConnections(
                values(database_), isDebugVerbosity(), m_migrator->repository(),
                [this](const auto &database)
    {
        // Install db repository and load schema state
        prepareDatabase(database);

        /* Next, we will check to see if a path option has been defined. If it has
               we will use the path relative to the root of this installation folder
               so that migrations may be run for any path within the applications. */
        m_migrator->run({isSet(pretend), isSet(step_)});

        info(QStringLiteral("Database migaration completed successfully."));

        int exitCode = EXIT_SUCCESS; // NOLINT(misc-const-correctness)

        /* Finally, if the "seed" option has been given, we will re-run the database
           seed task to re-populate the database, which is convenient when adding
           a migration and a seed at the same time, as it is only this command. */
        if (needsSeeding())
            exitCode |= runSeeder(database);

        // Return success only, if all executed commands were successful
        return exitCode == EXIT_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
    });
}

/* protected */

void MigrateCommand::prepareDatabase(const QString &database) const
{
    if (!m_migrator->repositoryExists())
        call(MigrateInstall, {longOption(database_, database)});

//    if (!m_migrator->hasRunAnyMigrations() && !isSet(pretend))
//        loadSchemaState();
}

//void MigrateCommand::loadSchemaState() const
//{
//    // CUR tom, finish load schema silverqx
//}

bool MigrateCommand::needsSeeding() const
{
    return !isSet(pretend) && isSet(seed);
}

int MigrateCommand::runSeeder(const QString &database) const
{
    return call(DbSeed, {longOption(database_, database),
                         longOption(force)});
}

} // namespace Tom::Commands::Migrations

TINYORM_END_COMMON_NAMESPACE
