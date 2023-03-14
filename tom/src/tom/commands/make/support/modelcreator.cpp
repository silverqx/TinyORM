#include "tom/commands/make/support/modelcreator.hpp"

#include <deque>
#include <fstream>

#include <range/v3/action/push_back.hpp>
#include <range/v3/action/remove_if.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/move.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

#include <orm/constants.hpp>
#include <orm/utils/container.hpp>
#include <orm/utils/string.hpp>

#include "tom/commands/make/modelcommandconcepts.hpp"
#include "tom/commands/make/stubs/modelstubs.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using fspath = std::filesystem::path;

using Orm::Constants::COMMA;
using Orm::Constants::DOT;
using Orm::Constants::EMPTY;
using Orm::Constants::NEWLINE;
using Orm::Constants::NOSPACE;
using Orm::Constants::QUOTE;
using Orm::Constants::SPACE;

using ContainerUtils = Orm::Utils::Container;
using StringUtils    = Orm::Utils::String;

using Tom::Commands::Make::Stubs::BelongsToManyStub;
using Tom::Commands::Make::Stubs::BelongsToManyStub2;
using Tom::Commands::Make::Stubs::BelongsToStub;
using Tom::Commands::Make::Stubs::ModelConnectionStub;
using Tom::Commands::Make::Stubs::ModelDateFormatStub;
using Tom::Commands::Make::Stubs::ModelDatesStub;
using Tom::Commands::Make::Stubs::ModelDisableTimestampsStub;
using Tom::Commands::Make::Stubs::ModelFillableStub;
using Tom::Commands::Make::Stubs::ModelForwardItemStub;
using Tom::Commands::Make::Stubs::ModelGuardedStub;
using Tom::Commands::Make::Stubs::ModelIncludeItemStub;
using Tom::Commands::Make::Stubs::ModelIncludeOrmItemStub;
using Tom::Commands::Make::Stubs::ModelIncrementingStub;
using Tom::Commands::Make::Stubs::ModelPrimaryKeyStub;
using Tom::Commands::Make::Stubs::ModelPrivateStub;
using Tom::Commands::Make::Stubs::ModelPublicStub;
using Tom::Commands::Make::Stubs::ModelRelationItemStub;
using Tom::Commands::Make::Stubs::ModelRelationsStub;
using Tom::Commands::Make::Stubs::ModelStub;
using Tom::Commands::Make::Stubs::ModelTableStub;
using Tom::Commands::Make::Stubs::ModelTouchesStub;
using Tom::Commands::Make::Stubs::ModelUsingItemStub;
using Tom::Commands::Make::Stubs::ModelWithStub;
using Tom::Commands::Make::Stubs::OneToOneStub;
using Tom::Commands::Make::Stubs::OneToManyStub;
using Tom::Commands::Make::Stubs::PivotModelStub;

