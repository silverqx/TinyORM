#include "tom/commands/make/migrationcommand.hpp"

#include <QCommandLineOption>

#include <orm/constants.hpp>
#include <orm/tiny/utils/string.hpp>

#include "tom/application.hpp"
#include "tom/exceptions/invalidargumenterror.hpp"

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Orm::Constants::NAME;

using StringUtils = Orm::Tiny::Utils::String;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make
{

/* public */

MigrationCommand::MigrationCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
{}

const std::vector<PositionalArgument> &MigrationCommand::positionalArguments() const
{
    static const std::vector<PositionalArgument> cached {
        {NAME, "The name of the migration"},
    };

    return cached;
}

QList<QCommandLineOption> MigrationCommand::optionsSignature() const
{
    return {
        {"create",   "The table to be created", "create"},
        {"table",    "The table to migrate", "table"},
        {"path",     "The location where the migration file should be created", "path"},
        {"realpath", "Indicate any provided migration file paths are pre-resolved "
                      "absolute paths"},
        {"fullpath", "Output the full path of the migration"},
    };
}

int MigrationCommand::run()
{
    Command::run();

    /* It's possible for the developer to specify the tables to modify in this
       schema operation. The developer may also specify if this table needs
       to be freshly created so we can create the appropriate migrations. */
    const auto name = StringUtils::snake(argument(NAME).trimmed());

    auto table = value(QStringLiteral("table"));

    auto createArg = value(QStringLiteral("create"));
    auto create = isSet(QStringLiteral("create"));

    /* If no table was given as an option but a create option is given then we
       will use the "create" option as the table name. This allows the devs
       to pass a table name into this option as a short-cut for creating. */
    if (table.isEmpty() && !createArg.isEmpty()) {
        table = createArg;

        create = true;
    }

    // CUR tom, finish TableGuesser, when --create/--table params are not passed silverqx
    /* Next, we will attempt to guess the table name if the migration name has
       "create" in the name. This will allow us to provide a convenient way
       of creating migrations that create new tables for the application. */
//    if (!table.isEmpty())
//        auto [table, create] = TableGuesser::guess(name);

    /* Now we are ready to write the migration out to disk. Once we've written
       the migration out. */
    writeMigration(name, table, create);

    return EXIT_SUCCESS;
}

void MigrationCommand::writeMigration(const QString &name, const QString &table,
                                      const bool create) const
{
    auto migrationFilePath = m_creator.create(name, getMigrationPath(), table, create);

    // make_preferred() returns reference and filename() creates a new fs::path instance
    const auto migrationFile = isSet("fullpath") ? migrationFilePath.make_preferred()
                                                 : migrationFilePath.filename();

    info(QLatin1String("Created Migration: "), false);

    note(QString::fromStdString(migrationFile.string()));
}

fspath MigrationCommand::getMigrationPath() const
{
    // Default location
    if (!isSet("path"))
        return application().getMigrationsPath();

    auto targetPath = value("path").toStdString();

    // The 'path' argument contains an absolute path
    if (isSet("realpath"))
        return {std::move(targetPath)};

    // The 'path' argument contains a relative path
    auto migrationsPath = fs::current_path() / std::move(targetPath);

    // Validate
    if (fs::exists(migrationsPath) && !fs::is_directory(migrationsPath))
        throw Exceptions::InvalidArgumentError(
                QLatin1String("Migrations path '%1' exists and it's not a directory.")
                .arg(migrationsPath.c_str()));

    return migrationsPath;
}

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE
