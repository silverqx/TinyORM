#pragma once
#ifndef TOM_COMMANDS_MAKE_MODELCOMMAND_HPP
#define TOM_COMMANDS_MAKE_MODELCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"
#include "tom/commands/make/support/modelcreator.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make
{

    /*! Create a new model class. */
    class ModelCommand : public Command
    {
        Q_DISABLE_COPY(ModelCommand)

        /*! Alias for the filesystem path. */
        using fspath = std::filesystem::path;
        /*! Alias for the command line option values. */
        using CmdOptions = Support::ModelCreator::CmdOptions;

    public:
        /*! Constructor. */
        ModelCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        inline ~ModelCommand() override = default;

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
        /*! Prepare a model class name. */
        static QString prepareModelClassname(QString &&className);

        /*! Write the model file to the disk. */
        void writeModel(const QString &className);

        /*! Create command line options instance. */
        CmdOptions createCmdOptions() const;
        /*! Get the model path (either specified by the --path option or the default
            location). */
        fspath getModelPath() const;

        /*! The model creator instance. */
        Support::ModelCreator m_creator {};

    private:
        /*! Throw if the model name constains a namespace or path. */
        static void throwIfContainsNamespaceOrPath(const QString &className);
    };

    /* public */

    QString ModelCommand::name() const
    {
        return Constants::MakeModel;
    }

    QString ModelCommand::description() const
    {
        return QStringLiteral("Create a new model class");
    }

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_MODELCOMMAND_HPP