namespace Tom::Commands::Make::Support
{

/* public */

fspath ModelCreator::create(const QString &className, const CmdOptions &cmdOptions,
                            const fspath &modelsPath, const bool isSetPreserveOrder)
{
    const auto basename = className.toLower();

    auto modelPath = getPath(basename, modelsPath);

    // Output it as binary stream to force line endings to LF
    std::ofstream(modelPath, std::ios::out | std::ios::binary)
            << populateStub(className, cmdOptions, isSetPreserveOrder);

    return modelPath;
}

/* protected */

fspath ModelCreator::getPath(const QString &basename, const fspath &path)
{
    return path / (basename.toStdString() + ".hpp");
}

std::string
ModelCreator::populateStub(const QString &className, const CmdOptions &cmdOptions,
                           const bool isSetPreserveOrder)
{
    const auto publicSection  = createPublicSection(className, cmdOptions,
                                                    isSetPreserveOrder);
    const auto privateSection = createPrivateSection(
                                    className, cmdOptions, !publicSection.isEmpty(),
                                    isSetPreserveOrder);

    const auto macroGuard = className.toUpper();

    const auto includesOrmSection = createIncludesOrmSection(cmdOptions);
    const auto includesSection    = createIncludesSection();
    const auto usingsSection      = createUsingsSection(cmdOptions);
    const auto relationsList      = createRelationsList();
    // I want to have all pivots after the related classes because of that this set exists
    const auto pivotsList         = createPivotsList();
    const auto forwardsSection    = createForwardsSection();

    return QString(cmdOptions.pivotModel ? PivotModelStub : ModelStub)
            .replace(QStringLiteral("DummyClass"),  className)
            .replace(QStringLiteral("{{ class }}"), className)
            .replace(QStringLiteral("{{class}}"),   className)

            .replace(QStringLiteral("{{ macroguard }}"), macroGuard)
            .replace(QStringLiteral("{{macroguard}}"),   macroGuard)

            .replace(QStringLiteral("{{ publicSection }}"), publicSection)
            .replace(QStringLiteral("{{publicSection}}"),   publicSection)

            .replace(QStringLiteral("{{ privateSection }}"), privateSection)
            .replace(QStringLiteral("{{privateSection}}"),   privateSection)

            .replace(QStringLiteral("{{ includesOrmSection }}"), includesOrmSection)
            .replace(QStringLiteral("{{includesOrmSection}}"),   includesOrmSection)
            .replace(QStringLiteral("{{ includesSection }}"),    includesSection)
            .replace(QStringLiteral("{{includesSection}}"),      includesSection)

            .replace(QStringLiteral("{{ usingsSection }}"),      usingsSection)
            .replace(QStringLiteral("{{usingsSection}}"),        usingsSection)
            .replace(QStringLiteral("{{ forwardsSection }}"),    forwardsSection)
            .replace(QStringLiteral("{{forwardsSection}}"),      forwardsSection)

            .replace(QStringLiteral("{{ relationsList }}"), relationsList)
            .replace(QStringLiteral("{{relationsList}}"),   relationsList)
            .replace(QStringLiteral("{{ pivotsList }}"),    pivotsList)
            .replace(QStringLiteral("{{pivotsList}}"),      pivotsList)

            .toStdString();
}

/* Public model section */

QString
ModelCreator::createPublicSection(const QString &className, const CmdOptions &cmdOptions,
                                  const bool isSetPreserveOrder)
{
    const auto &[
            relationsOrder,
            oneToOneList, oneToManyList,  belongsToList,    belongsToManyList,
            foreignKeys,  pivotTables,    pivotClasses,     pivotInverseClasses,
            asList,       withPivotList,  withTimestampsList,
            _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13
    ] = cmdOptions;

    const auto &[
            oneToOneForeign, oneToManyForeign, belongsToForeign, belongsToManyForeign
    ] = foreignKeys;

    const auto &[
            oneToOneOrder, oneToManyOrder, belongsToOrder, belongsToManyOrder
    ] = relationsOrder;

    computeReserveForRelationsList(oneToOneList, oneToManyList, belongsToList,
                                   belongsToManyList);

    // Nothing to do, no relations passed
    if (m_relationsListSize == 0)
        return {};

    RelationsWithOrder publicSectionList;
    publicSectionList.reserve(m_relationsListSize);

    publicSectionList |= ranges::actions::push_back(
                             createOneToOneRelation(className, oneToOneList,
                                                    oneToOneForeign, oneToOneOrder));
    publicSectionList |= ranges::actions::push_back(
                             createOneToManyRelation(className, oneToManyList,
                                                     oneToManyForeign, oneToManyOrder));
    publicSectionList |= ranges::actions::push_back(
                             createBelongsToRelation(className, belongsToList,
                                                     belongsToForeign, belongsToOrder));
    publicSectionList |= ranges::actions::push_back(
                             createBelongsToManyRelation(
                                 className, belongsToManyList, belongsToManyForeign,
                                 belongsToManyOrder, pivotTables, pivotClasses,
                                 pivotInverseClasses, asList, withPivotList,
                                 withTimestampsList));

    // Remove empty parts
    publicSectionList |= ranges::actions::remove_if([](const auto &value)
    {
        return value.content.isEmpty();
    });

    // Sort if the --preserve-order option was given on the command-line
    if (isSetPreserveOrder)
        std::ranges::sort(publicSectionList, {}, &RelationWithOrder::relationOrder);

    auto publicSection = joinRelationsList(std::move(publicSectionList));

    // Prepend public:
    if (!publicSection.isEmpty())
        publicSection.prepend(ModelPublicStub);

    return publicSection;
}

ModelCreator::RelationsWithOrder
ModelCreator::createOneToOneRelation(
        const QString &parentClass, const QStringList &relatedClasses,
        const QStringList &foreignKeys, const std::vector<std::size_t> &orderList)
{
    if (relatedClasses.isEmpty())
        return {};

    Q_ASSERT(relatedClasses.size() == foreignKeys.size());

    RelationsWithOrder result;
    result.reserve(static_cast<std::size_t>(relatedClasses.size()));

    for (const auto &[relatedClass, foreignKey, relationOrder] :
         ranges::views::zip(relatedClasses, foreignKeys, orderList)
    ) {
        // Insert to model includes, usings, and relations lists
        m_includesList.emplace(QString(ModelIncludeItemStub).arg(relatedClass.toLower()));
        m_usingsList.emplace(QString(ModelUsingItemStub).arg(QStringLiteral("HasOne")));
        m_forwardsList.emplace(QString(ModelForwardItemStub).arg(relatedClass));
        m_relationsList.emplace(relatedClass);

        const auto relationName      = guessOneTypeRelationName(relatedClass);
        const auto parentComment     = guessSingularComment(parentClass);
        const auto relatedComment    = guessSingularComment(relatedClass);
        const auto relationArguments = createRelationArguments(foreignKey);

        auto content =
                QString(OneToOneStub)
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

#ifdef __clang__
        result.push_back({relationOrder, std::move(content)});
#else
        result.emplace_back(relationOrder, std::move(content));
#endif
    }

    return result;
}

ModelCreator::RelationsWithOrder
ModelCreator::createOneToManyRelation(
        const QString &parentClass, const QStringList &relatedClasses,
        const QStringList &foreignKeys, const std::vector<std::size_t> &orderList)
{
    if (relatedClasses.isEmpty())
        return {};

    Q_ASSERT(relatedClasses.size() == foreignKeys.size());

    RelationsWithOrder result;
    result.reserve(static_cast<std::size_t>(relatedClasses.size()));

    for (const auto &[relatedClass, foreignKey, relationOrder] :
         ranges::views::zip(relatedClasses, foreignKeys, orderList)
    ) {
        // Insert to model includes, usings, and relations lists
        m_includesList.emplace(QString(ModelIncludeItemStub).arg(relatedClass.toLower()));
        m_usingsList.emplace(QString(ModelUsingItemStub).arg(QStringLiteral("HasMany")));
        m_forwardsList.emplace(QString(ModelForwardItemStub).arg(relatedClass));
        m_relationsList.emplace(relatedClass);

        const auto relationName      = guessManyTypeRelationName(relatedClass);
        const auto parentComment     = guessSingularComment(parentClass);
        const auto relatedComment    = guessPluralComment(relatedClass);
        const auto relationArguments = createRelationArguments(foreignKey);

        auto content =
                QString(OneToManyStub)
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

#ifdef __clang__
        result.push_back({relationOrder, std::move(content)});
#else
        result.emplace_back(relationOrder, std::move(content));
#endif
    }

    return result;
}

ModelCreator::RelationsWithOrder
ModelCreator::createBelongsToRelation(
        const QString &parentClass, const QStringList &relatedClasses,
        const QStringList &foreignKeys, const std::vector<std::size_t> &orderList)
{
    if (relatedClasses.isEmpty())
        return {};

    Q_ASSERT(relatedClasses.size() == foreignKeys.size());

    RelationsWithOrder result;
    result.reserve(static_cast<std::size_t>(relatedClasses.size()));

    for (const auto &[relatedClass, foreignKey, relationOrder] :
         ranges::views::zip(relatedClasses, foreignKeys, orderList)
    ) {
        // Insert to model includes, usings, and relations lists
        m_includesList.emplace(QString(ModelIncludeItemStub).arg(relatedClass.toLower()));
        m_usingsList.emplace(QString(ModelUsingItemStub)
                             .arg(QStringLiteral("BelongsTo")));
        m_forwardsList.emplace(QString(ModelForwardItemStub).arg(relatedClass));
        m_relationsList.emplace(relatedClass);

        const auto relationName      = guessOneTypeRelationName(relatedClass);
        const auto parentComment     = guessSingularComment(parentClass);
        const auto relatedComment    = guessSingularComment(relatedClass);
        const auto relationArguments = createRelationArguments(foreignKey);

        auto content =
                QString(BelongsToStub)
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

#ifdef __clang__
        result.push_back({relationOrder, std::move(content)});
#else
        result.emplace_back(relationOrder, std::move(content));
#endif
    }

    return result;
}

QString ModelCreator::createRelationArguments(const QString &foreignKey)
{
    if (foreignKey.isEmpty())
        return {};

    return StringUtils::wrapValue(foreignKey, QUOTE);
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

ModelCreator::RelationsWithOrder
ModelCreator::createBelongsToManyRelation(
        const QString &parentClass, const QStringList &relatedClasses,
        const std::vector<BelongToManyForeignKeys> &foreignKeys,
        const std::vector<std::size_t> &orderList, const QStringList &pivotTables,
        const QStringList &pivotClasses,
        const std::vector<QStringList> &pivotInverseClasses,
        const QStringList &asList, const std::vector<QStringList> &withPivotList,
        const std::vector<bool> &withTimestampsList)
{
    if (relatedClasses.isEmpty())
        return {};

    const auto relatedClassesSize = static_cast<std::size_t>(relatedClasses.size());

    // All lists must have the same number of items
    Q_ASSERT(allHaveSameSize(relatedClassesSize,
                             foreignKeys, pivotTables, pivotClasses, asList,
                             withPivotList, withTimestampsList));

    RelationsWithOrder result;
    result.reserve(relatedClassesSize);

    for (const auto &[relatedClass, foreignKey, relationOrder, pivotTable, pivotClass,
                      pivotInverseClass, as, withPivot, withTimestamps] :
         ranges::views::zip(relatedClasses, foreignKeys, orderList, pivotTables,
                            pivotClasses, pivotInverseClasses, asList, withPivotList,
                            withTimestampsList)
    ) {
        // Insert to model includes, usings, and relations lists
        m_includesList.emplace(QString(ModelIncludeItemStub).arg(relatedClass.toLower()));
        m_usingsList.emplace(QString(ModelUsingItemStub)
                             .arg(QStringLiteral("BelongsToMany")));
        m_forwardsList.emplace(QString(ModelForwardItemStub).arg(relatedClass));
        m_relationsList.emplace(relatedClass);

        const auto isPivotClassEmpty = pivotClass.isEmpty();

        // Pivot special logic
        handlePivotClass(pivotClass, isPivotClassEmpty);
        // Pivot for an inverse belongs-to-many relation special logic
        handlePivotInverseClass(pivotInverseClass);

        const auto relationName      = guessManyTypeRelationName(relatedClass);
        const auto parentComment     = guessSingularComment(parentClass);
        const auto relatedComment    = guessPluralComment(relatedClass);
        const auto pivot             = isPivotClassEmpty ? EMPTY
                                                         : NOSPACE.arg(COMMA, pivotClass);

        const auto relationCalls     = createRelationCalls(as, withPivot, withTimestamps);
        const auto relationArguments = createRelationArgumentsBtm(pivotTable, foreignKey);

        auto content =
                QString(relationCalls.isEmpty() ? BelongsToManyStub
                                                : BelongsToManyStub2)
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

#ifdef __clang__
        result.push_back({relationOrder, std::move(content)});
#else
        result.emplace_back(relationOrder, std::move(content));
#endif
    }

    return result;
}

QString ModelCreator::createRelationArgumentsBtm(
        const QString &pivotTable, const BelongToManyForeignKeys &foreignKey)
{
    const auto &[foreignPivotKey, relatedPivotKey] = foreignKey;

    std::deque<QString> argumentsList;

    if (!relatedPivotKey.isEmpty())
        argumentsList.push_back(StringUtils::wrapValue(relatedPivotKey, QUOTE));

    if (!foreignPivotKey.isEmpty())
        argumentsList.push_front(StringUtils::wrapValue(foreignPivotKey, QUOTE));
    else if (!relatedPivotKey.isEmpty())
        argumentsList.push_front(QStringLiteral("{}"));

    // Table name of the relationship's intermediate table
    if (!pivotTable.isEmpty())
        argumentsList.push_front(StringUtils::wrapValue(pivotTable, QUOTE));
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
    // Nothing to do
    if (isPivotClassEmpty)
        return;

    // User defined Pivot
    m_includesList.emplace(QString(ModelIncludeItemStub).arg(pivotClass.toLower()));
}

void ModelCreator::handlePivotInverseClass(const QStringList &pivotInverseClasses)
{
    // Nothing to do
    if (pivotInverseClasses.isEmpty())
        return;

    static const auto PivotConst = QStringLiteral("Pivot");

    std::ranges::for_each(pivotInverseClasses, [this](const auto &inverseClass)
    {
        // Nothing to do
        if (inverseClass.isEmpty())
            return;

        m_pivotsList.emplace(inverseClass);

        // Pivot class needs <orm> include and also using Orm::...
        if (inverseClass == PivotConst) {
            m_usingsList.emplace(QString(ModelUsingItemStub).arg(PivotConst));
            m_includesOrmList.emplace(QString(ModelIncludeOrmItemStub)
                                      .arg(QStringLiteral("tiny/relations/pivot.hpp")));
        } else
            m_includesList.emplace(QString(ModelIncludeItemStub)
                                   .arg(inverseClass.toLower()));
    });
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
            return StringUtils::wrapValue(pivotName, QUOTE);
        })
                | ranges::to<QStringList>();

