#!/usr/bin/env pwsh

[CmdletBinding(DefaultParameterSetName = 'Branch')]
Param(
    [Parameter(Position = 0, Mandatory,
        HelpMessage = 'Specifies the GitHub project (username/project).')]
    [ValidateNotNullOrEmpty()]
    [string] $Project,

    [Parameter(Position = 1, ParameterSetName = 'Branch', ValueFromPipeline,
        ValueFromPipelineByPropertyName,
        HelpMessage = 'Specifies a branch for which to download the package archive (works with ' +
            'a commit ID too).')]
    [ValidateNotNullOrEmpty()]
    [string] $Branch = 'main',

    [Parameter(Position = 1, Mandatory, ParameterSetName = 'Ref', ValueFromPipelineByPropertyName,
        HelpMessage = 'Specifies a commit ID for which to download the package archive.')]
    [ValidateNotNullOrEmpty()]
    [ValidatePattern('^[a-fA-F0-9]{40}$',
        ErrorMessage = 'The argument "{0}" is not the correct commit ID (SHA-1). ' +
            'The argument "{0}" does not match the "{1}" pattern.')]
    [string] $Ref
)

Set-StrictMode -Version 3.0

# Get a git object by the current parameter set
$currentObject = $PsCmdlet.ParameterSetName -eq 'Ref' ? $Ref : $Branch

# Create a new temporary file
$tempFile = New-TemporaryFile

# Turn off the Invoke-WebRequest progress output
$previousProgressPreference = $ProgressPreference
$ProgressPreference = 'SilentlyContinue'

try {
    Invoke-WebRequest "https://github.com/${Project}/archive/${currentObject}.tar.gz" -OutFile $tempFile
}
finally {
    $ProgressPreference = $previousProgressPreference
}

vcpkg hash $tempFile

Remove-Item -Path $tempFile

<#
 .Synopsis
  Computes the SHA-512 hash of a specified branch or git reference object

 .Description
  Computes the hash of a specified branch or git reference object. By default, it computes
  the SHA-512 hash. The output will be the computed hash for the given git object displayed directly
  on the console.

 .INPUTS
  System.String
    You can pipe a branch for which to download the package archive and compute the SHA-512 hash
    (works with a commit ID too).

 .OUTPUTS
  The computed SHA-512 hash for the given git object (branch or ref).
#>
