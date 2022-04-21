#include "tom/commands/migrations/refreshcommand.hpp"

#include <QCommandLineParser>

#include <orm/constants.hpp>

#include "tom/migrator.hpp"

using Orm::Constants::database_;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Migrations
{

/* public */

RefreshCommand::RefreshCommand(
        Application &application, QCommandLineParser &parser,
        std::shared_ptr<Migrator> migrator
)
    : Command(application, parser)
    , Concerns::Confirmable(*this, 0)
    , m_migrator(std::move(migrator))
{}

QList<QCommandLineOption> RefreshCommand::optionsSignature() const
{
    return {
        {database_,      "The database connection to use", database_}, // Value
        {"force",        "Force the operation to run when in production"},
//        {"seed",         "Indicates if the seed task should be re-run"},
//        {"seeder",       "The class name of the root seeder", "seeded"}, // Value
        {"step",         "The number of migrations to be reverted & re-run", "step"}, // Value
        {"step-migrate", "Force the migrations to be run so they can be rolled back "
                          "individually"},
    };
}

int RefreshCommand::run()
{
    Command::run();

    // Ask for confirmation in the production environment
    if (!confirmToProceed())
        return EXIT_FAILURE;

    // Database connection to use
    auto databaseCmd = valueCmd(database_);

    /* If the "step" option is specified it means we only want to rollback a small
       number of migrations before migrating again. For example, the user might
       only rollback and remigrate the latest four migrations instead of all. */
    if (const auto step = value("step").toInt(); step > 0)
        call("migrate:rollback", {databaseCmd,
                                  QStringLiteral("--force"),
                                  valueCmd("step")});
    else
        call("migrate:reset", {databaseCmd, QStringLiteral("--force")});

    call("migrate", {databaseCmd,
                     QStringLiteral("--force"),
                     boolCmd("step-migrate", "step")});

//    if (needsSeeding())
//        runSeeder(std::move(databaseCmd));

    return EXIT_SUCCESS;
}

/* protected */

bool RefreshCommand::needsSeeding() const
{
    return isSet("seed") || !value("seeder").isEmpty();
}

void RefreshCommand::runSeeder(QString &&databaseCmd) const
{
    call("db:seed", {std::move(databaseCmd),
                     QStringLiteral("--force"),
                     valueCmd("seeder", "class")});
}

} // namespace Tom::Commands::Migrations

TINYORM_END_COMMON_NAMESPACE