        relationCalls += QStringLiteral("%1withPivot(%2)")
                         .arg(relationCallsAlign(relationCalls),
                              // Wrap in init. list {} if more pivot columns passed
                              withPivot.size() == 1
                              ? withPivotWrapped.constFirst()
                              : StringUtils::wrapValue(withPivotWrapped.join(COMMA),
                                                       QLatin1Char('{'),
                                                       QLatin1Char('}')));
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
    return StringUtils::snake(className, SPACE).append(QLatin1Char('s'));
}

QString ModelCreator::guessOneTypeRelationName(const QString &className)
{
    return StringUtils::camel(className);
}

QString ModelCreator::guessManyTypeRelationName(const QString &className)
{
    return guessOneTypeRelationName(className).append(QLatin1Char('s'));
}

/* Private model section */

QString ModelCreator::createPrivateSection(
        const QString &className, const CmdOptions &cmdOptions,
        const bool hasPublicSection, const bool isSetPreserveOrder)
{
    const auto &[
            _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12,
            table,        primaryKey, connection,
            with,         fillable,   guarded,
            dateFormat,   dates,      touches,
            incrementing, disableIncrementing, disableTimestamps, _13
    ] = cmdOptions;

    QString privateSection;

    privateSection += createRelationsHash(className, cmdOptions, isSetPreserveOrder);

    // Append a newline after the relations hash
    if (!privateSection.isEmpty() && anyModelOptionGiven(cmdOptions))
        privateSection.append(NEWLINE);

    if (!table.isEmpty())
        privateSection += QString(ModelTableStub).arg(table);

    if (!primaryKey.isEmpty())
        privateSection += QString(ModelPrimaryKeyStub).arg(primaryKey);

    if (incrementing)
        privateSection += QString(ModelIncrementingStub).arg(QStringLiteral("true"));
    else if (disableIncrementing)
        privateSection += QString(ModelIncrementingStub).arg(QStringLiteral("false"));

    if (!connection.isEmpty())
        privateSection += QString(ModelConnectionStub).arg(connection);

    if (!with.isEmpty())
        privateSection += QString(ModelWithStub).arg(prepareInitializerListValues(with));

    if (!fillable.isEmpty())
        privateSection += QString(ModelFillableStub)
                          .arg(prepareInitializerListValues(fillable));

    if (!guarded.isEmpty())
        privateSection += QString(ModelGuardedStub)
                          .arg(prepareInitializerListValues(guarded));

    if (disableTimestamps)
        privateSection += ModelDisableTimestampsStub;

    if (!dateFormat.isEmpty())
        privateSection += QString(ModelDateFormatStub).arg(dateFormat);

    if (!dates.isEmpty())
        privateSection += QString(ModelDatesStub)
                          .arg(prepareInitializerListValues(dates));

    if (!touches.isEmpty())
        privateSection += QString(ModelTouchesStub)
                          .arg(prepareInitializerListValues(touches));

    if (!privateSection.isEmpty()) {
        // Prepend the private: specifier if the public section exists
        if (hasPublicSection)
            privateSection.prepend(ModelPrivateStub);
        else
            privateSection.prepend(NEWLINE);
    }

    return privateSection;
}

