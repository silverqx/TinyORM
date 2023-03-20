#include "tom/commands/integratecommand.hpp"

#include <QCommandLineOption>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>

#include <orm/constants.hpp>
#include <orm/utils/type.hpp>

#include "tom/config.hpp"

#include "tom/commands/stubs/integratestubs.hpp"
#include "tom/exceptions/runtimeerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::COMMA;
#if defined(__linux__) || defined(__MINGW32__)
using Orm::Constants::TMPL_SQUOTES;
#endif

using Tom::Constants::ShPwsh;
using Tom::Constants::path_;
using Tom::Constants::path_up;
using Tom::Constants::shell_;
using Tom::Constants::stdout_;

#if defined(__linux__) || defined(__MINGW32__)
using Tom::Constants::ShBash;
using Tom::Constants::ShZsh;
#endif

namespace Tom::Commands
{

/* public */

IntegrateCommand::IntegrateCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
{}

const std::vector<PositionalArgument> &IntegrateCommand::positionalArguments() const
{
    static const std::vector<PositionalArgument> cached {
        {shell_, QStringLiteral("The shell name (values: bash, pwsh, zsh)"), {}, true},
    };

    return cached;
}

QList<CommandLineOption> IntegrateCommand::optionsSignature() const
{
    return {
        {stdout_, QStringLiteral("Print content of the <info>integrate</info> command "
                                 "(instead of writing to the disk)")},
        {path_,   QStringLiteral("The location where the completion file should be "
                                 "created (zsh only)"), path_up}, // Value
    };
}

int IntegrateCommand::run()
{
    Command::run();

    const auto shellArg = argument(shell_);
    const auto isStdOutArg = isSet(stdout_);

#if defined(__linux__) || defined(__MINGW32__)
    if (shellArg == ShBash) {
        if (!isStdOutArg)
            return integrateBash();

        note(Stubs::TomBashCompletionContent);
        return EXIT_SUCCESS;
    }

    if (shellArg == ShZsh) {
        if (!isStdOutArg)
            return integrateZsh();

        note(Stubs::TomZshCompletionContent);
        return EXIT_SUCCESS;
    }
#endif

    if (shellArg == ShPwsh) {
        if (!isStdOutArg)
            return integratePwsh();

        note(Stubs::RegisterArgumentCompleter);
        return EXIT_SUCCESS;
    }

    throwIfUnknownShell(shellArg);

    Q_UNREACHABLE();
}

/* protected */

/* Pwsh integrate related */

namespace
{
    /*! Try to open pwsh profile file and throws exception if failed. */
    void openPwshProfileFile(
                QFile &pwshProfileFile, const QString &pwshProfileFolder,
                const QString &profileFileRelative, const QString &pwshProfile)
    {
        // Initialize open flags
        QFile::OpenMode openMode = QIODevice::Text;

        if (pwshProfile.isEmpty())
            openMode |= QIODevice::WriteOnly;
        else
            openMode |= QIODevice::ReadWrite;

        // BUG __tiny_func__ fails in this type of functions silverqx
        if (!pwshProfileFile.open(openMode))
            throw Exceptions::RuntimeError(
                    QStringLiteral("Can not open '%1' file in openPwshProfileFile().")
                    .arg(QStringLiteral("%1/%2").arg(pwshProfileFolder,
                                                     profileFileRelative)));
    }

