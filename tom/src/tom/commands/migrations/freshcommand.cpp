#include "tom/commands/migrations/freshcommand.hpp"

#include <QCommandLineParser>

#include <orm/constants.hpp>

#include "tom/migrator.hpp"

using Orm::Constants::database_;

using Tom::Constants::database_up;
using Tom::Constants::drop_types;
using Tom::Constants::drop_views;
using Tom::Constants::force;
using Tom::Constants::step_;
//using Tom::Constants::DbSeed;
using Tom::Constants::DbWipe;
using Tom::Constants::Migrate;

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
        {database_,     QStringLiteral("The database connection to use"), database_up}, // Value
        {drop_views,    QStringLiteral("Drop all tables and views")},
        {drop_types,    QStringLiteral("Drop all tables and types (Postgres only)")},
        {force,         QStringLiteral("Force the operation to run when in production")},
//        {"schema-path", QStringLiteral("The path to a schema dump file")}, // Value
//        {"seed",        QStringLiteral("Indicates if the seed task should be re-run")},
//        {"seeder",      QStringLiteral("The class name of the root seeder"), "seeded"}, // Value
        {step_,         QStringLiteral("Force the migrations to be run so they can be "
                                       "rolled back individually")},
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

    call(DbWipe, {databaseCmd,
                  longOption(force),
                  boolCmd(drop_views),
                  boolCmd(drop_types)});

    call(Migrate, {databaseCmd,
                   longOption(force),
                   boolCmd(step_)});
//                   valueCmd("schema-path")});

//    if (needsSeeding())
//        runSeeder(std::move(databaseCmd));

    return EXIT_SUCCESS;
}

/* protected */

//bool FreshCommand::needsSeeding() const
//{
//    return isSet("seed") || !value("seeder").isEmpty();
//}

//void FreshCommand::runSeeder(QString &&databaseCmd) const
//{
//    call(DbSeed, {std::move(databaseCmd),
//                  longOption(force),
//                  valueCmd("seeder", "class")});
//}

} // namespace Tom::Commands::Migrations

TINYORM_END_COMMON_NAMESPACE
