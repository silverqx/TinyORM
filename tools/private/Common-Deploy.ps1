#!/usr/bin/env pwsh

Set-StrictMode -Version 3.0

# This file contains some functions for deploy.ps1 script, they were extracted because they are
# reused in the vcpkg-Windows.yml and vcpkg-Linux.yml GitHub actions.

# Common functions
# ---

. $PSScriptRoot\Common-Host.ps1

# Exit if the $LASTEXITCODE isn't 0
function Test-LastExitCode {
    [OutputType([void])]
    Param()

    if ($LASTEXITCODE -eq 0) {
        return
    }

    Write-ExitError "The last command failed `$LASTEXITCODE was $LASTEXITCODE"
}

# Verify that the number of expected occurrences of version or unit tests numbers lines is correct
function Test-ExpectedLinesCount {
    [OutputType([void])]
    Param(
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [int]    $LinesCount,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [int]    $ExpectedOccurrences,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $RegEx,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $FilePath,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $Title
    )

    # Nothing to do
    if ($LinesCount -eq $ExpectedOccurrences) {
        return
    }

    Write-ExitError ("Found '$LinesCount' $Title lines in the '$FilePath' file, " +
        "expected occurrences must be '$ExpectedOccurrences' for the following RegEx: $RegEx")
}

# Vcpkg - update REF and SHA512 functions
# ---

# Get the origin/main commit ID (SHA-1)
function Get-VcpkgRef {
    [OutputType([string])]
    Param()

    Write-Progress 'Obtaining the origin/main commit ID (SHA-1)...'

    return (git rev-parse --verify origin/main)
}

# Verify if the portfile.cmake file contains the REF and SHA512 lines
function Test-RefAndHashLinesCountForVcpkg {
    [OutputType([void])]
    Param(
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [int]    $MatchedLinesCount,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [int]    $ExpectedOccurrences,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $RegEx,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $PortfilePath
    )

    # Nothing to do
    if ($MatchedLinesCount -eq $ExpectedOccurrences) {
        return
    }

    throw "Found '$MatchedLinesCount' hash lines for '$RegEx' regex in the '$PortfilePath' file, " +
        "expected occurrences must be '$ExpectedOccurrences'."
}

# Verify if the REF and SHA512 were correctly replaced in the given portfile
function Test-VcpkgRefAndHashReplaced {
    [OutputType([void])]
    Param(
        [Parameter(Mandatory, HelpMessage = 'Specifies the portfile.cmake filepath to update.')]
        [ValidateNotNullOrEmpty()]
        [string] $PortFile,

        [Parameter(Mandatory, HelpMessage = 'Specifies the REF in the portfile.cmake to update.')]
        [ValidateNotNullOrEmpty()]
        [string] $Ref,

        [Parameter(Mandatory,
            HelpMessage = 'Specifies the SHA512 hash in the portfile.cmake to update.')]
        [ValidateNotNullOrEmpty()]
        [string] $Hash
    )

    $refEscaped = [regex]::Escape($Ref)
    $hashEscaped = [regex]::Escape($Hash)

    $regexRef = "^(?:    REF )(?:${refEscaped})$"
    $regexHash = "^(?:    SHA512 )(?:${hashEscaped})$"
    $regexMatch = "$regexRef|$regexHash"

    $fileContent = Get-Content -Path $portfilePath

    $matchedLines = $fileContent -cmatch $regexMatch

    # Verify if the portfile.cmake file contains the REF and SHA512 lines
    $expectedOccurrences = 2
    Test-RefAndHashLinesCountForVcpkg `
        $matchedLines.Count $expectedOccurrences $regexMatch $portfilePath
}

# Update the REF and SHA512 in tinyorm and tinyorm-qt5 portfiles
function Edit-VcpkgRefAndHash {
    [OutputType([void])]
    Param(
        [Parameter(Mandatory,
            HelpMessage = 'Specifies the GitHub project, it will be used to obtain the SHA512 ' +
                'hash using the Get-VcpkgHash.ps1 script (expected format is xyz/project).')]
        [ValidateNotNullOrEmpty()]
        [string] $Project,

        [Parameter(Mandatory, HelpMessage = 'Specifies the REF in the portfile.cmake to update.')]
        [ValidateNotNullOrEmpty()]
        [string] $Ref,

        [Parameter(Mandatory, HelpMessage = 'Specifies the portfile.cmake filepath to update.')]
        [ValidateNotNullOrEmpty()]
        [string[]] $PortFile,

        [Parameter(HelpMessage = 'Specifies whether to enable retries for the Invoke-WebRequest.')]
        [switch] $EnableRetries = $false
    )

    Write-Progress "Obtaining the origin/$Ref archive hash (SHA512)..."

    # Allow to control this because I don't want to enable it during the deploy.ps1 as it extends
    # execution time if a hash isn't correct to 30 seconds but must be enabled on the self-hosted
    # runner because there are many request timeouts during peak times.
    $retries = [ordered] @{}
    if ($EnableRetries) {
        $retries = [ordered] @{
            MaximumRetryCount = 3
            RetryIntervalSec  = 10
        }
    }

    $toolsFolder = Resolve-Path -Path $PSScriptRoot\..
    $vcpkgHash =
        & "$toolsFolder\Get-VcpkgHash.ps1" -Project $Project -Ref $Ref -TimeoutSec 15 @retries
    Test-LastExitCode

    Write-Output $vcpkgHash

    foreach ($portfilePath in $PortFile) {
        $regexRef = '^(?<ref>    REF )(?:[0-9a-f]{40}|v\d+\.\d+\.\d+|[\w\d-_\/]+)$'
        $regexHash = '^(?<sha512>    SHA512 )(?:[0-9a-f]{128})$'
        $regexMatch = "$regexRef|$regexHash"

        $fileContent = Get-Content -Path $portfilePath

        $matchedLines = $fileContent -cmatch $regexMatch

        # Verify if the portfile.cmake file contains the REF and SHA512 lines
        $expectedOccurrences = 2
        Test-RefAndHashLinesCountForVcpkg `
            $matchedLines.Count $expectedOccurrences $regexMatch $portfilePath

        # Replace the old REF AND SHA512 values with the new values in the portfile.cmake
        $fileContentReplaced = $fileContent -creplace $regexRef,  "`${ref}$Ref" `
                                            -creplace $regexHash, "`${sha512}$vcpkgHash"

        # Save to the file
        ($fileContentReplaced -join "`n") + "`n" | Set-Content -Path $portfilePath -NoNewline

        # Verify if the REF and SHA512 were correctly replaced in the given portfile
        Test-VcpkgRefAndHashReplaced -PortFile $portfilePath -Ref $Ref -Hash $vcpkgHash
    }
}
