#include "tom/commands/make/support/modelcreator.hpp"

#include <deque>
#include <fstream>

#include <range/v3/action/push_back.hpp>
#include <range/v3/action/remove_if.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/move.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

#include <orm/constants.hpp>
#include <orm/macros/likely.hpp>
#include <orm/utils/container.hpp>
#include <orm/utils/string.hpp>

#include "tom/commands/make/modelcommandconcepts.hpp" // IWYU pragma: keep
#include "tom/commands/make/stubs/modelstubs.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Qt::StringLiterals::operator""_s;

using fspath = std::filesystem::path;

using Orm::Constants::COMMA;
using Orm::Constants::DOT;
using Orm::Constants::EMPTY;
using Orm::Constants::NEWLINE_C;
using Orm::Constants::NOSPACE;
using Orm::Constants::QUOTE;
using Orm::Constants::SPACE;

using ContainerUtils = Orm::Utils::Container;
using StringUtils    = Orm::Utils::String;

using Tom::Commands::Make::Stubs::AccessorMethodStub;
using Tom::Commands::Make::Stubs::BelongsToManyStub;
using Tom::Commands::Make::Stubs::BelongsToManyStub2;
using Tom::Commands::Make::Stubs::BelongsToStub;
using Tom::Commands::Make::Stubs::ModelAppendsStub;
using Tom::Commands::Make::Stubs::ModelCastsExampleStub;
using Tom::Commands::Make::Stubs::ModelConnectionStub;
using Tom::Commands::Make::Stubs::ModelDateFormatStub;
using Tom::Commands::Make::Stubs::ModelDatesStub;
using Tom::Commands::Make::Stubs::ModelDisableTimestampsStub;
using Tom::Commands::Make::Stubs::ModelFillableStub;
using Tom::Commands::Make::Stubs::ModelForwardItemStub;
using Tom::Commands::Make::Stubs::ModelGuardedStub;
using Tom::Commands::Make::Stubs::ModelHiddenStub;
using Tom::Commands::Make::Stubs::ModelIncludeItemStub;
using Tom::Commands::Make::Stubs::ModelIncludeOrmItemStub;
using Tom::Commands::Make::Stubs::ModelIncrementingStub;
using Tom::Commands::Make::Stubs::ModelMutatorItemStub;
using Tom::Commands::Make::Stubs::ModelMutatorsStub;
using Tom::Commands::Make::Stubs::ModelPrimaryKeyStub;
using Tom::Commands::Make::Stubs::ModelPrivateStub;
using Tom::Commands::Make::Stubs::ModelProtectedStub;
using Tom::Commands::Make::Stubs::ModelPublicStub;
using Tom::Commands::Make::Stubs::ModelRelationItemStub;
using Tom::Commands::Make::Stubs::ModelRelationsStub;
using Tom::Commands::Make::Stubs::ModelSnakeAttributesStub;
using Tom::Commands::Make::Stubs::ModelStub;
using Tom::Commands::Make::Stubs::ModelTableStub;
using Tom::Commands::Make::Stubs::ModelTouchesStub;
using Tom::Commands::Make::Stubs::ModelVisibleStub;
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
    const auto publicSection    = createPublicSection(className, cmdOptions,
                                                      isSetPreserveOrder);
    const auto protectedSection = createProtectedSection(cmdOptions);
    const auto privateSection   = createPrivateSection(
                                      className, cmdOptions, !publicSection.isEmpty(),
                                      !protectedSection.isEmpty(), isSetPreserveOrder);

    const auto macroGuard = className.toUpper();

    const auto includesOrmSection = createIncludesOrmSection(cmdOptions);
    const auto includesSection    = createIncludesSection();
    const auto usingsSection      = createUsingsSection(cmdOptions);
    const auto relationsList      = createRelationsList();
    // I want to have all pivots after the related classes because of that this set exists
    const auto pivotsList         = createPivotsList();
    const auto forwardsSection    = createForwardsSection();

    return QString(cmdOptions.pivotModel ? PivotModelStub : ModelStub)
            .replace(u"DummyClass"_s,  className)
            .replace(u"{{ class }}"_s, className)
            .replace(u"{{class}}"_s,   className)

            .replace(u"{{ macroguard }}"_s, macroGuard)
            .replace(u"{{macroguard}}"_s,   macroGuard)

            .replace(u"{{ publicSection }}"_s, publicSection)
            .replace(u"{{publicSection}}"_s,   publicSection)

            .replace(u"{{ protectedSection }}"_s, protectedSection)
            .replace(u"{{protectedSection}}"_s,   protectedSection)

            .replace(u"{{ privateSection }}"_s, privateSection)
            .replace(u"{{privateSection}}"_s,   privateSection)

            .replace(u"{{ includesOrmSection }}"_s, includesOrmSection)
            .replace(u"{{includesOrmSection}}"_s,   includesOrmSection)
            .replace(u"{{ includesSection }}"_s,    includesSection)
            .replace(u"{{includesSection}}"_s,      includesSection)

            .replace(u"{{ usingsSection }}"_s,   usingsSection)
            .replace(u"{{usingsSection}}"_s,     usingsSection)
            .replace(u"{{ forwardsSection }}"_s, forwardsSection)
            .replace(u"{{forwardsSection}}"_s,   forwardsSection)

            .replace(u"{{ relationsList }}"_s, relationsList)
            .replace(u"{{relationsList}}"_s,   relationsList)
            .replace(u"{{ pivotsList }}"_s,    pivotsList)
            .replace(u"{{pivotsList}}"_s,      pivotsList)

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
            _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17,
            _18, _19, _20
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
    if (m_relationsListsSize == 0)
        return {};

    RelationsWithOrder publicSectionList;
    publicSectionList.reserve(m_relationsListsSize);

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

/* Relationships section */

ModelCreator::RelationsWithOrder
ModelCreator::createOneToOneRelation(
        const QString &parentClass, const QStringList &relatedClasses,
        const QStringList &foreignKeys, const std::vector<std::size_t> &orderList)
{
    if (relatedClasses.isEmpty())
        return {};

    Q_ASSERT(relatedClasses.size() == foreignKeys.size());

    RelationsWithOrder result;
    result.reserve(static_cast<decltype (result)::size_type>(relatedClasses.size()));

    for (const auto &[relatedClass, foreignKey, relationOrder] :
         ranges::views::zip(relatedClasses, foreignKeys, orderList)
    ) {
        // Nothing to do, don't create relationship methods with empty names
        if (relatedClass.isEmpty())
            continue;

        // Insert to model includes, using-s, and relations lists
        m_includesList.emplace(ModelIncludeItemStub.arg(relatedClass.toLower()));
        m_forwardsList.emplace(ModelForwardItemStub.arg(relatedClass));
        m_relationsList.emplace(relatedClass);

        const auto relationName      = guessOneTypeRelationName(relatedClass);
        const auto parentComment     = guessSingularComment(parentClass);
        const auto relatedComment    = guessSingularComment(relatedClass);
        const auto relationArguments = createRelationArguments(foreignKey);

        auto content =
                QString(OneToOneStub)
                .replace(u"{{ parentClass }}"_s,     parentClass)
                .replace(u"{{parentClass}}"_s,       parentClass)
                .replace(u"{{ relatedClass }}"_s,    relatedClass)
                .replace(u"{{relatedClass}}"_s,      relatedClass)

                .replace(u"{{ relationName }}"_s,    relationName)
                .replace(u"{{relationName}}"_s,      relationName)

                .replace(u"{{ parentComment }}"_s,   parentComment)
                .replace(u"{{parentComment}}"_s,     parentComment)
                .replace(u"{{ relatedComment }}"_s,  relatedComment)
                .replace(u"{{relatedComment}}"_s,    relatedComment)

                .replace(u"{{ relationArguments }}"_s, relationArguments)
                .replace(u"{{relationArguments}}"_s,   relationArguments);

#if defined(__clang__) && __clang_major__ < 16
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
    result.reserve(static_cast<decltype (result)::size_type>(relatedClasses.size()));

    for (const auto &[relatedClass, foreignKey, relationOrder] :
         ranges::views::zip(relatedClasses, foreignKeys, orderList)
    ) {
        // Nothing to do, don't create relationship methods with empty names
        if (relatedClass.isEmpty())
            continue;

        // Insert to model includes, using-s, and relations lists
        m_includesList.emplace(ModelIncludeItemStub.arg(relatedClass.toLower()));
        m_forwardsList.emplace(ModelForwardItemStub.arg(relatedClass));
        m_relationsList.emplace(relatedClass);

        const auto relationName      = guessManyTypeRelationName(relatedClass);
        const auto parentComment     = guessSingularComment(parentClass);
        const auto relatedComment    = guessPluralComment(relatedClass);
        const auto relationArguments = createRelationArguments(foreignKey);

        auto content =
                QString(OneToManyStub)
                .replace(u"{{ parentClass }}"_s,     parentClass)
                .replace(u"{{parentClass}}"_s,       parentClass)
                .replace(u"{{ relatedClass }}"_s,    relatedClass)
                .replace(u"{{relatedClass}}"_s,      relatedClass)

                .replace(u"{{ relationName }}"_s,    relationName)
                .replace(u"{{relationName}}"_s,      relationName)

                .replace(u"{{ parentComment }}"_s,   parentComment)
                .replace(u"{{parentComment}}"_s,     parentComment)
                .replace(u"{{ relatedComment }}"_s,  relatedComment)
                .replace(u"{{relatedComment}}"_s,    relatedComment)

                .replace(u"{{ relationArguments }}"_s, relationArguments)
                .replace(u"{{relationArguments}}"_s,   relationArguments);

#if defined(__clang__) && __clang_major__ < 16
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
    result.reserve(static_cast<decltype (result)::size_type>(relatedClasses.size()));

    for (const auto &[relatedClass, foreignKey, relationOrder] :
         ranges::views::zip(relatedClasses, foreignKeys, orderList)
    ) {
        // Nothing to do, don't create relationship methods with empty names
        if (relatedClass.isEmpty())
            continue;

        // Insert to model includes, using-s, and relations lists
        m_includesList.emplace(ModelIncludeItemStub.arg(relatedClass.toLower()));
        m_forwardsList.emplace(ModelForwardItemStub.arg(relatedClass));
        m_relationsList.emplace(relatedClass);

        const auto relationName      = guessOneTypeRelationName(relatedClass);
        const auto parentComment     = guessSingularComment(parentClass);
        const auto relatedComment    = guessSingularComment(relatedClass);
        const auto relationArguments = createRelationArguments(foreignKey);

        auto content =
                QString(BelongsToStub)
                .replace(u"{{ parentClass }}"_s,     parentClass)
                .replace(u"{{parentClass}}"_s,       parentClass)
                .replace(u"{{ relatedClass }}"_s,    relatedClass)
                .replace(u"{{relatedClass}}"_s,      relatedClass)

                .replace(u"{{ relationName }}"_s,    relationName)
                .replace(u"{{relationName}}"_s,      relationName)

                .replace(u"{{ parentComment }}"_s,   parentComment)
                .replace(u"{{parentComment}}"_s,     parentComment)
                .replace(u"{{ relatedComment }}"_s,  relatedComment)
                .replace(u"{{relatedComment}}"_s,    relatedComment)

                .replace(u"{{ relationArguments }}"_s, relationArguments)
                .replace(u"{{relationArguments}}"_s,   relationArguments);

#if defined(__clang__) && __clang_major__ < 16
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
        /* I created containerSize() function that did static_cast<> only if (constexpr)
           !is_same_v<C::size_type, std::size_t> but it reported -Wsign-compare diagnostic
           warning on this line, so only option is to always call static_cast<>. */
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

    const auto relatedClassesSize = static_cast<RelationsWithOrder::size_type>(
                                        relatedClasses.size());

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
        // Nothing to do, don't create relationship methods with empty names
        if (relatedClass.isEmpty())
            continue;

        // Insert to model includes, using-s, and relations lists
        m_includesList.emplace(ModelIncludeItemStub.arg(relatedClass.toLower()));
        m_forwardsList.emplace(ModelForwardItemStub.arg(relatedClass));
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
                .replace(u"{{ parentClass }}"_s,     parentClass)
                .replace(u"{{parentClass}}"_s,       parentClass)
                .replace(u"{{ relatedClass }}"_s,    relatedClass)
                .replace(u"{{relatedClass}}"_s,      relatedClass)

                .replace(u"{{ relationName }}"_s,    relationName)
                .replace(u"{{relationName}}"_s,      relationName)

                .replace(u"{{ parentComment }}"_s,   parentComment)
                .replace(u"{{parentComment}}"_s,     parentComment)
                .replace(u"{{ relatedComment }}"_s,  relatedComment)
                .replace(u"{{relatedComment}}"_s,    relatedComment)

                .replace(u"{{ pivotClass }}"_s,      pivot)
                .replace(u"{{pivotClass}}"_s,        pivot)
                .replace(u"{{ relationCalls }}"_s,   relationCalls)
                .replace(u"{{relationCalls}}"_s,     relationCalls)

                .replace(u"{{ relationArguments }}"_s, relationArguments)
                .replace(u"{{relationArguments}}"_s,   relationArguments);

#if defined(__clang__) && __clang_major__ < 16
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
        argumentsList.push_front(u"{}"_s);

    // Table name of the relationship's intermediate table
    if (!pivotTable.isEmpty())
        argumentsList.push_front(StringUtils::wrapValue(pivotTable, QUOTE));
    else if (!foreignPivotKey.isEmpty() || !relatedPivotKey.isEmpty())
        argumentsList.push_front(u"{}"_s);

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

    // User-defined Pivot
    m_includesList.emplace(ModelIncludeItemStub.arg(pivotClass.toLower()));
}

void ModelCreator::handlePivotInverseClass(const QStringList &pivotInverseClasses)
{
    // Nothing to do
    if (pivotInverseClasses.isEmpty())
        return;

    static const auto PivotConst = u"Pivot"_s;

    std::ranges::for_each(pivotInverseClasses, [this](const auto &inverseClass)
    {
        // Nothing to do
        if (inverseClass.isEmpty())
            return;

        m_pivotsList.emplace(inverseClass);

        // Pivot class needs <orm> include and also using Orm::...
        if (inverseClass == PivotConst) {
            m_usingsList.emplace(ModelUsingItemStub.arg(PivotConst));
            m_includesOrmList.emplace(ModelIncludeOrmItemStub
                                      .arg(u"tiny/relations/pivot.hpp"_s));
        } else
            m_includesList.emplace(ModelIncludeItemStub.arg(inverseClass.toLower()));
    });
}

QString ModelCreator::createRelationCalls(
        const QString &as, const QStringList &withPivot, const bool withTimestamps)
{
    /*! Align a method call on the newline. */
    const auto relationCallsAlign = [](const auto &relationCalls)
    {
        return relationCalls.isEmpty() ? EMPTY
                                       : u"%1%2%3"_s.arg(NEWLINE_C,
                                                         QString(16, SPACE)).arg(DOT);
    };

    QString relationCalls;

    // The name for the pivot relation
    if (!as.isEmpty())
        relationCalls = uR"(as("%1"))"_s.arg(as);

    // Extra attributes for the pivot model
    if (!withPivot.isEmpty()) {
        // Wrap the pivot name inside quotes
        const auto withPivotWrapped = withPivot
                | ranges::views::transform([](const auto &pivotName)
        {
            return StringUtils::wrapValue(pivotName, QUOTE);
        })
                | ranges::to<QStringList>();

        relationCalls += u"%1withPivot(%2)"_s
                         .arg(relationCallsAlign(relationCalls),
                              // Wrap in init. list {} if more pivot columns passed
                              withPivot.size() == 1
                              ? withPivotWrapped.constFirst()
                              : StringUtils::wrapValue(withPivotWrapped.join(COMMA),
                                                       u'{', u'}'));
    }

    // Pivot table with timestamps
    if (withTimestamps)
        relationCalls += u"%1withTimestamps()"_s
                         .arg(relationCallsAlign(relationCalls));

    return relationCalls;
}

QString ModelCreator::guessSingularComment(const QString &className)
{
    return StringUtils::snake(className, SPACE);
}

QString ModelCreator::guessPluralComment(const QString &className)
{
    return StringUtils::snake(className, SPACE).append(u's');
}

QString ModelCreator::guessOneTypeRelationName(const QString &className)
{
    return StringUtils::camel(className);
}

QString ModelCreator::guessManyTypeRelationName(const QString &className)
{
    return guessOneTypeRelationName(className).append(u's');
}

/* Protected model section */

QString ModelCreator::createProtectedSection(const CmdOptions &cmdOptions)
{
    const auto &[
            _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17,
            _18, _19, _20, _21, _22, _23,
            accessors, appends,
            _26, _27, _28, _29, _30, _31, _32
    ] = cmdOptions;

    QString protectedSection;

    protectedSection += createAccessorMethods(accessors, appends);

    // Prepend the protected: specifier
    if (!protectedSection.isEmpty())
        protectedSection.prepend(ModelProtectedStub);

    return protectedSection;
}

QString ModelCreator::createAccessorMethods(const QStringList &accessors,
                                            const QStringList &appends)
{
    // Nothing to create, no accessors or appends passed
    if (accessors.empty() && appends.empty())
        return {};

    // Prepare accessor attribute names
    const auto accessorNames = prepareMutatorNames(accessors, appends);

    // Loop over accessor names and create individual accessor methods
    QStringList accessorMethodsList;
    accessorMethodsList.reserve(static_cast<decltype (accessorMethodsList)::size_type>(
                                    accessorNames.size()));

    for (const auto &accessor : accessorNames) {
        const auto accessorCamel = StringUtils::camel(accessor);

        accessorMethodsList
                << QString(AccessorMethodStub)
                   .replace(u"{{ accessorNameCamel }}"_s, accessorCamel)
                   .replace(u"{{accessorNameCamel}}"_s,   accessorCamel)

                   .replace(u"{{ accessorNameSnake }}"_s, accessor)
                   .replace(u"{{accessorNameSnake}}"_s,   accessor);
    }

    return accessorMethodsList.join(NEWLINE_C);
}

/* Private model section */

QString ModelCreator::createPrivateSection(
        const QString &className, const CmdOptions &cmdOptions,
        const bool hasPublicSection, const bool hasProtectedSection,
        const bool isSetPreserveOrder)
{
    const auto &[
            _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12,
            table,           primaryKey, connection,
            with,            fillable,   guarded,
            dateFormat,      dates,      touches,
            visible,         hidden,
            accessors,       appends,
            incrementing,    disableIncrementing, disableTimestamps,
            castsExample,
            snakeAttributes, disableSnakeAttributes, _13
    ] = cmdOptions;

    QString privateSection;

    privateSection += createRelationsHash(className, cmdOptions, isSetPreserveOrder);

    // Append a newline after the relations hash
    if (!privateSection.isEmpty() && anyModelOptionGiven(cmdOptions))
        privateSection.append(NEWLINE_C);

    if (!table.isEmpty())
        privateSection += ModelTableStub.arg(table);

    if (!primaryKey.isEmpty())
        privateSection += ModelPrimaryKeyStub.arg(primaryKey);

    if (incrementing)
        privateSection += ModelIncrementingStub.arg(u"true"_s);
    else if (disableIncrementing)
        privateSection += ModelIncrementingStub.arg(u"false"_s);

    if (!connection.isEmpty())
        privateSection += ModelConnectionStub.arg(connection);

    if (!with.isEmpty())
        privateSection += ModelWithStub.arg(prepareInitializerListValues(with));

    if (!fillable.isEmpty())
        privateSection += ModelFillableStub.arg(prepareInitializerListValues(fillable));

    if (!guarded.isEmpty())
        privateSection += ModelGuardedStub.arg(prepareInitializerListValues(guarded));

    if (disableTimestamps)
        privateSection += ModelDisableTimestampsStub;

    if (castsExample)
        privateSection += ModelCastsExampleStub;

    if (!dateFormat.isEmpty())
        privateSection += ModelDateFormatStub.arg(dateFormat);

    if (!dates.isEmpty())
        privateSection += ModelDatesStub.arg(prepareInitializerListValues(dates));

    if (!touches.isEmpty())
        privateSection += ModelTouchesStub.arg(prepareInitializerListValues(touches));

    if (snakeAttributes)
        privateSection += ModelSnakeAttributesStub.arg(u"true"_s);
    else if (disableSnakeAttributes)
        privateSection += ModelSnakeAttributesStub.arg(u"false"_s);

    if (!visible.isEmpty())
        privateSection += ModelVisibleStub.arg(prepareInitializerListValues(visible));

    if (!hidden.isEmpty())
        privateSection += ModelHiddenStub.arg(prepareInitializerListValues(hidden));

    privateSection += createMutatorsHash(className, accessors, appends);

    if (!appends.isEmpty())
        privateSection += ModelAppendsStub.arg(prepareAppendsListValues(appends));

    if (!privateSection.isEmpty()) {
        // Prepend the private: specifier if the public section exists
        if (hasPublicSection || hasProtectedSection)
            privateSection.prepend(ModelPrivateStub);
        else
            privateSection.prepend(NEWLINE_C);
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
            visible,      hidden,
            mutators,     appends,
            incrementing, disableIncrementing, disableTimestamps,
            castsExample,
            snakeAttributes, disableSnakeAttributes, _13
    ] = cmdOptions;

    return !table.isEmpty()      || !primaryKey.isEmpty() || !connection.isEmpty() ||
           !with.isEmpty()       || !fillable.isEmpty()   || !guarded.isEmpty()    ||
           !dateFormat.isEmpty() || !dates.isEmpty()      || !touches.isEmpty()    ||
           !visible.isEmpty()    || !hidden.isEmpty()     ||
           !mutators.isEmpty()   || !appends.isEmpty()    ||
           incrementing          || disableIncrementing   || disableTimestamps     ||
           castsExample          ||
           snakeAttributes       || disableSnakeAttributes;
}

QString ModelCreator::prepareInitializerListValues(const QStringList &list)
{
    /*! Wrap values in quotes and add the given prefix. */
    const auto wrapValues = [](const QStringList &values, const QString &prefix)
    {
        return values
                // Skip empty values (allows to create initializers like xyz {}).
                | ranges::views::filter([](const QString &value)
        {
            return !value.isEmpty();
        })
                | ranges::views::transform([&prefix](const QString &value)
        {
            return uR"(%2"%1")"_s.arg(value, prefix);
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
                                                        : EMPTY),
                          listSize > 2 ? u",\n"_s : COMMA);

    if (listSize > 2)
        listJoined.prepend(NEWLINE_C);
    else if (listSize > 1)
        listJoined.prepend(NOSPACE.arg(NEWLINE_C, QString(8, SPACE)));

    if (listSize > 1)
        listJoined.append(COMMA).append(NEWLINE_C).append(QString(4, SPACE));

    return listJoined;
}

QString ModelCreator::prepareAppendsListValues(const QStringList &appends)
{
    QStringList preparedAppends;
    preparedAppends.reserve(appends.size());

    for (const auto &append : appends)
        preparedAppends << StringUtils::snake(append);

    return prepareInitializerListValues(preparedAppends);
}

/* u_relations section */

QString ModelCreator::createRelationsHash(
        const QString &className, const CmdOptions &cmdOptions,
        const bool isSetPreserveOrder) const
{
    const auto &[
            relationsOrder,
            oneToOneList, oneToManyList, belongsToList, belongsToManyList,
            _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17,
            _18, _19, _20, _21, _22, _23, _24, _25, _26, _27
    ] = cmdOptions;

    // Nothing to create, no relations passed
    if (m_relationsListsSize == 0)
        return {};

    // Get max. size of relation names for align
    const auto relationsMaxSize = getRelationNamesMaxSize(cmdOptions);

    const auto &[
            oneToOneOrder, oneToManyOrder, belongsToOrder, belongsToManyOrder
    ] = relationsOrder;

    RelationsWithOrder relationItemsList;
    relationItemsList.reserve(m_relationsListsSize); // Use already the cached size value

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
            .replace(u"{{ relationItems }}"_s, relationItems)
            .replace(u"{{relationItems}}"_s,   relationItems);
}

QString::size_type ModelCreator::getRelationNamesMaxSize(const CmdOptions &cmdOptions)
{
    const auto &[
            _1,
            oneToOneList, oneToManyList, belongsToList, belongsToManyList,
            _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18,
            _19, _20, _21, _22, _23, _24, _25, _26, _27, _28
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

    return *std::ranges::max_element(relationSizes);
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
    result.reserve(static_cast<decltype (result)::size_type>(relatedClasses.size()));

    for (const auto &[relatedClass, relationOrder] :
         ranges::views::zip(relatedClasses, orderList)
    ) {
        // Nothing to do, don't create relation mappings with empty names
        if (relatedClass.isEmpty())
            continue;

        const auto relationName = guessOneTypeRelationName(relatedClass);
        const auto spaceAlign   = QString(relationsMaxSize - relationName.size(), SPACE);

        auto content = QString(ModelRelationItemStub)
                       .replace(u"{{ parentClass }}"_s,  parentClass)
                       .replace(u"{{parentClass}}"_s,    parentClass)

                       .replace(u"{{ relationName }}"_s, relationName)
                       .replace(u"{{relationName}}"_s,   relationName)

                       .replace(u"{{ spaceAlign }}"_s,   spaceAlign)
                       .replace(u"{{spaceAlign}}"_s,     spaceAlign);

#if defined(__clang__) && __clang_major__ < 16
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
    result.reserve(static_cast<decltype (result)::size_type>(relatedClasses.size()));

    for (const auto &[relatedClass, relationOrder] :
         ranges::views::zip(relatedClasses, orderList)
    ) {
        // Nothing to do, don't create relation mappings with empty names
        if (relatedClass.isEmpty())
            continue;

        const auto relationName = guessManyTypeRelationName(relatedClass);
        const auto spaceAlign   = QString(relationsMaxSize - relationName.size(), SPACE);

        auto content = QString(ModelRelationItemStub)
                       .replace(u"{{ parentClass }}"_s,  parentClass)
                       .replace(u"{{parentClass}}"_s,    parentClass)

                       .replace(u"{{ relationName }}"_s, relationName)
                       .replace(u"{{relationName}}"_s,   relationName)

                       .replace(u"{{ spaceAlign }}"_s,   spaceAlign)
                       .replace(u"{{spaceAlign}}"_s,     spaceAlign);

#if defined(__clang__) && __clang_major__ < 16
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
    result.reserve(static_cast<decltype (result)::size_type>(relatedClasses.size()));

    for (const auto &[relatedClass, relationOrder] :
         ranges::views::zip(relatedClasses, orderList)
    ) {
        // Nothing to do, don't create relation mappings with empty names
        if (relatedClass.isEmpty())
            continue;

        const auto relationName = guessOneTypeRelationName(relatedClass);
        const auto spaceAlign   = QString(relationsMaxSize - relationName.size(), SPACE);

        auto content = QString(ModelRelationItemStub)
                       .replace(u"{{ parentClass }}"_s,  parentClass)
                       .replace(u"{{parentClass}}"_s,    parentClass)

                       .replace(u"{{ relationName }}"_s, relationName)
                       .replace(u"{{relationName}}"_s,   relationName)

                       .replace(u"{{ spaceAlign }}"_s,   spaceAlign)
                       .replace(u"{{spaceAlign}}"_s,     spaceAlign);

#if defined(__clang__) && __clang_major__ < 16
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
    result.reserve(static_cast<decltype (result)::size_type>(relatedClasses.size()));

    for (const auto &[relatedClass, relationOrder] :
         ranges::views::zip(relatedClasses, orderList)
    ) {
        // Nothing to do, don't create relation mappings with empty names
        if (relatedClass.isEmpty())
            continue;

        const auto relationName = guessManyTypeRelationName(relatedClass);
        const auto spaceAlign   = QString(relationsMaxSize - relationName.size(), SPACE);

        auto content = QString(ModelRelationItemStub)
                       .replace(u"{{ parentClass }}"_s,  parentClass)
                       .replace(u"{{parentClass}}"_s,    parentClass)

                       .replace(u"{{ relationName }}"_s, relationName)
                       .replace(u"{{relationName}}"_s,   relationName)

                       .replace(u"{{ spaceAlign }}"_s,   spaceAlign)
                       .replace(u"{{spaceAlign}}"_s,     spaceAlign);

#if defined(__clang__) && __clang_major__ < 16
        result.push_back({relationOrder, std::move(content)});
#else
        result.emplace_back(relationOrder, std::move(content));
#endif
    }

    return result;
}

/* u_mutators section */

QString
ModelCreator::createMutatorsHash(const QString &className, const QStringList &accessors,
                                 const QStringList &appends)
{
    // Nothing to create, no accessors or appends passed
    if (accessors.empty() && appends.empty())
        return {};

    // Prepare mutator attribute names
    const auto mutatorNames = prepareMutatorNames(accessors, appends);
    // Get max. size of relation names for align
    const auto mutatorsMaxSize = getMutatorNamesMaxSize(mutatorNames);

    // Loop over mutator names and create individual mutator items
    QStringList mutatorItemsList;
    mutatorItemsList.reserve(static_cast<decltype (mutatorItemsList)::size_type>(
                                 mutatorNames.size()));

    for (const auto &mutator : mutatorNames)
        mutatorItemsList << createMutatorItem(className, mutator, mutatorsMaxSize);

    // Join and replace the main stub
    // The wrapMutatorItemsList() allows to create initializer like u_mutators {}
    const auto mutatorItems = wrapMutatorItemsList(mutatorItemsList);

    return QString(ModelMutatorsStub)
            .replace(u"{{ mutatorItems }}"_s, mutatorItems)
            .replace(u"{{mutatorItems}}"_s,   mutatorItems);
}

std::set<QString>
ModelCreator::prepareMutatorNames(const QStringList &accessors,
                                  const QStringList &appends)
{
    std::set<QString> accessorsSet;
    std::set<QString> appendsSet;

    for (const auto &accessor : accessors)
        if (!accessor.isEmpty())
            accessorsSet.emplace(StringUtils::snake(accessor));

    for (const auto &append : appends)
        if (!append.isEmpty())
            appendsSet.emplace(StringUtils::snake(append));

    accessorsSet.merge(std::move(appendsSet));

    return accessorsSet;
}

QString::size_type
ModelCreator::getMutatorNamesMaxSize(const std::set<QString> &mutators)
{
    // Nothing to do
    if (mutators.empty())
        return 0;

    return std::ranges::max_element(mutators,
                                    [](const QString &left, const QString &right)
    {
        return left.size() < right.size();
    })
        ->size();
}

QString
ModelCreator::createMutatorItem(const QString &className, const QString &mutator,
                                const QString::size_type mutatorsMaxSize)
{
    const auto spaceAlign   = QString(mutatorsMaxSize - mutator.size(), SPACE);
    const auto mutatorCamel = StringUtils::camel(mutator);

    return QString(ModelMutatorItemStub)
            .replace(u"{{ class }}"_s,      className)
            .replace(u"{{class}}"_s,        className)

            .replace(u"{{ spaceAlign }}"_s, spaceAlign)
            .replace(u"{{spaceAlign}}"_s,   spaceAlign)

            .replace(u"{{ mutatorNameSnake }}"_s, mutator)
            .replace(u"{{mutatorNameSnake}}"_s,   mutator)

            .replace(u"{{ mutatorNameCamel }}"_s, mutatorCamel)
            .replace(u"{{mutatorNameCamel}}"_s,   mutatorCamel);
}

QString ModelCreator::wrapMutatorItemsList(const QStringList &mutatorItems)
{
    // Nothing to do
    if (mutatorItems.isEmpty()) T_UNLIKELY
        return EMPTY;

    else T_LIKELY {
        auto mutatorItemsJoined = mutatorItems.join(NEWLINE_C);

        mutatorItemsJoined.prepend(NEWLINE_C)
                          .append(NOSPACE.arg(NEWLINE_C, QString(4, SPACE)));

        return mutatorItemsJoined;
    }
}

/* Global */

QString ModelCreator::createIncludesOrmSection(const CmdOptions &cmdOptions)
{
    // Will always contain
    m_includesOrmList.emplace(ModelIncludeOrmItemStub
                              .arg(cmdOptions.pivotModel
                                   ? u"tiny/relations/basepivot.hpp"_s
                                   : u"tiny/model.hpp"_s));

    return ContainerUtils::join(m_includesOrmList, NEWLINE_C);
}

QString ModelCreator::createIncludesSection() const
{
    // Nothing to create
    if (m_includesList.empty())
        return {};

    return NOSPACE.arg("\n\n", ContainerUtils::join(m_includesList, NEWLINE_C));
}

QString ModelCreator::createUsingsSection(const CmdOptions &cmdOptions)
{
    // Will always contain
    m_usingsList.emplace(cmdOptions.pivotModel
                         ? u"using Orm::Tiny::Relations::BasePivot;"_s
                         : u"using Orm::Tiny::Model;"_s);

    return ContainerUtils::join(m_usingsList, NEWLINE_C);
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

    return StringUtils::wrapValue(ContainerUtils::join(m_forwardsList, NEWLINE_C),
                                  QChar(QChar::LineFeed));
}

/* Common for public/private sections */

QString ModelCreator::joinRelationsList(RelationsWithOrder &&relationsList) // NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
{
    auto relationsQList = ranges::views::move(relationsList)
            | ranges::views::transform([](auto &&relationItem) -> QString
    {
        return std::move(relationItem.content);
    })
            | ranges::to<QStringList>();

    return relationsQList.join(NEWLINE_C);
}

} // namespace Tom::Commands::Make::Support

TINYORM_END_COMMON_NAMESPACE
