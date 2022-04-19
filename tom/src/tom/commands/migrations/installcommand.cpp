#include "tom/commands/migrations/installcommand.hpp"

#include <QCommandLineParser>

#include <orm/constants.hpp>

#include "tom/migrationrepository.hpp"

using Orm::Constants::database_;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Migrations
{

/* public */

InstallCommand::InstallCommand(
        Application &application, QCommandLineParser &parser,
        std::shared_ptr<MigrationRepository> repository
)
    : Command(application, parser)
    , m_repository(std::move(repository))
{}

QList<QCommandLineOption> InstallCommand::optionsSignature() const
{
    return {
        {database_, "The database connection to use", database_}, // Value
    };
}

int InstallCommand::run()
{
    Command::run();

    // Database connection to use
    m_repository->setConnection(value(database_), isDebugVerbosity());

    m_repository->createRepository();

    info(QLatin1String("Migration table created successfully."));

    return EXIT_SUCCESS;
}

} // namespace Tom::Commands::Migrations

TINYORM_END_COMMON_NAMESPACE

// FUTURE tom, add migrate:uninstall or migrate:install --uninstall silverqx
