#!/usr/bin/env pwsh

Param(
    [Parameter(HelpMessage = 'Specifies the parent qmake build trees path.')]
    [ValidateNotNullOrEmpty()]
    [string] $QMakeBuildTreesPath = 'E:\c\qMedia\TinyORM\TinyORM-builds-qmake',

    [Parameter(HelpMessage = 'Specifies the Qt version for which to deploy executables.')]
    [ValidateNotNullOrEmpty()]
    [ValidatePattern('^[5-9]\.\d{1,2}\.\d{1,2}$',
        ErrorMessage = 'The argument "{0}" is not the correct Qt version number. ' +
        'The argument "{0}" does not match the "{1}" pattern.')]
    [string] $QtVersion = '6.7.2',

    [Parameter(HelpMessage = 'Specifies the qmake build type for which to deploy executables.')]
    [ValidateNotNullOrEmpty()]
    [ValidateSet('Profile', 'Release')]
    [string] $BuildType = 'Release',

    [Parameter(HelpMessage = 'Specifies the destination deploy path.')]
    [ValidateNotNullOrEmpty()]
    [string] $DestinationPath = 'E:\dotfiles\bin',

    [Parameter(HelpMessage = 'Specifies the major version number for TinyORM libraries.')]
    [ValidateNotNull()]
    [uint] $MajorVersion = 0
)

Set-StrictMode -Version 3.0

qsdb-deploy.ps1 `
    -BuildTreesPath $QMakeBuildTreesPath `
    -QtVersion $QtVersion `
    -BuildType $BuildType `
    -DestinationPath $DestinationPath `
    -MajorVersion $MajorVersion `
    -BuildSystem qmake
