#pragma once
#ifndef TOM_COMMANDS_LISTCOMMAND_HPP
#define TOM_COMMANDS_LISTCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"
#include "tom/concerns/printsoptions.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands
{

    /*! List all available commands. */
    class ListCommand : public Command,
                        public Concerns::PrintsOptions
    {
        Q_DISABLE_COPY(ListCommand)

    public:
        /*! Constructor. */
        ListCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        inline ~ListCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! The console command positional arguments signature. */
        const std::vector<PositionalArgument> &positionalArguments() const override;
        /*! The signature of the console command. */
        QList<QCommandLineOption> optionsSignature() const override;

        /*! The console command help. */
        QString help() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! Output full commands list. */
        int full(const QString &namespaceArg);
        /*! Output raw commands list and nothing else (can be consumed by scripts). */
        int raw(const QString &namespaceArg);

        /* Commands section */
        /*! Print commands section. */
        void printCommandsSection(const QString &namespaceArg, int optionsMaxSize) const;
        /*! Get max. command size in all command names. */
        int commandsMaxSize(const std::vector<std::shared_ptr<Command>> &commands,
                           int optionsMaxSize) const;
        /*! Print commands to the console. */
        void printCommands(const std::vector<std::shared_ptr<Command>> &commands,
                           int commandsMaxSize, bool hasNamespaceArg) const;
        /*! Print a new namespace section. */
        void tryBeginNsSection(QString &renderingNamespace,
                               const QString &commandName, bool hasNamespaceArg) const;
        /*! Get command's namespace from a command name. */
        QString commandNamespace(const QString &commandName) const;

        /*! Wrapper for the two methods below, helps to avoid one copy. */
        const std::vector<std::shared_ptr<Command>> &
        getCommandsByNamespace(const QString &name) const;
        /*! Obtain all commands in the given namespace. */
        std::vector<std::shared_ptr<Command>>
        getCommandsInNamespace(const QString &name) const;
    };

    /* public */

    QString ListCommand::name() const
    {
        return QStringLiteral("list");
    }

    QString ListCommand::description() const
    {
        return QLatin1String("List commands");
    }

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_LISTCOMMAND_HPP
