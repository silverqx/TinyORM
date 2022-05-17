#include "tom/commands/make/migrationcommand.hpp"

#include <QCommandLineOption>

#include <orm/constants.hpp>
#include <orm/tiny/utils/string.hpp>

#include "tom/application.hpp"
#include "tom/exceptions/invalidargumenterror.hpp"
#include "tom/tableguesser.hpp"
#include "tom/tomconstants.hpp"
#include "tom/tomutils.hpp"

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Orm::Constants::DASH;
using Orm::Constants::NAME;
using Orm::Constants::UNDERSCORE;

using StringUtils = Orm::Tiny::Utils::String;

using Tom::Constants::create_;
using Tom::Constants::fullpath;
using Tom::Constants::path_;
using Tom::Constants::realpath_;
using Tom::Constants::table_;
using Tom::Constants::DateTimePrefix;

using TomUtils = Tom::Utils;

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
        {NAME, QStringLiteral("The name of the migration (best snake_case)")},
    };

    return cached;
}

QList<QCommandLineOption> MigrationCommand::optionsSignature() const
{
    return {
        {create_,   QStringLiteral("The table to be created"), create_.toUpper()}, // Value
        {table_,    QStringLiteral("The table to migrate"), table_.toUpper()}, // Value
        {path_,     QStringLiteral("The location where the migration file should be "
                                   "created"), path_.toUpper()}, // Value
        {realpath_, QStringLiteral("Indicate any provided migration file paths are "
                                   "pre-resolved absolute paths")},
        {fullpath,  QStringLiteral("Output the full path of the migration")},
    };
}

int MigrationCommand::run()
{
    Command::run();

    /* It's possible for the developer to specify the tables to modify in this
       schema operation. The developer may also specify if this table needs
       to be freshly created so we can create the appropriate migrations. */
    auto [datetimePrefix, migrationName, extension] =
            prepareMigrationNameClassname(argument(NAME).trimmed());

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
        std::tie(table, create) = TableGuesser::guess(migrationName);

    // Ready to write the migration to the disk 🧨✨
    writeMigration(std::move(datetimePrefix), migrationName, std::move(extension),
                   table, create);

    return EXIT_SUCCESS;
}

/* protected */

std::tuple<std::string, QString, std::string>
MigrationCommand::prepareMigrationNameClassname(QString &&migration)
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

    // Remove the datetime prefix from the migration name
    auto datetimePrefix = migration.mid(0, dateTimePrefixSize).toStdString();
    // +1 to exclude the _ after the datetime prefix
    migrationName = migration.mid(dateTimePrefixSize + 1);

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
            std::string &&datetimePrefix, const QString &name, std::string &&extension,
            const QString &table, const bool create) const
{
    auto migrationFilePath = m_creator.create(
                                 std::move(datetimePrefix), name, std::move(extension),
                                 getMigrationPath(), table, create);

    // make_preferred() returns reference and filename() creates a new fs::path instance
    const auto migrationFile = isSet(fullpath) ? migrationFilePath.make_preferred()
                                               : migrationFilePath.filename();

    info(QStringLiteral("Created Migration: "), false);

    note(QString::fromStdString(migrationFile.string()));
}

fspath MigrationCommand::getMigrationPath() const
{
    // Default location
    if (!isSet(path_))
        return application().getMigrationsPath();

    auto targetPath = value(path_).toStdString();

    // The 'path' argument contains an absolute path
    if (isSet(realpath_))
        return {std::move(targetPath)};

    // The 'path' argument contains a relative path
    auto migrationsPath = fs::current_path() / std::move(targetPath);

    // Validate
    if (fs::exists(migrationsPath) && !fs::is_directory(migrationsPath))
        throw Exceptions::InvalidArgumentError(
                QStringLiteral("Migrations path '%1' exists and it's not a directory.")
                .arg(migrationsPath.c_str()));

    return migrationsPath;
}

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE
