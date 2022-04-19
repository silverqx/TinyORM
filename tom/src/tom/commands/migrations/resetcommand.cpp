#include "tom/commands/migrations/resetcommand.hpp"

#include <QCommandLineParser>

#include <orm/constants.hpp>

#include "tom/migrator.hpp"

using Orm::Constants::database_;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Migrations
{

/* public */

ResetCommand::ResetCommand(
        Application &application, QCommandLineParser &parser,
        std::shared_ptr<Migrator> migrator
)
    : Command(application, parser)
    , Concerns::Confirmable(*this, 0)
    , m_migrator(std::move(migrator))
{}

QList<QCommandLineOption> ResetCommand::optionsSignature() const
{
    return {
        {database_,  "The database connection to use", database_}, // Value
        {"force",    "Force the operation to run when in production"},
        {"pretend",  "Dump the SQL queries that would be run"},
    };
}

int ResetCommand::run()
{
    Command::run();

    // Ask for confirmation in the production environment
    if (!confirmToProceed())
        return EXIT_FAILURE;

    // Database connection to use
    return m_migrator->usingConnection(value(database_), isDebugVerbosity(), [this]
    {
        if (!m_migrator->repositoryExists()) {
            comment(QLatin1String("Migration table not found."));

            return EXIT_FAILURE;
        }

        m_migrator->reset(isSet("pretend"));

        return EXIT_SUCCESS;
    });
}

} // namespace Tom::Commands::Migrations

TINYORM_END_COMMON_NAMESPACE
