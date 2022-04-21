#include "tom/commands/make/projectcommand.hpp"

#include <QCommandLineOption>

#include <orm/constants.hpp>

//using fspath = std::filesystem::path;

using Orm::Constants::NAME;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make
{

/* public */

ProjectCommand::ProjectCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
{}

const std::vector<PositionalArgument> &ProjectCommand::positionalArguments() const
{
    static const std::vector<PositionalArgument> cached {
        {NAME, "The name of the project"},
    };

    return cached;
}

QList<QCommandLineOption> ProjectCommand::signature() const
{
    return {
        {"qmake",     "Create qmake project"},
        {"cmake",     "Create CMake project"},
        {"tinyorm",   "Crete TinyORM project"},
        {"tom",       "Crete tom project"},
        {"path",      "The location where the project should be created", "path"},
        {"realpath",  "Indicate the <info>path</info> argument is an absolute path"},
    };
}

int ProjectCommand::run()
{
    Command::run();

    /* It's possible for the developer to specify the tables to modify in this
       schema operation. The developer may also specify if this table needs
       to be freshly created so we can create the appropriate migrations. */
//    const auto name = StringUtils::snake(argument(NAME).trimmed());

//    auto table = value(QStringLiteral("table"));

//    auto createArg = value(QStringLiteral("create"));
//    auto create = isSet(QStringLiteral("create"));

//    /* If no table was given as an option but a create option is given then we
//       will use the "create" option as the table name. This allows the devs
//       to pass a table name into this option as a short-cut for creating. */
//    if (table.isEmpty() && !createArg.isEmpty()) {
//        table = createArg;

//        create = true;
//    }

//    // CUR tom, finish, when --create/--table params are not passed silverqx
//    /* Next, we will attempt to guess the table name if the migration name has
//       "create" in the name. This will allow us to provide a convenient way
//       of creating migrations that create new tables for the application. */
////    if (!table.isEmpty())
////        auto [table, create] = TableGuesser::guess(name);

//    /* Now we are ready to write the migration out to disk. Once we've written
//       the migration out. */
//    writeMigration(name, table, create);

    return EXIT_SUCCESS;
}

//void ProjectCommand::writeMigration(const QString &name, const QString &table,
//                                      const bool create) const
//{
//    auto migrationFilePath = m_creator.create(name, getMigrationPath(), table, create);

//    // make_preferred() returns reference and filename() creates a new fs::path instance
//    const auto migrationFile = isSet("fullpath") ? migrationFilePath.make_preferred()
//                                                 : migrationFilePath.filename();

//    info(QLatin1String("Created Migration: "), false)
//            .note(QString::fromWCharArray(migrationFile.c_str()));
//}

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE
