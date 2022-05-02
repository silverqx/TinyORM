#include "tom/migrationcreator.hpp"

#include <QDateTime>

#include <fstream>

#include <orm/constants.hpp>
#include <orm/tiny/utils/string.hpp>

#include "tom/commands/make/stubs/migrationstubs.hpp"
#include "tom/exceptions/invalidargumenterror.hpp"
#include "tom/tomconstants.hpp"

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Orm::Constants::DOT;

using StringUtils = Orm::Tiny::Utils::String;

using Tom::Commands::Make::Stubs::MigrationCreateStub;
using Tom::Commands::Make::Stubs::MigrationUpdateStub;
using Tom::Commands::Make::Stubs::MigrationStub;
using Tom::Constants::DateTimePrefix;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{

/* public */

fspath MigrationCreator::create(
            std::string &&datetimePrefix, const QString &name, std::string &&extension,
            fspath &&migrationsPath, const QString &table, const bool create) const
{
    auto migrationPath = getPath(std::move(datetimePrefix), name, std::move(extension),
                                 migrationsPath);

    throwIfMigrationAlreadyExists(name, migrationsPath);

    /* First we will get the stub file for the migration, which serves as a type
       of template for the migration. Once we have those we will populate the
       various place-holders, and save the file. */
    auto stub = getStub(table, create);

    ensureDirectoryExists(migrationsPath);

    // Output it as binary stream to force line endings to LF
    std::ofstream(migrationPath, std::ios::out | std::ios::binary)
            << populateStub(name, std::move(stub), table);

    return migrationPath;
}

/* protected */

void MigrationCreator::throwIfMigrationAlreadyExists(const QString &name,
                                                     const fspath &migrationsPath) const
{
    // Nothing to check
    if (!fs::exists(migrationsPath))
        return;

    using options = fs::directory_options;

    for (const auto &entry :
         fs::directory_iterator(migrationsPath, options::skip_permission_denied)
    ) {
        // Check only files
        if (!entry.is_regular_file())
            continue;

        // CUR tom, use stem silverqx
        // Extract migration name without datetime prefix and extension
        auto entryName = QString::fromStdString(entry.path().filename().string())
                         .mid(DateTimePrefix.size() + 1);

        entryName.truncate(entryName.lastIndexOf(DOT));

        if (entryName == name)
            throw Exceptions::InvalidArgumentError(
                    QStringLiteral("A '%1' migration already exists.").arg(name));
    }
}

QString MigrationCreator::getStub(const QString &table, const bool create) const
{
    QString stub;

    if (table.isEmpty())
        stub = MigrationStub;

    else if (create)
        stub = MigrationCreateStub;

    else
        stub = MigrationUpdateStub;

    return stub;
}

fspath MigrationCreator::stubPath() const
{
    return fspath(__FILE__).parent_path() / "stubs";
}

fspath MigrationCreator::getPath(std::string &&datetimePrefix, const QString &name,
                                 std::string &&extension, const fspath &path) const
{
    std::string filename = datetimePrefix.empty() ? getDatePrefix()
                                                  : std::move(datetimePrefix);

    filename += '_' + name.toStdString();

    filename += extension.empty() ? ".hpp" : std::move(extension);

    return path / std::move(filename);
}

std::string MigrationCreator::getDatePrefix() const
{
    return QDateTime::currentDateTime().toString(DateTimePrefix).toStdString();
}

std::string MigrationCreator::populateStub(const QString &name, QString &&stub,
                                           const QString &table) const
{
    const auto className = getClassName(name);

    stub.replace(QStringLiteral("DummyClass"), className)
        .replace(QStringLiteral("{{ class }}"), className)
        .replace(QStringLiteral("{{class}}"), className);

    /* Here we will replace the table place-holders with the table specified by
       the developer, which is useful for quickly creating a tables creation
       or update migration from the console instead of typing it manually. */
    if (!table.isEmpty())
        stub.replace(QStringLiteral("DummyTable"), table)
            .replace(QStringLiteral("{{ table }}"), table)
            .replace(QStringLiteral("{{table}}"), table);

    return stub.toStdString();
}

QString MigrationCreator::getClassName(const QString &name) const
{
    return StringUtils::studly(name);
}

void MigrationCreator::ensureDirectoryExists(const fspath &path) const
{
    if (fs::exists(path) && fs::is_directory(path))
        return;

    fs::create_directories(path);
}

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE
