#!/usr/bin/env pwsh

Param(
    [Parameter(Mandatory,
        HelpMessage = 'Specifies the path to the TinyORM source folder.')]
    [ValidateNotNullOrEmpty()]
    [string] $SourcePath,

    [Parameter(
        HelpMessage = 'Specifies the path to the CMake build folder, is pwd by default.')]
    [ValidateNotNullOrEmpty()]
    [string] $BuildPath = (Get-Location).Path,

    [Parameter(HelpMessage = 'Specifies the cpp files to be processed, is joined with ' +
            'the | character and used in the parenthesized regex eg. (file1|file2).')]
    [ValidateNotNullOrEmpty()]
    [string[]] $Files,

    [Parameter(HelpMessage = 'Specifies the files paths to be processed, is joined with by the | ' +
            'character and used in the parenthesized regex eg. (folder1|folder2).')]
    [ValidateNotNullOrEmpty()]
    [string[]] $FilesPaths,

    [Parameter(HelpMessage = 'Specifies subfolders to lint. The pattern value is used ' +
        'in regular expression, eg. (drivers|examples|src|tests|tom).')]
    [AllowEmptyString()]
    [string] $InSubFoldersPattern = '(?:drivers|examples|src|tests|tom)',

    [Parameter(HelpMessage = 'Skip Clang Tidy analyzes.')]
    [switch] $SkipClangTidy,

    [Parameter(HelpMessage = 'Skip Clazy standalone analyzes.')]
    [switch] $SkipClazy,

    [Parameter(HelpMessage = 'Specifies Qt version to use for compilation (sources qtenvX.ps1).')]
    [ValidateRange(5, 6)]
    [int] $QtVersion = 6,

    [Parameter(HelpMessage = 'Clean CMake build (remove everything inside the -BuildPath).')]
    [switch] $CleanBuild,

    [Parameter(HelpMessage = 'Specifies the CMake build type.')]
    [ValidateSet('Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel')]
    [ValidateNotNullOrEmpty()]
    [string] $BuildType = 'Debug',

    [Parameter(
        HelpMessage = 'Specifies the checks filter, when not specified, use clang-tidy default ' +
            '(eg. -*,readability-magic-numbers to run only a specific check).')]
    [ValidateNotNullOrEmpty()]
    [string[]] $ClangTidyChecks,

    [Parameter(HelpMessage = 'Specifies the number of tidy instances to be run in parallel.')]
    [ValidateNotNull()]
    [int] $Parallel,

    [Parameter(HelpMessage = 'Build TinyORM database drivers that replaces QtSql module.')]
    [switch] $BuildDrivers,

    [Parameter(HelpMessage = 'Build TinyDrivers database driver (core/common code) ' +
        '(default: Loadable).')]
    [ValidateSet('Shared', 'Loadable', 'Static')]
    [ValidateNotNullOrEmpty()]
    [string] $DriversType = 'Loadable'
)

Set-StrictMode -Version 3.0

. $PSScriptRoot\private\Common-Host.ps1
. $PSScriptRoot\private\Common-Path.ps1

# Script variables section
# ---

Set-Variable STACK_NAME -Option Constant -Value $MyInvocation.MyCommand.Name

# Number of processes to spawn (value passed to the -j option)
if (-not $PSBoundParameters.ContainsKey('Parallel') -or $Parallel -lt 1) {
    $Script:Platform = $PSVersionTable.Platform

    switch ($Script:Platform) {
        'Win32NT' {
            $Parallel = $env:NUMBER_OF_PROCESSORS - 4
        }
        'Unix' {
            $Parallel = (nproc) - 4
        }
        Default {
            throw "$Script:Platform platform is not supported."
        }
    }
}

# Rules for linting folders using the -FilesPaths parameter:
#  - xyz    - passed folder only
#  - xyz.*? - passed folder and all subfolders
#  - xyz.+? - subfolders only excluding the passed folder

# Prepare regex paths and provide default values if not passed
$FilesPaths = $null -eq $FilesPaths ? '.+?'          : "(?:$($FilesPaths -join '|'))"
$Files      = $null -eq $Files      ? '[\w\d_\-\+]+' : "(?:$($Files -join '|'))"

$Script:RegEx = "[\\\/]+$InSubFoldersPattern[\\\/]+$FilesPaths[\\\/]+(?!mocs_)$Files\.cpp$"

