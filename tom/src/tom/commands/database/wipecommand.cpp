#include "tom/commands/database/wipecommand.hpp"

#include <QCommandLineParser>

#include <orm/databaseconnection.hpp>

#include "tom/tomconstants.hpp"

/*! Alias for the QStringLiteral(). */
#define sl(str) QStringLiteral(str)

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
        {database_,    sl("The database connection to use <comment>(multiple values "
                          "allowed)</comment>"), database_up}, // Value
        {drop_views,   sl("Drop all tables and views")},
        {drop_types,   sl("Drop all tables and types (Postgres only)")},

        {{QChar('f'),
          force},      sl("Force the operation to run when in production")},
        {pretend,      sl("Dump the SQL queries that would be run")},
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

            info(QStringLiteral("Dropped all views successfully."));
        }

        dropAllTables(connection);

        info(QStringLiteral("Dropped all tables successfully."));

        if (isSet(drop_types)) {
            dropAllTypes(connection);

            info(QStringLiteral("Dropped all types successfully."));
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
