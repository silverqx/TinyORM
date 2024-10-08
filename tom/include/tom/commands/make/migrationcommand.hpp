#pragma once
#ifndef TOM_COMMANDS_MAKE_MIGRATIONCOMMAND_HPP
#define TOM_COMMANDS_MAKE_MIGRATIONCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/make/makecommand.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make
{

    /*! Create a new migration file. */
    class MigrationCommand : public MakeCommand
    {
        Q_DISABLE_COPY_MOVE(MigrationCommand)

        /*! Alias for the filesystem path. */
        using fspath = std::filesystem::path;

    public:
        /*! Constructor. */
        MigrationCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        ~MigrationCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! The console command positional arguments signature. */
        const std::vector<PositionalArgument> &positionalArguments() const override;
        /*! Options signature of the console command. */
        QList<CommandLineOption> optionsSignature() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! Allows to pass the full migration filename with the datetime prefix. */
        static std::tuple<std::string, QString, std::string>
        prepareMigrationNameClassName(QString &&migration);
        /*! Prepare a migration name from the filename. */
        static std::tuple<std::string, QString, std::string>
        prepareMigrationNameFromFilename(
                bool startsWithDatetimePrefix, QString &&migration, bool hasExtension,
                std::string &&extension);
        /*! Try to extract the datetime prefix from the migration name. */
        static std::string
        tryExtractDateTimePrefixFromName(
                bool startsWithDatetimePrefix, QString &&migration,
                QString &migrationName);
        /*! Try to extract the extension from the migration name. */
        static std::string
        tryExtractExtensionFromName(bool hasExtension, std::string &&extension,
                                    QString &migrationName);
        /*! Prepare a final migration name. */
        static QString prepareFinalMigrationName(QString &&migration);

        /*! Write the migration file to the disk. */
        void writeMigration(std::string &&datetimePrefix, const QString &name,
                            const std::string &extension, const fspath &migrationsPath,
                            const QString &table, bool create) const;

        /*! Get the migrations path (either specified by the --path option or the default
            path). */
        fspath getMigrationsPath() const;
        /*! Get the migrations path specified by the --path option. */
        fspath getUserMigrationsPath() const;
        /*! Try to guess the migrations path based on the pwd or use the default path. */
        fspath guessMigrationsPath() const;
    };

    /* public */

    QString MigrationCommand::name() const
    {
        return Constants::MakeMigration;
    }

    QString MigrationCommand::description() const
    {
        return u"Create a new migration file"_s;
    }

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_MIGRATIONCOMMAND_HPP
