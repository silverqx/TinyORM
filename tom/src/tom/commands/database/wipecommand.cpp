#include "tom/commands/database/wipecommand.hpp"

#include <QCommandLineParser>

#include <orm/databaseconnection.hpp>

#include "tom/tomconstants.hpp"

using Orm::Constants::database_;

using Tom::Constants::drop_types;
using Tom::Constants::drop_views;
using Tom::Constants::force;

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
        {database_,  QLatin1String("The database connection to use"), database_}, // Value
        {drop_views, QLatin1String("Drop all tables and views")},
        {drop_types, QLatin1String("Drop all tables and types (Postgres only)")},
        {force,      QLatin1String("Force the operation to run when in production")},
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

    if (isSet(drop_views)) {
        dropAllViews(database);

        info(QLatin1String("Dropped all views successfully."));
    }

    dropAllTables(database);

    info(QLatin1String("Dropped all tables successfully."));

    if (isSet(drop_types)) {
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
