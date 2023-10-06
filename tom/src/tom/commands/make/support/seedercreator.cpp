#include "tom/commands/make/support/seedercreator.hpp"

#include <fstream>

#include <orm/utils/string.hpp>

#include "tom/commands/make/stubs/seederstubs.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using fspath = std::filesystem::path;

using StringUtils = Orm::Utils::String;

using Tom::Commands::Make::Stubs::SeederStub;

namespace Tom::Commands::Make::Support
{

/* public */

fspath SeederCreator::create(const QString &className, const fspath &seedersPath)
{
    const auto basename = className.toLower();

    auto seederPath = getPath(basename, seedersPath);

    /* Populate the various place-holders, and save the file.
       Output it as binary stream to force line endings to LF. */
    std::ofstream(seederPath, std::ios::out | std::ios::binary)
            << populateStub(className, getTableName(className));

    return seederPath;
}

/* protected */

fspath SeederCreator::getPath(const QString &basename, const fspath &path)
{
    return path / (basename.toStdString() + ".hpp");
}

std::string
SeederCreator::populateStub(const QString &className, const QString &table)
{
    QString stub(SeederStub);

    stub.replace(QStringLiteral("DummyClass"), className)
        .replace(QStringLiteral("{{ class }}"), className)
        .replace(QStringLiteral("{{class}}"), className);

    if (!table.isEmpty())
        stub.replace(QStringLiteral("DummyTable"), table)
            .replace(QStringLiteral("{{ table }}"), table)
            .replace(QStringLiteral("{{table}}"), table);

    return stub.toStdString();
}

QString SeederCreator::getTableName(QString className)
{
    static const auto Seeder = QStringLiteral("Seeder");

    // Remove the last Seeder word
    if (className.endsWith(Seeder))
        className.truncate(className.lastIndexOf(Seeder));

    // Pluralize
    if (!className.endsWith(QLatin1Char('s')))
        className.append(QLatin1Char('s'));

    return StringUtils::snake(className);
}

} // namespace Tom::Commands::Make::Support

TINYORM_END_COMMON_NAMESPACE
