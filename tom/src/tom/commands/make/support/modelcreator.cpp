#include "tom/commands/make/support/modelcreator.hpp"

#include <deque>
#include <fstream>

#include <range/v3/action/remove_if.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

#include <orm/constants.hpp>
#include <orm/tiny/utils/string.hpp>
#include <orm/utils/container.hpp>

#include "tom/commands/make/modelcommandconcepts.hpp"
#include "tom/commands/make/stubs/modelstubs.hpp"
#include "tom/exceptions/invalidargumenterror.hpp"

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Orm::Constants::COMMA;
using Orm::Constants::DOT;
using Orm::Constants::EMPTY;
using Orm::Constants::NEWLINE;
using Orm::Constants::NOSPACE;
using Orm::Constants::SPACE;

using ContainerUtils = Orm::Utils::Container;
using StringUtils    = Orm::Tiny::Utils::String;

using Tom::Commands::Make::Stubs::BelongsToManyStub;
using Tom::Commands::Make::Stubs::BelongsToManyStub2;
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
    const auto relationsList   = createRelationsList();
    // I want to have all pivots after the related classes because of that this set exists
    const auto pivotsList      = createPivotsList();
    const auto forwardsSection = createForwardsSection();

    return QString(ModelStub)
            .replace(QStringLiteral("DummyClass"),  className)
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
            .replace(QStringLiteral("{{usingsSection}}"),     usingsSection)
            .replace(QStringLiteral("{{ forwardsSection }}"), forwardsSection)
            .replace(QStringLiteral("{{forwardsSection}}"),   forwardsSection)

            .replace(QStringLiteral("{{ relationsList }}"), relationsList)
            .replace(QStringLiteral("{{relationsList}}"),   relationsList)
            .replace(QStringLiteral("{{ pivotsList }}"),    pivotsList)
            .replace(QStringLiteral("{{pivotsList}}"),      pivotsList)

            .toStdString();
}

/* Public model section */

QString ModelCreator::createPublicSection(const QString &className,
                                          const CmdOptions &cmdOptions)
{
    const auto &[
            oneToOneList, oneToManyList,  belongsToList, belongsToManyList,
            foreignKeys,  pivotTables,    pivotClasses,  asList, withPivotList,
            withTimestampsList,
            _1, _2, _3
    ] = cmdOptions;

    const auto &[
            oneToOneForeign, oneToManyForeign, belongsToForeign, belongsToManyForeign
    ] = foreignKeys;

    // FUTURE tom, make:model, preserve relations order defined on the command-line, add std::vector<std::size_t> relationsOrder to the CmdOptions and fill it during options search, add relationsOrder to ranges::zip() used in createXyzRelation() methods, below will return struct RelationItem { int order; QString content; } and then ranges::sort(), --preserve-order/-o option silverqx
    QStringList publicSectionList;
    publicSectionList.reserve(4);

    publicSectionList << createOneToOneRelation(className, oneToOneList,
                                                oneToOneForeign);
    publicSectionList << createOneToManyRelation(className, oneToManyList,
                                                 oneToManyForeign);
    publicSectionList << createBelongsToRelation(className, belongsToList,
                                                 belongsToForeign);
    publicSectionList << createBelongsToManyRelation(
                             className, belongsToManyList, belongsToManyForeign,
                             pivotTables, pivotClasses, asList, withPivotList,
                             withTimestampsList);

    // Remove empty parts
    publicSectionList |= ranges::actions::remove_if([](const auto &value)
    {
        return value.isEmpty();
    });

    auto publicSection = publicSectionList.join(NEWLINE);

    if (!publicSection.isEmpty())
        publicSection.prepend(ModelPublicStub);

    return publicSection;
}

