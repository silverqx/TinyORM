#!/usr/bin/env pwsh

Param(
    [Parameter(Position = 0,
        HelpMessage = 'Specifies the commit ID to download a package archive.')]
    [ValidateNotNullOrEmpty()]
    [ValidatePattern('^[a-fA-F0-9]{40}$',
        ErrorMessage = 'The argument "{0}" is not the correct commit ID (SHA-1). ' +
        'The argument "{0}" does not match the "{1}" pattern.')]
    [string] $Ref
)

Set-StrictMode -Version 3.0

Get-VcpkgHash.ps1 -Project silverqx/TinyORM @PSBoundParameters