bool ModelCreator::anyModelOptionGiven(const CmdOptions &cmdOptions)
{
    const auto &[
            _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12,
            table,        primaryKey, connection,
            with,         fillable,   guarded,
            dateFormat,   dates,      touches,
            incrementing, disableIncrementing, disableTimestamps, _13
    ] = cmdOptions;

    return !table.isEmpty()       || !primaryKey.isEmpty() || !connection.isEmpty() ||
           !with.isEmpty()        || !fillable.isEmpty()   || !guarded.isEmpty()    ||
           !dateFormat.isEmpty()  || !dates.isEmpty()      || !touches.isEmpty()    ||
           incrementing           || disableIncrementing   || disableTimestamps;
}

QString ModelCreator::prepareInitializerListValues(const QStringList &list)
{
    /*! Wrap values in quotes and add the given prefix. */
    const auto wrapValues = [](const QStringList &values, const QString &prefix)
    {
        return values
                | ranges::views::transform([&prefix](const auto &value)
        {
            return QStringLiteral("%2\"%1\"").arg(value, prefix);
        })
                | ranges::to<std::vector<QString>>();
    };

    const auto listSize = list.size();

    /* All values will be wrapped in the quotes.
       List with one value will be simply {"xx"}.
       List with two values will be on a new line and values will be separated by the ,.
       If a list has >2 values then every value will be on a new line. */

    auto listJoined = ContainerUtils::join(
                          wrapValues(list, listSize > 2 ? QString(8, SPACE)
                                                        : QString("")),
                          listSize > 2 ? QStringLiteral(",\n") : COMMA);

    if (listSize > 2)
        listJoined.prepend(NEWLINE);
    else if (listSize > 1)
        listJoined.prepend(NOSPACE.arg(NEWLINE, QString(8, SPACE)));

    if (listSize > 1)
        listJoined.append(NOSPACE.arg(NEWLINE, QString(4, SPACE)));

    return listJoined;
}