QString ModelCreator::createOneToOneRelation(
            const QString &parentClass, const QStringList &relatedClasses,
            const QStringList &foreignKeys)
{
    if (relatedClasses.isEmpty())
        return {};

    Q_ASSERT(relatedClasses.size() == foreignKeys.size());

    QStringList result;
    result.reserve(relatedClasses.size());

    for (const auto &[relatedClass, foreignKey] :
         ranges::views::zip(relatedClasses, foreignKeys)
    ) {
        // Insert to model includes, usings, and relations lists
        m_includesList.emplace(QString(ModelIncludeItemStub).arg(relatedClass.toLower()));
        m_usingsList.emplace(QString(ModelUsingItemStub).arg(QStringLiteral("HasOne")));
        m_forwardsList.emplace(QStringLiteral("class %1;").arg(relatedClass));
        m_relationsList.emplace(relatedClass);

        const auto relationName      = guessOneTypeRelationName(relatedClass);
        const auto parentComment     = guessSingularComment(parentClass);
        const auto relatedComment    = guessSingularComment(relatedClass);
        const auto relationArguments = createRelationArguments(foreignKey);

        result << QString(OneToOneStub)
                  .replace(QStringLiteral("{{ parentClass }}"),     parentClass)
                  .replace(QStringLiteral("{{parentClass}}"),       parentClass)
                  .replace(QStringLiteral("{{ relatedClass }}"),    relatedClass)
                  .replace(QStringLiteral("{{relatedClass}}"),      relatedClass)

                  .replace(QStringLiteral("{{ relationName }}"),    relationName)
                  .replace(QStringLiteral("{{relationName}}"),      relationName)

                  .replace(QStringLiteral("{{ parentComment }}"),   parentComment)
                  .replace(QStringLiteral("{{parentComment}}"),     parentComment)
                  .replace(QStringLiteral("{{ relatedComment }}"),  relatedComment)
                  .replace(QStringLiteral("{{relatedComment}}"),    relatedComment)

                  .replace(QStringLiteral("{{ relationArguments }}"), relationArguments)
                  .replace(QStringLiteral("{{relationArguments}}"),   relationArguments);
    }

    return result.join(NEWLINE);
}

QString ModelCreator::createOneToManyRelation(
            const QString &parentClass, const QStringList &relatedClasses,
            const QStringList &foreignKeys)
{
    if (relatedClasses.isEmpty())
        return {};

    Q_ASSERT(relatedClasses.size() == foreignKeys.size());

    QStringList result;
    result.reserve(relatedClasses.size());

    for (const auto &[relatedClass, foreignKey] :
         ranges::views::zip(relatedClasses, foreignKeys)
    ) {
        // Insert to model includes, usings, and relations lists
        m_includesList.emplace(QString(ModelIncludeItemStub).arg(relatedClass.toLower()));
        m_usingsList.emplace(QString(ModelUsingItemStub).arg(QStringLiteral("HasMany")));
        m_forwardsList.emplace(QStringLiteral("class %1;").arg(relatedClass));
        m_relationsList.emplace(relatedClass);

        const auto relationName      = guessManyTypeRelationName(relatedClass);
        const auto parentComment     = guessSingularComment(parentClass);
        const auto relatedComment    = guessPluralComment(relatedClass);
        const auto relationArguments = createRelationArguments(foreignKey);

        result << QString(OneToManyStub)
                  .replace(QStringLiteral("{{ parentClass }}"),     parentClass)
                  .replace(QStringLiteral("{{parentClass}}"),       parentClass)
                  .replace(QStringLiteral("{{ relatedClass }}"),    relatedClass)
                  .replace(QStringLiteral("{{relatedClass}}"),      relatedClass)

                  .replace(QStringLiteral("{{ relationName }}"),    relationName)
                  .replace(QStringLiteral("{{relationName}}"),      relationName)

                  .replace(QStringLiteral("{{ parentComment }}"),   parentComment)
                  .replace(QStringLiteral("{{parentComment}}"),     parentComment)
                  .replace(QStringLiteral("{{ relatedComment }}"),  relatedComment)
                  .replace(QStringLiteral("{{relatedComment}}"),    relatedComment)

                  .replace(QStringLiteral("{{ relationArguments }}"), relationArguments)
                  .replace(QStringLiteral("{{relationArguments}}"),   relationArguments);
    }

    return result.join(NEWLINE);
}

