#!/usr/bin/env pwsh

Param(
    [Parameter(Position = 0, Mandatory,
        HelpMessage = 'Specifies the Qt version for which to build the QMYSQL driver ' +
            '(version number inside the Qt installation folder).')]
    [ValidateNotNullOrEmpty()]
    [ValidatePattern('^[5-8]\.\d{1,2}\.\d{1,2}$',
        ErrorMessage = 'The argument "{0}" is not the correct Qt version number. ' +
        'The argument "{0}" does not match the "{1}" pattern.')]
    [string] $QtVersion,

    [Parameter(HelpMessage = 'Clean CMake build (delete the $QtVersion build folder).')]
    [switch] $CleanBuild
)

Set-StrictMode -Version 3.0

# Script variables section
# ---
Set-Variable STACK_NAME -Option Constant -Value $MyInvocation.MyCommand.Name

$Script:QtMajorVersion = $null
$Script:QtEnvVersion = $null
$Script:VisualStudioVersion = '17.0'

# Functions section
# ---
. $PSScriptRoot\private\Common-Host.ps1

# Initialize Qt's major and environment version script variables
function Initialize-QtVersions
{
    # Extract major and minor versions from the passed $QtVersion
    if (-not ($QtVersion -match '^(?<major>[5-8])\.(?<minor>\d{1,2})\.\d{1,2}$')) {
        throw 'Match for the ''$QtEnvVersion'' variable failed.'
    }

    $Script:QtMajorVersion = $Matches['major']
    $Script:QtEnvVersion   = $Matches['major'] + $Matches['minor']
}

# Check if the Qt version is >5
function Test-QtVersion
{
    Write-Progress "Testing if the Qt version is >5"

    if ($Script:QtMajorVersion -gt 5) {
        return
    }

    throw "The passed '$QtVersion' is not supported because it doesn't support the CMake build."
}

# Check whether the passed QtVersion is installed
function Test-QtVersionInstalled
{
    Write-Progress "Testing whether Qt $QtVersion is installed"

    if (Test-Path "C:\Qt\$QtVersion") {
        return
    }

    throw "The passed '$QtVersion' version is not installed in the 'C:\Qt\$QtVersion\' folder."
}

# Check whether the source files to build the Qt MySQL plugin are installed
function Test-QtSourcesInstalled {
    Write-Progress "Testing whether Qt $QtVersion source files are installed"

    if (Test-Path "C:\Qt\$QtVersion\Src\qtbase\src\plugins\sqldrivers") {
        return
    }

    throw "Source files to build the Qt MySQL plugin for the passed '$QtVersion' version are not " +
        "installed in the 'C:\Qt\$QtVersion\Src\qtbase\src\plugins\sqldrivers' folder."
}

# Remove $QtVersion build folder if the $CleanBuild was passed
function Invoke-CleanBuild
{
    if (-not $CleanBuild) {
        return
    }

    Write-Progress "Removing $QtVersion build folder (Clean build)"

    Remove-Item -Force -Recurse "$QtVersion"
}

# Create the build folders for debug and release builds
function New-BuildFolders
{
    Write-Progress 'Creating build folders'

    mkdir -p "$QtVersion/msvc2019_64/RelWithDebInfo"
    mkdir -p "$QtVersion/msvc2019_64/Debug"

    NewLine
}

# Test whether a Qt environment initialization was successful
function Test-BuildEnvironment
{
    Write-Progress 'Testing whether the build environment is ready'

    # Test MSVC build environment
    if (-not (Test-Path env:VisualStudioVersion) -or `
        $env:VisualStudioVersion -ne $Script:VisualStudioVersion
    ) {
        $majorVersion = ($Script:VisualStudioVersion -split '\.', 2)[0]
        throw "The Visual Studio '$majorVersion' build environment is not on the system path."
    }

    # Test Qt build environment
    [System.Version] $qtVersionOnPath = qmake -query QT_VERSION
    # This would need more handling because the $QtVersion.Revision is -1 and because of that
    # the same versions are not equal
    # [System.Version] $qtVersion = (Get-Command qmake -ErrorAction SilentlyContinue).Version

    if (-not $qtVersionOnPath -or $qtVersionOnPath -ne $QtVersion) {
        throw "The requested Qt version '$QtVersion' is not on the system path."
    }

    Write-Host 'Build environment is ready 🥳' -ForegroundColor DarkGreen
}

# Initialize the Qt and MSVC2022 build environment if it's not already there
function Initialize-QtEnvironment
{
    Write-Progress 'Initializing Qt and MSVC2022 build environment'

    if (Test-Path env:WindowsSDKLibVersion) {
        Write-Host ('The MSVC build environment already initialized. Exiting the Qt environment ' +
            'initialization!') -ForegroundColor DarkRed

        return
    }

    . "qtenv${Script:QtEnvVersion}.ps1"
}

# Main section
# ---

# Preparations
# ---

Clear-Host

Write-Header "Preparations"

Initialize-QtVersions
Test-QtVersion
Test-QtVersionInstalled
Test-QtSourcesInstalled

Push-Location -StackName $STACK_NAME

Set-Location "E:\c_libs\qt${Script:QtMajorVersion}_sqldrivers"

# Remove $QtVersion build folder if the $CleanBuild was passed
Invoke-CleanBuild
# Create the build folders for debug and release builds
New-BuildFolders

# Initialize the Qt and MSVC2022 build environment if it's not already there
Initialize-QtEnvironment
Test-BuildEnvironment

# Debug build
# ---
Write-Header 'Debug Build'

Set-Location "$QtVersion/msvc2019_64/Debug"

# Configure
Write-Progress 'Configuring...'

qt-cmake `
    -S "C:/Qt/$QtVersion/Src/qtbase/src/plugins/sqldrivers" `
    -B . `
    -G Ninja `
    -D CMAKE_BUILD_TYPE=Debug `
    -D CMAKE_INSTALL_PREFIX="C:/Qt/$QtVersion/msvc2019_64" `
    -D MySQL_INCLUDE_DIR='C:/Program Files/MySQL/MySQL Server 8.0/include' `
    -D MySQL_LIBRARY='C:/Program Files/MySQL/MySQL Server 8.0/lib/libmysql.lib' `
    -D FEATURE_sql_psql=OFF `
    -D FEATURE_sql_odbc=OFF `
    -D FEATURE_sql_sqlite=OFF

NewLine

# Build and install
Write-Progress 'Building and installing...'

cmake --build . --target install

# Release build
# ---
Write-Header 'Release Build'

Set-Location '../RelWithDebInfo'

Write-Progress 'Configuring...'

qt-cmake `
    -S "C:/Qt/$QtVersion/Src/qtbase/src/plugins/sqldrivers" `
    -B . `
    -G Ninja `
    -D CMAKE_BUILD_TYPE=RelWithDebInfo `
    -D CMAKE_INSTALL_PREFIX="C:/Qt/$QtVersion/msvc2019_64" `
    -D MySQL_INCLUDE_DIR='C:/Program Files/MySQL/MySQL Server 8.0/include' `
    -D MySQL_LIBRARY='C:/Program Files/MySQL/MySQL Server 8.0/lib/libmysql.lib' `
    -D FEATURE_sql_psql=OFF `
    -D FEATURE_sql_odbc=OFF `
    -D FEATURE_sql_sqlite=OFF

NewLine

# Build and install
Write-Progress 'Building and installing...'

cmake --build . --target install

# Done
# ---
Pop-Location -StackName $STACK_NAME

Write-Host "The QMYSQL driver for Qt $QtVersion was built and installed successfully. 🥳" `
    -ForegroundColor DarkGreen
