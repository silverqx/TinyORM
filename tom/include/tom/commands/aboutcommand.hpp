#pragma once
#ifndef TOM_COMMANDS_ABOUTCOMMAND_HPP
#define TOM_COMMANDS_ABOUTCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <unordered_set>

#include "tom/commands/command.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Types
{
    struct AboutValue;
}

namespace Tom
{
    struct SectionItem;
    struct SubSectionItem;

namespace Commands
{

    /*! Display basic information about the tom application. */
    class AboutCommand : public Command
    {
        Q_DISABLE_COPY_MOVE(AboutCommand)

    public:
        /*! Constructor. */
        AboutCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        ~AboutCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! The signature of the console command. */
        QList<CommandLineOption> optionsSignature() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! Alias for the AboutValue. */
        using AboutValue = Orm::Types::AboutValue;

        /*! Display the application information. */
        void display(const QList<SectionItem> &sections);
        /*! Display the application information as a detailed view. */
        void displayDetail(const QList<SectionItem> &sections) const;
        /*! Display the application information as JSON. */
        void displayJson(const QList<SectionItem> &sections);

        /*! Determine whether the only values list contains the given section name. */
        bool shouldSkipSection(const QString &sectionName) const;
        /*! Get and prepare the only values set. */
        std::unordered_set<QString> getOnlyValues() const;

        /*! Prepare the section name for JSON output (map to a short section name). */
        static QString prepareJsonSectionName(const QString &name);
        /*! Prepare the about item name for JSON output (don't snake_case macro names). */
        static QString prepareJsonAboutItemName(const QString &section,
                                                const QString &aboutItem);

        /*! Print about items as a detailed view. */
        void printAboutItemsDetail(const std::map<QString, QString> &aboutItems) const;
        /*! Print about items as a detailed view. */
        void printAboutItemsDetail(const std::map<QString, AboutValue> &aboutItems) const;

        /*! Prepare the about items for printing as JSON. */
        static void prepareAboutItemsJson(
                QJsonObject &jsonSubsections, QJsonObject &jsonAboutItems,
                const std::map<QString, QString> &aboutItems,
                const std::optional<QString> &subsectionName,
                const QString &sectionNamePrepared);
        /*! Prepare the about items for printing as JSON. */
        static void prepareAboutItemsJson(
                QJsonObject &jsonSubsections, QJsonObject &jsonAboutItems,
                const std::map<QString, AboutValue> &aboutItems,
                const std::optional<QString> &subsectionName,
                const QString &sectionNamePrepared);

        /* Gathering */
        /*! Gather all information about the application. */
        QList<SectionItem> gatherAllAboutInformation() const;
        /*! Gather environment-related information. */
        QList<SubSectionItem> gatherEnvironmentInformation() const;
        /*! Gather C preprocessor macros-related information. */
        static QList<SubSectionItem> gatherMacrosInformation();
        /*! Gather version-related information. */
        static QList<SubSectionItem> gatherVersionsInformation();
        /*! Gather database connections information. */
        static QList<SubSectionItem> gatherConnectionsInformation();

    private:
        /*! Print about item as a detailed view (common code). */
        void printAboutItemDetail(const QString &aboutName, const QString &aboutValue,
                                  bool newline) const;
        /*! Prepare the about item for printing as JSON (common code). */
        static void prepareAboutItemJson(
                QJsonObject &jsonSubsections, QJsonObject &jsonAboutItems,
                const QString &aboutName, const QString &aboutValue,
                const std::optional<QString> &subsectionName,
                const QString &sectionNamePrepared);
    };

    /* public */

    QString AboutCommand::name() const
    {
        return Constants::About;
    }

    QString AboutCommand::description() const
    {
        return u"Display basic information about the tom application"_s;
    }

} // namespace Commands
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_ABOUTCOMMAND_HPP
