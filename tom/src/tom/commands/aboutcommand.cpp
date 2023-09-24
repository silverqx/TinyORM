#include "tom/commands/aboutcommand.hpp"

#include <QJsonObject>
#include <QJsonValue>

#include <range/v3/algorithm/contains.hpp>

#include <orm/constants.hpp>
#include <orm/utils/string.hpp>
#include <orm/utils/type.hpp>

#include "tom/application.hpp"
#include "tom/version.hpp"

/*! Alias for the QStringLiteral(). */
#define sl(str) QStringLiteral(str)

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::NOSPACE;
using Orm::Constants::SPACE;

using StringUtils = Orm::Utils::String;
using TypeUtils = Orm::Utils::Type;

using Tom::Constants::json_;
using Tom::Constants::only_;
using Tom::Constants::only_up;
using Tom::Constants::pretty;

namespace Tom::Commands
{

/* public */

AboutCommand::AboutCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
{}

QList<CommandLineOption> AboutCommand::optionsSignature() const
{
    return {
        {json_,  sl("Output the information as JSON")},
        {pretty, sl("Enable JSON human readable output")},
        {only_,  sl("Sections to display <gray>(partial match)</gray> "
                    "<comment>(multiple values allowed)</comment>"), only_up}, // Value
    };
}

int AboutCommand::run()
{
    Command::run();

    display(gatherAllAboutInformation());

    return EXIT_SUCCESS;
}

/* protected */

void AboutCommand::display(const QVector<SectionItem> &sections)
{
    if (isSet(json_))
        displayJson(sections);
    else
        displayDetail(sections);
}

void AboutCommand::displayDetail(const QVector<SectionItem> &sections)
{
    auto firstSection = true;

    // Print sections
    for (const auto &[sectionName, subSections] : sections) {
        // Nothing to do, skipping section
        if (shouldSkipSection(sectionName))
            continue;

        // Main section title
        if (firstSection)
            firstSection = false;
        else
            newLine();
        line(QStringLiteral("<blue>%1</blue>").arg(sectionName));

        auto firstSubsection = true;

        // Print subsections
        for (const auto &[subsectionName, abouts] : subSections) {
            if (firstSubsection)
                firstSubsection = false;
            else
                // newline only if a subsection name was defined
                if (subsectionName)
                    newLine();

            // Subsection name is optional
            if (subsectionName)
                comment(*subsectionName);

            // Print about items
            for (const auto &[name, value] : abouts) {
                note(NOSPACE.arg(name).arg(SPACE), false);
                info(value);
            }
        }
    }
}

void AboutCommand::displayJson(const QVector<SectionItem> &sections)
{
    QJsonObject jsonSections;

    // Serialize sections
    for (const auto &[sectionName, subSections] : sections) {
        // Nothing to do, skipping section
        if (shouldSkipSection(sectionName))
            continue;

        // Prepare the section name for JSON output (map to a short section name)
        const auto sectionNamePrepared = prepareJsonSectionName(sectionName);

        QJsonObject jsonSubsections;

        // Serialize subsections
        for (const auto &[subsectionName, abouts] : subSections) {
            QJsonObject jsonAboutItems;

            // Serialize about items
            for (const auto &[name, value] : abouts)
                /* Subsection name is optional, if it's not defined then append directly
                   to the parent jsonSubsections. */
                if (subsectionName)
                    jsonAboutItems.insert(StringUtils::snake(name.toLower()),
                                          QJsonValue(value));
                else
                    // Don't use snake_case for preprocessor macro names
                    jsonSubsections.insert(
                                prepareJsonAboutItemName(sectionNamePrepared, name),
                                QJsonValue(value));

            /* Nothing to do, subsection name is the std::nullopt so the jsonAboutItems
               were directly appended to the parent jsonSubsections. */
            if (subsectionName)
                jsonSubsections.insert(StringUtils::snake(subsectionName->toLower()),
                                       jsonAboutItems);
        }

        jsonSections.insert(StringUtils::snake(sectionNamePrepared.toLower()),
                            jsonSubsections);
    }

    // Send to the stdout with disabled ANSI support
    withoutAnsi([this, &jsonSections]
    {
        line(StringUtils::stripTags(
                 QJsonDocument(jsonSections).toJson(jsonFormat())));
    });
}

bool AboutCommand::shouldSkipSection(const QString &sectionName) const
{
    static const auto only = getOnlyValues();

    const auto contains = ranges::contains(only, true,
                                           [sectionName = sectionName.toLower()]
                                           (const QString &onlyValue)
    {
        return sectionName.contains(onlyValue);
    });

    return !only.empty() && !contains;
}

std::unordered_set<QString> AboutCommand::getOnlyValues() const
{
    const auto onlyList = values(only_, Qt::SkipEmptyParts);

    std::unordered_set<QString> only;
    only.reserve(static_cast<std::unordered_set<QString>::size_type>(onlyList.size()));

    for (const auto &value : onlyList)
        only.emplace(value.toLower());

    return only;
}

QString AboutCommand::prepareJsonSectionName(const QString &name)
{
    static const std::unordered_map<QString, QString> sectionNamesMap {
        {sl("Macros <gray>C Preprocessor</gray>"), sl("Macros")},
    };

    if (!sectionNamesMap.contains(name))
        return name;

    return sectionNamesMap.at(name);
}

QString AboutCommand::prepareJsonAboutItemName(const QString &section,
                                               const QString &aboutItem)
{
    return section == sl("Macros") ? aboutItem
                                   : StringUtils::snake(aboutItem.toLower());
}

QVector<SectionItem> AboutCommand::gatherAllAboutInformation() const
{
    return {
        {sl("Environment"),                        gatherEnvironmentInformation()},
        {sl("Macros <gray>C Preprocessor</gray>"), gatherMacrosInformation()},
        {sl("Versions"),                           gatherVersionsInformation()},
    };
}

QVector<SubSectionItem> AboutCommand::gatherEnvironmentInformation() const
{
    static const auto ON  = QStringLiteral("ON");
    static const auto OFF = QStringLiteral("OFF");

    return {
        {std::nullopt,
            {
                {sl("Application name"),    QCoreApplication::applicationName()},
                {sl("Organization name"),   QCoreApplication::organizationName()},
                {sl("Organization domain"), QCoreApplication::organizationDomain()},
                {sl("tom version"),         TINYTOM_VERSION_STR},
                {sl("Environment"),         application().environment()},
#ifdef TINYORM_DEBUG
                {sl("Build type"),          "Debug"},
#else
                {sl("Build type"),          "Release"},
#endif
#ifdef TINYORM_BUILDING_SHARED
                {sl("Link type"),           "Shared (dynamic)"},
#else
                {sl("Link type"),           "Static"},
#endif
#ifdef TINYORM_MSVC_RUNTIME_DYNAMIC
                {sl("MSVC Runtime dynamic"),
                            TypeUtils::isCMakeTrue(
                                TINYTOM_STRINGIFY(TINYORM_MSVC_RUNTIME_DYNAMIC))
                            ? ON : OFF},
#endif
#ifdef TINYTOM_CMAKE_MSVC_RUNTIME_LIBRARY
                {sl("MSVC Runtime library"),
                            TINYTOM_STRINGIFY(TINYTOM_CMAKE_MSVC_RUNTIME_LIBRARY)},
#endif
            }},
    };
}

QVector<SubSectionItem> AboutCommand::gatherMacrosInformation()
{
    static const auto ON  = QStringLiteral("ON");
    static const auto OFF = QStringLiteral("OFF");

    return {
        {std::nullopt,
            {
#ifdef TINYORM_BUILDING_SHARED
                {sl("TINYORM_BUILDING_SHARED"), ON},
#else
                {sl("TINYORM_BUILDING_SHARED"), OFF},
#endif
#ifdef TINYORM_DEBUG
                {sl("TINYORM_DEBUG"), ON},
#else
                {sl("TINYORM_DEBUG"), OFF},
#endif
#ifdef TINYORM_DEBUG_SQL
                {sl("TINYORM_DEBUG_SQL"), ON},
#else
                {sl("TINYORM_DEBUG_SQL"), OFF},
#endif
#ifdef TINYORM_DISABLE_ORM
                {sl("TINYORM_DISABLE_ORM"), ON},
#else
                {sl("TINYORM_DISABLE_ORM"), OFF},
#endif
#ifdef TINYORM_DISABLE_THREAD_LOCAL
                {sl("TINYORM_DISABLE_THREAD_LOCAL"), ON},
#else
                {sl("TINYORM_DISABLE_THREAD_LOCAL"), OFF},
#endif
#ifdef TINYORM_DISABLE_TOM
                {sl("TINYORM_DISABLE_TOM"), ON},
#else
                {sl("TINYORM_DISABLE_TOM"), OFF},
#endif
#ifdef TINYORM_EXTERN_CONSTANTS
                {sl("TINYORM_EXTERN_CONSTANTS"), ON},
#else
                {sl("TINYORM_EXTERN_CONSTANTS"), OFF},
#endif
#ifdef TINYORM_INLINE_CONSTANTS
                {sl("TINYORM_INLINE_CONSTANTS"), ON},
#else
                {sl("TINYORM_INLINE_CONSTANTS"), OFF},
#endif
#ifdef TINYORM_MYSQL_PING
                {sl("TINYORM_MYSQL_PING"), ON},
#else
                {sl("TINYORM_MYSQL_PING"), OFF},
#endif
#ifdef TINYORM_NO_DEBUG
                {sl("TINYORM_NO_DEBUG"), ON},
#else
                {sl("TINYORM_NO_DEBUG"), OFF},
#endif
#ifdef TINYORM_NO_DEBUG_SQL
                {sl("TINYORM_NO_DEBUG_SQL"), ON},
#else
                {sl("TINYORM_NO_DEBUG_SQL"), OFF},
#endif
#ifdef TINYORM_STRICT_MODE
                {sl("TINYORM_STRICT_MODE"), ON},
#else
                {sl("TINYORM_STRICT_MODE"), OFF},
#endif
#ifdef TINYORM_TESTS_CODE
                {sl("TINYORM_TESTS_CODE"), ON},
#else
                {sl("TINYORM_TESTS_CODE"), OFF},
#endif
#ifdef TINYORM_TOM_EXAMPLE
                {sl("TINYORM_TOM_EXAMPLE"), ON},
#else
                {sl("TINYORM_TOM_EXAMPLE"), OFF},
#endif
#ifdef TINYORM_USING_PCH
                {sl("TINYORM_USING_PCH"), ON},
#else
                {sl("TINYORM_USING_PCH"), OFF},
#endif
#ifdef TINYORM_MSVC_RUNTIME_DYNAMIC
                {sl("TINYORM_MSVC_RUNTIME_DYNAMIC"), ON},
#else
                {sl("TINYORM_MSVC_RUNTIME_DYNAMIC"), OFF},
#endif
                {sl("TINYTOM_CMAKE_MSVC_RUNTIME_LIBRARY"),
                            TINYTOM_STRINGIFY(TINYTOM_CMAKE_MSVC_RUNTIME_LIBRARY)},

                {sl("TINYTOM_MIGRATIONS_DIR"), TINYTOM_STRINGIFY(TINYTOM_MIGRATIONS_DIR)},
                {sl("TINYTOM_MODELS_DIR"),     TINYTOM_STRINGIFY(TINYTOM_MODELS_DIR)},
                {sl("TINYTOM_SEEDERS_DIR"),    TINYTOM_STRINGIFY(TINYTOM_SEEDERS_DIR)},
            }},
    };
}

QVector<SubSectionItem> AboutCommand::gatherVersionsInformation()
{
    auto versions = Application::createVersionsSubsection();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                          // std::nullopt produces -Wmaybe-uninitialized in GCC release builds
    versions.emplaceFront(std::optional<QString> {std::nullopt},
                          QVector<AboutItem> {
                              {QStringLiteral("tom"), TINYTOM_VERSION_STR},
                          });
#else
    versions.push_front({std::nullopt, {
                             {QStringLiteral("tom"), TINYTOM_VERSION_STR},
                         }});
#endif

    return versions;
}

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE
