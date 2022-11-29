#pragma once
#ifndef TOM_COMMANDS_INTEGRATECOMMAND_HPP
#define TOM_COMMANDS_INTEGRATECOMMAND_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "tom/commands/command.hpp"
#include "tom/tomconstants.hpp"

class QTextStream;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands
{

    /*! Enable tab-completion for the given shell. */
    class IntegrateCommand : public Command
    {
        Q_DISABLE_COPY(IntegrateCommand)

    public:
        /*! Constructor. */
        IntegrateCommand(Application &application, QCommandLineParser &parser);
        /*! Virtual destructor. */
        inline ~IntegrateCommand() override = default;

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
        /* Pwsh integrate related */
        /*! Integrate tab-completion for the pwsh shell. */
        int integratePwsh() const;

        /*! Get system dependant Documents folder. */
        static QString getPwshProfileFolder();
        /*! Write the TinyORM tom tab-completion code to the pwsh profile. */
        static bool writeToPwshProfile(
                const QString &pwshProfileFolder, const QString &profileFileRelative,
                const QString &pwshProfile);
        /*! Detect whether the tom tab-completion is already registered (pwsh). */
        static bool isPwshCompletionRegistered(const QString &pwshProfile,
                                               QTextStream &pwshProfileStream);

        /* Bash integrate related */
#if defined(__linux__) || defined(__MINGW32__)
        /*! Integrate tab-completion for the bash shell. */
        int integrateBash() const;

        /*! Throw exception if the bash completion directory doesn't exists. */
        static void throwIfBashCompletionDirNotExists();
        /*! Write the TinyORM tom tab-completion code for the bash shell. */
        static bool writeTomBashCompletion();
        /*! Detect whether the tom tab-completion is already registered (bash). */
        static bool isBashCompletionRegistered();
#endif

        /* Zsh integrate related */
#if defined(__linux__) || defined(__MINGW32__)
        /*! Integrate tab-completion for the zsh shell. */
        int integrateZsh() const;

        /*! Write the TinyORM tom tab-completion code for the zsh shell. */
        bool writeTomZshCompletionWrapper() const;
        /*! Allow to override installation folder using the --path= option. */
        void zshOverrideInstallFolder() const;
        /*! Detect whether the tom tab-completion is already registered (zsh). */
        static bool isZshCompletionRegistered();
        /*! Write to already existing completion folder. */
        static bool writeTomZshCompletionToExistingFolder();
        /*! Check whether any of the completion folders exist. */
        static bool anyCompletionDirPathExists();
        /*! Write the TinyORM tom tab-completion code for the zsh shell. */
        static bool writeTomZshCompletion(const QString &filepath);
        /*! Create /usr/local/share/zsh/site-functions folder for zsh completions. */
        static void createZshCompletionFolder();
        /*! Get list of all completion filepaths. */
        static QStringList getCompletionFilepaths();
#endif

        /* Others */
        /*! Throw exception if unknown shell was passed. */
        static void throwIfUnknownShell(const QString &shellArg);
    };

    /* public */

    QString IntegrateCommand::name() const
    {
        return Constants::Integrate;
    }

    QString IntegrateCommand::description() const
    {
        return QStringLiteral("Enable tab-completion for the given shell");
    }

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_COMMANDS_INTEGRATECOMMAND_HPP
