#include "tom/commands/make/support/modelcreator.hpp"

#include <fstream>

#include <orm/constants.hpp>
#include <orm/tiny/utils/string.hpp>

#include "tom/commands/make/stubs/modelstubs.hpp"
#include "tom/exceptions/invalidargumenterror.hpp"

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Orm::Constants::NEWLINE;

using StringUtils = Orm::Tiny::Utils::String;

using Tom::Commands::Make::Stubs::ModelConnectionStub;
using Tom::Commands::Make::Stubs::ModelDisableTimestampsStub;
//using Tom::Commands::Make::Stubs::ModelPrivateStub;
using Tom::Commands::Make::Stubs::ModelStub;
using Tom::Commands::Make::Stubs::ModelTableStub;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make::Support
{

/* public */

fspath ModelCreator::create(const QString &className, CmdOptions &&cmdOptions,
                            fspath &&modelsPath) const
{
    const auto basename = className.toLower();

    auto modelPath = getPath(basename, modelsPath);

    throwIfModelAlreadyExists(className, basename, modelsPath);

    ensureDirectoryExists(modelsPath);

    // Output it as binary stream to force line endings to LF
    std::ofstream(modelPath, std::ios::out | std::ios::binary)
            << populateStub(className, std::move(cmdOptions));

    return modelPath;
}

/* protected */

fspath ModelCreator::getPath(const QString &basename, const fspath &path)
{
    return path / (basename.toStdString() + ".hpp");
}

void ModelCreator::ensureDirectoryExists(const fspath &path)
{
    if (fs::exists(path) && fs::is_directory(path))
        return;

    fs::create_directories(path);
}

std::string ModelCreator::populateStub(const QString &className, CmdOptions &&cmdOptions)
{
    const auto privateSection = createPrivateSection(cmdOptions);

    const auto macroGuard = className.toUpper();

    QString stub(ModelStub);

    stub.replace(QStringLiteral("DummyClass"), className)
        .replace(QStringLiteral("{{ class }}"), className)
        .replace(QStringLiteral("{{class}}"), className)

        .replace(QStringLiteral("{{ macroguard }}"), macroGuard)
        .replace(QStringLiteral("{{macroguard}}"), macroGuard)

        .replace(QStringLiteral("{{ privateSection }}"), privateSection)
        .replace(QStringLiteral("{{privateSection}}"), privateSection);


    return stub.toStdString();
}

QString ModelCreator::createPrivateSection(const CmdOptions &cmdOptions)
{
    const auto &[connection, table, disableTimestamps] = cmdOptions;

    QString privateSection;

    if (!connection.isEmpty())
        privateSection += QString(ModelConnectionStub)
                          .replace(QStringLiteral("{{ connection }}"), connection)
                          .replace(QStringLiteral("{{connection}}"), connection);

    if (!table.isEmpty())
        privateSection += QString(ModelTableStub)
                          .replace(QStringLiteral("{{ table }}"), table)
                          .replace(QStringLiteral("{{table}}"), table);

    if (disableTimestamps)
        privateSection += ModelDisableTimestampsStub;

    if (!privateSection.isEmpty())
        privateSection.prepend(NEWLINE);

    return privateSection;
}

/* private */

void ModelCreator::throwIfModelAlreadyExists(
            const QString &className, const QString &basename,
            const fspath &modelsPath) const
{
    // Nothing to check
    if (!fs::exists(modelsPath))
        return;

    using options = fs::directory_options;

    for (const auto &entry :
         fs::directory_iterator(modelsPath, options::skip_permission_denied)
    ) {
        // Check only files
        if (!entry.is_regular_file())
            continue;

        // Extract base filename without the extension
        auto entryName = QString::fromStdString(entry.path().stem().string());

        if (entryName == basename)
            throw Exceptions::InvalidArgumentError(
                    QStringLiteral("A '%1' model already exists.").arg(className));
    }
}

} // namespace Tom::Commands::Make::Support

TINYORM_END_COMMON_NAMESPACE
