#!/usr/bin/env pwsh

Param(
    [Parameter(Position = 0, Mandatory = $false,
        HelpMessage = 'Specifies the cpp files to be processed, is joined with by the | ' +
            'character and used in the parenthesized regex eg. (file1|file2).')]
    [ValidateNotNullOrEmpty()]
    [string[]] $Files,

    [Parameter(Mandatory = $false,
        HelpMessage = 'Specifies the files paths to be processed, is joined with by the | ' +
            'character and used in the parenthesized regex eg. (folder1|folder2).')]
    [ValidateNotNullOrEmpty()]
    [string[]] $FilesPaths,

    [Parameter(Mandatory = $false,
        HelpMessage = 'Specifies the path to the cmake build folder, is pwd by default.')]
    [ValidateNotNullOrEmpty()]
    [string] $BuildPath = $($(Get-Location).Path),

    [Parameter(HelpMessage = 'Skip Clang Tidy analyzes.')]
    [switch] $SkipClangTidy,

    [Parameter(HelpMessage = 'Skip Clazy standalone analyzes.')]
    [switch] $SkipClazy,

    [Parameter(HelpMessage = 'Specifies Qt version to use for compilation (sources qtenvX.ps1).')]
    [ValidateRange(5, 6)]
    [int] $QtVersion = 6,

    [Parameter(HelpMessage = 'Clean CMake build (remove everything inside the -BuildPath).')]
    [switch] $Clean,

    [Parameter(HelpMessage = 'Specifies the CMake build type.')]
    [ValidateSet('Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel')]
    [ValidateNotNullOrEmpty()]
    [string] $BuildType = 'Debug',

    [Parameter(Mandatory = $false,
        HelpMessage = 'Specifies the checks filter, when not specified, use clang-tidy default ' +
            '(eg. -*,readability-magic-numbers to run only a specific check).')]
    [ValidateNotNullOrEmpty()]
    [string[]] $Checks
)

Set-StrictMode -Version 3.0

# Number of processes to spawn (value passed to the -j option)
$Script:numberOfProcesses = $env:NUMBER_OF_PROCESSORS - 2

# Rules for linting folders using the -FilesPaths parameter:
#  - xyz    - passed folder only
#  - xyz.*? - passed folder and all subfolders
#  - xyz.+? - subfolders only excluding the passed folder

# Prepare regex paths and provide default values if not passed
$FilesPaths = $null -eq $FilesPaths ? '.+?'        : "(?:$($FilesPaths -join '|'))"
$Files      = $null -eq $Files      ? '[\w_\-\+]+' : "(?:$($Files -join '|'))"

$Script:RegEx = "(?:examples|src|tests)[\\\/]+$FilesPaths[\\\/]+(?!mocs_)$($Files)\.cpp$"

# Append the build type to the build path
$BuildPath = $BuildPath.TrimEnd(
    [IO.Path]::DirectorySeparatorChar,
    [IO.Path]::AltDirectorySeparatorChar
)
$BuildPath += "_$BuildType"

# Create build folder
if (-not (Test-Path $BuildPath)) {
    New-Item -Path $BuildPath -ItemType Directory | Out-Null
}

Push-Location

Set-Location -Path $BuildPath

# Initialize build environment if it's not already there
if (-not (Test-Path env:WindowsSDKLibVersion)) {
    . "qtenv${QtVersion}.ps1"
}

# Clean build
if ($Clean) {
    Remove-Item -Recurse -Force $BuildPath\*

    Write-Host "`nClean CMake $BuildType build`n" -ForegroundColor Green
}
else {
    Write-Host "`nCMake $BuildType build`n" -ForegroundColor DarkBlue
}

# Configure
if (-not (Test-Path $BuildPath\compile_commands.json)) {
    cmake `
        -S E:/c/qMedia/TinyOrm/TinyOrm `
        -B $BuildPath `
        -G 'Ninja' `
        -D CMAKE_BUILD_TYPE:BOOL=$BuildType `
        -D CMAKE_TOOLCHAIN_FILE:PATH=E:/c_libs/vcpkg/scripts/buildsystems/vcpkg.cmake `
        -D CMAKE_INSTALL_PREFIX:PATH=E:/c/qMedia/tmp/dummy `
        # -D CMAKE_CXX_COMPILER_LAUNCHER:FILEPATH='C:/Users/<username>/scoop/shims/ccache.exe' `
        -D CMAKE_VERBOSE_MAKEFILE:BOOL=OFF `
        -D VERBOSE_CONFIGURE:BOOL=ON `
        -D BUILD_TESTS:BOOL=ON `
        -D MYSQL_PING:BOOL=ON `
        -D TOM_EXAMPLE:BOOL=ON `
        -D CMAKE_DISABLE_PRECOMPILE_HEADERS:BOOL=ON `
        -D CMAKE_EXPORT_COMPILE_COMMANDS:BOOL=ON `
        -D MATCH_EQUAL_EXPORTED_BUILDTREE:BOOL=OFF `
        -D CMAKE_EXPORT_PACKAGE_REGISTRY:BOOL=OFF
}

cmake --build $BuildPath --target all `
    $($VerbosePreference -eq 'SilentlyContinue' ? '' : '--verbose')

if (-not $?) {
    Write-Host
    throw 'CMake build failed'
}

if (-not $SkipClangTidy) {
    Write-Host
    Write-Host 'Clang Tidy' -ForegroundColor DarkBlue
    Write-Host

    # Allow to pass a custom -checks option, the $null can't be swapped by the '' empty string,
    # it doesn't work then, I don't understand why
    $checksOption = $PSBoundParameters.ContainsKey('Checks') ? "-checks=$($Checks -join ',')" `
                                                             : $null

    & 'E:\dotfiles\bin\run-clang-tidy.ps1' -use-color -extra-arg-before='-Qunused-arguments' `
        -j $Script:numberOfProcesses -p="$BuildPath" $checksOption $Script:RegEx
}

if (-not $SkipClazy) {
    Write-Host
    Write-Host 'Clazy standalone' -ForegroundColor DarkBlue
    Write-Host

    # Disabled checks
    # Level 2      - qstring-allocations
    # Manual level - qt4-qstring-from-array, qt6-qlatin1stringchar-to-u,
    #                qvariant-template-instantiation
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
        # New in Clazy 1.11
        'unexpected-flag-enumerator-value,' +
        'use-arrow-operator-instead-of-data,' +
        # Checks Excluded from level2
        'no-qstring-allocations'

    & 'E:\dotfiles\bin\run-clazy-standalone.ps1' `
        -checks="$Script:Checks" `
        -extra-arg-before='-Qunused-arguments' -header-filter='(orm|tom|migrations)/.+\.(h|hpp)$' `
        -j $Script:numberOfProcesses `
        -p="$BuildPath" $Script:RegEx
}

Pop-Location
