#!/usr/bin/env pwsh

Set-StrictMode -Version 3.0

# Script variables section
# ---

# Determine which version number will be bumped
enum BumpType {
    Major
    Minor
    Bugfix
    None
}

# Version number format (with or without the v character prefix)
enum VersionType {
    VersionOnly
    VersionWith_v
}

# Bumping hash data (contains everything what is needed to bump the version numbers)
$Script:BumpsHash = $null
# Files in which the version numbers needs to be bumped (integer value is the number of updates)
$Script:VersionLocations = $null
# Vcpkg port filepaths for tinyorm and tinyorm-qt5 ports
$Script:VcpkgLocations = $null

# Functions section
# ---

. $PSScriptRoot\private\Common-Host.ps1

# Initialize all script variables that contain the Resolve-Path call
function Initialize-ScriptVariables {

    $Script:BumpsHash = [ordered] @{
        TinyORM   = @{type               = [BumpType]::None
                      versionOld         = $null
                      versionBumped      = $null
                      versionOldArray    = $null
                      versionBumpedArray = $null
                      versionHpp         = Resolve-Path -Path ./include/orm/version.hpp
                      macroPrefix        = 'TINYORM_VERSION_'}

        tom       = @{type               = [BumpType]::None
                      versionOld         = $null
                      versionBumped      = $null
                      versionOldArray    = $null
                      versionBumpedArray = $null
                      versionHpp         = Resolve-Path -Path ./tom/include/tom/version.hpp
                      macroPrefix        = 'TINYTOM_VERSION_'}

        TinyUtils = @{type               = [BumpType]::None
                      versionOld         = $null
                      versionBumped      = $null
                      versionOldArray    = $null
                      versionBumpedArray = $null
                      versionHpp         = Resolve-Path -Path ./tests/TinyUtils/src/version.hpp
                      macroPrefix        = 'TINYUTILS_VERSION_'}
    }

    $Script:VersionLocations = [ordered] @{
        TinyORM = @{
            VersionOnly = [ordered] @{
                (Resolve-Path -Path ./NOTES.txt)                                = 1
                (Resolve-Path -Path ./cmake/vcpkg/ports/tinyorm/vcpkg.json)     = 1
                (Resolve-Path -Path ./cmake/vcpkg/ports/tinyorm-qt5/vcpkg.json) = 1
                (Resolve-Path -Path ./docs/building/hello-world.mdx)            = 2
            }
            [VersionType]::VersionWith_v = [ordered] @{
                (Resolve-Path -Path ./NOTES.txt)                 = 1
                (Resolve-Path -Path ./README.md)                 = 2
                (Resolve-Path -Path ./docs/README.mdx)           = 1
                (Resolve-Path -Path ./docs/building/tinyorm.mdx) = 1
            }
        }

        tom = @{
            [VersionType]::VersionWith_v = [ordered] @{
                (Resolve-Path -Path ./NOTES.txt)       = 1
                (Resolve-Path -Path ./README.md)       = 2
                (Resolve-Path -Path ./docs/README.mdx) = 1
            }
        }
        # TinyUtils doesn't have any version numbers in files
    }

    $Script:VcpkgLocations = [ordered] @{
        tinyorm       = @{
            portfile  = Resolve-Path -Path ./cmake/vcpkg/ports/tinyorm/portfile.cmake
            vcpkgJson = Resolve-Path -Path ./cmake/vcpkg/ports/tinyorm/vcpkg.json
        }

        'tinyorm-qt5' = @{
            portfile  = Resolve-Path -Path ./cmake/vcpkg/ports/tinyorm-qt5/portfile.cmake
            vcpkgJson = Resolve-Path -Path ./cmake/vcpkg/ports/tinyorm-qt5/vcpkg.json
        }
    }
}

# Determine whether the deploy.ps1 script was executed from the TinyORM root folder (contains
# the .git/ folder)
function Test-GitRoot {
    [OutputType([void])]
    Param()

    Write-Progress 'Testing .git root...'

    if ((Test-Path .\.git) -and (Test-Path .\include\orm\version.hpp) -and
        (Split-Path -Path $PSScriptRoot -Leaf) -ceq 'tools'
    ) {
        return
    }

    Write-ExitError ('The deploy.ps1 script must be executed from the TinyORM root folder ' +
        '(.\tools\deploy.ps1)')
}

# Determine whether the current branch is develop
function Test-GitDevelopBranch {
    [OutputType([void])]
    Param()

    Write-Progress 'Testing the develop branch...'

    if ((git branch --show-current) -ceq 'develop') {
        return
    }

    Write-ExitError 'The TinyORM current branch must be ''develop'''
}

# Determine whether the main or develop branches are behind the origin
function Test-GitBehindOrigin {
    [OutputType([void])]
    Param()

    Write-Progress 'Testing if the main and develop branches are behind the origin...'

    $developBehind = git rev-list --count develop..origin/develop
    if ($developBehind -ne 0) {
        Write-ExitError ("The TinyORM 'develop' branch is $developBehind commits behind " +
            'the origin/develop')
    }

    $mainBehind = git rev-list --count main..origin/main
    if ($mainBehind -ne 0) {
        Write-ExitError "The TinyORM 'main' branch is $mainBehind commits behind the origin/main"
    }

}

# Determine whether the TinyORM Git working tree contains any modifications
function Test-WorkingTreeClean {
    [OutputType([void])]
    Param()

    Write-Progress 'Testing clean working tree...'

    if ($null -eq (git status --porcelain --untracked-files=normal)) {
        return
    }

    Write-ExitError 'The TinyORM working tree must be clean'
}

# Exit if the $LASTEXITCODE isn't 0
function Test-LastExitCode {
    [OutputType([void])]
    Param()

    if ($LASTEXITCODE -eq 0) {
        return
    }

    Write-ExitError "The last command failed `$LASTEXITCODE was $LASTEXITCODE"
}

# Reads the bump type to do for the given library
function Read-BumpType {
    [OutputType([BumpType])]
    Param(
        [Parameter(Mandatory,
            HelpMessage = 'Specifies the library name for which to bump the version number.')]
        [ValidateNotNullOrEmpty()]
        [string]
        $Name
    )

    $versionTypeChoice = [System.Management.Automation.Host.ChoiceDescription[]](@(
        New-Object System.Management.Automation.Host.ChoiceDescription('M&ajor', 'Major version')
        New-Object System.Management.Automation.Host.ChoiceDescription('&Minor', 'Minor version')
        New-Object System.Management.Automation.Host.ChoiceDescription('&Bugfix', 'Bugfix version')
        New-Object System.Management.Automation.Host.ChoiceDescription('&None', 'Don''t bump')
    ))

    $answer = $Host.Ui.PromptForChoice("Bump `e[32m$Name`e[0m", 'Choose the version number to bump',
                                       $versionTypeChoice, 3)

    # Convert to the real [BumpType] type
    return [Enum]::ToObject([BumpType], $answer)
}

# Read version numbers from version.hpp files and initialize the $Script:BumpsHash
function Read-VersionNumbers {
    [OutputType([void])]
    Param()

    foreach ($bumpRow in $Script:BumpsHash.GetEnumerator()) {
        $bumpValue = $bumpRow.Value

        # Nothing to bump
        if ($bumpValue.type -eq [BumpType]::None) {
            continue
        }

        $versionHppPath = $bumpValue.versionHpp
        $macroPrefix    = $bumpValue.macroPrefix

        # Obtain all C macros with version numbers
        # No Test-Path check needed as version.hpp filepaths were passed using the Resolve-Path
        $versionMacros = Get-Content -Path $versionHppPath
            | Where-Object {
                $_ -cmatch "^#define $macroPrefix(?:MAJOR|MINOR|BUGFIX) (?:\d+)$"
            }
        # Verify
        $versionMacrosLength = $versionMacros.Length
        if ($versionMacrosLength -ne 3) {
            throw "Found '$versionMacrosLength' version C macros for '$macroPrefix' " +
                "in the '$versionHppPath' file, they must be '3' (major, minor, and bugfix)."
        }

        # Obtain version numbers
        $version = $versionMacros | ForEach-Object {
            $_ -cmatch "^#define $macroPrefix(?:MAJOR|MINOR|BUGFIX) (?<version>\d+)$" | Out-Null
            [int] $Matches.version
        }
        # Verify
        $versionLength = $version.Length
        if ($versionLength -ne 3) {
            throw "Found '$versionLength' version numbers for '$macroPrefix' " +
                "in the '$versionHppPath' file, they must be '3' (major, minor, and bugfix)."
        }

        $bumpValue.versionOldArray = $version
    }
}

# Determine whether all bump types were set to the None value (don't bump)
function Test-AllBumpsEmpty {
    [OutputType([void])]
    Param()

    NewLine
    Write-Progress 'Testing if all bump types are empty...'

    if ($Script:BumpsHash.TinyORM.type   -ne [BumpType]::None -or
        $Script:BumpsHash.tom.type       -ne [BumpType]::None -or
        $Script:BumpsHash.TinyUtils.type -ne [BumpType]::None
    ) {
        return
    }

    Write-ExitError 'Nothing to bump, all bump types were set to don''t bump'
}

# Bump version numbers by the chosen bump type
function Update-VersionNumbers {
    [OutputType([void])]
    Param()

    Write-Progress 'Bumping version numbers...'

    foreach ($bumpRow in $Script:BumpsHash.GetEnumerator()) {
        $bumpValue = $bumpRow.Value
        $bumpType  = $bumpValue.type

        # Nothing to update
        if ($bumpType -eq [BumpType]::None) {
            continue
        }

        $versionOldArray    = $bumpValue.versionOldArray
        $versionBumpedArray = $versionOldArray.Clone()

        # Bump the version number by the chosen bump type
        $versionBumpedArray[$bumpType.GetHashCode()] += 1

        # Reset the major and minor numbers to zero if necessary
        if ($bumpType -eq [BumpType]::Major -or $bumpType -eq [BumpType]::Minor) {
            $versionBumpedArray[[BumpType]::Bugfix] = 0
        }
        if ($bumpType -eq [BumpType]::Major) {
            $versionBumpedArray[[BumpType]::Minor] = 0
        }

        $bumpValue.versionOld         = $versionOldArray    -join '.'
        $bumpValue.versionBumped      = $versionBumpedArray -join '.'
        $bumpValue.versionBumpedArray = $versionBumpedArray
    }
}

# Get the longest size of the bumped version numbers
function Get-MaxVersionNumberLength {
    [OutputType([int])]
    Param()

    [int] $result = 0

    foreach ($bumpRow in $Script:BumpsHash.GetEnumerator()) {
        $bumpValue = $bumpRow.Value

        # Nothing to do
        if ($bumpValue.type -eq [BumpType]::None) {
            continue
        }

        $versionBumpedLength = $bumpValue.versionBumped.Length
        if ($versionBumpedLength -le $result) {
            continue
        }

        $result = $versionBumpedLength
    }

    return $result
}

# Display version numbers summary
function Show-VersionNumbers {
    [OutputType([void])]
    Param()

    Write-Progress 'Showing bumped version numbers...'
    NewLine

    foreach ($bumpRow in $Script:BumpsHash.GetEnumerator()) {
        $bumpValue = $bumpRow.Value

        $bumpNamePadded = $bumpRow.Name.PadRight(10)
        Write-Host "`e[32m$bumpNamePadded`e[0m" -NoNewline

        # Nothing to do, skipped version number
        if ($bumpValue.type -eq [BumpType]::None) {
            Write-Host 'Skipped' -ForegroundColor DarkRed
            continue
        }

        # Display the current/old and the bumped/new version number
        $versionOldPadding = Get-MaxVersionNumberLength + 1
        $versionOldPadded = $bumpValue.versionOld.PadRight($versionOldPadding)

        Write-Host "$versionOldPadded -> $($bumpValue.versionBumped)"
    }
}

# Update version numbers in version.hpp files
function Edit-VersionNumbersInVersionHpp {
    [OutputType([void])]
    Param()

    NewLine
    Write-Progress 'Editing bumped version numbers in version.hpp files...'

    $mapBumpTypeToMacro = @{
        [BumpType]::Major  = 'MAJOR'
        [BumpType]::Minor  = 'MINOR'
        [BumpType]::Bugfix = 'BUGFIX'
    }

    foreach ($bumpRow in $Script:BumpsHash.GetEnumerator()) {
        $bumpValue = $bumpRow.Value
        $bumpType  = $bumpValue.type

        # Nothing to edit
        if ($bumpType -eq [BumpType]::None) {
            continue
        }

        $versionHppPath      = $bumpValue.versionHpp
        $macroPrefix         = $bumpValue.macroPrefix
        $versionOldArray     = $bumpValue.versionOldArray
        $versionBumpedArray  = $bumpValue.versionBumpedArray
        $expectedOccurrences = 1

        $regex = "^(#define $macroPrefix$($mapBumpTypeToMacro[$bumpType]) )(?<version>\d+)$"

        $fileContent = Get-Content -Path $versionHppPath

        $versionLines = $fileContent -cmatch $regex

        # Verify that we found exactly one line
        $versionLinesLength = $versionLines.Length
        if ($versionLinesLength -ne $expectedOccurrences) {
            throw "Found '$versionLinesLength' version lines for '$regex' regex " +
                "in the '$versionHppPath' file, expected occurrences must be " +
                "'$expectedOccurrences'."
        }
        # Verify that the version number in the bumpHash is still the same
        $versionLines[0] -cmatch $regex | Out-Null
        $versionNumberNow = [int] $Matches.version
        $versionNumberOld = $versionOldArray[$bumpType.GetHashCode()]
        if ($versionNumberNow -ne $versionNumberOld) {
            throw "The '$versionNumberNow -ne $versionNumberOld' for '$regex' regex " +
                "in the '$versionHppPath' file."
        }

        # Replace the old version number with the bumped version number
        $versionNumberBumped = $versionBumpedArray[$bumpType.GetHashCode()]
        $fileContentReplaced = $fileContent -creplace $regex, "`${1}$($versionNumberBumped)"

        # Save to the file
        ($fileContentReplaced -join "`n") + "`n" | Set-Content -Path $versionHppPath -NoNewline
    }
}

# Update version numbers in all files for all libraries defined in the $Script:VersionLocations
function Edit-VersionNumbersInAllFiles {
    [OutputType([void])]
    Param()

    Write-Progress 'Editing bumped version numbers in all files...'

    foreach ($versionLocationsAll in $Script:VersionLocations.GetEnumerator()) {
        $bumpValue = $Script:BumpsHash[$versionLocationsAll.Name]

        # Nothing to edit
        if ($bumpValue.type -eq [BumpType]::None) {
            continue
        }

        foreach ($locations in $versionLocationsAll.Value.GetEnumerator()) {
            $regex = ''
            $versionType = $locations.Name
            $versionOld  = $bumpValue.versionOld

            # Prepend the v character to better target the replacement (be more accurate)
            if ($versionType -eq [VersionType]::VersionWith_v) {
                $versionOld = 'v' + $versionOld
            }
            # Can't precede the v character
            else {
                $regex += '(?<!v)'
            }

            $regex += "(?<version>$versionOld)" -replace '\.', '\.'

            foreach ($versionLocation in $locations.Value.GetEnumerator()) {
                $filePath = $versionLocation.Name
                $expectedOccurrences = $versionLocation.Value

                $fileContent = Get-Content -Path $filePath

                $versionLines = $fileContent -cmatch $regex

                # Verify that the number of expected occurrences of version numbers is correct
                $versionLinesLength = $versionLines.Length
                if ($versionLinesLength -ne $expectedOccurrences) {
                    throw "Found '$versionLinesLength' version number lines for '$regex' regex " +
                        "in the '$filePath' file, expected occurrences must be " +
                        "'$expectedOccurrences'."
                }

                $versionBumped = $bumpValue.versionBumped

                # Prepend the v character (follow/copy the old version number format)
                if ($versionType -eq [VersionType]::VersionWith_v) {
                    $versionBumped = 'v' + $versionBumped
                }

                # Replace the old version number with the bumped version number
                $fileContentReplaced = $fileContent -creplace $regex, $versionBumped

                # Save to the file
                ($fileContentReplaced -join "`n") + "`n" | Set-Content -Path $filePath -NoNewline
            }
        }
    }
}

# Display the diff summary and approve to continue
function Show-DiffSummaryAndApprove {
    [OutputType([void])]
    Param()

    Write-Progress 'Showing diff summary...'
    NewLine
    git --no-pager diff --compact-summary

    NewLine
    Write-Info 'Please check updated versions in SmartGit...'
    Approve-Continue -Exit
}

# Put together a commit message based on all bump types and version numbers
function Get-BumpCommitMessage {
    [OutputType([string])]
    Param()

    Write-Progress 'Generating the bump commit message...'

    $message = 'bump version to '

    $TinyORMBumpValue   = $Script:BumpsHash.TinyORM
    $tomBumpValue       = $Script:BumpsHash.tom
    $TinyUtilsBumpValue = $Script:BumpsHash.TinyUtils

    $isTinyORMBump   = $TinyORMBumpValue.type   -ne [BumpType]::None
    $isTomBump       = $tomBumpValue.type       -ne [BumpType]::None
    $isTinyUtilsBump = $TinyUtilsBumpValue.type -ne [BumpType]::None

    # TinyORM
    if ($isTinyORMBump) {
        $message += 'TinyORM v' + $TinyORMBumpValue.versionBumped
    }

    # tom
    if ($isTomBump) {
        # Prepend the and word if necessary
        if ($isTinyORMBump) {
            $message += ' and '
        }
        $message += 'tom v' + $tomBumpValue.versionBumped
    }

    # TinyUtils
    if ($isTinyUtilsBump) {
        # Append the TinyUtils version to the commit message description
        if ($isTinyORMBump) {
            $message += "`n`nAlso bumped to TinyUtils v$($TinyUtilsBumpValue.versionBumped)."
        }
        # Append the TinyUtils version to the commit message title
        else {
            # Prepend the and word if necessary
            if ($isTomBump) {
                $message += ' and '
            }
            $message += 'TinyUtils v' + $TinyUtilsBumpValue.versionBumped
        }
    }

    return $message
}

# Switch to the main branch, merge develop branch, and push to the origin/main branch
function Invoke-MergeDevelopAndDeploy {
    [OutputType([void])]
    Param()

    # Verify if the current branch is develop and the working tree is still clean
    NewLine
    Test-GitBehindOrigin
    Test-GitDevelopBranch
    Test-WorkingTreeClean

    NewLine
    Write-Progress 'Pushing to origin/develop branch...'
    git push --progress origin refs/heads/develop:refs/heads/develop
    Test-LastExitCode

    NewLine
    Write-Progress 'Switching to main branch...'
    git switch main
    Test-LastExitCode

    NewLine
    Write-Progress 'Merging develop branch...'
    git merge --ff-only develop
    Test-LastExitCode

    NewLine
    Write-Progress 'Pushing to origin/main branch...'
    git push --progress origin refs/heads/main:refs/heads/main
    Test-LastExitCode

    NewLine
    Write-Progress 'Switching back to develop branch...'
    git switch develop
    Test-LastExitCode
}

