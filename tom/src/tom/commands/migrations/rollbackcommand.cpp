#include "tom/commands/migrations/rollbackcommand.hpp"

#include <QCommandLineParser>

#include <orm/constants.hpp>

#include "tom/migrator.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::database_;

using Tom::Constants::batch_;
using Tom::Constants::batch_up;
using Tom::Constants::database_up;
using Tom::Constants::force;
using Tom::Constants::pretend;
using Tom::Constants::step_;
using Tom::Constants::step_up;

namespace Tom::Commands::Migrations
{

/* public */

RollbackCommand::RollbackCommand(
        Application &application, QCommandLineParser &parser,
        std::shared_ptr<Migrator> migrator
)
    : Command(application, parser)
    , Concerns::UsingConnection(connectionResolver())
    , m_migrator(std::move(migrator))
{}

QList<CommandLineOption> RollbackCommand::optionsSignature() const
{
    return {
        {database_,   QStringLiteral("The database connection to use "
                                     "<comment>(multiple values allowed)</comment>"),
                      database_up}, // Value
        {{QChar('f'),
          force},     QStringLiteral("Force the operation to run when in production")},
        {pretend,     QStringLiteral("Dump the SQL queries that would be run")},
        {step_,       QStringLiteral("The number of migrations to be reverted"), step_up}, // Value
        {batch_,      QStringLiteral("The batch of migrations (identified by their "
                                     "batch number) to be reverted"), batch_up}, // Value
    };
}

int RollbackCommand::run()
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
        // Validation not needed as the toInt() returns 0 if conversion fails, like it
        m_migrator->rollback({.pretend   = isSet(pretend),
                              .stepValue = value(step_).toInt(),
                              .batch     = value(batch_).toInt()});

        return EXIT_SUCCESS;
    });
}

} // namespace Tom::Commands::Migrations

TINYORM_END_COMMON_NAMESPACE
