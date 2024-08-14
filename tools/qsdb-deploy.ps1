#!/usr/bin/env pwsh

Param(
    [Parameter(HelpMessage = 'Specifies the parent CMake/qmake build trees path.')]
    [ValidateNotNullOrEmpty()]
    [string] $BuildTreesPath,

    [Parameter(Mandatory,
        HelpMessage = 'Specifies the Qt version for which to deploy executables.')]
    [ValidateNotNullOrEmpty()]
    [ValidatePattern('^[5-9]\.\d{1,2}\.\d{1,2}$',
        ErrorMessage = 'The argument "{0}" is not the correct Qt version number. ' +
        'The argument "{0}" does not match the "{1}" pattern.')]
    [string] $QtVersion,

    [Parameter(
        HelpMessage = 'Specifies the CMake/qmake build types for which to deploy executables.')]
    [ValidateNotNullOrEmpty()]
    [ValidateSet('Release', 'RelWithDebInfo', 'MinSizeRel', 'Profile')]
    [string] $BuildType = 'Release',

    [Parameter(HelpMessage = 'Specifies the build tree from which to deploy executables.')]
    [ValidateNotNullOrEmpty()]
    [string] $BuildFolderTemplate = 'build-TinyORM-Desktop_Qt_{0}_MSVC2022_64bit-{1}',

    [Parameter(Mandatory, HelpMessage = 'Specifies the destination deploy path.')]
    [ValidateNotNullOrEmpty()]
    [string] $DestinationPath,

    [Parameter(Mandatory,
        HelpMessage = 'Specifies the major version number for TinyORM libraries.')]
    [ValidateNotNull()]
    [uint] $MajorVersion,

    [Parameter(Mandatory,
        HelpMessage = 'Specifies the build system that was used to build the TinyORM project.')]
    [ValidateNotNullOrEmpty()]
    [ValidateSet('CMake', 'qmake', IgnoreCase = $false)]
    [string] $BuildSystem
)

Set-StrictMode -Version 3.0

# Includes
# ---
. $PSScriptRoot\private\Common-Host.ps1

# Script variables section
# ---

# Determine an executable artifact type
enum ArtifactType {
    Executable
    SharedLibrary
}

# Computed build tree path
$Script:BuildTreePath = $null
# Deploy hash of executables to deploy
$Script:DeployHash = $null
# Deploy array of executables' filepaths to deploy (allows to copy all artifacts at once)
$Script:DeployArray = @()
# PDB filepaths to remove at deploy destination if they didn't exist in the build tree
$Script:PdbToRemoveArray = @()

# Functions section
# ---

# Initialize build tree path from which to deploy executables
function Initialize-BuildTreePath {
    [OutputType([void])]
    Param()

    $buildFolder = $BuildFolderTemplate -f $QtVersion.Replace('.', '_'), $BuildType

    $Script:BuildTreePath = Join-Path -Path $BuildTreesPath -ChildPath $buildFolder
}

# Initialize deploy hash of executables to deploy
function Initialize-DeployHash {
    [OutputType([void])]
    Param()

    $Script:DeployHash = [ordered] @{
        TinyDrivers  = @{
            type            = [ArtifactType]::SharedLibrary
            path            = "${Script:BuildTreePath}\drivers\common\"
            skipIfNotExists = $true
            appendVersion   = $true
        }
        TinyMySql    = @{
            type            = [ArtifactType]::SharedLibrary
            path            = "${Script:BuildTreePath}\drivers\mysql\"
            skipIfNotExists = $true
            appendVersion   = $true
        }
        TinyOrm      = @{
            type            = [ArtifactType]::SharedLibrary
            # This is a special case as this path is different for CMake/qmake
            path            = $BuildSystem -ceq 'qmake' ? "${Script:BuildTreePath}\src\"
                                                        : "${Script:BuildTreePath}\"
            skipIfNotExists = $false
            appendVersion   = $true
        }
        tom_testdata = @{
            type            = [ArtifactType]::Executable
            path            = "${Script:BuildTreePath}\tests\testdata_tom\"
            skipIfNotExists = $false
            appendVersion   = $false
        }
        tom          = @{
            type            = [ArtifactType]::Executable
            path            = "${Script:BuildTreePath}\examples\tom\"
            skipIfNotExists = $false
            appendVersion   = $false
        }
    }
}

# Compute an executable artifact's path based on the build system (different for CMake/qmake)
function Get-ArtifactPath {
    [OutputType([string])]
    Param(
        [Parameter(Mandatory, HelpMessage = "Specifies artifact's deploy path.")]
        [ValidateNotNullOrEmpty()]
        [string]
        $DeployPath
    )

    # Nothing to do, for CMake are paths correct by default and the debug_and_release qmake's
    # CONFIG option isn't defined on Unix by default, so no need to append the release/ sub-path
    if ($BuildSystem -ceq 'CMake' -or $PSVersionTable.Platform -ceq 'Unix') {
        return $DeployPath
    }

    # The debug_and_release qmake's CONFIG option is defined by default on Windows, so append
    # the release/ sub-path
    return Join-Path -Path $DeployPath -ChildPath 'release'
}

# Get an executable artifact's file extension
function Get-ArtifactExtension {
    [OutputType([string])]
    Param(
        [Parameter(Mandatory, HelpMessage = "Specifies artifact's type.")]
        [ArtifactType]
        $Type
    )

    switch ($Type) {
        ([ArtifactType]::Executable)    { return 'exe' }
        ([ArtifactType]::SharedLibrary) { return 'dll' }
        Default {
            throw 'Unexpected value for enum {0}.' -f [ArtifactType].Name
        }
    }
}

# Compute an executable artifact's filename based on the given arguments
function Get-ArtifactFilename {
    [OutputType([string])]
    Param(
        [Parameter(Mandatory, HelpMessage = "Specifies artifact's basename.")]
        [ValidateNotNullOrEmpty()]
        [string]
        $Basename,

        [Parameter(Mandatory, HelpMessage = "Specifies artifact's type.")]
        [ArtifactType]
        $Type
    )

    $artifactBasename = $deployValue.appendVersion ? $Basename + 0 : $Basename
    $artifactExtension = Get-ArtifactExtension -Type $Type

    return '{0}.{1}' -f $artifactBasename, $artifactExtension
}

# Determine whether to skip the executable artifact if doesn't exists
function Test-ShouldSkipArtifact {
    [OutputType([bool])]
    Param(
        [Parameter(HelpMessage = 'Specifies the parent CMake/qmake build trees path.')]
        [ValidateNotNullOrEmpty()]
        [string] $ArtifactFilePath,

        [Parameter(HelpMessage = 'Specifies the parent CMake/qmake build trees path.')]
        [ValidateNotNull()]
        [bool] $SkipIfNotExists
    )

    # Skip the executable artifact if doesn't exists
    if ($SkipIfNotExists -and -not (Test-Path -Path $artifactFilePath)) {
        return $true
    }

    # Don't check all artifacts if exists as I want to exit if it doesn't exists
    if (-not $SkipIfNotExists -and -not (Test-Path -Path $artifactFilePath)) {
        Write-ExitError "Executable artifact doesn't exist: $artifactFilePath" -NoNewlineBefore
    }

    return $false
}

# Append the PDB file to remove at the deploy destination if the PDB file isn't in the build tree
function Add-PdbToRemove {
    [OutputType([void])]
    Param(
        [Parameter(Mandatory, HelpMessage = "Specifies the PDB filename.")]
        [ValidateNotNullOrEmpty()]
        [string]
        $PdbFilename
    )

    # Compute artifact's PDB filepath at deploy destination
    $pdbDeployFilePath = Join-Path -Path $DestinationPath -ChildPath $pdbFilename

    # Nothing to do
    if (-not (Test-Path -Path $pdbDeployFilePath)) {
        return
    }

    $Script:PdbToRemoveArray += $pdbDeployFilePath
}

# Append the PDB file to deploy array or to the PDB files to remove array based on if it exists
function Sync-PdbFile {
    [OutputType([void])]
    Param(
        [Parameter(Mandatory, HelpMessage = "Specifies the PDB filename in the build tree.")]
        [ValidateNotNullOrEmpty()]
        [string]
        $PdbFilepath,

        [Parameter(Mandatory, HelpMessage = "Specifies the PDB filename.")]
        [ValidateNotNullOrEmpty()]
        [string]
        $PdbFilename
    )

    # Deploy the PDB file only if it exists
    if (Test-Path -Path $pdbFilePath) {
        $Script:DeployArray += $pdbFilePath
    }
    # Append the PDB file to remove at the deploy destination if the PDB file isn't
    # in the build tree
    else {
        Add-PdbToRemove -PdbFilename $pdbFilename
    }
}

# Pre-compute executable artifacts' filepaths (allows to copy all artifacts at once)
function Initialize-DeployArray {
    [OutputType([void])]
    Param()

    foreach ($deployRow in $Script:DeployHash.GetEnumerator()) {
        $deployName = $deployRow.Name
        $deployValue = $deployRow.Value

        # Compute artifact's absolute filepath
        $artifactFilename = Get-ArtifactFilename -Basename $deployName -Type $deployValue.type
        $artifactPath = Get-ArtifactPath -DeployPath $deployValue.path
        $artifactFilePath = Join-Path -Path $artifactPath -ChildPath $artifactFilename

        # Skip the executable artifact if it doesn't exist or exit the script if
        # the skipIfNotExists flag is set to $false and an artifact doesn't exist 🤔
        if (Test-ShouldSkipArtifact `
            -ArtifactFilePath $artifactFilePath `
            -SkipIfNotExists $deployValue.skipIfNotExists
        ) {
            continue
        }

        # Compute artifact's PDB filepath
        $pdbFilename = [System.IO.Path]::ChangeExtension($artifactFilename, '.pdb')
        $pdbFilePath = Join-Path -Path $deployValue.path -ChildPath $pdbFilename

        # Append results to the deploy array
        $Script:DeployArray += $artifactFilePath

        # Append the PDB file to deploy array or to the PDB files to remove array
        # based whether it exists
        Sync-PdbFile -PdbFilepath $pdbFilePath -PdbFilename $pdbFilename
    }
}

# Main section
# ---

# Initialize build tree path from which to deploy executables
Initialize-BuildTreePath
# Initialize deploy hash of executables to deploy
Initialize-DeployHash
# Pre-compute executable artifacts' filepaths (allows to copy all artifacts at once)
Initialize-DeployArray

# Copy all at once
Copy-Item -Path $Script:DeployArray -Destination $Script:DestinationPath
# Remove all PDB files at once
if ($Script:PdbToRemoveArray.Count -ne 0) {
    Remove-Item -Path $Script:PdbToRemoveArray -Force
}

Write-Info ('TinyORM was deployed successfully. 🥳 ({0})' -f $BuildSystem)
