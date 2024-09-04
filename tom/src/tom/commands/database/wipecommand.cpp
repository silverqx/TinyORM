#include "tom/commands/database/wipecommand.hpp"

#include <QCommandLineParser>

#include <orm/databaseconnection.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::database_;

using Tom::Constants::database_up;
using Tom::Constants::drop_types;
using Tom::Constants::drop_views;
using Tom::Constants::force;
using Tom::Constants::pretend;

namespace Tom::Commands::Database
{

/* public */

WipeCommand::WipeCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
    , Concerns::UsingConnection(connectionResolver())
{}

QList<CommandLineOption> WipeCommand::optionsSignature() const
{
    return {
        {database_,    u"The database connection to use <comment>(multiple values "
                        "allowed)</comment>"_s, database_up}, // Value
        {drop_views,   u"Drop all tables and views"_s},
        {drop_types,   u"Drop all tables and types (Postgres only)"_s},

        {{QChar('f'),
          force},      u"Force the operation to run when in production"_s},
        {pretend,      u"Dump the SQL queries that would be run"_s},
    };
}

int WipeCommand::run()
{
    Command::run();

    // Ask for confirmation in the production environment
    if (!confirmToProceed())
        return EXIT_FAILURE;

    // Database connection to use (multiple connections supported)
    return usingConnections(values(database_), isDebugVerbosity(),
                            [this](const QString &database)
    {
        auto &connection = this->connection(database);

        if (isSet(drop_views)) {
            dropAllViews(connection);

            info(u"Dropped all views successfully."_s);
        }

        dropAllTables(connection);

        info(u"Dropped all tables successfully."_s);

        if (isSet(drop_types)) {
            dropAllTypes(connection);

            info(u"Dropped all types successfully."_s);
        }

        return EXIT_SUCCESS;
    });
}

/* protected */

void WipeCommand::dropAllTables(DatabaseConnection &connection) const
{
    optionalPretend(isSet(pretend), connection,
                    [](auto &connection_)
    {
        connection_.getSchemaBuilder().dropAllTables();
    });
}

void WipeCommand::dropAllViews(DatabaseConnection &connection) const
{
    optionalPretend(isSet(pretend), connection,
                    [](auto &connection_)
    {
        connection_.getSchemaBuilder().dropAllViews();
    });
}

void WipeCommand::dropAllTypes(DatabaseConnection &connection) const
{
    optionalPretend(isSet(pretend), connection,
                    [](auto &connection_)
    {
        connection_.getSchemaBuilder().dropAllTypes();
    });
}

} // namespace Tom::Commands::Database

TINYORM_END_COMMON_NAMESPACE
