#!/usr/bin/env pwsh

Param(
    [Parameter(Position = 0, Mandatory,
        HelpMessage = 'Specifies the Qt version for which to build the QMYSQL driver ' +
            '(version number inside the Qt installation folder).')]
    [ValidateNotNullOrEmpty()]
    [ValidatePattern('^[5-9]\.\d{1,2}\.\d{1,2}$',
        ErrorMessage = 'The argument "{0}" is not the correct Qt version number. ' +
        'The argument "{0}" does not match the "{1}" pattern.')]
    [string] $QtVersion,

    [Parameter(Position = 1,
        HelpMessage = 'Specifies the MySQL Server version against which to build ' +
            'the QMYSQL driver (version number of the MySQL Server installation folder).')]
    [ValidateNotNullOrEmpty()]
    [ValidatePattern('^(?:[8-9]|1[0-2])\.\d{1,2}$',
        ErrorMessage = 'The argument "{0}" is not the correct Qt version number. ' +
        'The argument "{0}" does not match the "{1}" pattern.')]
    [string] $MySQLVersion = '9.1',

    [Parameter(HelpMessage = 'Clean CMake build (delete the $QtVersion build folder).')]
    [switch] $CleanBuild,

    [Parameter(
        HelpMessage = 'Specifies the main Qt path, is $env:TINY_QT_ROOT or C:\Qt by default.')]
    [ValidateNotNullOrEmpty()]
    [string] $QtRootPath,

    [Parameter(
        HelpMessage = 'Specifies the MySQL server installation path, by default is guessed using ' +
            'the MySQLVersion argument in the default $env:ProgramFiles installation location.')]
    [ValidateNotNullOrEmpty()]
    [string] $MySQLServerPath,

    [Parameter(
        HelpMessage = 'Specifies the parent path to the CMake build folders, ' +
            'is $env:TINY_QT_QMYSQL_BUILD_PATH or pwd by default.')]
    [ValidateNotNullOrEmpty()]
    [string] $BuildPath,

    [Parameter(
        HelpMessage = 'Specifies the Qt spec and is used in the CMAKE_INSTALL_PREFIX, ' +
            'is msvc2019_64 by default.')]
    [ValidateNotNullOrEmpty()]
    [string] $QtSpec = 'msvc2019_64',

    [Parameter(HelpMessage = 'Specifies whether to install the QMYSQL drivers.')]
    [switch] $NoInstall,

    [Parameter(HelpMessage = 'Specifies whether to skip initializing Build and Qt environments.')]
    [switch] $SkipInitializeBuildEnvironment
)

Set-StrictMode -Version 3.0

# Includes
# ---
. $PSScriptRoot\private\Common-Host.ps1
. $PSScriptRoot\private\Common-Path.ps1

# Script variables section
# ---
Set-Variable STACK_NAME -Option Constant -Value $MyInvocation.MyCommand.Name

$Script:QtRoot = $PSBoundParameters.ContainsKey('QtRootPath') `
                 ? $QtRootPath
                 : $env:TINY_QT_ROOT ?? 'C:\Qt'
$Script:QtRoot = Get-FullPath -Path $Script:QtRoot
$Script:QtRootAlt = $Script:QtRoot.Replace('\', '/')
$Script:QtMajorVersion = $null
$Script:QtEnvVersion = $null
$Script:VisualStudioVersion = '17.0'
$Script:MySqlServerPath = $PSBoundParameters.ContainsKey('MySQLServerPath') `
                          ? (Get-FullPath -Path $MySQLServerPath).Replace('\', '/')
                          : "C:/Program Files/MySQL/MySQL Server $MySQLVersion"
$Script:BuildPath = $PSBoundParameters.ContainsKey('BuildPath') `
                    ? $BuildPath
                    : $env:TINY_QT_QMYSQL_BUILD_PATH ?? $(Get-Location).Path
$Script:BuildPath = Get-FullPath -Path $Script:BuildPath
$Script:QtSqlDriversBuildPath = $null
$Script:BOL = '  '

# Functions section
# ---

# Initialize Qt's major and environment version script variables
function Initialize-QtVersions
{
    # Extract major and minor versions from the passed $QtVersion
    if (-not ($QtVersion -match '^(?<major>[5-9])\.(?<minor>\d{1,2})\.\d{1,2}$')) {
        Write-ExitError 'Match for the ''$QtEnvVersion'' variable failed.' -NoNewlineBefore
    }

    $Script:QtMajorVersion = $Matches['major']
    $Script:QtEnvVersion   = $Matches['major'] + $Matches['minor']
}

# Check if the Qt version is >5
function Test-QtVersion
{
    Write-Progress 'Testing if the Qt version is >5'

    if ($Script:QtMajorVersion -gt 5) {
        return
    }

    Write-ExitError ("The passed Qt version '$QtVersion' is not supported because it doesn't " +
        'support the CMake build.')
}

# Check whether the passed QtVersion is installed
function Test-QtVersionInstalled
{
    Write-Progress "Testing whether Qt $QtVersion is installed"

    if (Test-Path "$Script:QtRoot\$QtVersion") {
        return
    }

    Write-ExitError ("The passed Qt version '$QtVersion' is not installed at: " +
        "$Script:QtRoot\$QtVersion")
}

# Check whether the source files to build the Qt MySQL plugin are installed
function Test-QtSourcesInstalled {
    Write-Progress "Testing whether Qt $QtVersion source files are installed"

    if (Test-Path "$Script:QtRoot\$QtVersion\Src\qtbase\src\plugins\sqldrivers") {
        return
    }

    Write-ExitError ("Source files to build the Qt MySQL plugin for the passed " +
        "Qt version '$QtVersion' are not installed " +
        "in the '$Script:QtRoot\$QtVersion\Src\qtbase\src\plugins\sqldrivers' folder.")
}

# Check whether the MySQL Server is installed
function Test-MySQLServerInstalled
{
    Write-Progress "Testing whether MySQL Server $MySQLVersion is installed"

    if (Test-Path $Script:MySqlServerPath) {
        return
    }

    Write-ExitError ("The MySQL Server $MySQLVersion is not installed " +
        "in the '$Script:MySqlServerPath' folder.")
}

# Create the main Qt sqldrivers build folder
function New-QtSqlDriversBuildPath
{
    Write-Progress 'Creating the main Qt sqldrivers build folder'

    $Script:QtSqlDriversBuildPath = "${Script:BuildPath}\qt${Script:QtMajorVersion}_sqldrivers"

    if (-not (Test-Path -Path $Script:QtSqlDriversBuildPath)) {
        New-Item -ItemType Directory -Path $Script:QtSqlDriversBuildPath | Out-Null

        Write-Progress "${Script:BOL}Created at: $Script:QtSqlDriversBuildPath"
    }
    else {
        Write-Error "${Script:BOL}Build folder already exists at: $Script:QtSqlDriversBuildPath"
    }
}

# Remove $QtVersion build folder if the $CleanBuild was passed
function Invoke-CleanBuild
{
    if (-not $CleanBuild) {
        return
    }

    Write-Progress "Removing $QtVersion build folder (Clean build)"

    Remove-Item -Force -Recurse "./$QtVersion" -ErrorAction SilentlyContinue
}

# Create the build folders for debug and release builds
function New-BuildFolders
{
    Write-Progress 'Creating build folders'

    $relWithDebInfoPath = Join-Path -Path $PWD -ChildPath "$QtVersion/$QtSpec/RelWithDebInfo"

    if (-not (Test-Path $relWithDebInfoPath)) {
        New-Item -Type Directory -Path $relWithDebInfoPath | Out-Null

        Write-Progress "${Script:BOL}Created the Release folder at: $relWithDebInfoPath"
    }
    else {
        Write-Error "${Script:BOL}Release folder already exists at: $relWithDebInfoPath"
    }

    $debugPath = Join-Path -Path $PWD -ChildPath "$QtVersion/$QtSpec/Debug"

    if (-not (Test-Path $debugPath)) {
        New-Item -Type Directory -Path $debugPath | Out-Null

        Write-Progress "${Script:BOL}Created the Debug folder at: $debugPath"
    }
    else {
        Write-Error "${Script:BOL}Debug folder already exists at: $debugPath"
    }
}

# Test whether a Qt environment initialization was successful
function Test-BuildEnvironment
{
    Newline
    Write-Progress 'Testing whether the build environment is ready'

    # Test MSVC build environment
    if (-not (Test-Path env:VisualStudioVersion) -or `
        $env:VisualStudioVersion -ne $Script:VisualStudioVersion
    ) {
        $majorVersion = ($Script:VisualStudioVersion -split '\.', 2)[0]
        Write-ExitError ("The Visual Studio '$majorVersion' build environment is not " +
            'on the system path.')
    }

    # Test Qt build environment
    [System.Version] $qtVersionOnPath = qmake -query QT_VERSION
    # This would need more handling because the $QtVersion.Revision is -1 and because of that
    # the same versions are not equal
    # [System.Version] $qtVersion = (Get-Command qmake -ErrorAction SilentlyContinue).Version

    if (-not $qtVersionOnPath -or $qtVersionOnPath -ne $QtVersion) {
        Write-ExitError ("The requested Qt version '$QtVersion' is not on the system path.")
    }

    Write-Info 'Build environment is ready 🥳'
    Newline
}

