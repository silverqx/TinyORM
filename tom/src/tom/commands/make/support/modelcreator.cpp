#include "tom/commands/make/support/modelcreator.hpp"

#include <QStringList>

#include <fstream>

#include <range/v3/action/remove_if.hpp>

#include <orm/constants.hpp>
#include <orm/tiny/utils/string.hpp>
#include <orm/utils/container.hpp>

#include "tom/commands/make/stubs/modelstubs.hpp"
#include "tom/exceptions/invalidargumenterror.hpp"

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Orm::Constants::NEWLINE;
using Orm::Constants::NOSPACE;
using Orm::Constants::SPACE;

using ContainerUtils = Orm::Utils::Container;
using StringUtils    = Orm::Tiny::Utils::String;

using Tom::Commands::Make::Stubs::BelongsToManyStub;
using Tom::Commands::Make::Stubs::BelongsToStub;
using Tom::Commands::Make::Stubs::ModelConnectionStub;
using Tom::Commands::Make::Stubs::ModelDisableTimestampsStub;
using Tom::Commands::Make::Stubs::ModelIncludeItemStub;
using Tom::Commands::Make::Stubs::ModelPrivateStub;
using Tom::Commands::Make::Stubs::ModelPublicStub;
using Tom::Commands::Make::Stubs::ModelRelationItemStub;
using Tom::Commands::Make::Stubs::ModelRelationsStub;
using Tom::Commands::Make::Stubs::ModelStub;
using Tom::Commands::Make::Stubs::ModelTableStub;
using Tom::Commands::Make::Stubs::ModelUsingItemStub;
using Tom::Commands::Make::Stubs::OneToOneStub;
using Tom::Commands::Make::Stubs::OneToManyStub;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make::Support
{

/* public */

fspath ModelCreator::create(const QString &className, const CmdOptions &cmdOptions,
                            fspath &&modelsPath)
{
    const auto basename = className.toLower();

    auto modelPath = getPath(basename, modelsPath);

    throwIfModelAlreadyExists(className, basename, modelsPath);

    ensureDirectoryExists(modelsPath);

    // Output it as binary stream to force line endings to LF
    std::ofstream(modelPath, std::ios::out | std::ios::binary)
            << populateStub(className, cmdOptions);

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

std::string ModelCreator::populateStub(const QString &className,
                                       const CmdOptions &cmdOptions)
{
    const auto publicSection  = createPublicSection(className, cmdOptions);
    const auto privateSection = createPrivateSection(className, cmdOptions,
                                                     !publicSection.isEmpty());

    const auto macroGuard = className.toUpper();

    const auto includesSection = createIncludesSection();
    const auto usingsSection   = createUsingsSection();

    QString stub(ModelStub);

    stub.replace(QStringLiteral("DummyClass"),  className)
        .replace(QStringLiteral("{{ class }}"), className)
        .replace(QStringLiteral("{{class}}"),   className)

        .replace(QStringLiteral("{{ macroguard }}"), macroGuard)
        .replace(QStringLiteral("{{macroguard}}"),   macroGuard)

        .replace(QStringLiteral("{{ publicSection }}"), publicSection)
        .replace(QStringLiteral("{{publicSection}}"),   publicSection)

        .replace(QStringLiteral("{{ privateSection }}"), privateSection)
        .replace(QStringLiteral("{{privateSection}}"),   privateSection)

        .replace(QStringLiteral("{{ includesSection }}"), includesSection)
        .replace(QStringLiteral("{{includesSection}}"),   includesSection)
        .replace(QStringLiteral("{{ usingsSection }}"),   usingsSection)
        .replace(QStringLiteral("{{usingsSection}}"),     usingsSection);

    return stub.toStdString();
}

QString ModelCreator::createPublicSection(const QString &className,
                                          const CmdOptions &cmdOptions)
{
    const auto &[oneToOne, oneToMany, belongsTo, belongsToMany, _1, _2, _3] = cmdOptions;

    QStringList publicSectionList;

    publicSectionList << createOneToOneRelation(className, oneToOne);
    publicSectionList << createOneToManyRelation(className, oneToMany);
    publicSectionList << createBelongsToRelation(className, belongsTo);
    publicSectionList << createBelongsToManyRelation(className, belongsToMany);

    // Remove empty parts
    publicSectionList |= ranges::actions::remove_if([](const auto &value)
    {
        return value.isEmpty();
    });

    QString publicSection = publicSectionList.join(NEWLINE);

    if (!publicSection.isEmpty())
        publicSection.prepend(ModelPublicStub);

    return publicSection;
}

QString ModelCreator::createOneToOneRelation(const QString &parentClass,
                                             const QString &relatedClass)
{
    if (relatedClass.isEmpty())
        return {};

    const auto relationName   = guessOneTypeRelationName(relatedClass);
    const auto parentComment  = guessSingularComment(parentClass);
    const auto relatedComment = guessSingularComment(relatedClass);

    return QString(OneToOneStub)
            .replace(QStringLiteral("{{ parentClass }}"),     parentClass)
            .replace(QStringLiteral("{{parentClass}}"),       parentClass)
            .replace(QStringLiteral("{{ relatedClass }}"),    relatedClass)
            .replace(QStringLiteral("{{relatedClass}}"),      relatedClass)

            .replace(QStringLiteral("{{ relationName }}"),    relationName)
            .replace(QStringLiteral("{{relationName}}"),      relationName)

            .replace(QStringLiteral("{{ parentComment }}"),   parentComment)
            .replace(QStringLiteral("{{parentComment}}"),     parentComment)
            .replace(QStringLiteral("{{ relatedComment }}"),  relatedComment)
            .replace(QStringLiteral("{{relatedComment}}"),    relatedComment);
}

QString ModelCreator::createOneToManyRelation(const QString &parentClass,
                                              const QString &relatedClass)
{
    if (relatedClass.isEmpty())
        return {};

    const auto relationName   = guessManyTypeRelationName(relatedClass);
    const auto parentComment  = guessSingularComment(parentClass);
    const auto relatedComment = guessPluralComment(relatedClass);

    return QString(OneToManyStub)
            .replace(QStringLiteral("{{ parentClass }}"),     parentClass)
            .replace(QStringLiteral("{{parentClass}}"),       parentClass)
            .replace(QStringLiteral("{{ relatedClass }}"),    relatedClass)
            .replace(QStringLiteral("{{relatedClass}}"),      relatedClass)

            .replace(QStringLiteral("{{ relationName }}"),    relationName)
            .replace(QStringLiteral("{{relationName}}"),      relationName)

            .replace(QStringLiteral("{{ parentComment }}"),   parentComment)
            .replace(QStringLiteral("{{parentComment}}"),     parentComment)
            .replace(QStringLiteral("{{ relatedComment }}"),  relatedComment)
            .replace(QStringLiteral("{{relatedComment}}"),    relatedComment);
}

QString ModelCreator::createBelongsToRelation(const QString &parentClass,
                                              const QString &relatedClass)
{
    if (relatedClass.isEmpty())
        return {};

    const auto relationName   = guessOneTypeRelationName(relatedClass);
    const auto parentComment  = guessSingularComment(parentClass);
    const auto relatedComment = guessSingularComment(relatedClass);

    return QString(BelongsToStub)
            .replace(QStringLiteral("{{ parentClass }}"),     parentClass)
            .replace(QStringLiteral("{{parentClass}}"),       parentClass)
            .replace(QStringLiteral("{{ relatedClass }}"),    relatedClass)
            .replace(QStringLiteral("{{relatedClass}}"),      relatedClass)

            .replace(QStringLiteral("{{ relationName }}"),    relationName)
            .replace(QStringLiteral("{{relationName}}"),      relationName)

            .replace(QStringLiteral("{{ parentComment }}"),   parentComment)
            .replace(QStringLiteral("{{parentComment}}"),     parentComment)
            .replace(QStringLiteral("{{ relatedComment }}"),  relatedComment)
            .replace(QStringLiteral("{{relatedComment}}"),    relatedComment);
}

QString ModelCreator::createBelongsToManyRelation(const QString &parentClass,
                                                  const QString &relatedClass)
{
    if (relatedClass.isEmpty())
        return {};

    const auto relationName   = guessManyTypeRelationName(relatedClass);
    const auto parentComment  = guessSingularComment(parentClass);
    const auto relatedComment = guessPluralComment(relatedClass);

    return QString(BelongsToManyStub)
            .replace(QStringLiteral("{{ parentClass }}"),     parentClass)
            .replace(QStringLiteral("{{parentClass}}"),       parentClass)
            .replace(QStringLiteral("{{ relatedClass }}"),    relatedClass)
            .replace(QStringLiteral("{{relatedClass}}"),      relatedClass)

            .replace(QStringLiteral("{{ relationName }}"),    relationName)
            .replace(QStringLiteral("{{relationName}}"),      relationName)

            .replace(QStringLiteral("{{ parentComment }}"),   parentComment)
            .replace(QStringLiteral("{{parentComment}}"),     parentComment)
            .replace(QStringLiteral("{{ relatedComment }}"),  relatedComment)
            .replace(QStringLiteral("{{relatedComment}}"),    relatedComment);
}

QString ModelCreator::guessSingularComment(const QString &className)
{
    return StringUtils::snake(className, SPACE);
}

QString ModelCreator::guessPluralComment(const QString &className)
{
    return StringUtils::snake(className, SPACE).append(QChar('s'));
}

QString ModelCreator::guessOneTypeRelationName(const QString &className)
{
    return StringUtils::camel(className);
}

QString ModelCreator::guessManyTypeRelationName(const QString &className)
{
    return guessOneTypeRelationName(className).append(QChar('s'));
}

QString ModelCreator::createPrivateSection(
            const QString &className, const CmdOptions &cmdOptions,
            const bool hasPublicSection)
{
    const auto &[_1, _2, _3, _4, connection, table, disableTimestamps] = cmdOptions;

    QString privateSection;

    privateSection += createRelationsHash(className, cmdOptions);

    // Append a newline after the relations hash
    if (!privateSection.isEmpty() && (!table.isEmpty() || !connection.isEmpty()))
        privateSection.append(NEWLINE);

    if (!table.isEmpty())
        privateSection += QString(ModelTableStub)
                          .replace(QStringLiteral("{{ table }}"), table)
                          .replace(QStringLiteral("{{table}}"),   table);

    if (!connection.isEmpty())
        privateSection += QString(ModelConnectionStub)
                          .replace(QStringLiteral("{{ connection }}"), connection)
                          .replace(QStringLiteral("{{connection}}"),   connection);

    if (disableTimestamps)
        privateSection += ModelDisableTimestampsStub;

    if (!privateSection.isEmpty()) {
        // Prepend the private: specifier if the public section exists
        if (hasPublicSection)
            privateSection.prepend(ModelPrivateStub);
        else
            privateSection.prepend(NEWLINE);
    }

    return privateSection;
}

QString ModelCreator::createRelationsHash(const QString &className,
                                          const CmdOptions &cmdOptions)
{
    const auto &[oneToOne, oneToMany, belongsTo, belongsToMany, _1, _2, _3] = cmdOptions;

    // Nothing to create
    if (oneToOne.isEmpty() || oneToMany.isEmpty())
        return {};

    // Get max. size of relation names for align
    const std::vector relationSizes {oneToOne.size(), oneToMany.size() + 1};
    const auto relationsMaxSize = static_cast<QString::size_type>(
                                      *std::ranges::max_element(relationSizes));

    QStringList relationItemsList;
    relationItemsList << createOneToOneRelationItem(className, oneToOne,
                                                    relationsMaxSize);
    relationItemsList << createOneToManyRelationItem(className, oneToMany,
                                                     relationsMaxSize);
    relationItemsList << createBelongsToRelationItem(className, belongsTo,
                                                     relationsMaxSize);
    relationItemsList << createBelongsToManyRelationItem(className, belongsToMany,
                                                         relationsMaxSize);

    const auto relationItems = relationItemsList.join(NEWLINE);

    return QString(ModelRelationsStub)
            .replace(QStringLiteral("{{ relationItems }}"), relationItems)
            .replace(QStringLiteral("{{relationItems}}"),   relationItems);
}

QString ModelCreator::createOneToOneRelationItem(
            const QString &parentClass, const QString &relatedClass,
            const QString::size_type relationsMaxSize)
{
    if (relatedClass.isEmpty())
        return {};

    const auto relationName = guessOneTypeRelationName(relatedClass);
    const auto spaceAlign = QString(relationsMaxSize - relationName.size(), SPACE);

    QString result = QString(ModelRelationItemStub)
                     .replace(QStringLiteral("{{ parentClass }}"),  parentClass)
                     .replace(QStringLiteral("{{parentClass}}"),    parentClass)

                     .replace(QStringLiteral("{{ relationName }}"), relationName)
                     .replace(QStringLiteral("{{relationName}}"),   relationName)

                     .replace(QStringLiteral("{{ spaceAlign }}"),   spaceAlign)
                     .replace(QStringLiteral("{{spaceAlign}}"),     spaceAlign);

    // Insert to model includes and usings
    m_includesList.emplace(QString(ModelIncludeItemStub).arg(relatedClass.toLower()));
    m_usingsList.emplace(QString(ModelUsingItemStub).arg(QStringLiteral("HasOne")));

    return result;
}

QString ModelCreator::createOneToManyRelationItem(
            const QString &parentClass, const QString &relatedClass,
            const QString::size_type relationsMaxSize)
{
    if (relatedClass.isEmpty())
        return {};

    const auto relationName = guessManyTypeRelationName(relatedClass);
    const auto spaceAlign = QString(relationsMaxSize - relationName.size(), SPACE);

    QString result = QString(ModelRelationItemStub)
                     .replace(QStringLiteral("{{ parentClass }}"),  parentClass)
                     .replace(QStringLiteral("{{parentClass}}"),    parentClass)

                     .replace(QStringLiteral("{{ relationName }}"), relationName)
                     .replace(QStringLiteral("{{relationName}}"),   relationName)

                     .replace(QStringLiteral("{{ spaceAlign }}"),   spaceAlign)
                     .replace(QStringLiteral("{{spaceAlign}}"),     spaceAlign);

    // Insert to model includes and usings
    m_includesList.emplace(QString(ModelIncludeItemStub).arg(relatedClass.toLower()));
    m_usingsList.emplace(QString(ModelUsingItemStub).arg(QStringLiteral("HasMany")));

    return result;
}

QString ModelCreator::createBelongsToRelationItem(
            const QString &parentClass, const QString &relatedClass,
            const QString::size_type relationsMaxSize)
{
    if (relatedClass.isEmpty())
        return {};

    const auto relationName = guessOneTypeRelationName(relatedClass);
    const auto spaceAlign = QString(relationsMaxSize - relationName.size(), SPACE);

    QString result = QString(ModelRelationItemStub)
                     .replace(QStringLiteral("{{ parentClass }}"),  parentClass)
                     .replace(QStringLiteral("{{parentClass}}"),    parentClass)

                     .replace(QStringLiteral("{{ relationName }}"), relationName)
                     .replace(QStringLiteral("{{relationName}}"),   relationName)

                     .replace(QStringLiteral("{{ spaceAlign }}"),   spaceAlign)
                     .replace(QStringLiteral("{{spaceAlign}}"),     spaceAlign);

    // Insert to model includes and usings
    m_includesList.emplace(QString(ModelIncludeItemStub).arg(relatedClass.toLower()));
    m_usingsList.emplace(QString(ModelUsingItemStub).arg(QStringLiteral("BelongsTo")));

    return result;
}

QString ModelCreator::createBelongsToManyRelationItem(
            const QString &parentClass, const QString &relatedClass,
            const QString::size_type relationsMaxSize)
{
    if (relatedClass.isEmpty())
        return {};

    const auto relationName = guessManyTypeRelationName(relatedClass);
    const auto spaceAlign = QString(relationsMaxSize - relationName.size(), SPACE);

    QString result = QString(ModelRelationItemStub)
                     .replace(QStringLiteral("{{ parentClass }}"),  parentClass)
                     .replace(QStringLiteral("{{parentClass}}"),    parentClass)

                     .replace(QStringLiteral("{{ relationName }}"), relationName)
                     .replace(QStringLiteral("{{relationName}}"),   relationName)

                     .replace(QStringLiteral("{{ spaceAlign }}"),   spaceAlign)
                     .replace(QStringLiteral("{{spaceAlign}}"),     spaceAlign);

    // Insert to model includes and usings
    m_includesList.emplace(QString(ModelIncludeItemStub).arg(relatedClass.toLower()));
    m_usingsList.emplace(QString(ModelUsingItemStub)
                         .arg(QStringLiteral("BelongsToMany")));

    return result;
}

QString ModelCreator::createIncludesSection()
{
    // Nothing to create
    if (m_includesList.empty())
        return {};

    return NOSPACE.arg("\n\n", ContainerUtils::join(m_includesList, NEWLINE));
}

QString ModelCreator::createUsingsSection()
{
    // Nothing to create
    if (m_usingsList.empty())
        return {};

    return NOSPACE.arg(NEWLINE, ContainerUtils::join(m_usingsList, NEWLINE));
}

/* private */

void ModelCreator::throwIfModelAlreadyExists(
            const QString &className, const QString &basename, const fspath &modelsPath)
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