# Append the build type to the build path
$BuildPath = $BuildPath.TrimEnd((Get-Slashes))
$BuildPath += "_$BuildType"

# Functions section
# ---

# Create TinyDrivers-related CMake parameters based on the pwsh script arguments
function Set-DriversCMakeParameters {
    [OutputType([string[]])]
    Param()

    # The DRIVERS_TYPE can only be changed if the -BuildDrivers is explicitly specified, this is ok
    if ($BuildDrivers) {
        return @('-D BUILD_DRIVERS:BOOL=ON', "-D DRIVERS_TYPE:STRING=$DriversType")
    }

    # Explicitly pass OFF when the CMake build folder already exists, also, disable it only when
    # the -BuildDrivers:$false is explicitly specified, if not specified then CMake will use
    # the cached value
    if ($Script:PSBoundParameters.ContainsKey('BuildDrivers') -and `
        (Test-Path $BuildPath\CMakeCache.txt)
    ) {
        return @('-D BUILD_DRIVERS:BOOL=OFF')
    }

    # However, if the CMake build folder doesn't exist, pass nothing to avoid the unused parameter
    # warning
    return @()
}

# Main section
# ---

# Test whether the TinyORM source folder exists
if (-not (Test-Path $SourcePath)) {
    Write-ExitError "The TinyORM source folder doesn't exist."
}

# Create build folder
if (-not (Test-Path $BuildPath)) {
    New-Item -Path $BuildPath -ItemType Directory | Out-Null
}

Push-Location -StackName $STACK_NAME

Set-Location -Path $BuildPath

# Obtain and print Recommended Clang-Tidy and clazy version numbers
$regExClangTidyVersion = '(?:LLVM version )(?<version>\d+\.\d+\.\d+)'
(clang-tidy --version | Select-String -Pattern $regExClangTidyVersion) `
    -match $regExClangTidyVersion | Out-Null
$clangTidyCurrentVersion = $Matches.version
$regExClazyVersion = '(?:clazy version )(?<version>\d+\.\d+(?:\.\d+)?)'
(clazy-standalone --version | Select-String -Pattern $regExClazyVersion) `
    -match $regExClazyVersion | Out-Null
$clazyCurrentVersion = $Matches.version

Write-Info 'Recommended versions'
Newline
Write-Output "  Clang-Tidy `e[32mv19`e[0m (current `e[33mv$clangTidyCurrentVersion`e[0m)"
Write-Output "  Clazy standalone `e[32mv1.12`e[0m (current `e[33mv$clazyCurrentVersion`e[0m)"

# Initialize build environment if it's not already initialized
if ($PSVersionTable.Platform -ceq 'Win32NT' -and -not (Test-Path env:WindowsSDKLibVersion)) {
    Newline
    . "qtenv${QtVersion}.ps1"
}

# Clean build
if ($CleanBuild) {
    if ($null -eq (Get-Command -Name rmq.ps1 -ErrorAction SilentlyContinue)) {
        Remove-Item -Recurse -Force $BuildPath\*
    }
    else {
        rmq.ps1 -Path $BuildPath
    }

    Write-Header "`nClean CMake $BuildType build`n" -NoNewlines
}
else {
    Write-Header "`nCMake $BuildType build`n" -NoNewlines
}

# Configure and Build
cmake `
    -S $SourcePath `
    -B $BuildPath `
    -G 'Ninja' `
    -D CMAKE_CXX_COMPILER_LAUNCHER:FILEPATH=ccache `
    <# the $null can't be swapped by the '' empty string! #> `
    ($PSVersionTable.Platform -ceq 'Unix' ? '-D CMAKE_CXX_COMPILER:FILEPATH=clang++'
                                          : $null) `
    -D CMAKE_TOOLCHAIN_FILE:PATH="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
    -D CMAKE_DISABLE_PRECOMPILE_HEADERS:BOOL=ON `
    -D CMAKE_EXPORT_COMPILE_COMMANDS:BOOL=ON `
    -D CMAKE_EXPORT_PACKAGE_REGISTRY:BOOL=OFF `
    -D CMAKE_BUILD_TYPE:STRING=$BuildType `
    -D CMAKE_CXX_SCAN_FOR_MODULES:BOOL=OFF `
    -D VCPKG_APPLOCAL_DEPS:BOOL=OFF `
    -D VERBOSE_CONFIGURE:BOOL=ON `
    -D BUILD_TREE_DEPLOY:BOOL=OFF `
    -D MATCH_EQUAL_EXPORTED_BUILDTREE:BOOL=OFF `
    -D LEAN_HEADERS:BOOL=ON `
    -D STRICT_MODE:BOOL=ON `
    -D MYSQL_PING:BOOL=ON `
    -D BUILD_TESTS:BOOL=ON `
    -D ORM:BOOL=ON `
    -D TOM:BOOL=ON `
    -D TOM_EXAMPLE:BOOL=ON `
    (Set-DriversCMakeParameters)

if (-not $?) {
    Write-ExitError 'CMake configure failed.'
}

if (-not (Test-Path $BuildPath\compile_commands.json)) {
    Write-ExitError "CMake configure didn't generate '$BuildPath\compile_commands.json' " +
        'compilation database, CMAKE_EXPORT_COMPILE_COMMANDS must be set to ON.'
}

cmake --build $BuildPath --target all `
    $($VerbosePreference -eq 'SilentlyContinue' ? '' : '--verbose')

if (-not $?) {
    Write-ExitError 'CMake build failed.'
}

# This is critical so Clang-Tidy can correctly apply Checks from the .clang-tidy file
Set-Location -Path $SourcePath

# Analyze using the Clang Tidy and Clazy standalone
if (-not $SkipClangTidy) {
    Write-Header 'Clang Tidy'

    # Allow to pass a custom -checks option, the $null can't be swapped by the '' empty string!
    $checksOption = $PSBoundParameters.ContainsKey('ClangTidyChecks') `
                    ? "-checks=$($ClangTidyChecks -join ',')"
                    : $null

    run-clang-tidy.ps1 `
        -use-color `
        -extra-arg-before='-Qunused-arguments' `
        -j $Parallel `
        -p="$BuildPath" $checksOption $Script:RegEx

    Newline
}
else {
    Newline
    Write-Info 'Skipping Clang Tidy'
}

if (-not $SkipClazy) {
    Write-Header 'Clazy standalone'

    # Disabled checks
    # Level 2      - qstring-allocations
    # Manual level - qt-keyword-emit (I'm not using the emit keyword; new in v1.12)
    #                qt4-qstring-from-array (removed from v1.12)
    #                qt6-qlatin1stringchar-to-u
    #                qvariant-template-instantiation
    #                unused-result-check (causes crashes)
    # Others:
    # In v1.12 is also a new unused-result-check warning, but when I tried it clazy-standalone.exe
    # crashed (on Windows), this check isn't EVEN in the Clazy Changelog (Release Notes).
    $Script:Checks =
        'level0,level1,level2,' +
        # Manual checks
        'assert-with-side-effects,container-inside-loop,detaching-member,' +
        'heap-allocated-small-trivial-type,ifndef-define-typo,isempty-vs-count,jni-signatures,' +
        'qhash-with-char-pointer-key,qproperty-type-mismatch,qrequiredresult-candidates,' +
        'qstring-varargs,qt-keywords,qt6-deprecated-api-fixes,qt6-fwd-fixes,qt6-header-fixes,' +
        'qt6-qhash-signature,raw-environment-function,reserve-candidates,' +
        'signal-with-return-value,thread-with-slots,tr-non-literal,unneeded-cast,' +
        'use-chrono-in-qtimer,' +
        # New in Clazy v1.11
        'unexpected-flag-enumerator-value,' +
        'use-arrow-operator-instead-of-data,' +
        # Checks Excluded from level2
        'no-qstring-allocations,' +
        # New in Clazy v1.12
        'no-module-include,' +
        'sanitize-inline-keyword'

    run-clazy-standalone.ps1 `
        -checks="$Script:Checks" `
        -extra-arg-before='-Qunused-arguments' `
        -header-filter="[\\\/]+$InSubFoldersPattern[\\\/]+.+\.(h|hpp)$" `
        -j $Parallel `
        -p="$BuildPath" $Script:RegEx
}
else {
    Write-Info 'Skipping Clazy standalone'
}

Pop-Location -StackName $STACK_NAME