QString ModelCreator::createRelationsHash(
        const QString &className, const CmdOptions &cmdOptions,
        const bool isSetPreserveOrder) const
{
    const auto &[
            relationsOrder,
            oneToOneList, oneToManyList, belongsToList, belongsToManyList,
            _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17,
            _18, _19, _20
    ] = cmdOptions;

    // Nothing to create, no relations passed
    if (m_relationsListSize == 0)
        return {};

    // Get max. size of relation names for align
    const auto relationsMaxSize = getRelationNamesMaxSize(cmdOptions);

    const auto &[
            oneToOneOrder, oneToManyOrder, belongsToOrder, belongsToManyOrder
    ] = relationsOrder;

    RelationsWithOrder relationItemsList;
    relationItemsList.reserve(m_relationsListSize); // Use already the cached size value

    relationItemsList |= ranges::actions::push_back(
                             createOneToOneRelationItem(
                                 className, oneToOneList,
                                 relationsMaxSize, oneToOneOrder));
    relationItemsList |= ranges::actions::push_back(
                             createOneToManyRelationItem(
                                 className, oneToManyList, relationsMaxSize,
                                 oneToManyOrder));
    relationItemsList |= ranges::actions::push_back(
                             createBelongsToRelationItem(
                                 className, belongsToList, relationsMaxSize,
                                 belongsToOrder));
    relationItemsList |= ranges::actions::push_back(
                             createBelongsToManyRelationItem(
                                 className, belongsToManyList, relationsMaxSize,
                                 belongsToManyOrder));

    // Sort if the --preserve-order option was given on the command-line
    if (isSetPreserveOrder)
        std::ranges::sort(relationItemsList, {}, &RelationWithOrder::relationOrder);

    auto relationItems = joinRelationsList(std::move(relationItemsList));

    return QString(ModelRelationsStub)
            .replace(QStringLiteral("{{ relationItems }}"), relationItems)
            .replace(QStringLiteral("{{relationItems}}"),   relationItems);
}