QString ModelCreator::createBelongsToRelation(
            const QString &parentClass, const QStringList &relatedClasses,
            const QStringList &foreignKeys)
{
    if (relatedClasses.isEmpty())
        return {};

    Q_ASSERT(relatedClasses.size() == foreignKeys.size());

    QStringList result;
    result.reserve(relatedClasses.size());

    for (const auto &[relatedClass, foreignKey] :
         ranges::views::zip(relatedClasses, foreignKeys)
    ) {
        // Insert to model includes, usings, and relations lists
        m_includesList.emplace(QString(ModelIncludeItemStub).arg(relatedClass.toLower()));
        m_usingsList.emplace(QString(ModelUsingItemStub)
                             .arg(QStringLiteral("BelongsTo")));
        m_forwardsList.emplace(QStringLiteral("class %1;").arg(relatedClass));
        m_relationsList.emplace(relatedClass);

        const auto relationName      = guessOneTypeRelationName(relatedClass);
        const auto parentComment     = guessSingularComment(parentClass);
        const auto relatedComment    = guessSingularComment(relatedClass);
        const auto relationArguments = createRelationArguments(foreignKey);

        result << QString(BelongsToStub)
                  .replace(QStringLiteral("{{ parentClass }}"),     parentClass)
                  .replace(QStringLiteral("{{parentClass}}"),       parentClass)
                  .replace(QStringLiteral("{{ relatedClass }}"),    relatedClass)
                  .replace(QStringLiteral("{{relatedClass}}"),      relatedClass)

                  .replace(QStringLiteral("{{ relationName }}"),    relationName)
                  .replace(QStringLiteral("{{relationName}}"),      relationName)

                  .replace(QStringLiteral("{{ parentComment }}"),   parentComment)
                  .replace(QStringLiteral("{{parentComment}}"),     parentComment)
                  .replace(QStringLiteral("{{ relatedComment }}"),  relatedComment)
                  .replace(QStringLiteral("{{relatedComment}}"),    relatedComment)

                  .replace(QStringLiteral("{{ relationArguments }}"), relationArguments)
                  .replace(QStringLiteral("{{relationArguments}}"),   relationArguments);
    }

    return result.join(NEWLINE);
}

QString ModelCreator::createRelationArguments(const QString &foreignKey)
{
    if (foreignKey.isEmpty())
        return {};

    return StringUtils::wrapValue(foreignKey, QChar('"'));
}

namespace
{
    /*! Concept for the containers have same size algorithm. */
    template<typename T>
    concept ContainersSizeConcept =
            BtmPreparedValuesConcept<T> ||
            std::convertible_to<T, std::vector<BelongToManyForeignKeys>>;

    /*! Check whether all containers have the same size. */
    template<ContainersSizeConcept ...C>
    bool allHaveSameSize(const std::size_t size, const C &...container)
    {
        return ((size == static_cast<std::size_t>(container.size())) && ...);
    }
} // namespace

QString ModelCreator::createBelongsToManyRelation(
            const QString &parentClass, const QStringList &relatedClasses,
            const std::vector<BelongToManyForeignKeys> &foreignKeys,
            const QStringList &pivotTables, const QStringList &pivotClasses,
            const QStringList &asList, const std::vector<QStringList> &withPivotList,
            const std::vector<bool> &withTimestampsList)
{
    if (relatedClasses.isEmpty())
        return {};

    const auto relatedClassesSize = relatedClasses.size();

    // All lists must have the same number of items
    Q_ASSERT(allHaveSameSize(static_cast<std::size_t>(relatedClassesSize),
                             foreignKeys, pivotTables, pivotClasses, asList,
                             withPivotList, withTimestampsList));

    QStringList result;
    result.reserve(relatedClassesSize);

    for (const auto &[relatedClass, foreignKey, pivotTable, pivotClass, as, withPivot,
                      withTimestamps] :
         ranges::views::zip(relatedClasses, foreignKeys, pivotTables, pivotClasses,
                            asList, withPivotList, withTimestampsList)
    ) {
        // Insert to model includes, usings, and relations lists
        m_includesList.emplace(QString(ModelIncludeItemStub).arg(relatedClass.toLower()));
        m_usingsList.emplace(QString(ModelUsingItemStub)
                             .arg(QStringLiteral("BelongsToMany")));
        m_forwardsList.emplace(QStringLiteral("class %1;").arg(relatedClass));
        m_relationsList.emplace(relatedClass);

        const auto isPivotClassEmpty = pivotClass.isEmpty();

        // Pivot special logic
        handlePivotClass(pivotClass, isPivotClassEmpty);

        const auto relationName      = guessManyTypeRelationName(relatedClass);
        const auto parentComment     = guessSingularComment(parentClass);
        const auto relatedComment    = guessPluralComment(relatedClass);
        const auto pivot             = isPivotClassEmpty ? EMPTY
                                                         : NOSPACE.arg(COMMA, pivotClass);

        const auto relationCalls     = createRelationCalls(as, withPivot, withTimestamps);
        const auto relationArguments = createRelationArgumentsBtm(pivotTable, foreignKey);

        result << QString(relationCalls.isEmpty()
                          ? BelongsToManyStub : BelongsToManyStub2)
                  .replace(QStringLiteral("{{ parentClass }}"),     parentClass)
                  .replace(QStringLiteral("{{parentClass}}"),       parentClass)
                  .replace(QStringLiteral("{{ relatedClass }}"),    relatedClass)
                  .replace(QStringLiteral("{{relatedClass}}"),      relatedClass)

                  .replace(QStringLiteral("{{ relationName }}"),    relationName)
                  .replace(QStringLiteral("{{relationName}}"),      relationName)

                  .replace(QStringLiteral("{{ parentComment }}"),   parentComment)
                  .replace(QStringLiteral("{{parentComment}}"),     parentComment)
                  .replace(QStringLiteral("{{ relatedComment }}"),  relatedComment)
                  .replace(QStringLiteral("{{relatedComment}}"),    relatedComment)

                  .replace(QStringLiteral("{{ pivotClass }}"),      pivot)
                  .replace(QStringLiteral("{{pivotClass}}"),        pivot)
                  .replace(QStringLiteral("{{ relationCalls }}"),   relationCalls)
                  .replace(QStringLiteral("{{relationCalls}}"),     relationCalls)

                  .replace(QStringLiteral("{{ relationArguments }}"), relationArguments)
                  .replace(QStringLiteral("{{relationArguments}}"),   relationArguments);
    }

    return result.join(NEWLINE);
}

