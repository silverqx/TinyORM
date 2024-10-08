#pragma once
#ifndef TOM_COMMANDS_MAKE_MODELCOMMAND_HPP
#define TOM_COMMANDS_MAKE_MODELCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <unordered_set>

#include "tom/commands/make/concerns/prepareoptionvalues.hpp"
#include "tom/commands/make/makecommand.hpp"
#include "tom/commands/make/modelcommandconcepts.hpp"
#include "tom/commands/make/support/modelcreator.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make
{

namespace Support
{
    template<BtmPreparedValuesConcept P, BtmValuesConcept V>
    class PrepareBtmOptionValues;

    class PrepareForeignKeyValues;

} // namespace Support

    /*! Create a new model class. */
    class ModelCommand : public MakeCommand,
                         public Concerns::PrepareOptionValues
    {
        Q_DISABLE_COPY_MOVE(ModelCommand)

        // To access values() and isSetAll()
        friend Concerns::PrepareOptionValues;
        // To access relationNames(), values(), optionNames() and m_unusedBtmOptions
        friend Support::PrepareBtmOptionValues<QStringList, QStringList>;
        // To access relationNames() and m_unusedBtmOptions
        friend Support::PrepareBtmOptionValues<std::vector<QStringList>, QStringList>;
        // To access relationNames() and m_unusedBtmOptions
        friend Support::PrepareBtmOptionValues<std::vector<bool>, std::vector<bool>>;
        // To access relationNames(), optionNames(), comment() and m_shownUnusedForeignKey
        friend Support::PrepareForeignKeyValues;

        /*! Alias for the filesystem path. */
        using fspath = std::filesystem::path;

    public:
        /*! Constructor. */
        ModelCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        ~ModelCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! The console command positional arguments signature. */
        const std::vector<PositionalArgument> &positionalArguments() const override;
        /*! Options signature of the console command. */
        QList<CommandLineOption> optionsSignature() const override;

        /*! The console command help. */
        QString help() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! Prepare a model class names. */
        static std::tuple<QString, CmdOptions>
        prepareModelClassNames(QString &&className, CmdOptions &&cmdOptions);

        /*! Show unused options warning. */
        void showUnusedOptionsWarnings(const CmdOptions &cmdOptions);
        /*! Show unused btm options warning. */
        void showUnusedBtmOptionsWarnings(const CmdOptions &cmdOptions);
        /*! Find unused btm options if any --belongs-to-many= option is defined. */
        void findUnusedBtmOptions(const CmdOptions &cmdOptions);
        /*! Show unused options warning if generating a pivot model. */
        void showUnusedPivotModelOptionsWarnings();
        /*! Show unused disable-incremening option if passed also the incrementing
            option. */
        void showUnusedIncrementingWarning();
        /*! Show unused disable-snake-attributes option if passed also
            the snake-attributes option. */
        void showUnusedSnakeAttributesWarning();

        /*! Write the model file to the disk. */
        void writeModel(const QString &className, const CmdOptions &cmdOptions,
                        const fspath &modelsPath);

        /*! Create command-line options instance. */
        CmdOptions createCmdOptions();
        /*! Get the order of relationship methods defined on the command-line. */
        RelationsOrder relationsOrder();

        /* Others */
        /*! Get the models path (either specified by the --path option or the default
            path). */
        fspath getModelsPath() const;
        /*! Get the models path specified by the --path option. */
        fspath getUserModelsPath() const;
        /*! Try to guess the models path based on the pwd or use the default path. */
        fspath guessModelsPath() const;

        /*! Get all cmd. option relation names. */
        static const std::unordered_set<QString> &relationNames();

        /*! Create a migration file for the model. */
        void createMigration(const QString &className) const;
        /*! Create a seeder file for the model. */
        void createSeeder(const QString &className) const;

        /*! The model creator instance. */
        Support::ModelCreator m_creator;
        /*! Was shown an unused warning for the foreign-key option? */
        bool m_shownUnusedForeignKey = false;
        /*! Was shown an unused warning for the disable-/incrementing option? */
        bool m_shownUnusedIncrementing = false;
        /*! Was shown an unused warning for the disable-/snake-attributes option? */
        bool m_shownUnusedSnakeAttribtues = false;
        /*! Unused btm options, will be shown in the warning. */
        std::set<QString> m_unusedBtmOptions;
        /*! Unused options if generating a pivot model, will be shown in the warning. */
        std::set<QString> m_unusedPivotModelOptions;
        /*! Was --pivot-model option given on the command-line? */
        bool m_isSetPivotModel = false;

    private:
        /*! Throw if the model name contains a namespace or path. */
        static void throwIfContainsNamespaceOrPath(
                const std::vector<QStringList> &classNames, const QString &source,
                const QString &commandType = u"model"_s);
        /*! Throw if the model name contains a namespace or path. */
        static void throwIfContainsNamespaceOrPath(
                const QStringList &classNames, const QString &source,
                const QString &commandType = u"model"_s);
    };

    /* public */

    QString ModelCommand::name() const
    {
        return Constants::MakeModel;
    }

    QString ModelCommand::description() const
    {
        return u"Create a new model class"_s;
    }

} // namespace Tom::Commands::Make

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_MAKE_MODELCOMMAND_HPP