QString::size_type ModelCreator::getRelationNamesMaxSize(const CmdOptions &cmdOptions)
{
    const auto &[
            _1,
            oneToOneList, oneToManyList, belongsToList, belongsToManyList,
            _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18,
            _19, _20, _21
    ] = cmdOptions;

    // Get max. size of relation names for align
    // +1 because the s character is appended for the many type relations
    const std::vector relationSizes {
        oneToOneList.empty() ? 0 : std::ranges::max_element(oneToOneList)->size(),
        oneToManyList.empty() ? 0 : std::ranges::max_element(oneToManyList)->size() + 1,
        belongsToList.empty() ? 0 : std::ranges::max_element(belongsToList)->size(),
        belongsToManyList.empty() ? 0 : std::ranges::max_element(
                                            belongsToManyList)->size() + 1,
    };

    return static_cast<QString::size_type>(*std::ranges::max_element(relationSizes));
}

ModelCreator::RelationsWithOrder
ModelCreator::createOneToOneRelationItem(
        const QString &parentClass, const QStringList &relatedClasses,
        const QString::size_type relationsMaxSize,
        const std::vector<std::size_t> &orderList)
{
    if (relatedClasses.isEmpty())
        return {};

    RelationsWithOrder result;
    result.reserve(static_cast<std::size_t>(relatedClasses.size()));

    for (const auto &[relatedClass, relationOrder] :
         ranges::views::zip(relatedClasses, orderList)
    ) {
        const auto relationName = guessOneTypeRelationName(relatedClass);
        const auto spaceAlign   = QString(relationsMaxSize - relationName.size(), SPACE);

        auto content = QString(ModelRelationItemStub)
                       .replace(QStringLiteral("{{ parentClass }}"),  parentClass)
                       .replace(QStringLiteral("{{parentClass}}"),    parentClass)

                       .replace(QStringLiteral("{{ relationName }}"), relationName)
                       .replace(QStringLiteral("{{relationName}}"),   relationName)

                       .replace(QStringLiteral("{{ spaceAlign }}"),   spaceAlign)
                       .replace(QStringLiteral("{{spaceAlign}}"),     spaceAlign);

#ifdef __clang__
        result.push_back({relationOrder, std::move(content)});
#else
        result.emplace_back(relationOrder, std::move(content));
#endif
    }

    return result;
}

