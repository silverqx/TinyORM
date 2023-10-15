#!/usr/bin/env pwsh

[CmdletBinding(DefaultParameterSetName = 'Ref')]
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
    [string] $Ref,

    [Parameter(HelpMessage = 'Specifies how many times PowerShell retries a connection when ' +
        'a failure occurs.')]
    [ValidateNotNull()]
    [int] $MaximumRetryCount,

    [Parameter(HelpMessage = 'Specifies the interval between retries for the connection when ' +
        'a failure occurs.')]
    [ValidateNotNull()]
    [int] $RetryIntervalSec = 5,

    [Parameter(HelpMessage = 'Specifies how long the request can be pending before it times out.')]
    [ValidateNotNull()]
    [int] $TimeoutSec = 0
)

Set-StrictMode -Version 3.0

. $PSScriptRoot\private\Common-Host.ps1

# Get a git object by the current parameter set
# The vcpkg uses the archive/ prefix only without the refs/heads/ or refs/tags/:
# https://github.com/microsoft/vcpkg/blob/master/scripts/cmake/vcpkg_from_github.cmake#L107
# It doesn't matter if the refs/heads/ or refs/tags/ will be prepended, it returns the same SHA-512
# hashes if the archive/ without these refs/ is used.
# Most important are tags and commit IDs because these are used in the vcpkg_from_github REF option.
# Even if the commit ID, tag, or branch refer to the same commit they return different SHA-512 hash.
# I leave these refs/heads/ and refs/tags/ prepended because it's more accurate and the results
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

$url = "https://github.com/${Project}/archive/${currentObject}.tar.gz"

try {
    $retries = [ordered] @{
        RetryIntervalSec = $RetryIntervalSec
        TimeoutSec       = $TimeoutSec
    }
    if ($PSBoundParameters.ContainsKey('MaximumRetryCount')) {
        $retries.Insert(0, 'MaximumRetryCount', $MaximumRetryCount)
    }

    Invoke-WebRequest -Uri $url -OutFile $tempFile @retries

    vcpkg hash $tempFile
}
catch {
    # Exiting directly from the catch block is ok, the finally block will be invoked correctly
    Write-ExitError "Failed to download the archive for hashing from the URL:`n$url"
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

  Prints error message to the error stream and exits with the error code 1 if the download fails.

 .Parameter Project
  Specifies the GitHub project (username/project).

 .Parameter Branch
  Specifies a branch for which to download the package archive (works with a commit ID too).

 .Parameter Tag
  Specifies a tag for which to download the package archive.

 .Parameter Commit
  Specifies a commit ID for which to download the package archive.

 .Parameter Ref
  Specifies a git object, it can be the tag, commit ID, or branch.

 .Parameter MaximumRetryCount
  Specifies how many times PowerShell retries a connection when a failure occurs.

 .Parameter RetryIntervalSec
  Specifies the interval between retries for the connection when a failure occurs.

 .Parameter TimeoutSec
  Specifies how long the request can be pending before it times out.

 .INPUTS
  System.String
    You can pipe a branch for which to download the package archive and compute the SHA-512 hash
    (works with a commit ID too).

 .OUTPUTS
  The computed SHA-512 hash for the given git object (branch or ref).
#>
