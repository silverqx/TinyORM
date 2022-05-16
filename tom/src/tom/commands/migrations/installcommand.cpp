#include "tom/commands/migrations/installcommand.hpp"

#include <QCommandLineParser>

#include <orm/constants.hpp>

#include "tom/migrationrepository.hpp"

using Orm::Constants::database_;

using Tom::Constants::database_up;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Migrations
{

/* public */

InstallCommand::InstallCommand(
        Application &application, QCommandLineParser &parser,
        std::shared_ptr<MigrationRepository> repository
)
    : Command(application, parser)
    , Concerns::UsingConnection(resolver())
    , m_repository(std::move(repository))
{}

QList<QCommandLineOption> InstallCommand::optionsSignature() const
{
    return {
        {database_, QStringLiteral("The database connection to use"), database_up}, // Value
    };
}

int InstallCommand::run()
{
    Command::run();

    // Database connection to use
    return usingConnection(value(database_), isDebugVerbosity(), *m_repository, [this]
    {
        m_repository->createRepository();

        info(QStringLiteral("Migration table created successfully."));

        return EXIT_SUCCESS;
    });
}

} // namespace Tom::Commands::Migrations

TINYORM_END_COMMON_NAMESPACE

// FUTURE tom, add migrate:uninstall or migrate:install --uninstall silverqx