ModelCreator::RelationsWithOrder
ModelCreator::createOneToManyRelationItem(
        const QString &parentClass, const QStringList &relatedClasses,
        const QString::size_type relationsMaxSize,
        const std::vector<std::size_t> &orderList)
{
    if (relatedClasses.isEmpty())
        return {};

    RelationsWithOrder result;
    result.reserve(static_cast<std::size_t>(relatedClasses.size()));

    for (const auto &[relatedClass, relationOrder] :
         ranges::views::zip(relatedClasses, orderList)
    ) {
        const auto relationName = guessManyTypeRelationName(relatedClass);
        const auto spaceAlign   = QString(relationsMaxSize - relationName.size(), SPACE);

        auto content = QString(ModelRelationItemStub)
                       .replace(QStringLiteral("{{ parentClass }}"),  parentClass)
                       .replace(QStringLiteral("{{parentClass}}"),    parentClass)

                       .replace(QStringLiteral("{{ relationName }}"), relationName)
                       .replace(QStringLiteral("{{relationName}}"),   relationName)

                       .replace(QStringLiteral("{{ spaceAlign }}"),   spaceAlign)
                       .replace(QStringLiteral("{{spaceAlign}}"),     spaceAlign);

#ifdef __clang__
        result.push_back({relationOrder, std::move(content)});
#else
        result.emplace_back(relationOrder, std::move(content));
#endif
    }

    return result;
}

ModelCreator::RelationsWithOrder
ModelCreator::createBelongsToRelationItem(
        const QString &parentClass, const QStringList &relatedClasses,
        const QString::size_type relationsMaxSize,
        const std::vector<std::size_t> &orderList)
{
    if (relatedClasses.isEmpty())
        return {};

    RelationsWithOrder result;
    result.reserve(static_cast<std::size_t>(relatedClasses.size()));

    for (const auto &[relatedClass, relationOrder] :
         ranges::views::zip(relatedClasses, orderList)
    ) {
        const auto relationName = guessOneTypeRelationName(relatedClass);
        const auto spaceAlign   = QString(relationsMaxSize - relationName.size(), SPACE);

        auto content = QString(ModelRelationItemStub)
                       .replace(QStringLiteral("{{ parentClass }}"),  parentClass)
                       .replace(QStringLiteral("{{parentClass}}"),    parentClass)

                       .replace(QStringLiteral("{{ relationName }}"), relationName)
                       .replace(QStringLiteral("{{relationName}}"),   relationName)

                       .replace(QStringLiteral("{{ spaceAlign }}"),   spaceAlign)
                       .replace(QStringLiteral("{{spaceAlign}}"),     spaceAlign);

#ifdef __clang__
        result.push_back({relationOrder, std::move(content)});
#else
        result.emplace_back(relationOrder, std::move(content));
#endif
    }

    return result;
}

