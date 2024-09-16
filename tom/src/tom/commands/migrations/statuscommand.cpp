#include "tom/commands/migrations/statuscommand.hpp"

#include <QCommandLineParser>

#include <tabulate/table.hpp>

#include <orm/db.hpp>

#include "tom/migrationrepository.hpp"
#include "tom/migrator.hpp"

#include <range/v3/view/remove_if.hpp>

/* This header exists from the tabulate v1.4.0, it has been added in the middle of v1.3.0
   and doesn't exist at the v1.3.0 release. */
#if __has_include(<tabulate/tabulate.hpp>)
#  include <tabulate/tabulate.hpp>
#endif

#ifdef TINYTOM_TESTS_CODE
#  include <range/v3/algorithm/transform.hpp>
#  include <range/v3/view/move.hpp>

#  include "tom/exceptions/runtimeerror.hpp"
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
        {database_, u"The database connection to use <comment>(multiple values "
                     "allowed)</comment>"_s, database_up}, // Value
        {pending_,  u"Only list pending migrations"_s},
    };
}

namespace
{
    /*! Alias for the tabulate color. */
    using Color = tabulate::Color;
    /*! Default tabulate table colors. */
    struct TableColors
    {
        Color green = Color::green;
        Color red   = Color::red;
    };

    /*! Initialize tabulate table colors by supported ANSI. */
    inline TableColors initializeTableColors(const bool isAnsiOutput)
    {
        /* Even if I detect ANSI support as true, tabulate has its own detection logic,
           it only checks isatty(), it has some consequences, eg. no colors when output
           is redirected and --ansi was passed to the tom application, practically
           all logic in the isAnsiOutput() will be skipped because of this internal
           tabulate logic, not a big deal though. */
        if (isAnsiOutput)
            return {}; // Use default colors

        // Disable ANSI colors if eg. --no-ansi (or not supported)
        return {Color::none, Color::none};
    }

    /*! Callback function to format the tabulate table. */
    void formatStatusTable(tabulate::Table &table, const Concerns::InteractsWithIO &io)
    {
        // Initialize tabulate table colors by supported ANSI
        const auto [green, red] = initializeTableColors(io.isAnsiOutput());

        // Format table
        // thead - green text for all columns
        table.row(0).format().font_color(green);

        // tbody
        for (std::size_t i = 1; i < table.size() ; ++i) {
            auto &row = table.row(i);

            // Remove all lines between rows in the tbody (leave only the first one)
            if (i > 1)
                row.format().hide_border_top();

            // Ran? column : Yes - green, No - red
            {
                auto &cell0 = row.cell(0);
                auto &format = cell0.format();

                if (cell0.get_text() == "Yes")
                    format.font_color(green);
                else
                    format.font_color(red);
            }

            // Align the Batch column to the right (must be after the hide_border_top())
            row.cell(2).format().font_align(tabulate::FontAlign::right);
        }
    }
} // namespace

int StatusCommand::run()
{
    Command::run();

    // Database connection to use (multiple connections supported)
    return usingConnections(
                values(database_), isDebugVerbosity(), m_migrator->repository(),
                [this]
    {
        if (!m_migrator->repositoryExists()) {
            error(u"Migration table not found."_s);

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
                table({"Ran?", "Migration", "Batch"}, migrations, formatStatusTable);

            return EXIT_SUCCESS;
        }

#ifdef TINYTOM_TESTS_CODE
        if (inUnitTests())
            m_status.clear();
#endif

        error(u"No migrations found"_s);

        return EXIT_SUCCESS;
    });
}

/* protected */

std::vector<StatusCommand::TableRow>
StatusCommand::getStatusFor(const QList<QVariant> &ran,
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
// Prepare the tabulate version
#ifndef TABULATE_VERSION_MAJOR
#  define TABULATE_VERSION_MAJOR 0
#endif
#ifndef TABULATE_VERSION_MINOR
#  define TABULATE_VERSION_MINOR 0
#endif
#ifndef TABULATE_VERSION_PATCH
#  define TABULATE_VERSION_PATCH 0
#endif

/*! Tabulate version suitable for the QT_VERSION_CHECK comparison. */
#define TINY_TABULATE_VERSION QT_VERSION_CHECK(TABULATE_VERSION_MAJOR, \
                                               TABULATE_VERSION_MINOR, \
                                               TABULATE_VERSION_PATCH)
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