# Get the origin/main commit ID (SHA-1)
function Get-VcpkgRef {
    [OutputType([string])]
    Param()

    Write-Progress 'Obtaining the origin/main commit ID (SHA-1)...'

    return (git rev-parse --verify origin/main)
}

# Update the REF and SHA512 in tinyorm and tinyorm-qt5 portfiles
function Edit-VcpkgRefAndHash {
    [OutputType([void])]
    Param()

    $vcpkgRef = Get-VcpkgRef

    Write-Progress 'Obtaining the origin/main archive hash (SHA512)...'

    $vcpkgHash = Get-VcpkgHash -Project 'silverqx/TinyORM' -Branch 'main'

    foreach ($portfiles in $Script:VcpkgLocations.GetEnumerator()) {
        $regexRef   = '(?<ref>    REF )(?:[0-9a-f]{40})'
        $regexHash  = '(?<sha512>    SHA512 )(?:[0-9a-f]{128})'
        $regexMatch = "$regexRef|$regexHash"

        $portfilePath = $portfiles.Value.portfile
        $expectedOccurrences = 2

        $fileContent = Get-Content -Path $portfilePath

        $matchedLines = $fileContent -cmatch $regexMatch

        # Verify if the portfile.cmake file contains the REF and SHA512 lines
        $matchedLinesLength = $matchedLines.Length
        if ($matchedLinesLength -ne $expectedOccurrences) {
            throw "Found '$matchedLinesLength' hash lines for '$regexMatch' regex " +
                "in the '$portfilePath' file, expected occurrences must be " +
                "'$expectedOccurrences'."
        }

        # Replace the old REF AND SHA512 values with the new values in the portfile.cmake
        $fileContentReplaced = $fileContent -creplace $regexRef, "`${ref}$vcpkgRef" `
                                            -creplace $regexHash, "`${sha512}$vcpkgHash"

        # Save to the file
        ($fileContentReplaced -join "`n") + "`n" | Set-Content -Path $portfilePath -NoNewline
    }
}

# Remove the 'port-version' field from the vcpkg.json if needed
function Remove-PortVersion {
    [OutputType([string])]
    Param()

    Write-Progress 'Removing the port-version field from vcpkg.json...'

    # Nothing to do, the TinyORM version wasn't bumped
    if ($Script:BumpsHash.TinyORM.type -eq [BumpType]::None) {
        return
    }

    foreach ($portfiles in $Script:VcpkgLocations.GetEnumerator()) {
        $regex = '\s*?"port-version"\s*?:\s*?\d+\s*?,?\s*?'

        $vcpkgJsonPath = $portfiles.Value.vcpkgJson
        $expectedOccurrences = 1

        $fileContent = Get-Content -Path $vcpkgJsonPath

        $matchedLines = $fileContent -cmatch $regex

        # Verify if the vcpkg.json file contains the port-version line
        $matchedLinesLength = $matchedLines.Length
        if ($matchedLinesLength -ne $expectedOccurrences) {
            throw "Found '$matchedLinesLength' hash lines for '$regex' regex " +
            "in the '$vcpkgJsonPath' file, expected occurrences must be " +
            "'$expectedOccurrences'."
        }

        # Remove the port-version field from the vcpkg.json file
        $fileContentReplaced = $fileContent -creplace $regex, ''

        # Save to the file
        ($fileContentReplaced -join "`n") + "`n" | Set-Content -Path $vcpkgJsonPath -NoNewline
    }
}

# Get the vcpkg commit message (updated vcpkg REF and SHA512)
function Get-VcpkgCommitMessage {
    [OutputType([string])]
    Param()

    Write-Progress 'Generating the vcpkg commit message...'

    return "updated vcpkg REF and SHA512`n`n[skip ci]"
}