ModelCreator::RelationsWithOrder
ModelCreator::createBelongsToManyRelationItem(
        const QString &parentClass, const QStringList &relatedClasses,
        const QString::size_type relationsMaxSize,
        const std::vector<std::size_t> &orderList)
{
    if (relatedClasses.isEmpty())
        return {};

    RelationsWithOrder result;
    result.reserve(static_cast<std::size_t>(relatedClasses.size()));

    for (const auto &[relatedClass, relationOrder] :
         ranges::views::zip(relatedClasses, orderList)
    ) {
        const auto relationName = guessManyTypeRelationName(relatedClass);
        const auto spaceAlign = QString(relationsMaxSize - relationName.size(), SPACE);

        auto content = QString(ModelRelationItemStub)
                       .replace(QStringLiteral("{{ parentClass }}"),  parentClass)
                       .replace(QStringLiteral("{{parentClass}}"),    parentClass)

                       .replace(QStringLiteral("{{ relationName }}"), relationName)
                       .replace(QStringLiteral("{{relationName}}"),   relationName)

                       .replace(QStringLiteral("{{ spaceAlign }}"),   spaceAlign)
                       .replace(QStringLiteral("{{spaceAlign}}"),     spaceAlign);

#ifdef __clang__
        result.push_back({relationOrder, std::move(content)});
#else
        result.emplace_back(relationOrder, std::move(content));
#endif
    }

    return result;
}

/* Global */

QString ModelCreator::createIncludesOrmSection(const CmdOptions &cmdOptions)
{
    // Will always contain
    m_includesOrmList.emplace(QString(ModelIncludeOrmItemStub)
                              .arg(cmdOptions.pivotModel
                                   ? QStringLiteral("tiny/relations/basepivot.hpp")
                                   : QStringLiteral("tiny/model.hpp")));

    return ContainerUtils::join(m_includesOrmList, NEWLINE);
}

QString ModelCreator::createIncludesSection() const
{
    // Nothing to create
    if (m_includesList.empty())
        return {};

    return NOSPACE.arg("\n\n", ContainerUtils::join(m_includesList, NEWLINE));
}

QString ModelCreator::createUsingsSection(const CmdOptions &cmdOptions)
{
    // Will always contain
    m_usingsList.emplace(cmdOptions.pivotModel
                         ? QStringLiteral("using Orm::Tiny::Relations::BasePivot;")
                         : QStringLiteral("using Orm::Tiny::Model;"));

    return ContainerUtils::join(m_usingsList, NEWLINE);
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
                                  QChar(QChar::LineFeed));
}

/* Common for public/private sections */

std::size_t ModelCreator::computeReserveForRelationsList(
        const QStringList &oneToOne, const QStringList &oneToMany,
        const QStringList &belongsTo, const QStringList &belongsToMany)
{
    // Cache the computed reserve size to avoid recomputation in the private section
    m_relationsListSize = static_cast<std::size_t>(oneToOne.size()) +
                          static_cast<std::size_t>(oneToMany.size()) +
                          static_cast<std::size_t>(belongsTo.size()) +
                          static_cast<std::size_t>(belongsToMany.size());

    return m_relationsListSize;
}

QString ModelCreator::joinRelationsList(RelationsWithOrder &&relationsList)
{
    auto relationsQList = ranges::views::move(relationsList)
            | ranges::views::transform([](auto &&relationItem) -> QString
    {
        return relationItem.content;
    })
            | ranges::to<QStringList>();

    return relationsQList.join(NEWLINE);
}

} // namespace Tom::Commands::Make::Support

TINYORM_END_COMMON_NAMESPACE