QString ModelCreator::createRelationArgumentsBtm(
            const QString &pivotTable, const BelongToManyForeignKeys &foreignKey)
{
    const auto &[foreignPivotKey, relatedPivotKey] = foreignKey;

    std::deque<QString> argumentsList;

    if (!relatedPivotKey.isEmpty())
        argumentsList.push_back(StringUtils::wrapValue(relatedPivotKey, QChar('"')));

    if (!foreignPivotKey.isEmpty())
        argumentsList.push_front(StringUtils::wrapValue(foreignPivotKey, QChar('"')));
    else if (!relatedPivotKey.isEmpty())
        argumentsList.push_front(QStringLiteral("{}"));

    // Table name of the relationship's intermediate table
    if (!pivotTable.isEmpty())
        argumentsList.push_front(StringUtils::wrapValue(pivotTable, QChar('"')));
    else if (!foreignPivotKey.isEmpty() || !relatedPivotKey.isEmpty())
        argumentsList.push_front(QStringLiteral("{}"));

    // Nothing to create
    if (argumentsList.empty())
        return {};

    return ContainerUtils::join(argumentsList, COMMA);
}

void ModelCreator::handlePivotClass(const QString &pivotClass,
                                    const bool isPivotClassEmpty)
{
    // Basic Pivot
    if (isPivotClassEmpty) {
        m_pivotsList.emplace(QStringLiteral("Pivot"));
        m_usingsList.emplace(QString(ModelUsingItemStub).arg(QStringLiteral("Pivot")));
    }
    // User defined Pivot
    else {
        m_pivotsList.emplace(pivotClass);
        m_includesList.emplace(QString(ModelIncludeItemStub).arg(pivotClass.toLower()));
    }
}

