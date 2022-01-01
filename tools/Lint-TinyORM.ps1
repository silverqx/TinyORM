#!/usr/bin/env pwsh

Param(
    [Parameter(Position = 0, Mandatory = $false,
        HelpMessage = 'Specifies the path to the cmake build folder, is pwd by default.')]
    [ValidateNotNullOrEmpty()]
    [string] $BuildPath = $($(Get-Location).Path),

    [Parameter(HelpMessage = 'Skip Clang Tidy analyzes.')]
    [switch] $SkipClangTidy,

    [Parameter(HelpMessage = 'Skip Clazy standalone analyzes.')]
    [switch] $SkipClazy
)

Set-StrictMode -Version 3.0

$Script:RegEx = '(?:src|tests)[\\\/]+.+?[\\\/]+(?!mocs_)[\w_\-\+]+\.cpp$'

Push-Location

Set-Location -Path $BuildPath

# Initialize build environment if it's not already there
if (-not (Test-Path env:WindowsSDKLibVersion)) {
    . E:\dotfiles\bin\qtenv5.ps1
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
        -p="$BuildPath" $Script:RegEx
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
        'qstring-varargs,qt6-fwd-fixes,qt6-header-fixes,qt6-qhash-signature,' +
        'raw-environment-function,reserve-candidates,signal-with-return-value,thread-with-slots,' +
        'tr-non-literal,' +
        # Will be added in Clazy 1.11
        # 'unexpected-flag-enumerator-value,'
        'unneeded-cast,' +
        # Will be added in Clazy 1.11
        # 'use-arrow-operator-instead-of-data,'
        'use-chrono-in-qtimer,' +
        # Checks Excluded from level2
        'no-copyable-polymorphic,no-ctor-missing-parent-argument,no-function-args-by-ref,' +
        'no-qstring-allocations,no-rule-of-three'

    & 'E:\dotfiles\bin\run-clazy-standalone.ps1' `
        -checks="$Script:Checks" `
        -extra-arg-before='-Qunused-arguments' -header-filter='orm/.+\.(h|hpp)$' `
        -p="$BuildPath" $Script:RegEx
}

Pop-Location
