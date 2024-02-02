#include "tom/commands/migrations/refreshcommand.hpp"

#include <QCommandLineParser>

#include <orm/constants.hpp>

#include "tom/migrator.hpp"

#ifndef sl
/*! Alias for the QStringLiteral(). */
#  define sl(str) QStringLiteral(str)
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::database_;

using Tom::Constants::class_;
using Tom::Constants::database_up;
using Tom::Constants::force;
using Tom::Constants::seed;
using Tom::Constants::seeder;
using Tom::Constants::seeder_up;
using Tom::Constants::step_;
using Tom::Constants::step_up;
using Tom::Constants::step_migrate;
using Tom::Constants::DbSeed;
using Tom::Constants::Migrate;
using Tom::Constants::MigrateReset;
using Tom::Constants::MigrateRollback;

namespace Tom::Commands::Migrations
{

/* Don't add the --pretend option for the migrate:refresh command. It doesn't make sense
   to have it because it doesn't show relevant information as we can't really call
   the migrate:rollback command, so it would show pretended queries on the base
   of the current state of the migration repository table and not pretended queries which
   would be normally executed without the --pretend option.
   So eg. if the DB with 4 migrations would be half migrated and
   the migrate:refresh --pretend would be called, then the output would be to rollback
   the first two migrations and migrate the LAST two unmigrated migrations.
   The correct output is to rollback the first two migrations and migrate the FIRST two
   migrations. 🤯🙃😵‍💫 */

/* public */

RefreshCommand::RefreshCommand(
        Application &application, QCommandLineParser &parser,
        std::shared_ptr<Migrator> migrator
)
    : Command(application, parser)
    , Concerns::UsingConnection(connectionResolver())
    , m_migrator(std::move(migrator))
{}

QList<CommandLineOption> RefreshCommand::optionsSignature() const
{
    return {
        {database_,    sl("The database connection to use <comment>(multiple values "
                          "allowed)</comment>"), database_up}, // Value
        {{QChar('f'),
          force},      sl("Force the operation to run when in production")},
        {seed,         sl("Indicates if the seed task should be re-run")},
        {seeder,       sl("The class name of the root seeder"), seeder_up}, // Value
        {step_,        sl("The number of migrations to be reverted & re-run"), step_up}, // Value
        {step_migrate, sl("Force the migrations to be run so they can be rolled back "
                          "individually")},
    };
}

int RefreshCommand::run()
{
    Command::run();

    // Ask for confirmation in the production environment
    if (!confirmToProceed())
        return EXIT_FAILURE;

    // Database connection to use (multiple connections supported)
    return usingConnections(values(database_), isDebugVerbosity(),
                            [this](const QString &database)
    {
        // Database connection to use
        auto databaseCmd = longOption(database_, database);
        int exitCode = EXIT_SUCCESS; // NOLINT(misc-const-correctness)

        /* If the "step" option is specified it means we only want to rollback a small
           number of migrations before migrating again. For example, the user might
           only rollback and remigrate the latest four migrations instead of all. */
        if (const auto step = value(step_).toInt(); step > 0)
            exitCode |= call(MigrateRollback, {databaseCmd,
                                               longOption(force),
                                               valueCmd(step_)});
        else
            exitCode |= call(MigrateReset, {databaseCmd, longOption(force)});

        exitCode |= call(Migrate, {databaseCmd,
                                   longOption(force),
                                   boolCmd(step_migrate, step_)});

        // Invoke seeder
        if (needsSeeding())
            exitCode |= runSeeder(std::move(databaseCmd));

        // Return success only, if all executed commands were successful
        return exitCode == EXIT_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
    });
}

/* protected */

bool RefreshCommand::needsSeeding() const
{
    return isSet(seed) || !value(seeder).isEmpty();
}

int RefreshCommand::runSeeder(QString &&databaseCmd) const
{
    return call(DbSeed, {std::move(databaseCmd),
                         longOption(force),
                         valueCmd(seeder, class_)});
}

} // namespace Tom::Commands::Migrations

TINYORM_END_COMMON_NAMESPACE