QString ModelCreator::createRelationCalls(
            const QString &as, const QStringList &withPivot, const bool withTimestamps)
{
    /*! Align a method call on the newline. */
    const auto relationCallsAlign = [](const auto &relationCalls)
    {
        return relationCalls.isEmpty() ? EMPTY
                                       : QStringLiteral("%1%2%3")
                                         .arg(NEWLINE, QString(16, SPACE)).arg(DOT);
    };

    QString relationCalls;

    // The name for the pivot relation
    if (!as.isEmpty())
        relationCalls = QStringLiteral("as(\"%1\")").arg(as);

    // Extra attributes for the pivot model
    if (!withPivot.isEmpty()) {
        // Wrap the pivot name inside quotes
        const auto withPivotWrapped = withPivot
                | ranges::views::transform([](const auto &pivotName)
        {
            return StringUtils::wrapValue(pivotName, QChar('"'));
        })
                | ranges::to<QStringList>();

        relationCalls += QStringLiteral("%1withPivot(%2)")
                         .arg(relationCallsAlign(relationCalls),
                              // Wrap in init. list {} if more pivot columns passed
                              withPivot.size() == 1
                              ? withPivotWrapped.constFirst()
                              : StringUtils::wrapValue(withPivotWrapped.join(COMMA),
                                                       QChar('{'), QChar('}')));
    }

    // Pivot table with timestamps
    if (withTimestamps)
        relationCalls += QStringLiteral("%1withTimestamps()")
                         .arg(relationCallsAlign(relationCalls));

    return relationCalls;
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

/* Private model section */

QString ModelCreator::createPrivateSection(
            const QString &className, const CmdOptions &cmdOptions,
            const bool hasPublicSection)
{
    const auto &[
            _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,
            connection, table, disableTimestamps
    ] = cmdOptions;

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
    const auto &[
            oneToOneList, oneToManyList, belongsToList, belongsToManyList,
            _1, _2, _3, _4, _5, _6, _7, _8, _10
    ] = cmdOptions;

    // Nothing to create
    if (oneToOneList.isEmpty() || oneToManyList.isEmpty() || belongsToList.isEmpty() ||
        belongsToManyList.isEmpty()
    )
        return {};

    // Get max. size of relation names for align
    const auto relationsMaxSize = getRelationNamesMaxSize(cmdOptions);

    QStringList relationItemsList;
    relationItemsList.reserve(4);

    relationItemsList << createOneToOneRelationItem(className, oneToOneList,
                                                    relationsMaxSize);
    relationItemsList << createOneToManyRelationItem(className, oneToManyList,
                                                     relationsMaxSize);
    relationItemsList << createBelongsToRelationItem(className, belongsToList,
                                                     relationsMaxSize);
    relationItemsList << createBelongsToManyRelationItem(className, belongsToManyList,
                                                         relationsMaxSize);

    const auto relationItems = relationItemsList.join(NEWLINE);

    return QString(ModelRelationsStub)
            .replace(QStringLiteral("{{ relationItems }}"), relationItems)
            .replace(QStringLiteral("{{relationItems}}"),   relationItems);
}

QString::size_type ModelCreator::getRelationNamesMaxSize(const CmdOptions &cmdOptions)
{
    const auto &[
            oneToOneList, oneToManyList, belongsToList, belongsToManyList,
            _1, _2, _3, _4, _5, _6, _7, _8, _10
    ] = cmdOptions;

    // Get max. size of relation names for align
    // +1 because the s character is appended for the many type relations
    const std::vector relationSizes {
        std::ranges::max_element(oneToOneList)->size(),
        std::ranges::max_element(oneToManyList)->size() + 1,
        std::ranges::max_element(belongsToList)->size(),
        std::ranges::max_element(belongsToManyList)->size() + 1,
    };

    return static_cast<QString::size_type>(*std::ranges::max_element(relationSizes));
}

QString ModelCreator::createOneToOneRelationItem(
            const QString &parentClass, const QStringList &relatedClasses,
            const QString::size_type relationsMaxSize)
{
    if (relatedClasses.isEmpty())
        return {};

    QStringList result;
    result.reserve(relatedClasses.size());

    for (const auto &relatedClass : relatedClasses) {

        const auto relationName = guessOneTypeRelationName(relatedClass);
        const auto spaceAlign   = QString(relationsMaxSize - relationName.size(), SPACE);

        result << QString(ModelRelationItemStub)
                  .replace(QStringLiteral("{{ parentClass }}"),  parentClass)
                  .replace(QStringLiteral("{{parentClass}}"),    parentClass)

                  .replace(QStringLiteral("{{ relationName }}"), relationName)
                  .replace(QStringLiteral("{{relationName}}"),   relationName)

                  .replace(QStringLiteral("{{ spaceAlign }}"),   spaceAlign)
                  .replace(QStringLiteral("{{spaceAlign}}"),     spaceAlign);
    }

    return result.join(NEWLINE);
}

QString ModelCreator::createOneToManyRelationItem(
            const QString &parentClass, const QStringList &relatedClasses,
            const QString::size_type relationsMaxSize)
{
    if (relatedClasses.isEmpty())
        return {};

    QStringList result;
    result.reserve(relatedClasses.size());

    for (const auto &relatedClass : relatedClasses) {

        const auto relationName = guessManyTypeRelationName(relatedClass);
        const auto spaceAlign   = QString(relationsMaxSize - relationName.size(), SPACE);

        result << QString(ModelRelationItemStub)
                  .replace(QStringLiteral("{{ parentClass }}"),  parentClass)
                  .replace(QStringLiteral("{{parentClass}}"),    parentClass)

                  .replace(QStringLiteral("{{ relationName }}"), relationName)
                  .replace(QStringLiteral("{{relationName}}"),   relationName)

                  .replace(QStringLiteral("{{ spaceAlign }}"),   spaceAlign)
                  .replace(QStringLiteral("{{spaceAlign}}"),     spaceAlign);
    }

    return result.join(NEWLINE);
}

QString ModelCreator::createBelongsToRelationItem(
            const QString &parentClass, const QStringList &relatedClasses,
            const QString::size_type relationsMaxSize)
{
    if (relatedClasses.isEmpty())
        return {};

    QStringList result;
    result.reserve(relatedClasses.size());

    for (const auto &relatedClass : relatedClasses) {

        const auto relationName = guessOneTypeRelationName(relatedClass);
        const auto spaceAlign   = QString(relationsMaxSize - relationName.size(), SPACE);

        result << QString(ModelRelationItemStub)
                  .replace(QStringLiteral("{{ parentClass }}"),  parentClass)
                  .replace(QStringLiteral("{{parentClass}}"),    parentClass)

                  .replace(QStringLiteral("{{ relationName }}"), relationName)
                  .replace(QStringLiteral("{{relationName}}"),   relationName)

                  .replace(QStringLiteral("{{ spaceAlign }}"),   spaceAlign)
                  .replace(QStringLiteral("{{spaceAlign}}"),     spaceAlign);
    }

    return result.join(NEWLINE);
}

QString ModelCreator::createBelongsToManyRelationItem(
            const QString &parentClass, const QStringList &relatedClasses,
            const QString::size_type relationsMaxSize)
{
    if (relatedClasses.isEmpty())
        return {};

    QStringList result;
    result.reserve(relatedClasses.size());

    for (const auto &relatedClass : relatedClasses) {

        const auto relationName = guessManyTypeRelationName(relatedClass);
        const auto spaceAlign = QString(relationsMaxSize - relationName.size(), SPACE);

        result << QString(ModelRelationItemStub)
                  .replace(QStringLiteral("{{ parentClass }}"),  parentClass)
                  .replace(QStringLiteral("{{parentClass}}"),    parentClass)

                  .replace(QStringLiteral("{{ relationName }}"), relationName)
                  .replace(QStringLiteral("{{relationName}}"),   relationName)

                  .replace(QStringLiteral("{{ spaceAlign }}"),   spaceAlign)
                  .replace(QStringLiteral("{{spaceAlign}}"),     spaceAlign);
    }

    return result.join(NEWLINE);
}

/* Global */

QString ModelCreator::createIncludesSection() const
{
    // Nothing to create
    if (m_includesList.empty())
        return {};

    return NOSPACE.arg("\n\n", ContainerUtils::join(m_includesList, NEWLINE));
}

QString ModelCreator::createUsingsSection() const
{
    // Nothing to create
    if (m_usingsList.empty())
        return {};

    return NOSPACE.arg(NEWLINE, ContainerUtils::join(m_usingsList, NEWLINE));
}

QString ModelCreator::createRelationsList() const
{
    // Nothing to create
    if (m_relationsList.empty())
        return {};

    return ContainerUtils::join(m_relationsList, COMMA).prepend(COMMA);
}

QString ModelCreator::createPivotsList() const
{
    // Nothing to create
    if (m_pivotsList.empty())
        return {};

    return ContainerUtils::join(m_pivotsList, COMMA).prepend(COMMA);
}

QString ModelCreator::createForwardsSection() const
{
    // Nothing to create
    if (m_forwardsList.empty())
        return {};

    return StringUtils::wrapValue(ContainerUtils::join(m_forwardsList, NEWLINE),
                                  QChar('\n'));
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
