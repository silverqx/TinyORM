#pragma once
#ifndef TOM_COMMANDS_HELPCOMMAND_HPP
#define TOM_COMMANDS_HELPCOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"
#include "tom/concerns/printsoptions.hpp"
#include "tom/tomconstants.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands
{

    /*! Display help for a command. */
    class HelpCommand : public Command,
                        public Concerns::PrintsOptions
    {
        Q_DISABLE_COPY_MOVE(HelpCommand)

    public:
        /*! Constructor. */
        HelpCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        ~HelpCommand() override = default;

        /*! The console command name. */
        inline QString name() const override;
        /*! The console command description. */
        inline QString description() const override;

        /*! The console command positional arguments signature. */
        const std::vector<PositionalArgument> &positionalArguments() const override;
        /*! The console command help. */
        QString help() const override;

        /*! Execute the console command. */
        int run() override;

    protected:
        /*! Create command by the given name. */
        std::unique_ptr<Command> createCommand(const QString &name) const;
        /*! Validate if all required positional arguments are after optional arguments. */
        bool validateRequiredArguments(
                const std::vector<PositionalArgument> &arguments) const;

        /*! Print description section. */
        void printDescriptionSection(const Command &command) const;
        /*! Print usage section. */
        void printUsageSection(const Command &command,
                               const std::vector<PositionalArgument> &arguments) const;

        /*! Print positional arguments section. */
        void printArgumentsSection(
                const std::vector<PositionalArgument> &arguments) const;
        /*! Get max. positional argument size in all command arguments. */
        static QString::size_type
        argumentsMaxSize(const std::vector<PositionalArgument> &arguments);
        /*! Count all argument sizes in the given container. */
        static QString::size_type
        countArgumentsSizes(const std::vector<PositionalArgument> &arguments);
        /*! Print a positional's argument default value part. */
        void printArgumentDefaultValue(const PositionalArgument &argument) const;

        /*! Print options section. */
        PrintsOptions::SizeType printOptionsSection(const Command &command) const;
        /*! Print help section. */
        void printHelpSection(const Command &command) const;
    };

    /* public */

    QString HelpCommand::name() const
    {
        return Constants::Help;
    }

    QString HelpCommand::description() const
    {
        return QStringLiteral("Display help for a command");
    }

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_HELPCOMMAND_HPP
