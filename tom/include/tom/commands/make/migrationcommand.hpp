#pragma once
#ifndef TOM_COMMANDS_MAKE_MIGRATIONCOMMAND_HPP
#define TOM_COMMANDS_MAKE_MIGRATIONCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"
#include "tom/migrationcreator.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make
{

    /*! Create a new migration file. */
    class MigrationCommand : public Command
    {
        Q_DISABLE_COPY(MigrationCommand)

        /*! Alias for the filesystem path. */
        using fspath = std::filesystem::path;

    public:
        /*! Constructor. */
        MigrationCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        inline ~MigrationCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! The console command positional arguments signature. */
        const std::vector<PositionalArgument> &positionalArguments() const override;
        /*! The signature of the console command. */
        QList<QCommandLineOption> optionsSignature() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! Allows to pass the full migration filename with the datetime prefix. */
        static std::tuple<std::string, QString, std::string>
        prepareMigrationNameClassname(QString &&migration);
        /*! Prepare a migration name from the filename. */
        static std::tuple<std::string, QString, std::string>
        prepareMigrationNameFromFilename(
                    bool startsWithDatetimePrefix, QString &&migration, bool hasExt,
                    std::string &&ext);
        /*! Try to extract the datetime prefix from the migration name. */
        static std::string
        tryExtractDateTimePrefixFromName(
                    bool startsWithDatetimePrefix, QString &&migration,
                    QString &migrationName);
        /*! Try to extract the extension from the migration name. */
        static std::string
        tryExtractExtensionFromName(bool hasExt, std::string &&ext,
                                    QString &migrationName);
        /*! Prepare a final migration name. */
        static QString prepareFinalMigrationName(QString &&migration);

        /*! Write the migration file to disk. */
        void writeMigration(std::string &&datetimePrefix, const QString &name,
                            std::string &&extension, const QString &table,
                            bool create) const;

        /*! Get migration path (either specified by '--path' option or default
            location). */
        fspath getMigrationPath() const;

        /*! The migration creator instance. */
        MigrationCreator m_creator {};
    };

    /* public */

    QString MigrationCommand::name() const
    {
        return Constants::MakeMigration;
    }

    QString MigrationCommand::description() const
    {
        return QStringLiteral("Create a new migration file");
    }

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_MIGRATIONCOMMAND_HPP
