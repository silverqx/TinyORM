#pragma once
#ifndef TOM_COMMANDS_ABOUTCOMMAND_HPP
#define TOM_COMMANDS_ABOUTCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <unordered_set>

#include "tom/commands/command.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

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
        /*! Display the application information. */
        void display(const QVector<SectionItem> &sections);
        /*! Display the application information as a detailed view. */
        void displayDetail(const QVector<SectionItem> &sections) const;
        /*! Display the application information as JSON. */
        void displayJson(const QVector<SectionItem> &sections);

        /*! Determine whether the only values list contains the given section name. */
        bool shouldSkipSection(const QString &sectionName) const;
        /*! Get and prepare the only values set. */
        std::unordered_set<QString> getOnlyValues() const;

        /*! Prepare the section name for JSON output (map to a short section name). */
        static QString prepareJsonSectionName(const QString &name);
        /*! Prepare the about item name for JSON output (don't snake_case macro names). */
        static QString prepareJsonAboutItemName(const QString &section,
                                                const QString &aboutItem);

        /*! Gather all information about the application. */
        QVector<SectionItem> gatherAllAboutInformation() const;
        /*! Gather environment-related information. */
        QVector<SubSectionItem> gatherEnvironmentInformation() const;
        /*! Gather C preprocessor macros-related information. */
        static QVector<SubSectionItem> gatherMacrosInformation();
        /*! Gather version-related information. */
        static QVector<SubSectionItem> gatherVersionsInformation();
        /*! Gather database connections information. */
        static QVector<SubSectionItem> gatherConnectionsInformation();
    };

    /* public */

    QString AboutCommand::name() const
    {
        return Constants::About;
    }

    QString AboutCommand::description() const
    {
        return QStringLiteral("Display basic information about the tom application");
    }

} // namespace Commands
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_ABOUTCOMMAND_HPP
