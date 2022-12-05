#include "tom/commands/migrations/statuscommand.hpp"

#include <QCommandLineParser>

#include <orm/db.hpp>

#include "tom/migrationrepository.hpp"
#include "tom/migrator.hpp"

#ifdef TINYTOM_TESTS_CODE
#  include <range/v3/algorithm/transform.hpp>
#  include <range/v3/view/move.hpp>

#  include "tom/exceptions/runtimeerror.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::database_;

using Tom::Constants::database_up;

namespace Tom::Commands::Migrations
{

/* public */

StatusCommand::StatusCommand(
        Application &application, QCommandLineParser &parser,
        std::shared_ptr<Migrator> migrator
)
    : Command(application, parser)
    , Concerns::UsingConnection(connectionResolver())
    , m_migrator(std::move(migrator))
{}

QList<CommandLineOption> StatusCommand::optionsSignature() const
{
    return {
        {database_, QStringLiteral("The database connection to use "
                                   "<comment>(multiple values allowed)</comment>"),
                    database_up}, // Value
    };
}

int StatusCommand::run()
{
    Command::run();

    // Database connection to use (multiple connections supported)
    return usingConnections(
                values(database_), isDebugVerbosity(), m_migrator->repository(),
                [this]
    {
        if (!m_migrator->repositoryExists()) {
            error(QStringLiteral("Migration table not found."));

            return EXIT_FAILURE;
        }

        const auto &repository = m_migrator->repository();

        if (auto migrations = getStatusFor(repository.getRanSimple(),
                                           repository.getMigrationBatches());
            !migrations.empty()
        ) {
            /* During testing save the result of a status command to the global
                   variable instead of outputting it, to be able to verify results. */
#ifdef TINYTOM_TESTS_CODE
            if (m_inUnitTests)
                m_status = statusForUnitTest(std::move(migrations));
            else
#endif
                table({"Ran?", "Migration", "Batch"}, migrations);

            return EXIT_SUCCESS;
        }

#ifdef TINYTOM_TESTS_CODE
        if (m_inUnitTests)
            m_status.clear();
#endif

        error(QStringLiteral("No migrations found"));

        return EXIT_SUCCESS;
    });
}

/* protected */

std::vector<StatusCommand::TableRow>
StatusCommand::getStatusFor(QVector<QVariant> &&ran,
                            std::map<QString, QVariant> &&batches) const
{
    return m_migrator->migrationNames()
            | ranges::views::transform([&ran, &batches](const auto &migration)
                                       -> TableRow
    {
        auto migrationName = migration.toStdString();

        if (ran.contains(migration))
            return {"Yes", std::move(migrationName),
                    batches.at(migration).toString().toStdString()};

        return {"No", std::move(migrationName)};
    })
            | ranges::to<std::vector<TableRow>>();
}

#ifdef TINYTOM_TESTS_CODE
std::vector<StatusCommand::StatusRow>
StatusCommand::statusForUnitTest(std::vector<TableRow> &&migrations)
{
    return ranges::views::move(migrations)
            | ranges::views::transform([](auto &&migration)
                                       -> StatusRow
    {
        StatusRow row;

        ranges::transform(std::forward<decltype (migration)>(migration),
                          std::back_inserter(row), [](auto &&cell)
        {
            if (!std::holds_alternative<std::string>(cell))
                throw Exceptions::RuntimeError(
                        "Nested tables or type other than std::string is not supported "
                        "in StatusCommand::statusForTest().");

            return std::get<std::string>(std::forward<decltype (cell)>(cell));
        });

        return row;
    })
            | ranges::to<std::vector<StatusRow>>();
}
#endif

} // namespace Tom::Commands::Migrations

TINYORM_END_COMMON_NAMESPACE
