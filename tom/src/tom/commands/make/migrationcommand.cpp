#include "tom/commands/make/migrationcommand.hpp"

#include <QCommandLineOption>

#include <orm/constants.hpp>
#include <orm/utils/string.hpp>

#include "tom/application.hpp"
#include "tom/commands/make/support/migrationcreator.hpp"
#include "tom/commands/make/support/tableguesser.hpp"
#include "tom/exceptions/invalidargumenterror.hpp"
#include "tom/tomconstants.hpp"
#include "tom/tomutils.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Orm::Constants::DASH;
using Orm::Constants::NAME;
using Orm::Constants::UNDERSCORE;

using StringUtils = Orm::Utils::String;

using Tom::Constants::create_;
using Tom::Constants::create_up;
using Tom::Constants::force;
using Tom::Constants::from_model;
using Tom::Constants::fullpath;
using Tom::Constants::path_;
using Tom::Constants::path_up;
using Tom::Constants::realpath_;
using Tom::Constants::table_;
using Tom::Constants::table_up;
using Tom::Constants::DateTimePrefix;

using TomUtils = Tom::Utils;

namespace Tom::Commands::Make
{

/* public */

MigrationCommand::MigrationCommand(Application &application, QCommandLineParser &parser)
    : MakeCommand(application, parser)
{}

const std::vector<PositionalArgument> &MigrationCommand::positionalArguments() const
{
    static const std::vector<PositionalArgument> cached {
        {NAME, QStringLiteral("The name of the migration (best snake_case)")},
    };

    return cached;
}

QList<CommandLineOption> MigrationCommand::optionsSignature() const
{
    return {
        {create_,      QStringLiteral("The table name to be created"), create_up}, // Value
        {table_,       QStringLiteral("The table name to migrate (update)"), table_up}, // Value
        {path_,        QStringLiteral("The location where the migration file should be "
                                      "created"), path_up}, // Value
        {realpath_,    QStringLiteral("Indicate that any provided migration file paths "
                                      "are pre-resolved absolute paths")},
        {fullpath,     QStringLiteral("Output the full path of the created migration")},

        {{QChar('f'),
          force},      QStringLiteral("Overwrite the migration file if already exists")},
        // Hidden option, used in the special case when called from the make:model
        {from_model,   QStringLiteral("Internal argument used when guessing a path"),
                       true},
    };
}

int MigrationCommand::run()
{
    Command::run();

    /* It's possible for the developer to specify the tables to modify in this
       schema operation. The developer may also specify if this table needs
       to be freshly created so we can create the appropriate migrations. */
    auto [datetimePrefix, migrationName, extension] =
            prepareMigrationNameClassName(argument(NAME).trimmed());

    const auto migrationsPath = getMigrationsPath();

    // Check whether a migration file already exists and create parent folder if needed
    prepareFileSystem(QStringLiteral("migration"), migrationsPath, migrationName);

    auto table = value(table_);

    auto createArg = value(create_);
    auto create = isSet(create_);

    /* If no table was given as an option but a create option is given then we
       will use the "create" option as the table name. This allows the devs
       to pass a table name into this option as a short-cut for creating. */
    if (table.isEmpty() && !createArg.isEmpty()) {
        table = createArg;

        create = true;
    }

    /* Next, we will attempt to guess the table name if the migration name has
       "create" in the name. This will allow us to provide a convenient way
       of creating migrations that create new tables for the application. */
    if (table.isEmpty())
        std::tie(table, create) = Support::TableGuesser::guess(migrationName);

    // Ready to write the migration to the disk 🧨✨
    writeMigration(std::move(datetimePrefix), migrationName, extension, migrationsPath,
                   table, create);

    return EXIT_SUCCESS;
}

/* protected */

std::tuple<std::string, QString, std::string>
MigrationCommand::prepareMigrationNameClassName(QString &&migration)
{
    // Try to extract the extension from the migration name
    auto ext = fspath(migration.toStdString()).extension().string();
    const auto hasExt = !ext.empty();

    const auto startsWithDatetimePrefix = TomUtils::startsWithDatetimePrefix(migration);

    /* Classname was passed on the command-line */
    if (!startsWithDatetimePrefix && !hasExt)
        return {{}, prepareFinalMigrationName(std::move(migration)), std::string {}};

    /* Filename was passed on the command-line */
    return prepareMigrationNameFromFilename(
                startsWithDatetimePrefix, std::move(migration), hasExt, std::move(ext));
}

std::tuple<std::string, QString, std::string>
MigrationCommand::prepareMigrationNameFromFilename(
        const bool startsWithDatetimePrefix, QString &&migration, const bool hasExt,
        std::string &&ext)
{
    QString migrationName;

    // Try to extract the datetime prefix
    auto datetimePrefix = tryExtractDateTimePrefixFromName(
                              startsWithDatetimePrefix, std::move(migration),
                              migrationName);

    // Try to extract the extension
    auto extension = tryExtractExtensionFromName(hasExt, std::move(ext), migrationName);

    return {std::move(datetimePrefix),
            prepareFinalMigrationName(std::move(migrationName)),
            std::move(extension)};
}

std::string
MigrationCommand::tryExtractDateTimePrefixFromName(
            const bool startsWithDatetimePrefix, QString &&migration,
            QString &migrationName)
{
    // Nothing to extract
    if (!startsWithDatetimePrefix) {
        migrationName = std::move(migration);

        return {};
    }

    const auto dateTimePrefixSize = DateTimePrefix.size();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // Remove the datetime prefix from the migration name
    auto datetimePrefix = migration.sliced(0, dateTimePrefixSize).toStdString();
    // +1 to exclude the _ after the datetime prefix
    migrationName = migration.sliced(dateTimePrefixSize + 1);
#else
    // Remove the datetime prefix from the migration name
    auto datetimePrefix = migration.mid(0, dateTimePrefixSize).toStdString();
    // +1 to exclude the _ after the datetime prefix
    migrationName = migration.mid(dateTimePrefixSize + 1);
#endif

    return datetimePrefix;
}

std::string
MigrationCommand::tryExtractExtensionFromName(const bool hasExt, std::string &&ext,
                                              QString &migrationName)
{
    // Nothing to extract
    if (!hasExt)
        return {};

    auto extension = std::move(ext);

    // Remove the extension from the migration name
    migrationName.truncate(migrationName.size() -
                           static_cast<QString::size_type>(extension.size()));

    return extension;
}

QString MigrationCommand::prepareFinalMigrationName(QString &&migration)
{
    return StringUtils::snake(std::move(migration)).replace(DASH, UNDERSCORE);
}

void MigrationCommand::writeMigration(
        std::string &&datetimePrefix, const QString &name, const std::string &extension,
        const fspath &migrationsPath, const QString &table, const bool create) const
{
    auto migrationFilePath = Support::MigrationCreator::create(
                                 std::move(datetimePrefix), name, extension,
                                 migrationsPath, table, create);

    // make_preferred() returns reference and filename() creates a new fs::path instance
    const auto migrationFile = isSet(fullpath) ? migrationFilePath.make_preferred()
                                               : migrationFilePath.filename();

    info(QStringLiteral("Created Migration: "), false);

    note(QString::fromStdString(migrationFile.string()));
}

fspath MigrationCommand::getMigrationsPath() const
{
    /* If a user passes the --path parameter use it, otherwise try to guess migrations
       path based on the pwd and if not found use the default path which is set
       by the TINYTOM_MIGRATIONS_DIR macro. */
    auto migrationsPath = isSet(path_)
                          // User defined path
                          ? getUserMigrationsPath()
                          // Try to guess or use the default path
                          : guessMigrationsPath();

    // Validate
    if (fs::exists(migrationsPath) && !fs::is_directory(migrationsPath))
        throw Exceptions::InvalidArgumentError(
                QStringLiteral("Migrations path '%1' exists and it's not a directory.")
                .arg(migrationsPath.c_str()));

    return migrationsPath;
}

fspath MigrationCommand::getUserMigrationsPath() const
{
    auto targetPath = fspath(value(path_).toStdString()).lexically_normal();

    return isSet(realpath_)
            // The 'path' argument contains an absolute path
            ? std::move(targetPath)
            // The 'path' argument contains a relative path
            : fs::current_path() / targetPath;
}

fspath MigrationCommand::guessMigrationsPath() const
{
    return guessPathForMakeByPwd(
                application().getMigrationsPath(),
                /* Models path needed to correctly guess the path in one special case,
                   when this command is called from the make:model. */
                isSet(from_model) ? std::make_optional(
                                        std::cref(application().getModelsPath()))
                                  : std::nullopt);
}

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE
