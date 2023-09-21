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

    [Parameter(Position = 1, Mandatory, ParameterSetName = 'Tag', ValueFromPipeline,
        ValueFromPipelineByPropertyName,
        HelpMessage = 'Specifies a tag for which to download the package archive.')]
    [ValidateNotNullOrEmpty()]
    [string] $Tag,

    [Parameter(Position = 1, Mandatory, ParameterSetName = 'Commit', ValueFromPipeline,
        ValueFromPipelineByPropertyName,
        HelpMessage = 'Specifies a commit ID for which to download the package archive.')]
    [ValidateNotNullOrEmpty()]
    [ValidatePattern('^[a-fA-F0-9]{40}$',
        ErrorMessage = 'The argument "{0}" is not the correct commit ID (SHA-1). ' +
            'The argument "{0}" does not match the "{1}" pattern.')]
    [string] $Commit,

    [Parameter(Position = 1, Mandatory, ParameterSetName = 'Ref', ValueFromPipeline,
        ValueFromPipelineByPropertyName,
        HelpMessage = 'Specifies a git object, it can be the tag, commit ID, or branch.')]
    [ValidateNotNullOrEmpty()]
    [string] $Ref
)

Set-StrictMode -Version 3.0

# Get a git object by the current parameter set
# The vcpkg uses the archive/ prefix only without the refs/heads/ or refs/tags/:
# https://github.com/microsoft/vcpkg/blob/master/scripts/cmake/vcpkg_from_github.cmake#L107
# It doesn't matter if the refs/heads/ or refs/tags/ will be prepended, it returns the same SHA-512
# hashes if the archive/ without these refs/ is used.
# Most important are tags and commit IDs because these are used in the vcpkg_from_github REF option.
# Even if the commit ID, tag, or branch refer to the same commit they return different SHA-512 hash.
# So I leave these refs/heads/ and refs/tags/ prepended because it's more accurate and the results
# are always the same as the vcpkg_from_github generates.
switch ($PsCmdlet.ParameterSetName) {
    'Branch' { $currentObject = "refs/heads/$Branch" }
    'Tag'    { $currentObject = "refs/tags/$Tag" }
    'Commit' { $currentObject = $Commit }
    'Ref'    { $currentObject = $Ref }
    Default {
        throw 'Unreachable code.'
    }
}

# Create a new temporary file
$tempFile = New-TemporaryFile

# Turn off the Invoke-WebRequest progress output
$previousProgressPreference = $ProgressPreference
$ProgressPreference = 'SilentlyContinue'

try {
    Invoke-WebRequest "https://github.com/${Project}/archive/${currentObject}.tar.gz" -OutFile $tempFile

    vcpkg hash $tempFile
}
finally {
    $ProgressPreference = $previousProgressPreference

    Remove-Item -Path $tempFile
}

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
