#pragma once
#ifndef TOM_COMMANDS_MAKE_SEEDERCOMMAND_HPP
#define TOM_COMMANDS_MAKE_SEEDERCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"
#include "tom/seedercreator.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make
{

    /*! Create a new seeder class. */
    class SeederCommand : public Command
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
        QList<QCommandLineOption> optionsSignature() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! Prepare a seeder class name (eg. appends Seeder if needed). */
        static QString prepareSeederClassname(QString &&className);

        /*! Write the seeder file to the disk. */
        void writeSeeder(const QString &className) const;

        /*! Get the seeder path (either specified by the --path option or the default
            location). */
        fspath getSeederPath() const;

        /*! The seeder creator instance. */
        SeederCreator m_creator {};

    private:
        /*! Throw if the seeder name constains a namespace or path. */
        static void throwIfContainsNamespaceOrPath(const QString &className);
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
