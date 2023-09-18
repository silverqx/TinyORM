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

# Yes/No enum type
# Used to determine whether to bump the vcpkg port-version field
enum YesNoType {
    Yes
    No
}

# Before of Line (to indent)
$Script:BOL = '  '

# Bumping hash data (contains everything what is needed to bump the version numbers)
$Script:BumpsHash = $null
# Files in which the version numbers needs to be bumped (integer value is the number of updates)
$Script:VersionLocations = $null
# Vcpkg port filepaths and bumping port-version field for tinyorm and tinyorm-qt5 ports
$Script:VcpkgHash = $null

# Functions section
# ---

# Common functions
# ---

. $PSScriptRoot\private\Common-Host.ps1

# Initialize all script variables that contain the Resolve-Path call
function Initialize-ScriptVariables {

    $Script:BumpsHash = [ordered] @{
        TinyOrm   = @{type               = [BumpType]::None
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
        TinyOrm = @{
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

    $Script:VcpkgHash = [ordered] @{
        tinyorm           = @{
            portfile      = Resolve-Path -Path ./cmake/vcpkg/ports/tinyorm/portfile.cmake
            vcpkgJson     = Resolve-Path -Path ./cmake/vcpkg/ports/tinyorm/vcpkg.json
            bumpType      = [YesNoType]::No
            versionOld    = $null
            versionBumped = $null
        }

        'tinyorm-qt5' = @{
            portfile      = Resolve-Path -Path ./cmake/vcpkg/ports/tinyorm-qt5/portfile.cmake
            vcpkgJson     = Resolve-Path -Path ./cmake/vcpkg/ports/tinyorm-qt5/vcpkg.json
            bumpType      = [YesNoType]::No
            versionOld    = $null
            versionBumped = $null
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

    Write-ExitError 'The TinyORM project working tree must be clean'
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

# Throw exception if the RegEx result is $false
function Test-RegExResult {
    [OutputType([void])]
    Param(
        [Parameter(Position = 0, Mandatory,
            HelpMessage = 'Specifies the RegEx to display in the exception message.')]
        [ValidateNotNullOrEmpty()]
        [string]
        $RegEx,

        [Parameter(Mandatory, HelpMessage = 'Specifies the RegEx result to test.')]
        [ValidateNotNull()]
        [bool]
        $Result
    )

    # Nothing to do
    if ($Result) {
        return
    }

    throw "The '$RegEx' regex failed."
}

# Bumping version numbers functions
# ---

# Read the bump type to do for the given library
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

    $answer = $Host.Ui.PromptForChoice(
        "Bump `e[32m$Name`e[0m", 'Choose the version number to bump:', $versionTypeChoice, 3
    )

    # Convert to the real [BumpType] type
    return [Enum]::ToObject([BumpType], $answer)
}

# Determine whether all bump types were set to the None value (don't bump)
function Test-AllBumpsEmpty {
    [OutputType([void])]
    Param()

    NewLine
    Write-Progress 'Testing if all bump types are empty...'

    if ($Script:BumpsHash.TinyOrm.type -ne [BumpType]::None -or
        $Script:BumpsHash.tom.type -ne [BumpType]::None -or
        $Script:BumpsHash.TinyUtils.type -ne [BumpType]::None
    ) {
        return
    }

    # I will exit here in this case and don't future process the vcpkg update logic, is ok
    Write-ExitError 'Nothing to bump, all bump types were set to don''t bump'
}

# Verify that the count of matched C macro version numbers is correct
function Test-VersionMacrosCount {
    [OutputType([void])]
    Param(
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [int]    $VersionMacrosCount,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [int]    $ExpectedOccurrences,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $MacroPrefix,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $VersionHppPath
    )

    # Nothing to do
    if ($VersionMacrosCount -eq $ExpectedOccurrences) {
        return
    }

    throw "Found '$VersionMacrosCount' version C macros for '$MacroPrefix' " +
        "in the '$VersionHppPath' file, they must be '$ExpectedOccurrences' " +
        '(major, minor, and bugfix).'
}

# Verify that the count of the version numbers array is correct
function Test-VersionArrayCount {
    [OutputType([void])]
    Param(
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [int]    $VersionArrayCount,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [int]    $ExpectedOccurrences,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $MacroPrefix,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $VersionHppPath
    )

    # Nothing to do
    if ($VersionArrayCount -eq $ExpectedOccurrences) {
        return
    }

    throw "Found '$VersionArrayCount' version numbers for '$MacroPrefix' " +
        "in the '$VersionHppPath' file, they must be '$ExpectedOccurrences' " +
        '(major, minor, and bugfix).'
}

# Read version numbers from version.hpp files and initialize the $Script:BumpsHash
function Read-VersionNumbers {
    [OutputType([void])]
    Param()

    Write-Progress 'Reading version numbers from version.hpp files...'

    foreach ($bumpRow in $Script:BumpsHash.GetEnumerator()) {
        $bumpValue = $bumpRow.Value

        # Nothing to bump
        if ($bumpValue.type -eq [BumpType]::None) {
            continue
        }

        $versionHppPath      = $bumpValue.versionHpp
        $macroPrefix         = $bumpValue.macroPrefix
        $expectedOccurrences = 3

        $regex = "^#define $macroPrefix(?:MAJOR|MINOR|BUGFIX) (?<version>\d+)$"

        # Obtain all C macros with version numbers
        # No Test-Path check needed as version.hpp filepaths were passed using the Resolve-Path
        $versionMacros = (Get-Content -Path $versionHppPath) -cmatch $regex

        # Verify that the count of matched C macro version numbers is correct
        Test-VersionMacrosCount `
            $versionMacros.Count $expectedOccurrences $macroPrefix $versionHppPath

        # Obtain version numbers
        $versionArray = $versionMacros | ForEach-Object {
            $result = $_ -cmatch $regex
            Test-RegExResult $regex -Result $result

            [int] $Matches.version
        }

        # Verify that the count of the version numbers array is correct
        Test-VersionArrayCount $versionArray.Count $expectedOccurrences $macroPrefix $versionHppPath

        $bumpValue.versionOldArray = $versionArray
    }
}

# Compute bump types to reset based on the given bump type.
function Get-BumpTypesToReset {
    [OutputType([BumpType[]])]
    Param(
        [Parameter(Mandatory,
            HelpMessage = 'Specifies a bump type based on which to decide bump types to reset.')]
        [ValidateNotNull()]
        [BumpType] $BumpType
    )

    $bumpTypes = @()

    if ($bumpType -eq [BumpType]::Major) {
        $bumpTypes += [BumpType]::Minor
    }
    if ($bumpType -eq [BumpType]::Major -or $bumpType -eq [BumpType]::Minor) {
        $bumpTypes += [BumpType]::Bugfix
    }

    return $bumpTypes
}

# Bump version numbers by the chosen bump types
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

        # Reset the minor and bugfix version numbers to zero if necessary
        foreach ($bumpTypeToReset in Get-BumpTypesToReset -BumpType $bumpType) {
            $versionBumpedArray[$bumpTypeToReset] = 0
        }

        $bumpValue.versionOld         = $versionOldArray    -join '.'
        $bumpValue.versionBumped      = $versionBumpedArray -join '.'
        $bumpValue.versionBumpedArray = $versionBumpedArray
    }
}

# Get the longest size of the bumped version numbers
function Get-MaxVersionNumbersLength {
    [OutputType([int])]
    Param()

    [int] $result = 0

    foreach ($bumpRow in $Script:BumpsHash.GetEnumerator()) {
        $bumpValue = $bumpRow.Value

        # Nothing to do
        if ($bumpValue.type -eq [BumpType]::None) {
            continue
        }

        $versionOldLength = $bumpValue.versionOld.Length
        if ($versionOldLength -le $result) {
            continue
        }

        $result = $versionOldLength
    }

    return $result
}

# Display version numbers summary
function Show-VersionNumbers {
    [OutputType([void])]
    Param()

    Write-Progress 'Showing bumped version numbers...'
    NewLine

    $maxVersionNumbersLength = Get-MaxVersionNumbersLength

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
        $versionOldPadded = $bumpValue.versionOld.PadRight($maxVersionNumbersLength)

        Write-Host "$versionOldPadded -> $($bumpValue.versionBumped)"
    }
}

# Verify that we found exactly the right count of lines based on the version number to bump
function Test-VersionLinesForVersionHpp {
    [OutputType([void])]
    Param(
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [int]    $VersionLinesCount,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [int]    $ExpectedOccurrences,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $RegEx,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $VersionHppPath,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [array]  $BumpTypesToMatchMapped
    )

    # Nothing to do
    if ($VersionLinesCount -eq $ExpectedOccurrences) {
        return
    }

    throw "Found '$VersionLinesCount' version lines for '$RegEx' regex " +
        "in the '$VersionHppPath' file, expected occurrences must be '$ExpectedOccurrences' " +
        "($($BumpTypesToMatchMapped -join (', ')))."
}

# Verify that the version numbers in the $bumpHash are still the same
function Test-SameVersionNumbersForVersionHpp {
    [OutputType([void])]
    Param(
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [array]  $VersionLines,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $RegEx,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [array]  $VersionOldArray,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [array]  $BumpTypesToMatch,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $VersionHppPath
    )

    # Obtain version numbers based on the version number to bump
    [int[]] $versionNumbersNow = $VersionLines | ForEach-Object {
        $result = $_ -cmatch $RegEx
        Test-RegExResult $RegEx -Result $result

        [int] $Matches.version
    }

    # Fill the beginning with $null-s so the count matches the $bumpHash.versionOld array count
    $versionOldArrayCount = $VersionOldArray.Count
    $versionNumbersNowCount = $versionNumbersNow.Count

    if ($versionNumbersNowCount -ne $versionOldArrayCount) {
        (1..($versionOldArrayCount - $versionNumbersNowCount)).ForEach({
            $versionNumbersNow = @($null) + $versionNumbersNow
        })
    }

    # Verify
    foreach ($bumpTypeToVerifyRaw in $BumpTypesToMatch) {
        $bumpTypeToVerify = $bumpTypeToVerifyRaw.GetHashCode()
        $versionNumberNow = $versionNumbersNow[$bumpTypeToVerify]
        $versionNumberOld = $VersionOldArray[$bumpTypeToVerify]

        # Nothing to do
        if ($versionNumberNow -eq $versionNumberOld) {
            continue
        }

        throw "The '$versionNumberNow -ne $versionNumberOld' for '$bumpTypeToVerifyRaw' " +
            "bump type for '$RegEx' regex in the '$VersionHppPath' file."
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

        $bumpTypesToReset       = (Get-BumpTypesToReset -BumpType $bumpType)
        $bumpTypesToMatch       = @($bumpType) + $bumpTypesToReset
        $bumpTypesToMatchMapped = $bumpTypesToMatch | ForEach-Object { $mapBumpTypeToMacro[$_] }

        $regexTmpl = "^(?<before>#define $macroPrefix(?:{0}) )(?<version>\d+)$"

        $fileContent = Get-Content -Path $versionHppPath

        # Obtain all C macros version lines
        $regex = $regexTmpl -f ($bumpTypesToMatchMapped -join '|')
        $versionLines = $fileContent -cmatch $regex

        # Verify that we found exactly the right count of lines based on the version number to bump
        Test-VersionLinesForVersionHpp `
            $versionLines.Count $bumpTypesToMatch.Count $regex $versionHppPath `
            $bumpTypesToMatchMapped
        # Verify that the version numbers in the $bumpHash are still the same
        Test-SameVersionNumbersForVersionHpp `
            $versionLines $regex $versionOldArray $bumpTypesToMatch $versionHppPath

        # Replace the old version number with the bumped version number
        $versionNumberBumped = $versionBumpedArray[$bumpType.GetHashCode()]
        $regex = $regexTmpl -f $mapBumpTypeToMacro[$bumpType]
        $fileContentReplaced = $fileContent -creplace $regex, "`${before}$versionNumberBumped"

        # Reset the minor and bugfix version numbers to zero if necessary
        foreach ($bumpTypeToReset in $bumpTypesToReset) {
            # Nothing to do, already 0
            if ($versionOldArray[$bumpTypeToReset] -eq 0) {
                continue
            }

            $regex = $regexTmpl -f $mapBumpTypeToMacro[$bumpTypeToReset]
            $fileContentReplaced = $fileContentReplaced -creplace $regex, "`${before}0"
        }

        # Save to the file
        ($fileContentReplaced -join "`n") + "`n" | Set-Content -Path $versionHppPath -NoNewline
    }
}

# Verify that the number of expected occurrences of version numbers is correct
function Test-VersionLinesCount {
    [OutputType([void])]
    Param(
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [int]    $VersionLinesCount,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [int]    $ExpectedOccurrences,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $RegEx,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $FilePath
    )

    # Nothing to do
    if ($VersionLinesCount -eq $ExpectedOccurrences) {
        return
    }

    throw "Found '$VersionLinesCount' version number lines for '$RegEx' regex " +
        "in the '$FilePath' file, expected occurrences must be '$ExpectedOccurrences'."
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
                Test-VersionLinesCount $versionLines.Count $expectedOccurrences $regex $filePath

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
    Param(
        [Parameter(
            HelpMessage = 'Specifies the folder for which to show  git diff, is pwd by default.')]
        [ValidateNotNullOrEmpty()]
        [string[]] $Path = $($(Get-Location).Path)
    )

    Write-Progress 'Showing diff summary...'
    NewLine
    git --no-pager diff --compact-summary $Path

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

    $TinyOrmBumpValue   = $Script:BumpsHash.TinyOrm
    $tomBumpValue       = $Script:BumpsHash.tom
    $TinyUtilsBumpValue = $Script:BumpsHash.TinyUtils

    $isTinyOrmBump   = $TinyOrmBumpValue.type   -ne [BumpType]::None
    $isTomBump       = $tomBumpValue.type       -ne [BumpType]::None
    $isTinyUtilsBump = $TinyUtilsBumpValue.type -ne [BumpType]::None

    # TinyOrm
    if ($isTinyOrmBump) {
        # The TinyORM here is correct, it's the only one exception when the casing doesn't match,
        # it should be TinyOrm (because it's a version number for the TinyOrm library) but I take it
        # as TinyORM project (as whole) version number
        $message += 'TinyORM v' + $TinyOrmBumpValue.versionBumped
    }

    # tom
    if ($isTomBump) {
        # Prepend the and word if necessary
        if ($isTinyOrmBump) {
            $message += ' and '
        }
        $message += 'tom v' + $tomBumpValue.versionBumped
    }

    # TinyUtils
    if ($isTinyUtilsBump) {
        # Append the TinyUtils version to the commit message description
        if ($isTinyOrmBump) {
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

# Vcpkg - updating REF and SHA512 functions
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

# Update the REF and SHA512 in tinyorm and tinyorm-qt5 portfiles
function Edit-VcpkgRefAndHash {
    [OutputType([void])]
    Param()

    $vcpkgRef = Get-VcpkgRef

    Write-Progress 'Obtaining the origin/main archive hash (SHA512)...'

    $vcpkgHash = Get-VcpkgHash -Project 'silverqx/TinyORM' -Branch 'main'

    foreach ($portfiles in $Script:VcpkgHash.GetEnumerator()) {
        $regexRef   = '(?<ref>    REF )(?:[0-9a-f]{40})'
        $regexHash  = '(?<sha512>    SHA512 )(?:[0-9a-f]{128})'
        $regexMatch = "$regexRef|$regexHash"

        $portfilePath = $portfiles.Value.portfile

        $fileContent = Get-Content -Path $portfilePath

        $matchedLines = $fileContent -cmatch $regexMatch

        # Verify if the portfile.cmake file contains the REF and SHA512 lines
        $expectedOccurrences = 2
        Test-RefAndHashLinesCountForVcpkg `
            $matchedLines.Count $expectedOccurrences $regexMatch $portfilePath

        # Replace the old REF AND SHA512 values with the new values in the portfile.cmake
        $fileContentReplaced = $fileContent -creplace $regexRef, "`${ref}$vcpkgRef" `
                                            -creplace $regexHash, "`${sha512}$vcpkgHash"

        # Save to the file
        ($fileContentReplaced -join "`n") + "`n" | Set-Content -Path $portfilePath -NoNewline
    }
}

# Vcpkg - updating port-version field functions
# ---

# Select for which ports to bump the port-version number
function Read-PortVersionsToBump {
    [OutputType([array])]
    Param()

    $versionTypeChoice = [System.Management.Automation.Host.ChoiceDescription[]](@(
            New-Object System.Management.Automation.Host.ChoiceDescription(
                '&tinyorm', 'tinyorm Qt6 port'
            )
            New-Object System.Management.Automation.Host.ChoiceDescription(
                'tinyorm-qt&5', 'tinyorm-qt5 Qt5 port'
            )
            New-Object System.Management.Automation.Host.ChoiceDescription(
                '&Both', 'Both ports tinyorm and tinyorm-qt5'
            )
            New-Object System.Management.Automation.Host.ChoiceDescription(
                '&None', 'Don''t bump'
            )
        ))

    $answer = $Host.Ui.PromptForChoice(
        "Bump `e[32mtinyorm`e[0m and/or `e[32mtinyorm-qt5`e[0m port-version numbers",
        'Choose the vcpkg port/s to bump:', $versionTypeChoice, 3
    )

    # References
    $tinyormValue    = $Script:VcpkgHash.tinyorm
    $tinyormQt5Value = $Script:VcpkgHash['tinyorm-qt5']

    switch ($answer) {
        0 { $tinyormValue.bumpType    = [YesNoType]::Yes }
        1 { $tinyormQt5Value.bumpType = [YesNoType]::Yes }
        2 {
            $tinyormValue.bumpType    = [YesNoType]::Yes
            $tinyormQt5Value.bumpType = [YesNoType]::Yes
        }
        3 { return $true }
        Default {
            throw 'Unreachable code.'
        }
    }

    return $false
}

# Verify that exactly zero or one port-version field line was found in the vcpkg.json file
function Test-PortVersionFieldForVcpkg {
    [OutputType([void])]
    Param(
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [int]    $PortVersionFieldCount,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $RegEx,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $VcpkgJsonPath,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [array]  $ExpectedOccurrences
    )

    # Nothing to do
    if ($ExpectedOccurrences.Contains($PortVersionFieldCount)) {
        return
    }

    throw "Found '$PortVersionFieldCount' port-version field for '$RegEx' regex " +
        "in the '$VcpkgJsonPath' file, expected occurrences must be '" +
        ($ExpectedOccurrences -join "' or '") + "'."
}

# Read port-version numbers from vcpkg.json files and initialize the $Script:VcpkgHash
function Read-PortVersionNumbers {
    [OutputType([void])]
    Param()

    Write-Progress 'Reading port-version numbers from vcpkg.json files...'

    foreach ($portRow in $Script:VcpkgHash.GetEnumerator()) {
        $portValue = $portRow.Value

        # Nothing to bump
        if ($portValue.bumpType -eq [YesNoType]::No) {
            continue
        }

        $vcpkgJsonPath = $portValue.vcpkgJson

        $regex = '"port-version"\s*?:\s*?(?<version>\d+)\s*?,?'

        # Obtain the port-version field with version number
        # No Test-Path check needed as vcpkg.json filepaths were passed using the Resolve-Path
        $portVersionField = (Get-Content -Path $vcpkgJsonPath) -cmatch $regex

        $portVersionFieldCount = $portVersionField.Count
        $expectedOccurrences   = @(0, 1)

        # Verify that exactly zero or one port-version field line was found in the vcpkg.json file
        Test-PortVersionFieldForVcpkg `
            $portVersionFieldCount $regex $vcpkgJsonPath $expectedOccurrences

        # Nothing to do, the port-version field is not defined in the vcpkg.json file
        # I leave the versionOld in the $null state in this case (don't set it to 0) so we don't
        # lost this information, this state information can be used during
        # the Show-PortVersionNumbers so instead of displaying the 0 we can display None
        if ($portVersionFieldCount -eq 0) {
            continue
        }

        # Obtain the port-version number
        $result = $portVersionField[0] -cmatch $regex
        Test-RegExResult $regex -Result $result
        $portVersion = [int] $Matches.version

        $portValue.versionOld = $portVersion
    }
}

# Bump port-version numbers by the chosen bump types (Yes/No)
function Update-PortVersionNumbers {
    [OutputType([void])]
    Param()

    Write-Progress 'Bumping port-version numbers...'

    foreach ($portRow in $Script:VcpkgHash.GetEnumerator()) {
        $portValue = $portRow.Value

        # Nothing to update
        if ($portValue.bumpType -eq [YesNoType]::No) {
            continue
        }

        # Bump the port-version number
        $portValue.versionBumped = ($portValue.versionOld ?? 0) + 1
    }
}

# Get the longest size of the bumped port-version numbers
function Get-MaxPortVersionNumbersLength {
    [OutputType([int])]
    Param()

    [int] $result = 0
    $noneLength = 'None'.Length

    foreach ($portRow in $Script:VcpkgHash.GetEnumerator()) {
        $portValue = $portRow.Value

        # Nothing to do
        if ($portValue.bumpType -eq [YesNoType]::No) {
            continue
        }

        $versionOld = $portValue.versionOld
        $isVersionOldNull = $null -eq $versionOld

        # If the port-version number is $null than the None string will be displayed
        if ($isVersionOldNull -and $noneLength -gt $result) {
            $result = $noneLength
            continue
        }

        # Nothing to do
        if ($isVersionOldNull) {
            continue
        }

        $versionOldLength = $versionOld.ToString().Length

        if ($versionOldLength -gt $result) {
            $result = $versionOldLength
        }
    }

    return $result
}

# Display port-version numbers summary
function Show-PortVersionNumbers {
    [OutputType([void])]
    Param()

    Write-Progress 'Showing bumped port-version numbers...'
    NewLine

    $maxPortVersionNumbersLength = Get-MaxPortVersionNumbersLength

    foreach ($portRow in $Script:VcpkgHash.GetEnumerator()) {
        $portValue = $portRow.Value

        $portNamePadded = $portRow.Name.PadRight(12)
        Write-Host "`e[32m$portNamePadded`e[0m" -NoNewline

        # Nothing to do, skipped version number
        if ($portValue.bumpType -eq [YesNoType]::No) {
            Write-Host 'Skipped' -ForegroundColor DarkRed
            continue
        }

        # Display the current/old and the bumped/new version number
        $versionOld = $portValue.versionOld ?? 'None'
        $versionOldPadded = $versionOld.ToString().PadRight($maxPortVersionNumbersLength)

        # Make the None string DarkRed
        if ($versionOldPadded.StartsWith('None')) {
            $versionOldPadded = $versionOldPadded.Replace('None', "`e[31mNone`e[0m")
        }

        Write-Host "$versionOldPadded -> $($portValue.versionBumped)"
    }
}

# Add the port-version number field into the vcpkg.json
function Add-PortVersionNumber {
    [OutputType([string])]
    Param(
        [Parameter(Mandatory,
            HelpMessage = 'Specifies the vcpkg port name for which to update the port-version ' +
                'number.')]
        [ValidateNotNullOrEmpty()]
        [string]
        $Name,

        [Parameter(Mandatory, HelpMessage = 'Specifies the vcpkg.json file content.')]
        [ValidateNotNullOrEmpty()]
        [string[]]
        $FileContent
    )

    Write-Progress "${Script:BOL}Adding bumped port-version number..."

    $portValue = $Script:VcpkgHash[$Name]

    # Verification of the port-version number isn't needed because we know that there isn't
    # any port-version field in the vcpkg.json file

    $regex = '"version-semver"\s*?:\s*?"\d+(?:\.\d+){2,3}"\s*?,?.*'

    # Replace the old version number with the bumped version number
    $portVersionBumped = $portValue.versionBumped
    return $FileContent -creplace $regex, "`$&`n  `"port-version`": $portVersionBumped,"
}

# Verify that the port-version number in the $Script:VcpkgHash is still the same
function Test-SamePortVersionNumberForVcpkg {
    [OutputType([void])]
    Param(
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $PortVersionField,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $RegEx,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $PortVersionOld,
        [Parameter(Mandatory)] [ValidateNotNullOrEmpty()] [string] $VcpkgJsonPath
    )

    $result = $PortVersionField -cmatch $RegEx
    Test-RegExResult $RegEx -Result $result

    $portVersionNow = [int] $Matches.version

    # Nothing to do
    if ($portVersionNow -eq $PortVersionOld) {
        return
    }

    throw "The '$portVersionNow -ne $PortVersionOld' for '$RegEx' regex " +
        "in the '$VcpkgJsonPath' file."
}

# Update the port-version number field in the vcpkg.json
function Edit-PortVersionNumber {
    [OutputType([string])]
    Param(
        [Parameter(Mandatory,
            HelpMessage = 'Specifies the vcpkg port name for which to update the port-version ' +
                'number.')]
        [ValidateNotNullOrEmpty()]
        [string]
        $Name,

        [Parameter(Mandatory, HelpMessage = 'Specifies the port-version field line.')]
        [ValidateNotNullOrEmpty()]
        [string]
        $PortVersionField,

        [Parameter(Mandatory, HelpMessage = 'Specifies the vcpkg.json file content.')]
        [ValidateNotNullOrEmpty()]
        [string[]]
        $FileContent
    )

    Write-Progress "${Script:BOL}Updating bumped port-version number..."

    $portValue = $Script:VcpkgHash[$Name]
    $regex = '"port-version"\s*?:\s*?(?<version>\d+)\s*?,?'

    # Verify that the port-version number in the $Script:VcpkgHash is still the same
    Test-SamePortVersionNumberForVcpkg `
        $PortVersionField $regex $portValue.versionOld $portValue.vcpkgJson

    # Replace the old version number with the bumped version number
    $portVersionBumped = $portValue.versionBumped
    return $FileContent -creplace $regex, "`"port-version`": $portVersionBumped,"
}

# Update port-version numbers in vcpkg.json files
function Edit-PortVersionNumbers {
    [OutputType([void])]
    Param()

    NewLine
    Write-Progress 'Editing bumped port-version numbers in vcpkg.json files...'

    foreach ($portRow in $Script:VcpkgHash.GetEnumerator()) {
        $portValue = $portRow.Value
        $bumpType = $portValue.bumpType

        # Nothing to edit
        if ($bumpType -eq [YesNoType]::No) {
            continue
        }

        $vcpkgJsonPath = $portValue.vcpkgJson

        $regex = '"port-version"\s*?:\s*?(?<version>\d+)\s*?,?'

        # Obtain the port-version field with version number
        # No Test-Path check needed as vcpkg.json filepaths were passed using the Resolve-Path
        $fileContent = Get-Content -Path $vcpkgJsonPath
        $portVersionField = $fileContent -cmatch $regex

        $portVersionFieldCount = $portVersionField.Count
        $expectedOccurrences   = @(0, 1)

        # Verify that exactly zero or one port-version field line was found in the vcpkg.json file
        Test-PortVersionFieldForVcpkg `
            $portVersionFieldCount $regex $vcpkgJsonPath $expectedOccurrences

        $fileContentReplaced = $null
        $portName = $portRow.Name

        # The vcpkg.json file doesn't contain the port-version field so add it
        if ($portVersionFieldCount -eq 0) {
            $fileContentReplaced = Add-PortVersionNumber -Name $portName -FileContent $fileContent
        }
        # The vcpkg.json file contains the port-version field so update it
        else {
            $fileContentReplaced = `
                Edit-PortVersionNumber -Name $portName -PortVersionField $portVersionField[0] `
                                       -FileContent $fileContent
        }

        # Save to the file
        ($fileContentReplaced -join "`n") + "`n" | Set-Content -Path $vcpkgJsonPath -NoNewline
    }
}

# Remove the 'port-version' field from vcpkg.json files if needed
function Remove-PortVersions {
    [OutputType([string])]
    Param()

    Write-Progress 'Removing the port-version field from vcpkg.json...'

    foreach ($portfiles in $Script:VcpkgHash.GetEnumerator()) {
        $vcpkgJsonPath = $portfiles.Value.vcpkgJson

        $regex = '\s*?"port-version"\s*?:\s*?\d+\s*?,?\s*?'

        $fileContent = Get-Content -Path $vcpkgJsonPath

        $portVersionField = $fileContent -cmatch $regex

        $portVersionFieldCount = $portVersionField.Count
        $expectedOccurrences   = @(0, 1)

        # Verify that exactly zero or one port-version field line was found in the vcpkg.json file
        Test-PortVersionFieldForVcpkg `
            $portVersionFieldCount $regex $vcpkgJsonPath $expectedOccurrences

        # Remove the port-version field from the vcpkg.json file
        $fileContentReplaced = $fileContent | Where-Object { $_ -cnotmatch $regex }

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

# Main section related functions
# ---

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

# Bump version numbers
function Invoke-BumpVersions {
    [OutputType([string])]
    Param()

    # Allow to skip bumping version numbers
    $answer = Approve-Continue 'Do you want to bump version numbers?' -DefaultChoice 0
    if ($answer -eq 1) {
        NewLine
        Write-Error 'Skipping version numbers bumping'
        return
    }

    # Select which version numbers to bump
    $Script:BumpsHash.TinyOrm.type   = Read-BumpType -Name TinyOrm
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
    Write-Info 'TinyORM project was bumped and deployed successfully. 🥳'
}

# Main logic to bump vcpkg port-version fields in the tinyorm and tinyorm-qt5 ports
function Invoke-BumpPortVersions {
    [OutputType([string])]
    Param()

    # Select for which ports to bump the port-version number
    $cancelBumping = Read-PortVersionsToBump
    # Nothing to bump, bumping canceled
    if ($cancelBumping) {
        return
    }

    # Obtain vcpkg port-version numbers from vcpkg.json files
    Read-PortVersionNumbers

    # Bump all port-version numbers
    Update-PortVersionNumbers
    Show-PortVersionNumbers

    # Update these port-version numbers in vcpkg.json files
    Edit-PortVersionNumbers
}

# Vcpkg ports update
function Invoke-UpdateVcpkgPorts {
    [OutputType([string])]
    Param()

    # Don't allow to skip updating the vcpkg ports because they must be always updated if merging
    # into the main branch

    Write-Header 'Updating vcpkg ports REF and SHA512'

    Edit-VcpkgRefAndHash

    # Allow to updated port-version fields if TinyOrm version wasn't bumped
    if ($Script:BumpsHash.TinyOrm.type -eq [BumpType]::None) {
        Invoke-BumpPortVersions
        NewLine
    }
    # Remove a port-version fields if the TinyOrm version was bumped
    else {
        Remove-PortVersions
    }

    Show-DiffSummaryAndApprove -Path (Resolve-Path .\cmake\vcpkg)

    NewLine
    Write-Progress 'Committing vcpkg REF and SHA512...'
    git commit --all --message=$(Get-VcpkgCommitMessage)
    Test-LastExitCode

    # Merge to origin/main and push
    Invoke-MergeDevelopAndDeploy

    NewLine
    Write-Info 'Vcpkg ports were updated and deployed successfully. 🥳'
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

# Need to initialize these variables later because of the Resolve-Path calls
Initialize-ScriptVariables

# Fire it up 🔥
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
    - remove the port-version field from vcpkg.json files if TinyOrm version number was bumped
    - if TinyOrm version number wasn't bumped allow to bump the port-version number in vcpkg.json
  - prepare the vcpkg commit message
  - do the commit, merge to the `main` branch (ff-only), and push to the `origin/main`

 .INPUTS
  Nothing.

 .OUTPUTS
  Progress information.
#>
