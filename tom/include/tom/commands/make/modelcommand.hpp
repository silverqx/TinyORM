#pragma once
#ifndef TOM_COMMANDS_MAKE_MODELCOMMAND_HPP
#define TOM_COMMANDS_MAKE_MODELCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <unordered_set>

#include "tom/commands/command.hpp"
#include "tom/commands/make/concerns/prepareoptionvalues.hpp"
#include "tom/commands/make/modelcommandconcepts.hpp"
#include "tom/commands/make/support/modelcreator.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands::Make
{

namespace Support
{
    template<BtmPreparedValuesConcept P, BtmValuesConcept V>
    class PrepareBtmOptionValues;
}

    /*! Create a new model class. */
    class ModelCommand : public Command,
                         protected Concerns::PrepareOptionValues
    {
        Q_DISABLE_COPY(ModelCommand)

        // To access parser() and isSetAll()
        friend Concerns::PrepareOptionValues;
        // To access relationNames() and m_unusedBtmOptions
        friend Support::PrepareBtmOptionValues<QStringList, QStringList>;
        // To access relationNames() and m_unusedBtmOptions
        friend Support::PrepareBtmOptionValues<std::vector<QStringList>, QStringList>;
        // To access relationNames() and m_unusedBtmOptions
        friend Support::PrepareBtmOptionValues<std::vector<bool>, std::vector<bool>>;

        /*! Alias for the filesystem path. */
        using fspath = std::filesystem::path;
        /*! Alias for the command line option values. */
        using CmdOptions = Support::ModelCreator::CmdOptions;
        /*! Alias for the foreign keys. */
        using ForeignKeys = Support::ModelCreator::ForeignKeys;

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
        /*! Prepare a model class names. */
        static std::tuple<QString, CmdOptions>
        prepareModelClassnames(QString &&className, CmdOptions &&cmdOptions);

        /*! Show unused options warning. */
        void showUnusedOptionsWarnings(const CmdOptions &cmdOptions);
        /*! Find unused btm options if any --belongs-to-many= option is defined. */
        void findUnusedBtmOptions(const CmdOptions &cmdOptions);

        /*! Write the model file to the disk. */
        void writeModel(const QString &className, const CmdOptions &cmdOptions);

        /*! Create command line options instance. */
        CmdOptions createCmdOptions();

        /* Foreign key names */
        /*! Divide foreign key names by relation types. */
        ForeignKeys prepareForeignKeys(const QStringList &foreignKeyValues);
        /*! Try to start a new relation during foreign key names search. */
        static bool startNewRelation(
                    const std::unordered_set<QString> &relationNames, QString &option,
                    QString &currentRelation, ForeignKeys &foreignKeys,
                    bool &wasForeignKeySet, bool &wasForeignKeySetPartial);
        /*! Insert the default value if no foreign key was passed on the cmd. line. */
        static void insertEmptyForeignList(const QString &currentRelation,
                                           ForeignKeys &foreignKeys);
        /*! Foreign key name found, assign it to the correct relation type. */
        static void insertForeignKeyName(
                    const QString &currentRelation, ForeignKeys &foreignKeys,
                    const QStringList &foreignKeyValues,
                    QStringList::size_type &foreignIndex, bool &wasForeignKeySet,
                    bool &wasForeignKeySetPartial);
        /*! Foreign key name found, assign it to the correct relation type (for btm). */
        static void insertForeignKeyNameBtm(
                    ForeignKeys &foreignKeys, const QStringList &foreignKeyValues,
                    QStringList::size_type &foreignIndex, bool &wasForeignKeySet,
                    bool &wasForeignKeySetPartial);
        /*! Show unused foreign key option warning. */
        void showUnusedForeignKeyWarning();

        /* Others */
        /*! Get the model path (either specified by the --path option or the default
            location). */
        fspath getModelPath() const;
        /*! Set of all cmd. option relation names. */
        const std::unordered_set<QString> &relationNames();

        /*! The model creator instance. */
        Support::ModelCreator m_creator {};
        /*! Indicates whether the unused warning for foreign-key option been shown. */
        bool m_shownUnusedForeign = false;
        /*! Unused btm options, will be shown in the warning. */
        std::set<QString> m_unusedBtmOptions {};

    private:
        /*! Throw if the model name constains a namespace or path. */
        static void throwIfContainsNamespaceOrPath(const QStringList &classNames,
                                                   const QString &source);
        /*! Throw if the model name constains a namespace or path. */
        static void throwIfContainsNamespaceOrPath(const QString &className,
                                                   const QString &source);
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
