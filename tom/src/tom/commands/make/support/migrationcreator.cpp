#include "tom/commands/make/support/migrationcreator.hpp"

#include <QDateTime>

#include <fstream>

#include <orm/utils/string.hpp>

#include "tom/commands/make/stubs/migrationstubs.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using fspath = std::filesystem::path;

using StringUtils = Orm::Utils::String;

using Tom::Commands::Make::Stubs::MigrationCreateStub;
using Tom::Commands::Make::Stubs::MigrationUpdateStub;
using Tom::Commands::Make::Stubs::MigrationStub;
using Tom::Constants::DateTimePrefix;

namespace Tom::Commands::Make::Support
{

/* public */

fspath MigrationCreator::create(
        std::string &&datetimePrefix, const QString &name, const std::string &extension,
        const fspath &migrationsPath, const QString &table, const bool create)
{
    auto migrationPath = getPath(std::move(datetimePrefix), name, extension,
                                 migrationsPath);

    /* First we will get the stub file for the migration, which serves as a type
       of template for the migration. Once we have those we will populate the
       various place-holders, and save the file. */
    auto stub = getStub(table, create);

    // Output it as binary stream to force line endings to LF
    std::ofstream(migrationPath, std::ios::out | std::ios::binary)
            << populateStub(name, std::move(stub), table);

    return migrationPath;
}

/* protected */

QString MigrationCreator::getStub(const QString &table, const bool create)
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

fspath MigrationCreator::getPath(std::string &&datetimePrefix, const QString &name,
                                 const std::string &extension, const fspath &path)
{
    std::string filename = datetimePrefix.empty() ? getDatePrefix()
                                                  : std::move(datetimePrefix);

    filename += '_' + name.toStdString();

    filename += extension.empty() ? ".hpp" : extension;

    return path / filename;
}

std::string MigrationCreator::getDatePrefix()
{
    return QDateTime::currentDateTime().toString(DateTimePrefix).toStdString();
}

std::string
MigrationCreator::populateStub(const QString &name, QString &&stub, const QString &table) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
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

QString MigrationCreator::getClassName(const QString &name)
{
    return StringUtils::studly(name);
}

} // namespace Tom::Commands::Make::Support

TINYORM_END_COMMON_NAMESPACE