# Initialize the Qt and MSVC build environment if it's not already there
function Initialize-QtEnvironment
{
    Write-Progress 'Initializing Qt and MSVC build environment'

    if (Test-Path env:WindowsSDKLibVersion) {
        Write-Error ("${Script:BOL}The MSVC build environment already initialized. " +
            'Exiting the Qt environment initialization!')

        return
    }

    . "qtenv${Script:QtEnvVersion}.ps1"
}

# Main section
# ---

# Preparations
# ---

if (-not (Test-Path env:RUNNER_ENVIRONMENT)) {
    Clear-Host
}

Write-Header "Preparations"

Initialize-QtVersions
Test-QtVersion
Test-QtVersionInstalled
Test-QtSourcesInstalled
Test-MySQLServerInstalled

# Initialize the Qt and MSVC build environment if it's not already there
if (-not $SkipInitializeBuildEnvironment) {
    Initialize-QtEnvironment
    Test-BuildEnvironment
}
else {
    Write-Error 'Skipping Qt and MSVC build environment initialization'
}

# Prepare the build folder
# Create the main Qt sqldrivers build folder
New-QtSqlDriversBuildPath

Push-Location -StackName $STACK_NAME
Set-Location $Script:QtSqlDriversBuildPath

# Remove $QtVersion build folder if the $CleanBuild was passed
Invoke-CleanBuild
# Create the build folders for debug and release builds
New-BuildFolders

# Debug build
# ---
Write-Header 'Debug Build'

Set-Location "$QtVersion/$QtSpec/Debug"

# Configure
Write-Progress 'Configuring...'

qt-cmake `
    --log-level=VERBOSE `
    -S "$Script:QtRootAlt/$QtVersion/Src/qtbase/src/plugins/sqldrivers" `
    -B . `
    -G Ninja `
    -D CMAKE_BUILD_TYPE:STRING=Debug `
    -D CMAKE_INSTALL_PREFIX:PATH="$Script:QtRootAlt/$QtVersion/$QtSpec" `
    -D MySQL_INCLUDE_DIR:PATH="${Script:MySqlServerPath}/include" `
    -D MySQL_LIBRARY:FILEPATH="${Script:MySqlServerPath}/lib/libmysql.lib" `
    -D FEATURE_sql_psql:BOOL=OFF `
    -D FEATURE_sql_odbc:BOOL=OFF `
    -D FEATURE_sql_sqlite:BOOL=OFF

NewLine

# Build and/or install
$Script:InstallingText = $NoInstall ? 'only' : 'and installing'
Write-Progress "Building ${Script:InstallingText}..."

cmake --build . --target ($NoInstall ? 'all' : 'install')

# Release build
# ---
Write-Header 'Release Build'

Set-Location '../RelWithDebInfo'

Write-Progress 'Configuring...'

qt-cmake `
    --log-level=VERBOSE `
    -S "$Script:QtRootAlt/$QtVersion/Src/qtbase/src/plugins/sqldrivers" `
    -B . `
    -G Ninja `
    -D CMAKE_BUILD_TYPE:STRING=RelWithDebInfo `
    -D CMAKE_INSTALL_PREFIX:PATH="$Script:QtRootAlt/$QtVersion/$QtSpec" `
    -D MySQL_INCLUDE_DIR:PATH="${Script:MySqlServerPath}/include" `
    -D MySQL_LIBRARY:FILEPATH="${Script:MySqlServerPath}/lib/libmysql.lib" `
    -D FEATURE_sql_psql:BOOL=OFF `
    -D FEATURE_sql_odbc:BOOL=OFF `
    -D FEATURE_sql_sqlite:BOOL=OFF

NewLine

# Build and/or install
Write-Progress "Building ${Script:InstallingText}..."

cmake --build . --target ($NoInstall ? 'all' : 'install')

# Done
# ---

Pop-Location -StackName $STACK_NAME

Newline
Write-Progress 'Linked against the MySQL Server at: ' -NoNewline
Write-Info $Script:MySqlServerPath

if (-not $NoInstall) {
    Write-Progress 'QMYSQL driver installed to: ' -NoNewline
    Write-Info "$Script:QtRootAlt/$QtVersion/$QtSpec"
}

$Script:InstalledText = $NoInstall ? '' : ' and installed'
Newline
Write-Info "The QMYSQL driver for Qt $QtVersion was built${Script:InstalledText} successfully. 🥳"
