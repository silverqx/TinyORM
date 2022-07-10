#include "tom/commands/database/wipecommand.hpp"

#include <QCommandLineParser>

#include <orm/databaseconnection.hpp>

#include "tom/tomconstants.hpp"

using Orm::Constants::database_;

using Tom::Constants::database_up;
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
    , Concerns::UsingConnection(resolver())
{}

QList<QCommandLineOption> WipeCommand::optionsSignature() const
{
    return {
        {database_,    QStringLiteral("The database connection to use "
                                      "<comment>(multiple values allowed)</comment>"),
                       database_up}, // Value
        {drop_views,   QStringLiteral("Drop all tables and views")},
        {drop_types,   QStringLiteral("Drop all tables and types (Postgres only)")},

        {{QChar('f'),
          force},      QStringLiteral("Force the operation to run when in production")},
    };
}

int WipeCommand::run()
{
    Command::run();

    // Ask for confirmation in the production environment
    if (!confirmToProceed())
        return EXIT_FAILURE;

    const auto databases = values(database_);

    auto result = EXIT_SUCCESS;
    const auto shouldPrintConnection = databases.size() > 1;
    auto first = true;

    // Database connection to use (multiple connections supported)
    for (const auto &database : databases) {
        // Visually divide individual connections
        printConnection(database, shouldPrintConnection, first);

        result &= usingConnection(database, isDebugVerbosity(), [this, &database]
        {
            if (isSet(drop_views)) {
                dropAllViews(database);

                info(QStringLiteral("Dropped all views successfully."));
            }

            dropAllTables(database);

            info(QStringLiteral("Dropped all tables successfully."));

            if (isSet(drop_types)) {
                dropAllTypes(database);

                info(QStringLiteral("Dropped all types successfully."));
            }

            return EXIT_SUCCESS;
        });
    }

    return result;
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
