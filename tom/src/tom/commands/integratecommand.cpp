#include "tom/commands/integratecommand.hpp"

#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>

#include "tom/exceptions/runtimeerror.hpp"

using Tom::Constants::ShPwsh;
using Tom::Constants::shell_;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands
{

/* public */

IntegrateCommand::IntegrateCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
{}

const std::vector<PositionalArgument> &IntegrateCommand::positionalArguments() const
{
    static const std::vector<PositionalArgument> cached {
        {shell_, QStringLiteral("The shell name (values: pwsh)"), {}, true},
    };

    return cached;
}

int IntegrateCommand::run()
{
    Command::run();

    const auto shellArg = argument(shell_);

//    if (shellArg == ShBash)
//        return integrateBash();

    if (shellArg == ShPwsh)
        return integratePwsh();

//    if (shellArg == ShZsh)
//        return integrateZsh();

    return EXIT_SUCCESS;
}

/* protected */

/* pwsh profile related */

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
                    .arg(QStringLiteral("%1/%2").arg(std::move(pwshProfileFolder),
                                                     std::move(profileFileRelative))));
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
                    .arg(QStringLiteral("%1/%2").arg(std::move(documentsFolder),
                                                     std::move(powershellFolder))));
    }
} // namespace

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

    // Get system dependant Documents folder
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
        info(QStringLiteral(
                 "Tab-completion for the pwsh shell was successfully registered. 🎉"));
    else
        comment(QStringLiteral(
                    "Tab-completion for the pwsh shell is already registered. 🙌"));

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
    // Prepare QFile and QTextStream
    QFile pwshProfileFile(QStringLiteral("%1/%2").arg(pwshProfileFolder,
                                                      profileFileRelative));
    openPwshProfileFile(pwshProfileFile, pwshProfileFolder, profileFileRelative,
                        pwshProfile);

    QTextStream pwshProfileStream(&pwshProfileFile);

    // Detect whether the tom tab-completion is already registered
    if (isPwshCompletionRegistered(pwshProfile, pwshProfileStream))
        return false;

    // Apppend tab-completion code to the pwsh profile
    pwshProfileStream << getRegisterArgumentCompleter();

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

QString IntegrateCommand::getRegisterArgumentCompleter()
{
    return QStringLiteral(R"(
# TinyORM tom tab-completion
# ---

Register-ArgumentCompleter -Native -CommandName tom -ScriptBlock {
    param($wordToComplete, $commandAst, $cursorPosition)
    [Console]::InputEncoding = [Console]::OutputEncoding = $OutputEncoding = [System.Text.Utf8Encoding]::new()
    $Local:word = $wordToComplete.Replace('"', '""')
    $Local:ast = $commandAst.ToString().Replace('"', '""')
    tom complete --word="$Local:word" --commandline="$Local:ast" --position=$cursorPosition
        | ForEach-Object {
            $completionText, $listText, $toolTip = $_ -split ';', 3
            $listText ??= $completionText
            $toolTip ??= $completionText
            [System.Management.Automation.CompletionResult]::new(
                $completionText, $listText, 'ParameterValue', $toolTip)
        }
}
)");
}

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE
