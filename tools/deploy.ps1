#!/usr/bin/env pwsh

Param(
    [Parameter(
        HelpMessage = 'Specifies whether to skip git push and updating vcpkg ports ' +
            '(semi pretend or dry run).')]
    [switch] $Pretend
)

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

# The GitHub repository to deploy to
$Script:GitHubProject = 'silverqx/TinyORM'

# Before of Line (to indent)
$Script:BOL = '  '

# Bumping hash data (contains everything what is needed to bump the version numbers)
$Script:BumpsHash = $null
# Files in which the version numbers needs to be bumped (integer value is the number of updates)
$Script:VersionLocations = $null
# Files in which a number of unit tests needs to be updated (integer value is the number of updates)
$Script:NumberOfUnitTestsLocations = $null
# Vcpkg port file paths and bumping port-version field for tinyorm port
$Script:VcpkgHash = $null

# Tag version with the v prefix
$Script:TagVersion = $null
# Determine whether bumping version numbers was skipped
$Script:IsBumpingSkipped = $false
# Bump commit message cache (to avoid regeneration for the tag)
$Script:BumpCommitMessage = $null

# RegEx to match ten thousand number (12345 or 12 345)
$Script:NumberTenThousandRegEx = '\d{1,2} ?\d{3}'

# Base template RegEx to match Number of Unit Tests in all files
$Script:NumberOfUnitTestsRegExTmpl =
    '(?<before>(?:with|currently|then|all) (?:__)?)(?<number>{0})(?<after>(?:__)? unit (?:and|tests))'
# RegEx to match Number of Unit Tests in all files
$Script:NumberOfUnitTestsRegEx = $Script:NumberOfUnitTestsRegExTmpl -f $Script:NumberTenThousandRegEx
# The current Number of unit tests found in the README.md file
$Script:NumberOfUnitTestsCurrent = $null
# Number of unit tests to update in all files
$Script:NumberOfUnitTestsNew = $null
# Number of skipped unit tests (will be used in the commit message only)
$Script:NumberOfSkippedUnitTestsNew = $null

# RegEx to match the port-version in the vcpkg.json file
$Script:PortVersionRegEx = '"port-version"\s*?:\s*?(?<version>\d{1,3})\s*?,?'

# Functions section
# ---

# Common functions
# ---

. $PSScriptRoot\private\Common-Host.ps1
. $PSScriptRoot\private\Common-Deploy.ps1

# Initialize all script variables that contain the Resolve-Path call
function Initialize-ScriptVariables {

    $Script:BumpsHash = [ordered] @{
        TinyOrm     = @{type               = [BumpType]::None
                        versionOld         = $null
                        versionBumped      = $null
                        versionOldArray    = $null
                        versionBumpedArray = $null
                        versionHpp         = Resolve-Path -Path ./include/orm/version.hpp
                        macroPrefix        = 'TINYORM_VERSION_'}

        tom         = @{type               = [BumpType]::None
                        versionOld         = $null
                        versionBumped      = $null
                        versionOldArray    = $null
                        versionBumpedArray = $null
                        versionHpp         = Resolve-Path -Path ./tom/include/tom/version.hpp
                        macroPrefix        = 'TINYTOM_VERSION_'}

        TinyDrivers = @{type               = [BumpType]::None
                        versionOld         = $null
                        versionBumped      = $null
                        versionOldArray    = $null
                        versionBumpedArray = $null
                        versionHpp         = Resolve-Path -Path ./drivers/common/include/orm/drivers/version.hpp
                        macroPrefix        = 'TINYDRIVERS_VERSION_'}

        TinyMySql   = @{type               = [BumpType]::None
                        versionOld         = $null
                        versionBumped      = $null
                        versionOldArray    = $null
                        versionBumpedArray = $null
                        versionHpp         = Resolve-Path -Path ./drivers/mysql/include/orm/drivers/mysql/version.hpp
                        macroPrefix        = 'TINYMYSQL_VERSION_'}

        TinyUtils   = @{type               = [BumpType]::None
                        versionOld         = $null
                        versionBumped      = $null
                        versionOldArray    = $null
                        versionBumpedArray = $null
                        versionHpp         = Resolve-Path -Path ./tests/TinyUtils/src/version.hpp
                        macroPrefix        = 'TINYUTILS_VERSION_'}
    }

    # The number represents the Number of Lines, not the number of occurrences, so eg. 3 means there
    # are 3 lines with this version number. So if a line contains more occurrences, it counts as 1.
    # Also, RegEx searches only for x.y.z (Major, Minor, Bugfix), it doesn't matter whether
    # the version number also contains the Build version number, it will be untouched, RegEx only
    # replaces Major, Minor, Bugfix version numbers in this case.
    # Only files defined below are searched, eg. portfile.cmake also contains vX.Y.Z version
    # numbers, but these will be replaced by another function later.
    # ❗Don't define here to replace the REF version number in portfile.cmake.
    $Script:VersionLocations = [ordered] @{
        TinyOrm = @{
            # RegEx: (?<!v)(?<version>0\.0\.0)
            [VersionType]::VersionOnly = [ordered] @{
                (Resolve-Path -Path ./NOTES.txt)                                = 4
                (Resolve-Path -Path ./cmake/vcpkg/ports/tinyorm/vcpkg.json)     = 1
                (Resolve-Path -Path ./docs/building/hello-world.mdx)            = 4
                (Resolve-Path -Path ./docs/building/migrations.mdx)             = 1
                (Resolve-Path -Path ./docs/building/tinyorm.mdx)                = 1
            }
            # RegEx: (?<version>v0\.0\.0)
            [VersionType]::VersionWith_v = [ordered] @{
                (Resolve-Path -Path ./NOTES.txt)                 = 2
                (Resolve-Path -Path ./README.md)                 = 2
                (Resolve-Path -Path ./docs/README.mdx)           = 1
                (Resolve-Path -Path ./docs/building/tinyorm.mdx) = 1
            }
        }

        tom = @{
            # RegEx: (?<version>v0\.0\.0)
            [VersionType]::VersionWith_v = [ordered] @{
                (Resolve-Path -Path ./NOTES.txt)       = 1
                (Resolve-Path -Path ./README.md)       = 2
                (Resolve-Path -Path ./docs/README.mdx) = 1
            }
        }

        TinyDrivers = @{
            # RegEx: (?<!v)(?<version>0\.0\.0)
            [VersionType]::VersionOnly = [ordered] @{
                (Resolve-Path -Path ./tom/src/tom/commands/aboutcommand.cpp) = 1
            }
            # RegEx: (?<version>v0\.0\.0)
            [VersionType]::VersionWith_v = [ordered] @{
                (Resolve-Path -Path ./README.md)       = 2
                (Resolve-Path -Path ./docs/README.mdx) = 1
            }
        }

        TinyMySql = @{
            # RegEx: (?<version>v0\.0\.0)
            [VersionType]::VersionWith_v = [ordered] @{
                (Resolve-Path -Path ./README.md)       = 2
                (Resolve-Path -Path ./docs/README.mdx) = 1
            }
        }

        # TinyUtils doesn't have any version numbers in files
    }

    # RegEx: see $Script:NumberOfUnitTestsRegExTmpl
    $Script:NumberOfUnitTestsLocations = [ordered] @{
        # The README.md will be read from to populate the current number of unit tests using this
        # RegEx: with __(?<number>\d{1,2} ?\d{3})__ unit
        (Resolve-Path -Path ./README.md)                            = 3
        (Resolve-Path -Path ./docs/features-summary.mdx)            = 1
        (Resolve-Path -Path ./docs/README.mdx)                      = 2
        (Resolve-Path -Path ./docs/supported-compilers.mdx)         = 1
        (Resolve-Path -Path ./docs/tinydrivers/getting-started.mdx) = 1
    }

    $Script:VcpkgHash = [ordered] @{
        tinyorm           = @{
            portfile      = Resolve-Path -Path ./cmake/vcpkg/ports/tinyorm/portfile.cmake
            vcpkgJson     = Resolve-Path -Path ./cmake/vcpkg/ports/tinyorm/vcpkg.json
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

# Determine whether the main or develop branches are ahead the origin
function Test-GitAheadOrigin {
    [OutputType([void])]
    Param()

    Write-Progress 'Testing if the main and develop branches are ahead the origin...'

    $developAhead = git rev-list --count origin/develop..develop
    if ($developAhead -ne 0) {
        Write-ExitError ("The TinyORM 'develop' branch is $developAhead commits ahead " +
            'the origin/develop')
    }

    $mainAhead = git rev-list --count origin/main..main
    if ($mainAhead -ne 0) {
        Write-ExitError "The TinyORM 'main' branch is $mainAhead commits ahead the origin/main"
    }

}

# Determine whether the main or develop branches are behind the origin
function Test-GitBehindOrigin {
    [OutputType([void])]
    Param()

    Write-Progress 'Testing if the main and develop branches are behind the origin...'

    Write-Progress "${Script:BOL}Fetching from origin..."
    git fetch --tags origin

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

# Post deploy actions
# ---

# Remove MSYS2 UCRT64 build trees to avoid the tst_Versions test case failure (ccache bug)
function Remove-Msys2BuildTrees {
    [OutputType([void])]
    Param()

    # Nothing to do, don't remove folders in the Pretend mode
    if ($Pretend) {
        return
    }

    Write-Progress 'Removing MSYS2 CMake Build Trees...'

    $cmakeBuildTrees = Join-Path -Path $env:ActionRunnerTinyORM `
        -ChildPath _work\TinyORM\TinyORM-builds-cmake

    # Nothing to do, CMake build trees folder doesn't exist
    if (-not (Test-Path -Path $cmakeBuildTrees)) {
        return
    }

    Get-Item -Path $cmakeBuildTrees\Drivers-msys2-u-gcc-*, $cmakeBuildTrees\Drivers-msys2-u-clang-*
        | Remove-Item -Force -Recurse
}

# Print post-deploy warnings and messages about what is needed next to finish the deployment
function Write-PostDeployWarnings {
    [OutputType([void])]
    Param()

    NewLine
    Write-Error 'Post-deploy actions needed after bumping TinyORM version numbers'
    NewLine

    Write-Error (" - remove all 'TinyORM-builds-cmake\Drivers-msys2-u-*' build trees " +
        'as tst_Versions test case will fail (ccache bug)')
    Write-Error ' - create a new tinyorm Gentoo ebuild'
    Write-Error ' - deploy TinyORM-github.io documentation'
    Write-Error ' - create a new Release in GitHub UI'
    NewLine
}

# Update Number of Unit Tests
# ---

# Read a number of unit tests from the README.md file
function Read-NumberOfUnitTestsCurrent {
    [OutputType([void])]
    Param()

    Write-Progress 'Reading number of unit tests from README.md...'

    # Get the first README.md filepath
    $filePath = $Script:NumberOfUnitTestsLocations.Keys.Item(0)
    $regex = 'with __(?<number>{0})__ unit' -f $Script:NumberTenThousandRegEx

    $numberOfUnitTestsLines = (Get-Content -Path $filePath) -cmatch $regex

    # Verify that the number of expected occurrences of unit tests numbers is correct
    $expectedOccurrences = 1
    Test-ExpectedLinesCount `
        $numberOfUnitTestsLines.Count $expectedOccurrences $regex $filePath 'Unit Tests number'

    # Obtain the number of unit tests
    $result = $numberOfUnitTestsLines[0] -cmatch $regex
    Test-RegExResult $regex -Result $result

    $Script:NumberOfUnitTestsCurrent = [int] $Matches.number.Replace(' ', '')
}

# Verify the current number of unit tests in all files whether is the same everywhere
function Test-NumberOfUnitTestsCurrent {
    [OutputType([void])]
    Param()

    Write-Progress 'Verifying a number of unit tests in all documentation files...'

    foreach ($numberOfUnitTestLocationsAll in $Script:NumberOfUnitTestsLocations.GetEnumerator()) {
        $filePath = $numberOfUnitTestLocationsAll.Name
        $expectedOccurrences = $numberOfUnitTestLocationsAll.Value

        # Find occurrences of the CURRENT number of unit tests
        $regex = $Script:NumberOfUnitTestsRegExTmpl -f $Script:NumberOfUnitTestsCurrent
        $numberOfUnitTestsLines = (Get-Content -Path $filePath) -cmatch $regex

        # Verify that the number of expected occurrences of unit tests numbers is correct
        Test-ExpectedLinesCount `
            $numberOfUnitTestsLines.Count $expectedOccurrences $regex $filePath 'Unit Tests number'
    }
}

# Print the number of unit tests to the console
function Show-NumberOfUnitTestsCurrent {
    [OutputType([void])]
    Param()

    NewLine
    Write-Output ("The current `e[36mNumber of Unit Tests`e[0m is set to " +
        "`e[35m$Script:NumberOfUnitTestsCurrent`e[0m 🤯")
}

# Read the new number of unit tests to update in all files and skipped unit tests for commit message
function Read-NumberOfUnitTestsNew {
    [OutputType([void])]
    Param()

    Newline
    $Script:NumberOfUnitTestsNew = Read-Host -Prompt "Enter the new `e[36mNumber of Unit Tests`e[0m"

    $Script:NumberOfUnitTestsNew = $Script:NumberOfUnitTestsNew.Replace(' ', '')

    $Script:NumberOfSkippedUnitTestsNew =
        Read-Host -Prompt "Enter the new `e[36mNumber of Skipped Unit Tests`e[0m"

    $Script:NumberOfSkippedUnitTestsNew = $Script:NumberOfSkippedUnitTestsNew.Replace(' ', '')
}

# Verify the new number of unit tests and skipped unit tests
function Test-NumberOfUnitTestsNew {
    [OutputType([void])]
    Param()

    Newline
    Write-Progress 'Verifying a new number of unit tests...'

    [int] $parseResult = 0

    # Verify Unit Tests number
    [int] $maxValue = 5000

    if (-not ([int]::TryParse($Script:NumberOfUnitTestsNew, [ref] $parseResult))) {
        Write-ExitError ("The new number of unit tests must be the integer number between " +
            "$($Script:NumberOfUnitTestsCurrent + 1) - $maxValue")
    }

    # Type-cast before comparisons
    $Script:NumberOfUnitTestsNew = $Script:NumberOfUnitTestsNew -as [int]

    if ($null -eq $Script:NumberOfUnitTestsNew) {
        Write-ExitError 'Type-cast of the $Script:NumberOfUnitTestsNew to [int] failed'
    }

    if ($Script:NumberOfUnitTestsCurrent -ge $Script:NumberOfUnitTestsNew) {
        Write-ExitError ("The new number of unit tests '$Script:NumberOfUnitTestsNew' must " +
            "be greater than the current number '$Script:NumberOfUnitTestsCurrent'")
    }

    if ($Script:NumberOfUnitTestsNew -gt $maxValue) {
        Write-ExitError ("The new number of unit tests '$Script:NumberOfUnitTestsNew' is too " +
            "high. The current max. value is set to '$maxValue'.")
    }

    # Verify Skipped Unit Tests number
    [int] $maxSkippedValue = 150

    if (-not ([int]::TryParse($Script:NumberOfSkippedUnitTestsNew, [ref] $parseResult))) {
        Write-ExitError 'The new number of skipped unit tests must be the integer number'
    }

    # Type-cast before comparisons
    $Script:NumberOfSkippedUnitTestsNew = $Script:NumberOfSkippedUnitTestsNew -as [int]

    if ($null -eq $Script:NumberOfSkippedUnitTestsNew) {
        Write-ExitError 'Type-cast of the $Script:NumberOfSkippedUnitTestsNew to [int] failed'
    }

    if ($Script:NumberOfSkippedUnitTestsNew -gt $maxSkippedValue) {
        Write-ExitError (
            "The new number of skipped unit tests '$Script:NumberOfSkippedUnitTestsNew' " +
            "is too high. The current max. value is set to '$maxSkippedValue'."
        )
    }
}

# Update the Number of Unit Tests in all files defined in the $Script:NumberOfUnitTestsLocations
function Edit-NumberOfUnitTestsInAllFiles {
    [OutputType([void])]
    Param()

    Write-Progress 'Editing number of unit tests in all files...'

    foreach ($numberOfUnitTestLocationsAll in $Script:NumberOfUnitTestsLocations.GetEnumerator()) {
        $filePath = $numberOfUnitTestLocationsAll.Name
        $expectedOccurrences = $numberOfUnitTestLocationsAll.Value

        $fileContent = Get-Content -Path $filePath

        $numberOfUnitTestsLines = $fileContent -cmatch $Script:NumberOfUnitTestsRegEx

        # Verify that the number of expected occurrences of unit tests numbers is correct
        Test-ExpectedLinesCount `
            $numberOfUnitTestsLines.Count $expectedOccurrences $Script:NumberOfUnitTestsRegEx `
            $filePath 'Unit Tests number'

        # Replace the old number of unit tests with the new number (doesn't work with `$1)
        $regexReplace = "`${before}${Script:NumberOfUnitTestsNew}`${after}"
        $fileContentReplaced =
            $fileContent -creplace $Script:NumberOfUnitTestsRegEx, $regexReplace

        # Save to the file
        ($fileContentReplaced -join "`n") + "`n" | Set-Content -Path $filePath -NoNewline
    }
}

# Get the commit message for updated number of unit tests
function Get-UpdatedNumberOfUnitTestsCommitMessage {
    [OutputType([string])]
    Param()

    Write-Progress 'Generating the commit message for updated number of unit tests...'

    return "docs updated number of unit tests to ${Script:NumberOfUnitTestsNew} " +
        "(${Script:NumberOfSkippedUnitTestsNew})`n`n[skip ci]"
}

# Bump version numbers functions
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

    $versionTypeChoice = [System.Management.Automation.Host.ChoiceDescription[]] @(
        New-Object System.Management.Automation.Host.ChoiceDescription('M&ajor', 'Major version')
        New-Object System.Management.Automation.Host.ChoiceDescription('&Minor', 'Minor version')
        New-Object System.Management.Automation.Host.ChoiceDescription('&Bugfix', 'Bugfix version')
    )

    $isNameTinyOrm = $Name -eq 'TinyOrm'

    # The TinyOrm must always be bumped because the tag must be created
    if (-not $isNameTinyOrm) {
        $versionTypeChoice += `
            New-Object System.Management.Automation.Host.ChoiceDescription('&None', 'Don''t bump')
    }

    # TinyOrm -> Bugfix and all others None
    $defaultChoice = $isNameTinyOrm ? 2 : 3

    $answer = $Host.Ui.PromptForChoice(
        "Bump `e[32m$Name`e[0m", 'Choose the version number to bump:', $versionTypeChoice,
        $defaultChoice
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

    if ($Script:BumpsHash.TinyOrm.type     -ne [BumpType]::None -or
        $Script:BumpsHash.tom.type         -ne [BumpType]::None -or
        $Script:BumpsHash.TinyDrivers.type -ne [BumpType]::None -or
        $Script:BumpsHash.TinyMySql.type   -ne [BumpType]::None -or
        $Script:BumpsHash.TinyUtils.type   -ne [BumpType]::None
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

        # We always need to read version numbers because of 'Not bumped' section in a commit message

        $versionHppPath      = $bumpValue.versionHpp
        $macroPrefix         = $bumpValue.macroPrefix
        $expectedOccurrences = 3

        $regex = "^#define $macroPrefix(?:MAJOR|MINOR|BUGFIX) (?<version>\d{1,5})$"

        # Obtain all C macros with version numbers
        # No Test-Path check needed as version.hpp file paths were passed using the Resolve-Path
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
        $bumpValue.versionOld      = $versionArray -join '.'
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

        $bumpValue.versionBumped      = $versionBumpedArray -join '.'
        $bumpValue.versionBumpedArray = $versionBumpedArray
    }

    # Also prepare the tag version
    $Script:TagVersion = 'v' + $Script:BumpsHash.TinyOrm.versionBumped
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

        $bumpNamePadded = $bumpRow.Name.PadRight(12)
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

    NewLine
    Write-Output "Tag version set to `e[35m$Script:TagVersion`e[0m"
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

# Verify that the version numbers in the $Script:BumpHash are still the same
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

        # Double {{x}} is needed by the -f operator to keep the curly braces in the formatted string
        $regexTmpl = "^(?<before>#define $macroPrefix(?:{0}) )(?<version>\d{{1,5}})$"

        $fileContent = Get-Content -Path $versionHppPath

        # Obtain all C macros version lines
        $regex = $regexTmpl -f ($bumpTypesToMatchMapped -join '|')
        $versionLines = $fileContent -cmatch $regex

        # Verify that we found exactly the right count of lines based on the version number to bump
        Test-VersionLinesForVersionHpp `
            $versionLines.Count $bumpTypesToMatch.Count $regex $versionHppPath `
            $bumpTypesToMatchMapped
        # Verify that the version numbers in the $Script:BumpHash are still the same
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

# Update version numbers in all files for all libraries defined in the $Script:VersionLocations
function Edit-VersionNumbersInAllFiles {
    [OutputType([void])]
    Param()

    Write-Progress 'Editing bumped version numbers in all files...'

    foreach ($versionLocationsAll in $Script:VersionLocations.GetEnumerator()) {
        $libraryName = $versionLocationsAll.Name
        $bumpValue = $Script:BumpsHash[$libraryName]
        $isTinyDriversOrMySql = $libraryName -in @('TinyDrivers', 'TinyMySql')

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

                # Currently, TinyDrivers and TinyMySql have the same version number so replacing
                # doesn't work, we must be more specific to better target the version number to
                # search.
                if ($isTinyDriversOrMySql) {
                    $regex = "(?<before>$libraryName(?:__ | |-))"
                }
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
                Test-ExpectedLinesCount `
                    $versionLines.Count $expectedOccurrences $regex $filePath 'Version number'

                $versionBumped = $bumpValue.versionBumped

                # Prepend the v character (follow/copy the old version number format)
                if ($versionType -eq [VersionType]::VersionWith_v) {
                    $versionBumped = 'v' + $versionBumped

                    # Also, we must prepend what was there before for this special case (same
                    # version numbers).
                    if ($isTinyDriversOrMySql) {
                        $versionBumped = "`${before}$versionBumped"
                    }
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
        [string[]] $Path = (Get-Location).Path,

        # Don't use an enum for this, it would be too many unnecessary mappings up and down
        [Parameter(Mandatory,
            HelpMessage = 'Specifies the name of updated artifacts (used in the info message).')]
        [ValidateNotNullOrEmpty()]
        [ValidateSet('Number of Unit Tests', 'Version Numbers', 'Vcpkg Port', IgnoreCase = $false)]
        [string]
        $SummaryFor
    )

    Write-Progress 'Showing diff summary...'
    NewLine
    git --no-pager diff --compact-summary $Path

    NewLine
    Write-Info "Please check updated `e[35m$SummaryFor`e[32m in SmartGit..."
    Approve-Continue -Exit
}

# Determine whether TinyOrm version number was bumped (it must always be bumped here)
function Test-TinyOrmBumped
{
    [OutputType([void])]
    Param()

    Write-Progress 'Testing if the TinyOrm version number was bumped...'

    if ($Script:BumpsHash.TinyOrm.type -ne [BumpType]::None) {
        return
    }

    Write-ExitError 'TinyORM version must always be bumped during commit message generation.'
}

# Put together a commit message based on all bump types and version numbers
function Get-BumpCommitMessage {
    [OutputType([string])]
    Param()

    Write-Progress 'Generating the bump commit message and setting it to clipboard...'

    # Shortcuts
    $TinyOrmBumpValue     = $Script:BumpsHash.TinyOrm
    $tomBumpValue         = $Script:BumpsHash.tom
    $TinyDriversBumpValue = $Script:BumpsHash.TinyDrivers
    $TinyMySqlBumpValue   = $Script:BumpsHash.TinyMySql
    $TinyUtilsBumpValue   = $Script:BumpsHash.TinyUtils

    $isTomBump         = $tomBumpValue.type         -ne [BumpType]::None
    $isTinyDriversBump = $TinyDriversBumpValue.type -ne [BumpType]::None
    $isTinyMySqlBump   = $TinyMySqlBumpValue.type   -ne [BumpType]::None
    $isTinyUtilsBump   = $TinyUtilsBumpValue.type   -ne [BumpType]::None

    # TinyOrm
    # Determine whether TinyOrm version number was bumped (it must always be bumped here)
    Test-TinyOrmBumped

    # The TinyORM here is correct, it's the only one exception when the casing doesn't match,
    # it should be TinyOrm (because it's a version number for the TinyOrm library) but I take it
    # as TinyORM project (as whole) version number
    $message = 'bump version to TinyORM v' + $TinyOrmBumpValue.versionBumped

    # tom
    # I want to have the tom version number in the commit title if it was bumped
    if ($isTomBump) {
        $message += ' and tom v' + $tomBumpValue.versionBumped
    }

    # All other bumped version numbers
    if ($isTinyDriversBump -or $isTinyMySqlBump -or $isTinyUtilsBump) {
        $message += "`n`nAlso bumped to:`n"

        if ($isTinyDriversBump) {
            $message += "`n - TinyDrivers v" + $TinyDriversBumpValue.versionBumped
        }
        if ($isTinyMySqlBump) {
            $message += "`n - TinyMySql v" + $TinyMySqlBumpValue.versionBumped
        }
        if ($isTinyUtilsBump) {
            $message += "`n - TinyUtils v" + $TinyUtilsBumpValue.versionBumped
        }
    }

    # Not bumped (to also print the current versions)
    if (-not $isTomBump -or -not $isTinyDriversBump -or -not $isTinyMySqlBump -or
        -not $isTinyUtilsBump
    ) {
        $message += "`n`nNot bumped:`n"

        if (-not $isTomBump) {
            $message += "`n - tom v" + $tomBumpValue.versionOld
        }
        if (-not $isTinyDriversBump) {
            $message += "`n - TinyDrivers v" + $TinyDriversBumpValue.versionOld
        }
        if (-not $isTinyMySqlBump) {
            $message += "`n - TinyMySql v" + $TinyMySqlBumpValue.versionOld
        }
        if (-not $isTinyUtilsBump) {
            $message += "`n - TinyUtils v" + $TinyUtilsBumpValue.versionOld
        }
    }

    Set-Clipboard -Value $message

    # Cache the commit message for the tag
    $Script:BumpCommitMessage = $message

    return $message
}

# Create tag functions
# ---

# Get the tag message
function Get-TagMessage {
    [OutputType([string])]
    Param()

    Write-Progress 'Generating the tag message and setting it to clipboard...'

    # Make the tag message the same as the commit message so no information is not lost
    $tagMessage = $Script:BumpCommitMessage

    Set-Clipboard -Value $tagMessage

    return $tagMessage
}

# Vcpkg - update port-version field functions
# ---

# Select for which ports to bump the port-version number
function Read-PortVersionsToBump {
    [OutputType([bool])]
    Param()

    $versionTypeChoice = [System.Management.Automation.Host.ChoiceDescription[]](@(
            New-Object System.Management.Automation.Host.ChoiceDescription(
                '&tinyorm', 'tinyorm Qt6 port'
            )
            New-Object System.Management.Automation.Host.ChoiceDescription(
                '&None', 'Don''t bump'
            )
        ))

    $answer = $Host.Ui.PromptForChoice(
        "Bump `e[32mtinyorm`e[0m port-version number",
        'Choose the vcpkg port to bump:', $versionTypeChoice, 1
    )

    # References
    $tinyormValue = $Script:VcpkgHash.tinyorm

    switch ($answer) {
        0 { $tinyormValue.bumpType = [YesNoType]::Yes }
        1 { return $true }
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

        # Obtain the port-version field with version number
        # No Test-Path check needed as vcpkg.json file paths were passed using the Resolve-Path
        $portVersionField = (Get-Content -Path $vcpkgJsonPath) -cmatch $Script:PortVersionRegEx

        $portVersionFieldCount = $portVersionField.Count
        $expectedOccurrences   = @(0, 1)

        # Verify that exactly zero or one port-version field line was found in the vcpkg.json file
        Test-PortVersionFieldForVcpkg `
            $portVersionFieldCount $Script:PortVersionRegEx $vcpkgJsonPath $expectedOccurrences

        # Nothing to do, the port-version field is not defined in the vcpkg.json file
        # I leave the versionOld in the $null state in this case (don't set it to 0) so we don't
        # lost this information, this state information can be used during
        # the Show-PortVersionNumbers so instead of displaying the 0 we can display None
        if ($portVersionFieldCount -eq 0) {
            continue
        }

        # Obtain the port-version number
        $result = $portVersionField[0] -cmatch $Script:PortVersionRegEx
        Test-RegExResult $Script:PortVersionRegEx -Result $result
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

    $regex = '"version-semver"\s*?:\s*?"{0}"\s*?,?.*' -f $Script:PortSemVersionRegEx

    # Add a new port-version number under the version-semver field (port-version will always be 1
    # in this case)
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

    # Verify that the port-version number in the $Script:VcpkgHash is still the same
    Test-SamePortVersionNumberForVcpkg `
        $PortVersionField $Script:PortVersionRegEx $portValue.versionOld $portValue.vcpkgJson

    # Replace the old version number with the bumped version number
    $portVersionBumped = $portValue.versionBumped
    return $FileContent -creplace $Script:PortVersionRegEx, "`"port-version`": $portVersionBumped,"
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

        # Obtain the port-version field with version number
        # No Test-Path check needed as vcpkg.json file paths were passed using the Resolve-Path
        $fileContent = Get-Content -Path $vcpkgJsonPath
        $portVersionField = $fileContent -cmatch $Script:PortVersionRegEx

        $portVersionFieldCount = $portVersionField.Count
        $expectedOccurrences   = @(0, 1)

        # Verify that exactly zero or one port-version field line was found in the vcpkg.json file
        Test-PortVersionFieldForVcpkg `
            $portVersionFieldCount $Script:PortVersionRegEx $vcpkgJsonPath $expectedOccurrences

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
    [OutputType([void])]
    Param()

    Write-Progress 'Removing the port-version field from vcpkg.json...'

    foreach ($portfiles in $Script:VcpkgHash.GetEnumerator()) {
        $vcpkgJsonPath = $portfiles.Value.vcpkgJson

        $fileContent = Get-Content -Path $vcpkgJsonPath

        $portVersionField = $fileContent -cmatch $Script:PortVersionRegEx

        $portVersionFieldCount = $portVersionField.Count
        $expectedOccurrences   = @(0, 1)

        # Verify that exactly zero or one port-version field line was found in the vcpkg.json file
        Test-PortVersionFieldForVcpkg `
            $portVersionFieldCount $Script:PortVersionRegEx $vcpkgJsonPath $expectedOccurrences

        # Remove the port-version field from the vcpkg.json file
        $fileContentReplaced = $fileContent | Where-Object { $_ -cnotmatch $Script:PortVersionRegEx }

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
    Param(
        [Parameter(Mandatory, HelpMessage = 'Writes an info message to the host at the end.')]
        [ValidateNotNullOrEmpty()]
        [string]
        $Message,

        [Parameter(HelpMessage = 'Specifies whether to push also tags.')]
        [switch] $FollowTags
    )

    # This is a special case, the $FollowTags indicates that the Invoke-MergeDevelopAndDeploy
    # was invoked after bumping version numbers code and in this case we also need to push tags
    # using the git push --follow-tags parameter, this is needed only after bumping version
    # numbers code.
    # It allows to have a nicer code in the main section. 🫤
    $isInvokedAfterVersionsBump = $FollowTags
    if ($isInvokedAfterVersionsBump -and $Script:IsBumpingSkipped) {
        return
    }

    NewLine
    # Test if the develop and main branches are behind the origin
    Test-GitBehindOrigin
    # Verify if the current branch is develop and the working tree is still clean
    Test-GitDevelopBranch
    Test-WorkingTreeClean

    # Allow to skip push-es (for testing)
    if (-not $Pretend) {
        NewLine
        Write-Progress 'Pushing to origin/develop branch...'
        git push --progress origin refs/heads/develop:refs/heads/develop
        Test-LastExitCode
    }

    NewLine
    Write-Progress 'Switching to main branch...'
    git switch main
    Test-LastExitCode

    NewLine
    Write-Progress 'Merging develop branch...'
    git merge --ff-only develop
    Test-LastExitCode

    # Allow to skip push-es (for testing)
    if (-not $Pretend) {
        NewLine
        Write-Progress 'Pushing to origin/main branch...'
        $followTagsArg = $FollowTags ? '--follow-tags' : $null
        git push --progress $followTagsArg origin refs/heads/main:refs/heads/main
        Test-LastExitCode
    }

    NewLine
    Write-Progress 'Switching back to develop branch...'
    git switch develop
    Test-LastExitCode

    NewLine
    Write-Info -Message $Message
}

# Update number of unit tests
function Invoke-UpdateNumberOfUnitTests {
    [OutputType([void])]
    Param()

    Write-Header 'Updating Number of Unit Tests'

    Read-NumberOfUnitTestsCurrent
    Test-NumberOfUnitTestsCurrent
    Show-NumberOfUnitTestsCurrent

    $answer = Approve-Continue 'Do you want to update number of unit tests?' -DefaultChoice 1
    if ($answer -eq 1) {
        NewLine
        Write-Error 'Skipping unit tests number update 🦘'
        return
    }

    # Read a new number of unit tests to update in all files
    Read-NumberOfUnitTestsNew
    Test-NumberOfUnitTestsNew

    Edit-NumberOfUnitTestsInAllFiles

    Show-DiffSummaryAndApprove -SummaryFor 'Number of Unit Tests'

    NewLine
    Write-Progress 'Committing updated number of unit tests...'
    git commit --all --edit --message=$(Get-UpdatedNumberOfUnitTestsCommitMessage)
    Test-LastExitCode
}

# Bump version numbers
function Invoke-BumpVersions {
    [OutputType([void])]
    Param()

    Write-Header 'Bumping version numbers' -NoNewlineAfter

    # Allow to skip bumping version numbers
    $answer = Approve-Continue 'Do you want to bump version numbers?' -DefaultChoice 0
    if ($answer -eq 1) {
        # Set global flag to skip merge and deploy
        $Script:IsBumpingSkipped = $true

        NewLine
        Write-Error 'Skipping version numbers bumping 🦘'
        return
    }

    # Select which version numbers to bump
    # TinyOrm must always be first
    $Script:BumpsHash.TinyOrm.type     = Read-BumpType -Name TinyOrm
    $Script:BumpsHash.tom.type         = Read-BumpType -Name tom
    $Script:BumpsHash.TinyDrivers.type = Read-BumpType -Name TinyDrivers
    $Script:BumpsHash.TinyMySql.type   = Read-BumpType -Name TinyMySql
    $Script:BumpsHash.TinyUtils.type   = Read-BumpType -Name TinyUtils

    Test-AllBumpsEmpty

    # Obtain version numbers from version.hpp files
    Read-VersionNumbers

    # Bump all version numbers
    Update-VersionNumbers
    Show-VersionNumbers

    # Update these version numbers in all files
    Edit-VersionNumbersInVersionHpp
    Edit-VersionNumbersInAllFiles

    Show-DiffSummaryAndApprove -SummaryFor 'Version Numbers'

    NewLine
    Write-Progress 'Committing bumped version numbers...'
    git commit --all --edit --message=$(Get-BumpCommitMessage)
    Test-LastExitCode
}

# Main logic to bump vcpkg port-version field in the tinyorm port
function Invoke-BumpPortVersions {
    [OutputType([void])]
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

# Create the tag based on the bumped version number
function Invoke-CreateTag {
    [OutputType([void])]
    Param()

    if ($null -eq $Script:TagVersion) {
        Write-Error ('Skipping tag creation because the TinyOrm library version number ' +
            'was not bumped 🦘')
        return
    }

    Write-Header "Creating $Script:TagVersion tag for TinyORM project"

    git tag --sign --edit --message=$(Get-TagMessage) $Script:TagVersion
    Test-LastExitCode
}

# Vcpkg ports update
function Invoke-UpdateVcpkgPorts {
    [OutputType([void])]
    Param()

    # Don't allow to skip updating the vcpkg ports because they must be always updated if merging
    # into the main branch (of course excluding the Pretend).

    # This feature cannot be completed without a git push, so the entire function must be skipped
    # in Pretend mode.
    if ($Pretend) {
        return
    }

    Write-Header 'Updating vcpkg ports REF and SHA512'

    # Behind and ahead tests are only needed if version numbers bumping was skipped because
    # in this case  the develop and main branches were not pushed and if these branches are ahead
    # then the vcpkg_from_github() REF option would point to the wrong commit ID as this commit ID
    # is obtained from the origin/main. Because of this we need to test ahead in this specific case.
    if ($Script:IsBumpingSkipped) {
        Test-GitAheadOrigin
        # Test behind is not needed here as it is always tested at the beginning of this script
    }

    $vcpkgRef = $Script:TagVersion ?? (Get-VcpkgRef)
    # Collect all portfiles to update (as an array)
    $portfiles = $Script:VcpkgHash | ForEach-Object { $_.Values.portfile }

    Edit-VcpkgRefAndHash -Project $Script:GitHubProject -Ref $vcpkgRef -PortFile $portfiles `
                         -RefAsVersionVariable:(-not [string]::IsNullOrEmpty($Script:TagVersion))

    # Increase the port-version fields if TinyOrm version wasn't bumped
    if ($Script:BumpsHash.TinyOrm.type -eq [BumpType]::None) {
        Invoke-BumpPortVersions
        NewLine
    }
    # Remove the port-version fields if the TinyOrm version was bumped
    else {
        Remove-PortVersions
    }

    Show-DiffSummaryAndApprove -Path (Resolve-Path .\cmake\vcpkg) -SummaryFor 'Vcpkg Port'

    NewLine
    Write-Progress 'Committing vcpkg REF and SHA512...'
    git commit --all --message=$(Get-VcpkgCommitMessage)
    Test-LastExitCode
}

# Invoke post-deploy actions that are needed to finish the deployment
function Invoke-PostDeployActions {
    [OutputType([void])]
    Param()

    Write-Header 'Invoking Post-deploy actions'

    # Remove MSYS2 UCRT64 build trees to avoid the tst_Versions test case failure (ccache bug)
    Remove-Msys2BuildTrees

    # Warnings about what is needed next to finish the deployment
    Write-PostDeployWarnings
}

# Main section
# ---

Clear-Host
Write-Header 'Deploying TinyORM library'

# Verify whether the current working tree is in the correct state (required preconditions)
Test-GitRoot
Test-GitBehindOrigin
Test-GitDevelopBranch
Test-WorkingTreeClean

# Need to initialize these variables later because of the Resolve-Path calls
Initialize-ScriptVariables

# Fire it up 🔥
Invoke-UpdateNumberOfUnitTests
Invoke-BumpVersions
Invoke-CreateTag
# Merge develop to main and and push to origin/main
Invoke-MergeDevelopAndDeploy `
    -Message 'TinyORM project was bumped and deployed successfully. 🥳' -FollowTags

Invoke-UpdateVcpkgPorts
# Merge develop to main and and push to origin/main
Invoke-MergeDevelopAndDeploy -Message 'Vcpkg ports were updated and deployed successfully. 🥳'

# Invoke post-deploy actions that are needed to finish the deployment
Invoke-PostDeployActions

<#
 .Synopsis
  The TinyORM library bumping version numbers and deployment script

 .Description
  The `deploy.ps1` script helps with bumping version numbers for all libraries and deploying these changes to the `main` branch. It also outputs all the available information and allows to confirm or cancel future processing.

  It does the following actions:

  - test the current working directory is correct
  - test if the main or develop branch are behind
  - test if the current branch is develop
  - test if the current working tree is clean
  - updating number of unit tests in all documentation files
    - obtain the current number of unit tests from the README.md file
    - verify the current number of unit tests in all files whether is the same everywhere
    - print the number of unit tests to the console
    - read the new number of unit tests to update in all files
    - verify the new number of unit tests
    - update the Number of Unit Tests in all files defined in the $Script:NumberOfUnitTestsLocations
    - display the diff summary and approve to continue
    - get the commit message for updated number of unit tests
    - commit
  - allow to skip bumping version numbers
  - ask which version numbers to bump for the `TinyOrm`, `tom`, `TinyDrivers`, `TinyMySql`, and `TinyUtils` projects
  - display bumped version numbers to verify them
  - display bumped tag number to verify it
  - update bumped version numbers in all `version.hpp` files and also in all other files
  - show diff summary
  - prepare the bump commit message with the bumped version numbers
  - commit the bump commit message
  - prepare a tag message with the bumped version number
  - create an annotated git tag
  - merge the `develop` to the `main` branch (ff-only), and push to the `origin/main`
  - update the vcpkg `tinyorm` portfile
    - obtain the `origin/main` commit ID (SHA-1) or use the tag number (based on select mode)
    - update the REF value
    - obtain the `origin/main` archive hash (SHA512)
    - update the SHA512 value
    - remove the port-version field from vcpkg.json files if TinyOrm version number was bumped
    - if TinyOrm version number wasn't bumped allow to bump the port-version number in vcpkg.json
  - prepare the vcpkg commit message
  - do the commit, merge to the `main` branch (ff-only), and push to the `origin/main`
  - invoke post-deploy actions
    - remove MSYS2 CMake Build Trees for GitHub Windows self-hosted runner
      - remove all 'TinyORM-builds-cmake\Drivers-msys2-u-*' build trees as tst_Versions test case will fail (ccache bug)
    - write to the output all other post-deploy actions that need to be done manually

  The deployment is not possible without bumping the `TinyOrm` library version number because a git tag must be created. That means that bumping only the `tom`, `TinyDrivers`, `TinyMySql`, or `TinyUtils` isn't possible. This used to be possible, but not anymore because git tags are created during deployment.

  There is a second mode that allows to completely skip bumping version numbers and in this case is possible to deploy the vcpkg ports only. The vcpkg ports will use the git commit ID of the main branch for the vcpkg_from_github REF option in this case. The first approval question allows you to select this so-called second mode.

 .INPUTS
  Nothing.

 .OUTPUTS
  Progress information.
#>
