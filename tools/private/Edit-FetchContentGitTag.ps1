#!/usr/bin/env pwsh

Param(
    [Parameter(Mandatory, HelpMessage = 'Specifies the CMakeLists.txt filepath to update.')]
    [ValidateNotNullOrEmpty()]
    [string] $CMakeLists,

    [Parameter(Mandatory, HelpMessage = 'Specifies the FetchContent GIT_TAG to update.')]
    [ValidateNotNullOrEmpty()]
    [string] $GitTag
)

Set-StrictMode -Version 3.0

# Used by the vcpkg-Windows.yml and vcpkg-Linux.yml workflow

# Common functions
# ---

. $PSScriptRoot\Common-Host.ps1
. $PSScriptRoot\Common-Deploy.ps1

# Main section
# ---

Write-Progress 'Updating the FetchContent GIT_TAG in the CMakeLists.txt file...'

$regex = '^(?<before> +GIT_TAG +)(?:origin/main)$'

$cmakeListsPath = Resolve-Path -Path $CMakeLists
$fileContent    = Get-Content -Path $cmakeListsPath

# Obtain the GIT_TAG line
$gitTagLine = $fileContent -cmatch $regex

# Verify that we found exactly one GIT_TAG line
$expectedOccurrences = 1
Test-ExpectedLinesCount $gitTagLine.Count $expectedOccurrences $regex $cmakeListsPath 'GIT_TAG'

# Replace the origin/main with the given branch branch, it can be origin/silverqx-develop or
# origin/main
$fileContentReplaced = $fileContent -creplace $regex, "`${before}$GitTag"

# Save to the file
($fileContentReplaced -join "`n") + "`n" | Set-Content -Path $cmakeListsPath -NoNewline

<#
 .Synopsis
  Update the FetchContent GIT_TAG in the given CMakeLists.txt

 .Parameter CMakeLists
  Specifies the CMakeLists.txt filepath to update.

 .Parameter GitTag
  Specifies the FetchContent GIT_TAG to update.

 .INPUTS
  Nothing.

 .OUTPUTS
  Nothing.
#>