# Bump version numbers
function Invoke-BumpVersions {
    [OutputType([string])]
    Param()

    # Allow to skip bumping version numbers
    $answer = Approve-Continue -Message 'Do you want to bump version numbers?'
    if ($answer -eq 1) {
        return
    }

    # Need to initialize these variables later because of the Resolve-Path calls
    Initialize-ScriptVariables

    # Select which version numbers to bump
    $Script:BumpsHash.TinyORM.type   = Read-BumpType -Name TinyORM
    $Script:BumpsHash.tom.type       = Read-BumpType -Name tom
    $Script:BumpsHash.TinyUtils.type = Read-BumpType -Name TinyUtils

    Test-AllBumpsEmpty

    # Obtain version numbers from version.hpp files
    Read-VersionNumbers

    # Bump all version numbers
    Update-VersionNumbers
    Show-VersionNumbers

    # Update these version numbers in all files
    Edit-VersionNumbersInVersionHpp
    Edit-VersionNumbersInAllFiles

    Show-DiffSummaryAndApprove

    NewLine
    Write-Progress 'Committing bumped version numbers...'
    git commit --all --edit --message=$(Get-BumpCommitMessage)
    Test-LastExitCode

    # Merge to origin/main and push
    Invoke-MergeDevelopAndDeploy

    NewLine
    Write-Info 'TinyORM was bumped and deployed successfully. 🥳'
}

# Vcpkg ports update
function Invoke-UpdateVcpkgPorts {
    [OutputType([string])]
    Param()

    # Don't allow to skip updating the vcpkg ports because they must be always updated if merging
    # into the main branch

    Write-Header 'Updating vcpkg ports REF and SHA512'

    Edit-VcpkgRefAndHash
    Remove-PortVersion

    Show-DiffSummaryAndApprove

    NewLine
    Write-Progress 'Committing vcpkg REF and SHA512...'
    git commit --all --message=$(Get-VcpkgCommitMessage)
    Test-LastExitCode

    # Merge to origin/main and push
    Invoke-MergeDevelopAndDeploy

    NewLine
    Write-Info 'vcpkg ports were updated and deployed successfully. 🥳'
}

# Main section
# ---

Clear-Host
Write-Header 'Deploying TinORM library'

# Verify whether the current working tree is in the correct state (required preconditions)
Test-GitRoot
Test-GitBehindOrigin
Test-GitDevelopBranch
Test-WorkingTreeClean

Invoke-BumpVersions
Invoke-UpdateVcpkgPorts

<#
 .Synopsis
  The TinyORM library bumping version numbers and deployment script

 .Description
  The `deploy.ps1` script helps with bumping version numbers for all libraries and deploying these
  changes to the `main` branch. It also outputs all the available information and allows to confirm
  or cancel future processing.

  It does the following actions:

  - test the current working directory is correct
  - test if the main or develop branch are behind
  - test if the current branch is develop
  - test if the current working tree is clean
  - ask which version numbers to bump for the `TinyOrm`, `tom`, and `TinyUtils` projects
  - display bumped version numbers to verify them
  - update bumped version numbers in all `version.hpp` files and also in all other files
  - show diff summary
  - prepare the bump commit message with the bumped version numbers
  - do the commit, merge to the `main` branch (ff-only), and push to the `origin/main`
  - update the vcpkg `tinyorm` and `tinyorm-qt5` portfiles
    - obtain the `origin/main` commit ID (SHA-1)
    - update the REF value
    - obtain the `origin/main` archive hash (SHA512)
    - update the SHA512 value
    - remove the port-version field in vcpkg.json if needed
  - prepare the vcpkg commit message
  - do the commit, merge to the `main` branch (ff-only), and push to the `origin/main`

 .INPUTS
  Nothing.

 .OUTPUTS
  Progress information.
#>
