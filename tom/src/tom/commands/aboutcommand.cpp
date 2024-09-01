#include "tom/commands/aboutcommand.hpp"

#include <QJsonObject>
#include <QJsonValue>

#include <range/v3/algorithm/contains.hpp>

#ifdef TINYORM_USING_TINYDRIVERS
#  include <orm/drivers/libraryinfo.hpp>
#endif

#include <orm/constants.hpp>
#include <orm/db.hpp>
#include <orm/libraryinfo.hpp>
#include <orm/utils/string.hpp>
#include <orm/utils/type.hpp>

#include "tom/application.hpp"
#include "tom/version.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

#ifdef TINYORM_USING_TINYDRIVERS
using DriversLibraryInfo = Orm::Drivers::LibraryInfo;
#endif

using Orm::Constants::COMMA;
using Orm::Constants::NOSPACE;
using Orm::Constants::PARENTH_ONE;
using Orm::Constants::SPACE;

using Orm::DB;
using Orm::LibraryInfo;

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
        {json_,  u"Output the information as JSON"_s},
        {pretty, u"Enable JSON human readable output"_s},
        {only_,  u"Sections to display <gray>(partial match)</gray> "
                  "<comment>(multiple values allowed)</comment>"_s, only_up}, // Value
    };
}

int AboutCommand::run()
{
    Command::run();

    display(gatherAllAboutInformation());

    return EXIT_SUCCESS;
}

/* protected */

void AboutCommand::display(const QList<SectionItem> &sections)
{
    if (isSet(json_))
        displayJson(sections);
    else
        displayDetail(sections);
}

void AboutCommand::displayDetail(const QList<SectionItem> &sections) const
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
            if (std::holds_alternative<std::map<QString, QString>>(abouts))
                printAboutItemsDetail(std::get<std::map<QString, QString>>(abouts));

            else if (std::holds_alternative<std::map<QString, AboutValue>>(abouts))
                printAboutItemsDetail(std::get<std::map<QString, AboutValue>>(abouts));

            else
                Q_UNREACHABLE(); // Correct, std::variant<> can't hold anything else
        }
    }
}

void AboutCommand::displayJson(const QList<SectionItem> &sections)
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
            if (std::holds_alternative<std::map<QString, QString>>(abouts))
                prepareAboutItemsJson(jsonSubsections, jsonAboutItems,
                                      std::get<std::map<QString, QString>>(abouts),
                                      subsectionName, sectionNamePrepared);

            else if (std::holds_alternative<std::map<QString, AboutValue>>(abouts))
                prepareAboutItemsJson(jsonSubsections, jsonAboutItems,
                                      std::get<std::map<QString, AboutValue>>(abouts),
                                      subsectionName, sectionNamePrepared);
            else
                Q_UNREACHABLE(); // Correct, std::variant<> can't hold anything else

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
    only.reserve(static_cast<decltype (only)::size_type>(onlyList.size()));

    for (const auto &value : onlyList)
        only.emplace(value.toLower());

    return only;
}

QString AboutCommand::prepareJsonSectionName(const QString &name)
{
    static const std::unordered_map<QString, QString> sectionNamesMap {
        {u"Macros <gray>C Preprocessor</gray>"_s, u"Macros"_s},
    };

    if (!sectionNamesMap.contains(name))
        return name;

    return sectionNamesMap.at(name);
}

QString AboutCommand::prepareJsonAboutItemName(const QString &section,
                                               const QString &aboutItem)
{
    return section == u"Macros"_s ? aboutItem
                                  : StringUtils::snake(aboutItem.toLower());
}

void
AboutCommand::printAboutItemsDetail(const std::map<QString, QString> &aboutItems) const
{
    for (const auto &[name, value] : aboutItems)
        printAboutItemDetail(name, value, true);
}

void
AboutCommand::printAboutItemsDetail(const std::map<QString, AboutValue> &aboutItems) const
{
    for (const auto &[name, about] : aboutItems) {
        printAboutItemDetail(name, about.value, false);

        // Item components
        if (const auto &components = about.components; components)
           muted(SPACE + PARENTH_ONE.arg(components->join(COMMA)), false);

        newLine();
    }
}

/* I won't serialize the Dependencies components into the JSON output because that
   would add unnecessary complexity to the consumers' parsing code.
   Currently the output looks like:
"dependencies": {"qt": "6.7.2", "range-v3": "0.12.0", "tinydrivers": "0.2.0"}

   With the Dependencies' components it would look like:
"dependencies": {
    "qt": {
        "version": "6.7.2",
        "components": ["Core"]
    },
    "range-v3": "0.12.0",
    "tinydrivers": : {
        "version": "6.7.2",
        "components": ["MySQL"]
    }
}
*/

void AboutCommand::prepareAboutItemsJson(
        QJsonObject &jsonSubsections, QJsonObject &jsonAboutItems,
        const std::map<QString, QString> &aboutItems,
        const std::optional<QString> &subsectionName,
        const QString &sectionNamePrepared)
{
    for (const auto &[name, value] : aboutItems)
        prepareAboutItemJson(jsonSubsections, jsonAboutItems, name, value,
                             subsectionName, sectionNamePrepared);
}

