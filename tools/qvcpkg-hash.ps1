#!/usr/bin/env pwsh

[CmdletBinding(DefaultParameterSetName = 'Branch')]
Param(
    [Parameter(Position = 1, ParameterSetName = 'Branch', ValueFromPipeline,
        ValueFromPipelineByPropertyName,
        HelpMessage = 'Specifies the branch for which to download a package archive (also ' +
        'works with a commit ID).')]
    [ValidateNotNullOrEmpty()]
    [string] $Branch = 'main',

    [Parameter(Position = 1, Mandatory, ParameterSetName = 'Ref', ValueFromPipelineByPropertyName,
        HelpMessage = 'Specifies the commit ID for which to download a package archive.')]
    [ValidateNotNullOrEmpty()]
    [ValidatePattern('^[a-fA-F0-9]{40}$',
        ErrorMessage = 'The argument "{0}" is not the correct commit ID (SHA-1). ' +
        'The argument "{0}" does not match the "{1}" pattern.')]
    [string] $Ref
)

Set-StrictMode -Version 3.0

Get-VcpkgHash.ps1 -Project silverqx/TinyORM @PSBoundParameters
