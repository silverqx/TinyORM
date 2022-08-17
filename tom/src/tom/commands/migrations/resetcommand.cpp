#include "tom/commands/migrations/resetcommand.hpp"

#include <QCommandLineParser>

#include <orm/constants.hpp>

#include "tom/migrator.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::database_;

using Tom::Constants::database_up;
using Tom::Constants::force;
using Tom::Constants::pretend;

namespace Tom::Commands::Migrations
{

/* public */

ResetCommand::ResetCommand(
        Application &application, QCommandLineParser &parser,
        std::shared_ptr<Migrator> migrator
)
    : Command(application, parser)
    , Concerns::Confirmable(*this, 0)
    , Concerns::UsingConnection(resolver())
    , m_migrator(std::move(migrator))
{}

QList<QCommandLineOption> ResetCommand::optionsSignature() const
{
    return {
        {database_,   QStringLiteral("The database connection to use "
                                     "<comment>(multiple values allowed)</comment>"),
                      database_up}, // Value
        {{QChar('f'),
          force},     QStringLiteral("Force the operation to run when in production")},
        {pretend,     QStringLiteral("Dump the SQL queries that would be run")},
    };
}

int ResetCommand::run()
{
    Command::run();

    // Ask for confirmation in the production environment
    if (!confirmToProceed())
        return EXIT_FAILURE;

    // Database connection to use (multiple connections supported)
    return usingConnections(
                values(database_), isDebugVerbosity(), m_migrator->repository(),
                [this]
    {
        if (!m_migrator->repositoryExists()) {
            comment(QStringLiteral("Migration table not found."));

            return EXIT_FAILURE;
        }

        m_migrator->reset(isSet(pretend));

        return EXIT_SUCCESS;
    });
}

} // namespace Tom::Commands::Migrations

TINYORM_END_COMMON_NAMESPACE
