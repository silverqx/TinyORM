#pragma once
#ifndef TOM_COMMANDS_MAKE_SEEDERCOMMAND_HPP
#define TOM_COMMANDS_MAKE_SEEDERCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/make/makecommand.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make
{

    /*! Create a new seeder class. */
    class SeederCommand : public MakeCommand
    {
        Q_DISABLE_COPY(SeederCommand)

        /*! Alias for the filesystem path. */
        using fspath = std::filesystem::path;

    public:
        /*! Constructor. */
        SeederCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        inline ~SeederCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! The console command positional arguments signature. */
        const std::vector<PositionalArgument> &positionalArguments() const override;
        /*! The signature of the console command. */
        QList<CommandLineOption> optionsSignature() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! Prepare a seeder class name (eg. appends Seeder if needed). */
        static QString prepareSeederClassName(QString &&className);

        /*! Write the seeder file to the disk. */
        void writeSeeder(const QString &className, const fspath &seedersPath) const;

        /*! Get the seeders path (either specified by the --path option or the default
            path). */
        fspath getSeedersPath() const;
        /*! Get the seeders path specified by the --path option. */
        fspath getUserSeedersPath() const;
        /*! Try to guess the seeders path based on the pwd or use the default path. */
        fspath guessSeedersPath() const;
    };

    /* public */

    QString SeederCommand::name() const
    {
        return Constants::MakeSeeder;
    }

    QString SeederCommand::description() const
    {
        return QStringLiteral("Create a new seeder class");
    }

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_SEEDERCOMMAND_HPP
