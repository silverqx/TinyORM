#include "tom/commands/database/wipecommand.hpp"

#include <QCommandLineParser>

#include <orm/databaseconnection.hpp>

using Orm::Constants::database_;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Database
{

/* public */

WipeCommand::WipeCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
    , Concerns::Confirmable(*this, 0)
{}

QList<QCommandLineOption> WipeCommand::optionsSignature() const
{
    return {
        {database_,    "The database connection to use", database_}, // Value
        {"drop-views", "Drop all tables and views"},
        {"drop-types", "Drop all tables and types (Postgres only)"},
        {"force",      "Force the operation to run when in production"},
    };
}

int WipeCommand::run()
{
    Command::run();

    // Ask for confirmation in the production environment
    if (!confirmToProceed())
        return EXIT_FAILURE;

    // Database connection to use
    const auto database = value(database_);

    if (isSet("drop-views")) {
        dropAllViews(database);

        info(QLatin1String("Dropped all views successfully."));
    }

    dropAllTables(database);

    info(QLatin1String("Dropped all tables successfully."));

    if (isSet("drop-types")) {
        dropAllTypes(database);

        info(QLatin1String("Dropped all types successfully."));
    }

    return EXIT_SUCCESS;
}

/* protected */

void WipeCommand::dropAllTables(const QString &database) const
{
    connection(database).getSchemaBuilder()->dropAllTables();
}

void WipeCommand::dropAllViews(const QString &database) const
{
    connection(database).getSchemaBuilder()->dropAllViews();
}

void WipeCommand::dropAllTypes(const QString &database) const
{
    connection(database).getSchemaBuilder()->dropAllTypes();
}

} // namespace Tom::Commands::Database

TINYORM_END_COMMON_NAMESPACE