void AboutCommand::prepareAboutItemsJson(
        QJsonObject &jsonSubsections, QJsonObject &jsonAboutItems,
        const std::map<QString, AboutValue> &aboutItems,
        const std::optional<QString> &subsectionName,
        const QString &sectionNamePrepared)
{
    for (const auto &[name, about] : aboutItems)
        prepareAboutItemJson(jsonSubsections, jsonAboutItems, name, about.value,
                             subsectionName, sectionNamePrepared);
}

/* Gathering */

QList<SectionItem> AboutCommand::gatherAllAboutInformation() const
{
    return {
        {u"Environment"_s,                        gatherEnvironmentInformation()},
        {u"Macros <gray>C Preprocessor</gray>"_s, gatherMacrosInformation()},
        {u"Versions"_s,                           gatherVersionsInformation()},
        {u"Connections"_s,                        gatherConnectionsInformation()},
    };
}

QList<SubSectionItem> AboutCommand::gatherEnvironmentInformation() const
{
    return {
        {std::nullopt,
            std::map<QString, QString> {
                {u"Application name"_s,    QCoreApplication::applicationName()},
                {u"Organization name"_s,   QCoreApplication::organizationName()},
                {u"Organization domain"_s, QCoreApplication::organizationDomain()},
                {u"tom version"_s,         TINYTOM_VERSION_STR},
                {u"Environment"_s,         application().environment()},
#ifdef TINYORM_DEBUG
                {u"Build type"_s,          "Debug"},
#else
                {u"Build type"_s,          "Release"},
#endif
// Newline needed - QtCreator syntax highlighting bug
#ifdef TINYORM_BUILDING_SHARED
                {u"Link type"_s,           "Shared (dynamic)"},
#else
                {u"Link type"_s,           "Static"},
#endif
// CMake ON/OFF
// qmake has bad support for ltcg so it's not handled in qmake (don't do it in future)
#ifdef TINYORM_LTO
                {u"Link Time Optimization"_s,
                            TypeUtils::normalizeCMakeBool(TINY_STRINGIFY(TINYORM_LTO))},
#endif
// CMake ON/OFF/NOTFOUND (TriState bool)
#ifdef TINYORM_MSVC_RUNTIME_DYNAMIC
                {u"MSVC Runtime dynamic"_s,
                            TypeUtils::normalizeCMakeTriStateBool(
                                TINY_STRINGIFY(TINYORM_MSVC_RUNTIME_DYNAMIC))},
#endif
// CMake ON/OFF
#ifdef TINYTOM_CMAKE_MSVC_RUNTIME_LIBRARY
                {u"MSVC Runtime library"_s,
                            TINY_STRINGIFY(TINYTOM_CMAKE_MSVC_RUNTIME_LIBRARY)},
#endif
            }},
    };
}

QList<SubSectionItem> AboutCommand::gatherMacrosInformation()
{
    auto allCMacrosMap = LibraryInfo::allCMacrosMap();
#ifdef TINYORM_USING_TINYDRIVERS
    allCMacrosMap.merge(DriversLibraryInfo::allCMacrosMap());
#endif

    return {
        {std::nullopt, std::move(allCMacrosMap)},
    };
}

QList<SubSectionItem> AboutCommand::gatherVersionsInformation()
{
    auto versions = Application::createVersionsSubsection();

                          // std::nullopt produces -Wmaybe-uninitialized in GCC release builds
    versions.emplaceFront(std::optional<QString> {std::nullopt},
                          std::map<QString, QString> {
                              {QStringLiteral("tom"), TINYTOM_VERSION_STR},
                          });

    return versions;
}

QList<SubSectionItem> AboutCommand::gatherConnectionsInformation()
{
    return {
        {std::nullopt,
            std::map<QString, QString> {
                {u"Available drivers"_s,   DB::supportedDrivers().join(SPACE)},
                {u"Connection names"_s,    DB::connectionNames().join(SPACE)},
                {u"Default connection"_s,  DB::getDefaultConnection()},
            }},
    };
}

/* private */

void AboutCommand::printAboutItemDetail(
        const QString &aboutName, const QString &aboutValue, const bool newline) const
{
    note(NOSPACE.arg(aboutName).arg(SPACE), false);
    info(aboutValue, newline);
}

void AboutCommand::prepareAboutItemJson(
        QJsonObject &jsonSubsections, QJsonObject &jsonAboutItems,
        const QString &aboutName, const QString &aboutValue,
        const std::optional<QString> &subsectionName,
        const QString &sectionNamePrepared)
{
    /* Subsection name is optional, if it's not defined then append directly
       to the parent jsonSubsections. */
    if (subsectionName)
        jsonAboutItems.insert(StringUtils::snake(aboutName.toLower()),
                              QJsonValue(aboutValue));
    else
        jsonSubsections.insert(
                    // Don't use snake_case for preprocessor macro names
                    prepareJsonAboutItemName(sectionNamePrepared, aboutName),
                    QJsonValue(aboutValue));
}

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE
