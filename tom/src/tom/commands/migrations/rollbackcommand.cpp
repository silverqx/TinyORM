#include "tom/commands/migrations/rollbackcommand.hpp"

#include <QCommandLineParser>

#include <orm/constants.hpp>

#include "tom/migrator.hpp"

using Orm::Constants::database_;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Migrations
{

/* public */

RollbackCommand::RollbackCommand(
        Application &application, QCommandLineParser &parser,
        std::shared_ptr<Migrator> migrator
)
    : Command(application, parser)
    , Concerns::Confirmable(*this, 0)
    , m_migrator(std::move(migrator))
{}

QList<QCommandLineOption> RollbackCommand::optionsSignature() const
{
    return {
        {database_,  "The database connection to use", database_}, // Value
        {"force",    "Force the operation to run when in production"},
        {"pretend",  "Dump the SQL queries that would be run"},
        {"step",     "The number of migrations to be reverted", "step"}, // Value
    };
}

int RollbackCommand::run()
{
    Command::run();

    // Ask for confirmation in the production environment
    if (!confirmToProceed())
        return EXIT_FAILURE;

    // Database connection to use
    return m_migrator->usingConnection(value(database_), isDebugVerbosity(), [this]
    {
        // Validation not needed as the toInt() returns 0 if conversion fails, like it
        m_migrator->rollback({.pretend   = isSet("pretend"),
                              .stepValue = value("step").toInt()});

        return EXIT_SUCCESS;
    });
}

} // namespace Tom::Commands::Migrations

TINYORM_END_COMMON_NAMESPACE
