#include "tom/commands/make/support/seedercreator.hpp"

#include <fstream>

#include <orm/tiny/utils/string.hpp>

#include "tom/commands/make/stubs/seederstubs.hpp"
#include "tom/exceptions/invalidargumenterror.hpp"

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using StringUtils = Orm::Tiny::Utils::String;

using Tom::Commands::Make::Stubs::SeederStub;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make::Support
{

/* public */

fspath SeederCreator::create(const QString &className, fspath &&seedersPath) const
{
    const auto basename = className.toLower();

    auto seederPath = getPath(basename, seedersPath);

    throwIfSeederAlreadyExists(className, basename, seedersPath);

    /* First we will get the stub file for the seeder, which serves as a type
       of template for the seeder. Once we have those we will populate the
       various place-holders, and save the file. */
    auto stub = getStub();

    ensureDirectoryExists(seedersPath);

    // Output it as binary stream to force line endings to LF
    std::ofstream(seederPath, std::ios::out | std::ios::binary)
            << populateStub(className, getTableName(className), std::move(stub));

    return seederPath;
}

/* protected */

QString SeederCreator::getStub()
{
    return SeederStub;
}

fspath SeederCreator::getPath(const QString &basename, const fspath &path)
{
    return path / (basename.toStdString() + ".hpp");
}

void SeederCreator::ensureDirectoryExists(const fspath &path)
{
    if (fs::exists(path) && fs::is_directory(path))
        return;

    fs::create_directories(path);
}

std::string
SeederCreator::populateStub(const QString &className, QString &&table, QString &&stub)
{
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
    if (!className.endsWith(QChar('s')))
        className.append(QChar('s'));

    return StringUtils::snake(className);
}

/* private */

void SeederCreator::throwIfSeederAlreadyExists(
            const QString &className, const QString &basename,
            const fspath &seedersPath) const
{
    // Nothing to check
    if (!fs::exists(seedersPath))
        return;

    using options = fs::directory_options;

    for (const auto &entry :
         fs::directory_iterator(seedersPath, options::skip_permission_denied)
    ) {
        // Check only files
        if (!entry.is_regular_file())
            continue;

        // Extract base filename without the extension
        auto entryName = QString::fromStdString(entry.path().stem().string());

        if (entryName == basename)
            throw Exceptions::InvalidArgumentError(
                    QStringLiteral("A '%1' seeder already exists.").arg(className));
    }
}

} // namespace Tom::Commands::Make::Support

TINYORM_END_COMMON_NAMESPACE