    /*! Detect whether PowerShell profile folder already exists and create it
        if doesn't. */
    void preparePowerShellFolder(
                const QString &pwshProfile, const QString &documentsFolder,
                const QDir &documentsDir, const QString &powershellFolder)
    {
        if (!pwshProfile.isEmpty() || documentsDir.exists(powershellFolder))
            return;

        // Create folder
        if (documentsDir.mkdir(powershellFolder))
            return;

        // BUG __tiny_func__ fails in this type of functions silverqx
        throw Exceptions::RuntimeError(
                    QStringLiteral("Can not create '%1' directory "
                                   "in preparePowerShellFolder().")
                    .arg(QStringLiteral("%1/%2").arg(documentsFolder,
                                                     powershellFolder)));
    }
} // namespace

/*! Template string for tab-completion installed successfully. */
Q_GLOBAL_STATIC_WITH_ARGS(
        QString, IntegrateSuccessTmpl, // NOLINT(misc-use-anonymous-namespace)
        ("Tab-completion for the %1 shell was successfully registered. 🎉"))
/*! Template string for tab-completion already registered. */
Q_GLOBAL_STATIC_WITH_ARGS(
        QString, IntegrateAlreadyRegisteredTmpl, // NOLINT(misc-use-anonymous-namespace)
        ("Tab-completion for the %1 shell is already registered. 🙌"))

int IntegrateCommand::integratePwsh() const
{
    // Prepare all paths
#ifdef _MSC_VER
    const auto powershellFolder = QStringLiteral("PowerShell");
#else
    const auto powershellFolder = QStringLiteral("powershell");
#endif
    const auto profileFileRelative = QStringLiteral("%1/Microsoft.PowerShell_profile.ps1")
                                     .arg(powershellFolder);

    // Get system dependant PowerShell folder
    const auto pwshProfileFolder = getPwshProfileFolder();
    const QDir documentsDir(pwshProfileFolder);

    // Try to locate pwsh profile file
#ifdef _MSC_VER
    auto pwshProfile = QStandardPaths::locate(QStandardPaths::DocumentsLocation,
                                              profileFileRelative);
#else
    auto pwshProfile = QStandardPaths::locate(QStandardPaths::GenericConfigLocation,
                                              profileFileRelative);
#endif

    // Detect whether PowerShell profile folder already exists and create it if doesn't
    preparePowerShellFolder(pwshProfile, pwshProfileFolder, documentsDir,
                            powershellFolder);

    // Write the TinyORM tom tab-completion code to the pwsh profile
    if (writeToPwshProfile(pwshProfileFolder, profileFileRelative, pwshProfile))
        // Yeah 🙌
        info(IntegrateSuccessTmpl->arg(ShPwsh));
    else
        comment(IntegrateAlreadyRegisteredTmpl->arg(ShPwsh));

    return EXIT_SUCCESS;
}

QString IntegrateCommand::getPwshProfileFolder()
{
#ifdef _MSC_VER
    auto pwshProfileFolders = QStandardPaths::standardLocations(
                                  QStandardPaths::DocumentsLocation);
#else
    auto pwshProfileFolders = QStandardPaths::standardLocations(
                                  QStandardPaths::GenericConfigLocation);
#endif

    Q_ASSERT(!pwshProfileFolders.isEmpty());

    return std::move(pwshProfileFolders.first());
}

bool IntegrateCommand::writeToPwshProfile(
            const QString &pwshProfileFolder, const QString &profileFileRelative,
            const QString &pwshProfile)
{
    const auto pwshProfileFilepath = QStringLiteral("%1/%2").arg(pwshProfileFolder,
                                                                 profileFileRelative);
    // Prepare QFile and QTextStream
    QFile pwshProfileFile(pwshProfileFilepath);
    openPwshProfileFile(pwshProfileFile, pwshProfileFolder, profileFileRelative,
                        pwshProfile);

    QTextStream pwshProfileStream(&pwshProfileFile);

    // Detect whether the tom tab-completion is already registered
    if (isPwshCompletionRegistered(pwshProfile, pwshProfileStream))
        return false;

    // Apppend tab-completion code to the pwsh profile
    pwshProfileStream << Stubs::RegisterArgumentCompleter;

#ifdef TINYTOM_DEBUG
    pwshProfileStream.flush();
#endif

    // Handle failed write
    if (pwshProfileStream.status() != QTextStream::WriteFailed)
        return true;

    throw Exceptions::RuntimeError(
                QStringLiteral(
                    "Write operation of the tom pwsh tab-completion code to the '%1' "
                    "file failed in %2().")
                .arg(pwshProfileFilepath, __tiny_func__));

    return true;
}

bool IntegrateCommand::isPwshCompletionRegistered(const QString &pwshProfile,
                                                  QTextStream &pwshProfileStream)
{
    if (pwshProfile.isEmpty())
        return false;

    auto line = pwshProfileStream.readLine();

    while (!line.isNull())
        if (line.contains(QStringLiteral("tom complete --word")))
            return true;
        else
            line = pwshProfileStream.readLine();

    return false;
}

/* Bash integrate related */

#if defined(__linux__) || defined(__MINGW32__)
int IntegrateCommand::integrateBash() const
{
    // Some validation
    throwIfBashCompletionDirNotExists();

    /* Write the TinyORM tom tab-completion code to:
       /usr/share/bash-completion/completions/tom */
    if (writeTomBashCompletion())
        // Yeah 🙌
        info(IntegrateSuccessTmpl->arg(ShBash));
    else
        comment(IntegrateAlreadyRegisteredTmpl->arg(ShBash));

    return EXIT_SUCCESS;
}

/*! Bash completions directory path. */
Q_GLOBAL_STATIC_WITH_ARGS(QString, BashCompletionsDirPath, // NOLINT(misc-use-anonymous-namespace)
                          ("/usr/share/bash-completion/completions"))

/*! Path to the TinyORM tom bash completion file. */
Q_GLOBAL_STATIC_WITH_ARGS(QString, TomBashCompletionFilepath, // NOLINT(misc-use-anonymous-namespace)
                          (QString("%1/tom").arg(*BashCompletionsDirPath)))

void IntegrateCommand::throwIfBashCompletionDirNotExists()
{
    if (QDir(*BashCompletionsDirPath).exists())
        return;

    throw Exceptions::RuntimeError(
                QStringLiteral("Bash completion directory '%1' doesn't exists in %2().")
                .arg(*BashCompletionsDirPath, __tiny_func__));
}

bool IntegrateCommand::writeTomBashCompletion()
{
    if (isBashCompletionRegistered())
        return false;

    QFile tomBashCompletionFile(*TomBashCompletionFilepath);
    if (!tomBashCompletionFile.open(QIODevice::WriteOnly | QIODevice::Text))
        throw Exceptions::RuntimeError(
                QStringLiteral("Can not open '%1' tom bash completion file in %2().")
                .arg(*TomBashCompletionFilepath, __tiny_func__));

    QTextStream tomBashCompletionStream(&tomBashCompletionFile);

    tomBashCompletionStream << Stubs::TomBashCompletionContent;

#ifdef TINYTOM_DEBUG
    tomBashCompletionStream.flush();
#endif

    // Handle failed write
    if (tomBashCompletionStream.status() != QTextStream::WriteFailed)
        return true;

    throw Exceptions::RuntimeError(
                QStringLiteral(
                    "Write operation of the tom bash tab-completion file '%1' failed "
                    "in %2().")
                .arg(*TomBashCompletionFilepath, __tiny_func__));
}

bool IntegrateCommand::isBashCompletionRegistered()
{
    return QFile::exists(*TomBashCompletionFilepath);
}

/* Zsh integrate related */

int IntegrateCommand::integrateZsh() const
{
    /* It's design to work on Ubuntu, Manjaro, Arch, and Gentoo, if any of these folders
       exist /usr/[local/]share/zsh/site-functions then write the _tom completion file
       into it, if don't then try to create the /usr/local/share/zsh/site-functions
       folder and write to it. These folders should be on the zsh's $fpath on all above
       described OS-es, finito, I'm done with this, it's good enough 😎. */

    /* Write the TinyORM tom tab-completion code to:
       /usr/[local/]share/zsh/site-functions/_tom */
    if (writeTomZshCompletionWrapper())
        // Yeah 🙌
        info(IntegrateSuccessTmpl->arg(ShZsh));
    else
        comment(IntegrateAlreadyRegisteredTmpl->arg(ShZsh));

    return EXIT_SUCCESS;
}

/*! Zsh completions directory path. */
Q_GLOBAL_STATIC_WITH_ARGS(QStringList, ZshCompletionsDirPaths, // NOLINT(misc-use-anonymous-namespace)
                          ({"/usr/local/share/zsh/site-functions",
                            "/usr/share/zsh/site-functions"}))

/*! Zsh completion files structure holds parent directory path and also filepath. */
struct ZshCompletionPathsItem
{
    /*! Parent directory path. */
    QString dirPath;
    /*! Absolute completion file filepath. */
    QString filePath;
};

/*! Paths to the TinyORM tom zsh completion files. */
Q_GLOBAL_STATIC_WITH_ARGS(
        QVector<ZshCompletionPathsItem>, TomZshCompletionPaths, // NOLINT(misc-use-anonymous-namespace)
        ({{ZshCompletionsDirPaths->constFirst(),
           QString("%1/_tom").arg(ZshCompletionsDirPaths->constFirst())},
          {ZshCompletionsDirPaths->at(1),
           QString("%1/_tom").arg(ZshCompletionsDirPaths->at(1))}}))

bool IntegrateCommand::writeTomZshCompletionWrapper() const
{
    // Allow to override installation folder using the --path= option
    zshOverrideInstallFolder();

    if (isZshCompletionRegistered())
        return false;

    // One of the folder paths exists, so loop and write completion file
    if (anyCompletionDirPathExists()) {
        if (writeTomZshCompletionToExistingFolder())
            return true;
    }
    // Try to create a folder and write completion file
    else {
        createZshCompletionFolder();

        // And try to write the completion file again
        if (writeTomZshCompletion(TomZshCompletionPaths->constFirst().filePath))
            return true;
    }

    throw Exceptions::RuntimeError(
                QStringLiteral(
                    "Write operation of the tom zsh tab-completion files %1 failed "
                    "in %2(), please run with sudo.")
                .arg(getCompletionFilepaths().join(COMMA), __tiny_func__));
}

void IntegrateCommand::zshOverrideInstallFolder() const
{
    if (!isSet(path_))
        return;

    const auto completionsDir = QDir::cleanPath(value(path_));

    TomZshCompletionPaths->prepend({{completionsDir},
                                    {QStringLiteral("%1/_tom").arg(completionsDir)}});
}

bool IntegrateCommand::isZshCompletionRegistered()
{
    return std::ranges::any_of(*TomZshCompletionPaths,
                               [](const auto &completionPathsItem)
    {
        return QFile::exists(completionPathsItem.filePath);
    });
}

bool IntegrateCommand::writeTomZshCompletionToExistingFolder()
{
    return std::ranges::any_of(*TomZshCompletionPaths,
                               [](const auto &completionPathsItem)
    {
        return QDir(completionPathsItem.dirPath).exists() &&
                writeTomZshCompletion(completionPathsItem.filePath);
    });
}

bool IntegrateCommand::anyCompletionDirPathExists()
{
    return std::ranges::any_of(*TomZshCompletionPaths,
                               [](const auto &completionPathsItem)
    {
        return QDir(completionPathsItem.dirPath).exists();
    });
}

bool IntegrateCommand::writeTomZshCompletion(const QString &filepath)
{
    QFile tomZshCompletionFile(filepath);
    if (!tomZshCompletionFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream tomZshCompletionStream(&tomZshCompletionFile);

    tomZshCompletionStream << Stubs::TomZshCompletionContent;

#ifdef TINYTOM_DEBUG
    tomZshCompletionStream.flush();
#endif

    // Handle failed write
    return tomZshCompletionStream.status() != QTextStream::WriteFailed;
}

void IntegrateCommand::createZshCompletionFolder()
{
    if (QDir(QStringLiteral("/"))
            .mkpath(TomZshCompletionPaths->constFirst().dirPath)
    )
        return;

    throw Exceptions::RuntimeError(
                QStringLiteral("Can not create '%1' directory in %2(), please run "
                               "with sudo.")
                .arg(TomZshCompletionPaths->constFirst().dirPath, __tiny_func__));
}

QStringList IntegrateCommand::getCompletionFilepaths()
{
    QStringList completionFilepaths;

    std::ranges::transform(*TomZshCompletionPaths,
                           std::back_inserter(completionFilepaths),
                           [](const auto &completionPaths)
    {
        return TMPL_SQUOTES.arg(completionPaths.filePath);
    });

    return completionFilepaths;
}
#endif

/* Others */

void IntegrateCommand::throwIfUnknownShell(const QString &shellArg)
{
    const QStringList allowedShells {
#if defined(__linux__) || defined(__MINGW32__)
        ShBash,
#endif
        ShPwsh,
#if defined(__linux__) || defined(__MINGW32__)
        ShZsh,
#endif
    };

    throw Exceptions::RuntimeError(
                QStringLiteral("Unknown shell name '%1' (allowed values: %2).")
                .arg(shellArg, allowedShells.join(COMMA)));
}

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE
