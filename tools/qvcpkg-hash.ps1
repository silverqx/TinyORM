#!/usr/bin/env pwsh

[CmdletBinding(DefaultParameterSetName = 'Ref')]
Param(
    [Parameter(Position = 0, ParameterSetName = 'Branch', ValueFromPipeline,
        ValueFromPipelineByPropertyName,
        HelpMessage = 'Specifies a branch for which to download the package archive (works with ' +
            'a commit ID too).')]
    [ValidateNotNullOrEmpty()]
    [ArgumentCompletions('develop', 'main')]
    [string] $Branch = 'main',

    [Parameter(Position = 0, Mandatory, ParameterSetName = 'Tag', ValueFromPipeline,
        ValueFromPipelineByPropertyName,
        HelpMessage = 'Specifies a tag for which to download the package archive.')]
    [ValidateNotNullOrEmpty()]
    [string] $Tag,

    [Parameter(Position = 0, Mandatory, ParameterSetName = 'Commit', ValueFromPipeline,
        ValueFromPipelineByPropertyName,
        HelpMessage = 'Specifies a commit ID for which to download the package archive.')]
    [ValidateNotNullOrEmpty()]
    [ValidatePattern('^[a-fA-F0-9]{40}$',
        ErrorMessage = 'The argument "{0}" is not the correct commit ID (SHA-1). ' +
            'The argument "{0}" does not match the "{1}" pattern.')]
    [string] $Commit,

    [Parameter(Position = 0, Mandatory, ParameterSetName = 'Ref', ValueFromPipeline,
        ValueFromPipelineByPropertyName,
        HelpMessage = 'Specifies a git object, it can be the tag, commit ID, or branch.')]
    [ValidateNotNullOrEmpty()]
    [ArgumentCompletions('develop', 'main')]
    [string] $Ref
)

Set-StrictMode -Version 3.0

Get-VcpkgHash.ps1 -Project silverqx/TinyORM @PSBoundParameters
