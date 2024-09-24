#pragma once
#ifndef TOM_COMMANDS_MAKE_PROJECTCOMMAND_HPP
#define TOM_COMMANDS_MAKE_PROJECTCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"
//#include "tom/migrationcreator.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make
{

    /*! Create a new Tom application project. */
    class ProjectCommand : public Command
    {
        Q_DISABLE_COPY_MOVE(ProjectCommand)

        /*! Alias for the filesystem path. */
//        using path = std::filesystem::path;

    public:
        /*! Constructor. */
        ProjectCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        ~ProjectCommand() override = default;

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
        /*! Write the migration file to disk. */
//        void writeMigration(const QString &name, const QString &table,
//                            bool create) const;
        /*! Get migration path (either specified by '--path' option or default
            location). */
//        inline path getMigrationPath() const;

        /*! The migration creator instance. */
//        MigrationCreator m_creator {};
    };

    /* public */

    QString ProjectCommand::name() const
    {
        return Constants::MakeProject;
    }

    QString ProjectCommand::description() const
    {
        return u"Create a new Tom application project"_s;
    }

    /* protected */

//    std::filesystem::path ProjectCommand::getMigrationPath() const
//    {
//        return path(__FILE__).parent_path().parent_path().parent_path() / "migrations";
//    }

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_PROJECTCOMMAND_HPP
