#pragma once
#ifndef TOM_COMMANDS_LISTCOMMAND_HPP
#define TOM_COMMANDS_LISTCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <unordered_set>

#include "tom/commands/command.hpp"
#include "tom/concerns/printsoptions.hpp"
#include "tom/tomconstants.hpp"

class QCommandLineOption;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands
{

    /*! List all available commands. */
    class ListCommand : public Command,
                        public Concerns::PrintsOptions
    {
        Q_DISABLE_COPY_MOVE(ListCommand)

    public:
        /*! Constructor. */
        ListCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        ~ListCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! The console command positional arguments signature. */
        const std::vector<PositionalArgument> &positionalArguments() const override;
        /*! The signature of the console command. */
        QList<CommandLineOption> optionsSignature() const override;

        /*! The console command help. */
        QString help() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! Output full commands list. */
        int full(const QString &namespaceArg);
        /*! Output raw commands list and nothing else (can be consumed by scripts). */
        int raw(const QString &namespaceArg);

        /* Guess namespace name section */
        /*! Get the namespace name including the guess namespace name logic. */
        QString getNamespaceName(const QString &namespaceArg) const;
        /*! Try to guess the namespace name. */
        static std::vector<QString> guessNamespace(const QString &namespaceArg);
        /*! Print the error wall with ambiguous namespaces. */
        Q_NORETURN void
        printAmbiguousNamespaces(const QString &namespaceName,
                                 const std::vector<QString> &namespaces) const;

        /* Commands section */
        /*! Print commands section. */
        void printCommandsSection(const QString &namespaceName, int optionsMaxSize) const;
        /*! Get max. command size in all command names. */
        static int commandsMaxSize(const std::vector<std::shared_ptr<Command>> &commands,
                                   int optionsMaxSize);
        /*! Print commands to the console. */
        void printCommands(const std::vector<std::shared_ptr<Command>> &commands,
                           int commandsMaxSize, bool hasNamespaceName) const;
        /*! Print a new namespace section. */
        void tryBeginNsSection(QString &renderingNamespace,
                               const QString &commandName, bool hasNamespaceName) const;
        /*! Get command's namespace from a command name. */
        static QString commandNamespace(const QString &commandName);

        /*! Wrapper for the two methods below, helps to avoid one copy. */
        const std::vector<std::shared_ptr<Command>> &
        getCommandsByNamespace(const QString &name) const;

        /*! Commands to exclude from the list. */
        /*const*/ std::unordered_set<QString> m_dontList {
            Tom::Constants::Complete,
        };
    };

    /* public */

    QString ListCommand::name() const
    {
        return Constants::List;
    }

    QString ListCommand::description() const
    {
        return QStringLiteral("List commands");
    }

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_LISTCOMMAND_HPP
