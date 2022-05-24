#!/usr/bin/env pwsh

Param(
    [Parameter(Position = 0, Mandatory = $false,
        HelpMessage = 'Specifies the files to be processed, is joined with by the | character ' +
            'and used in the parenthesized regex eg. (file1|file2).')]
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
    [int] $QtVersion = 5,

    [Parameter(HelpMessage = 'Clean CMake build (remove everything inside the -BuildPath).')]
    [switch] $Clean
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

$Script:RegEx = "(?:src|tests)[\\\/]+$FilesPaths[\\\/]+(?!mocs_)$($Files)\.cpp$"

Push-Location

Set-Location -Path $BuildPath

# Initialize build environment if it's not already there
if (-not (Test-Path env:WindowsSDKLibVersion)) {
    . "E:\dotfiles\bin\qtenv${QtVersion}.ps1"
}

# Clean build
if ($Clean) {
    Remove-Item -Recurse -Force $BuildPath\*

    Write-Host "`nClean CMake build`n" -ForegroundColor Green
}
else {
    Write-Host "`nCMake build`n" -ForegroundColor DarkBlue
}

# Configure
if (-not (Test-Path $BuildPath\compile_commands.json)) {
    cmake `
        -S E:/c/qMedia/TinyOrm/TinyOrm `
        -B $BuildPath `
        -G 'Ninja' `
        -D CMAKE_BUILD_TYPE:BOOL=Debug `
        -D CMAKE_TOOLCHAIN_FILE:PATH=E:/c_libs/vcpkg/scripts/buildsystems/vcpkg.cmake `
        -D CMAKE_INSTALL_PREFIX:PATH=E:/c/qMedia/tmp/dummy `
        -D VERBOSE_CONFIGURE:BOOL=ON `
        -D BUILD_TESTS:BOOL=ON `
        -D MYSQL_PING:BOOL=ON `
        -D TOM_EXAMPLE:BOOL=ON `
        -D CMAKE_DISABLE_PRECOMPILE_HEADERS:BOOL=ON `
        -D CMAKE_EXPORT_COMPILE_COMMANDS:BOOL=ON `
        -D MATCH_EQUAL_EXPORTED_BUILDTREE:BOOL=OFF
}

cmake --build $BuildPath --target all

if (-not $?) {
    Write-Host
    throw 'CMake build failed'
}

if (-not $SkipClangTidy) {
    Write-Host
    Write-Host 'Clang Tidy' -ForegroundColor DarkBlue
    Write-Host

    & 'E:\dotfiles\bin\run-clang-tidy.ps1' -extra-arg-before='-Qunused-arguments' `
        -j $Script:numberOfProcesses -p="$BuildPath" $Script:RegEx
}

if (-not $SkipClazy) {
    Write-Host
    Write-Host 'Clazy standalone' -ForegroundColor DarkBlue
    Write-Host

    $Script:Checks =
        'level0,level1,level2,' +
        # Manual checks
        'assert-with-side-effects,container-inside-loop,detaching-member,' +
        'heap-allocated-small-trivial-type,ifndef-define-typo,isempty-vs-count,jni-signatures,' +
        'qhash-with-char-pointer-key,qproperty-type-mismatch,qrequiredresult-candidates,' +
        'qstring-varargs,qt6-deprecated-api-fixes,qt6-fwd-fixes,qt6-header-fixes,' +
        'qt6-qhash-signature,raw-environment-function,reserve-candidates,' +
        'signal-with-return-value,thread-with-slots,tr-non-literal,unneeded-cast,' +
        'use-chrono-in-qtimer,' +
        # New in Clazy 1.11
        'unexpected-flag-enumerator-value,' +
        'use-arrow-operator-instead-of-data,' +
        # Checks Excluded from level2
        'no-copyable-polymorphic,no-qstring-allocations'

    & 'E:\dotfiles\bin\run-clazy-standalone.ps1' `
        -checks="$Script:Checks" `
        -extra-arg-before='-Qunused-arguments' -header-filter='(orm|tom|migrations)/.+\.(h|hpp)$' `
        -j $Script:numberOfProcesses `
        -p="$BuildPath" $Script:RegEx
}

Pop-Location
