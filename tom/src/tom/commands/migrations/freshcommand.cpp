#include "tom/commands/migrations/freshcommand.hpp"

#include <QCommandLineParser>

#include <orm/constants.hpp>

#include "tom/migrator.hpp"

using Orm::Constants::database_;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Migrations
{

/* public */

FreshCommand::FreshCommand(
        Application &application, QCommandLineParser &parser,
        std::shared_ptr<Migrator> migrator
)
    : Command(application, parser)
    , Concerns::Confirmable(*this, 0)
    , m_migrator(std::move(migrator))
{}

QList<QCommandLineOption> FreshCommand::optionsSignature() const
{
    return {
        {database_,     "The database connection to use", database_}, // Value
        {"drop-views",  "Drop all tables and views"},
        {"drop-types",  "Drop all tables and types (Postgres only)"},
        {"force",       "Force the operation to run when in production"},
//        {"schema-path", "The path to a schema dump file"}, // Value
//        {"seed",        "Indicates if the seed task should be re-run"},
//        {"seeder",      "The class name of the root seeder", "seeded"}, // Value
        {"step",        "Force the migrations to be run so they can be rolled back "
                        "individually"},
    };
}

int FreshCommand::run()
{
    Command::run();

    // Ask for confirmation in the production environment
    if (!confirmToProceed())
        return EXIT_FAILURE;

    // Database connection to use
    auto databaseCmd = valueCmd(database_);

    call("db:wipe", {databaseCmd,
                     QStringLiteral("--force"),
                     boolCmd("drop-views"),
                     boolCmd("drop-types")});

    call("migrate", {databaseCmd,
                     QStringLiteral("--force"),
                     boolCmd("step"),
                     valueCmd("schema-path")});

//    if (needsSeeding())
//        runSeeder(std::move(databaseCmd));

    return EXIT_SUCCESS;
}

/* protected */

bool FreshCommand::needsSeeding() const
{
    return isSet("seed") || !value("seeder").isEmpty();
}

void FreshCommand::runSeeder(QString &&databaseCmd) const
{
    call("db:seed", {std::move(databaseCmd),
                     QStringLiteral("--force"),
                     valueCmd("seeder", "class")});
}

} // namespace Tom::Commands::Migrations

TINYORM_END_COMMON_NAMESPACE
