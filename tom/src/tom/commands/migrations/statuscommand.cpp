#include "tom/commands/migrations/statuscommand.hpp"

#include <QCommandLineParser>

#include <orm/db.hpp>

#include "tom/migrationrepository.hpp"
#include "tom/migrator.hpp"

#include <range/v3/view/remove_if.hpp>

#ifdef TINYTOM_TESTS_CODE
#  include <range/v3/algorithm/transform.hpp>
#  include <range/v3/view/move.hpp>

#  include "tom/exceptions/runtimeerror.hpp"
#endif

#ifndef sl
/*! Alias for the QStringLiteral(). */
#  define sl(str) QStringLiteral(str)
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::database_;

using Tom::Constants::database_up;
using Tom::Constants::pending_;

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
        {database_, sl("The database connection to use <comment>(multiple values "
                       "allowed)</comment>"), database_up}, // Value
        {pending_,  sl("Only list pending migrations")},
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
            if (inUnitTests())
                m_status = statusForUnitTest(std::move(migrations));
            else
#endif
                table({"Ran?", "Migration", "Batch"}, migrations);

            return EXIT_SUCCESS;
        }

#ifdef TINYTOM_TESTS_CODE
        if (inUnitTests())
            m_status.clear();
#endif

        error(QStringLiteral("No migrations found"));

        return EXIT_SUCCESS;
    });
}

/* protected */

std::vector<StatusCommand::TableRow>
StatusCommand::getStatusFor(const QVector<QVariant> &ran,
                            const std::map<QString, QVariant> &batches) const
{
    return m_migrator->migrationNames()
            | ranges::views::remove_if([&ran, pending = isSet(pending_)]
                                       (const auto &migration)
    {
        return pending && ran.contains(migration);
    })
            | ranges::views::transform([&ran, &batches](const auto &migration)
                                       -> TableRow
    {
        auto migrationName = migration.toStdString();

        if (ran.contains(migration))
            return {std::string("Yes", 3), std::move(migrationName),
                    batches.at(migration).toString().toStdString()};

        return {std::string("No", 2), std::move(migrationName)};
    })
            | ranges::to<std::vector<TableRow>>();
}

#ifdef TINYTOM_TESTS_CODE
namespace
{
#if TINY_TABULATE_VERSION >= QT_VERSION_CHECK(1, 3, 0)
    constexpr auto transformPredicateForStatus = [](auto &&cell) -> std::string
    {
        const auto holdsStdString = std::holds_alternative<std::string>(cell);

        if (!holdsStdString && !std::holds_alternative<const char *>(cell))
            throw Exceptions::RuntimeError(
                "Nested tables or types other than the std::string or const char * "
                "are not supported in StatusCommand::statusForTest().");

        if (holdsStdString)
            return std::get<std::string>(std::forward<decltype (cell)>(cell));

        return std::get<const char *>(std::forward<decltype (cell)>(cell));
    };
#else
    constexpr auto transformPredicateForStatus = [](auto &&cell)
    {
        if (!std::holds_alternative<std::string>(cell))
            throw Exceptions::RuntimeError(
                "Nested tables or type other than std::string is not supported "
                "in StatusCommand::statusForTest().");

        return std::get<std::string>(std::forward<decltype (cell)>(cell));
    };
#endif
} // namespace

std::vector<StatusCommand::StatusRow>
StatusCommand::statusForUnitTest(std::vector<TableRow> &&migrations) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
    return ranges::views::move(migrations)
            | ranges::views::transform([](auto &&migration)
    {
        StatusRow row;
        row.reserve(migration.size());

        ranges::transform(std::forward<decltype (migration)>(migration),
                          std::back_inserter(row), transformPredicateForStatus);

        return row;
    })
            | ranges::to<std::vector<StatusRow>>();
}
#endif

} // namespace Tom::Commands::Migrations

TINYORM_END_COMMON_NAMESPACE
