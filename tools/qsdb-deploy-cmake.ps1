#!/usr/bin/env pwsh

Param(
    [Parameter(HelpMessage = 'Specifies the parent CMake build trees path.')]
    [ValidateNotNullOrEmpty()]
    [string] $CMakeBuildTreesPath = 'E:\c\qMedia\TinyORM\TinyORM-builds-cmake',

    [Parameter(HelpMessage = 'Specifies the Qt version for which to deploy executables.')]
    [ValidateNotNullOrEmpty()]
    [ValidatePattern('^[5-9]\.\d{1,2}\.\d{1,2}$',
        ErrorMessage = 'The argument "{0}" is not the correct Qt version number. ' +
        'The argument "{0}" does not match the "{1}" pattern.')]
    [string] $QtVersion = '6.7.2',

    [Parameter(HelpMessage = 'Specifies the CMake build type for which to deploy executables.')]
    [ValidateNotNullOrEmpty()]
    [ValidateSet('Release', 'RelWithDebInfo', 'MinSizeRel')]
    [string] $BuildType = 'RelWithDebInfo',

    [Parameter(HelpMessage = 'Specifies the destination deploy path.')]
    [ValidateNotNullOrEmpty()]
    [string] $DestinationPath = 'E:\dotfiles\bin',

    [Parameter(HelpMessage = 'Specifies the major version number for TinyORM libraries.')]
    [ValidateNotNull()]
    [uint] $MajorVersion = 0
)

Set-StrictMode -Version 3.0

qsdb-deploy.ps1 `
    -BuildTreesPath $CMakeBuildTreesPath `
    -QtVersion $QtVersion `
    -BuildType $BuildType `
    -DestinationPath $DestinationPath `
    -MajorVersion $MajorVersion `
    -BuildSystem CMake
