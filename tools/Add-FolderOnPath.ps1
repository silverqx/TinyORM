#!/usr/bin/env pwsh

[CmdletBinding()]
Param(
    [Parameter(Position = 0, ValueFromPipeline, ValueFromPipelinebyPropertyName,
        HelpMessage = 'Specifies the folder to add on the system path, is pwd by default.')]
    [ValidateNotNullOrEmpty()]
    [string[]] $Path = $($(Get-Location).Path),

    [Parameter(Position = 1, ValueFromPipelinebyPropertyName,
        HelpMessage = 'Specifies the environment variable to which to add a Path, is env:PATH ' +
            'by default.')]
    [string] $Variable,

    [Parameter(HelpMessage = 'Add a Path to the LD_LIBRARY_PATH environment variable (Variable ' +
        'parameter has higher priority).')]
    [switch] $LibraryPath,

    [Parameter(HelpMessage = 'Restore original path values for all changed environment variables.')]
    [switch] $RestorePath,

    [Parameter(HelpMessage = 'Append to the environment variable (default action is prepend).')]
    [switch] $Append
)

begin {
    Set-StrictMode -Version 3.0

    if (-not (Test-Path 'variable:global:TinyBackedUpVariables')) {
        $Global:TinyBackedUpVariables = @()
    }
    $Script:Slashes = $null

    . $PSScriptRoot\private\Common-Path.ps1

    function Get-EnvVariable {
        [OutputType([string[]])]
        Param()

        # Get by passed Variable value
        if ($Variable) {
            if ($Variable.StartsWith('env:')) {
                return $Variable, ($Variable.TrimStart('env:'))
            }

            return "env:$Variable", $Variable
        }

        $platform = $PSVersionTable.Platform

        # Get by platform defaults
        switch ($platform) {
            'Win32NT' {
                return 'env:Path', 'Path'
            }
            'Unix' {
                if ($LibraryPath) {
                    return 'env:LD_LIBRARY_PATH', 'LD_LIBRARY_PATH'
                }
                return 'env:PATH', 'PATH'
            }
            Default {
                throw "$platform platform is not supported."
            }
        }
    }

    function Backup-Environment {
        [OutputType([void])]
        Param()

        # Restore mode or a env. variable value already backed up
        if ($RestorePath -or $Global:TinyBackedUpVariables.Contains($($Script:EnvVariableRaw))) {
            return
        }

        # Keep track of backed up env. variable names
        $Global:TinyBackedUpVariables += $Script:EnvVariableRaw

        $globalName = "Tiny_$Script:EnvVariableRaw"

        # Backup to global variable
        if (Test-Path $Script:EnvVariable) {
            Set-Variable -Scope global -Name $globalName `
                -Value (Get-Item $Script:EnvVariable).Value
        }
        # Env. variable to back up is empty, so set global variable to $null
        else {
            Set-Variable -Scope global -Name $globalName -Value $null
        }
    }

    function Redo-GlobalState {
        [OutputType([void])]
        Param(
            [Parameter(Position = 0, Mandatory,
                HelpMessage = 'Specifies a variable name for which to clean the global state.')]
            [ValidateNotNullOrEmpty()]
            [string] $Variable
        )

        Remove-Variable -Scope global -Name "Tiny_$Variable" -ErrorAction SilentlyContinue

        $Global:TinyBackedUpVariables = $Global:TinyBackedUpVariables.Where({ $_ -ne $Variable })
    }

    function Restore-Environment {
        [OutputType([void])]
        Param()

        foreach ($variable in $Global:TinyBackedUpVariables) {
            Set-Item -Path "env:$variable" `
                -Value (Get-Variable -Scope global "Tiny_$variable").Value

            Redo-GlobalState -Variable $variable
        }

        Remove-Variable -Scope global -Name 'TinyBackedUpVariables' -ErrorAction SilentlyContinue

        Write-Host 'Restored' -ForegroundColor DarkGreen
    }

    # Obtain paths to add and excluded paths
    function Get-Paths {
        [CmdletBinding(PositionalBinding = $false)]
        [OutputType([string[]])]
        Param(
            [Parameter(Mandatory,
                HelpMessage = 'Specifies a value of env. variable, set to the $null value if ' +
                    "env. variable doesn't exist.")]
            [AllowEmptyString()]
            [string]
            $VariableValue
        )

        # Check if paths to add are already on the $Script:EnvVariable ($env:$Variable)
        $pathsToAdd = @()
        $pathsExcluded = @()
        if (-not ($VariableValue -eq '')) {

            foreach ($pathToAdd in $Path) {
                $pathToMatch = Get-PathToMatch -Path $pathToAdd

                $pathToAddNormalized = $(Get-FullPath -Path $pathToAdd)

                if ($VariableValue -notmatch $pathToMatch) {
                    $pathsToAdd += $pathToAddNormalized
                }
                else {
                    $pathsExcluded += $pathToAddNormalized
                }
            }
        }
        # When env. variable value is empty then add all Paths
        else {
            $pathsToAdd = Get-FullPath -Path $Path
        }

        return $pathsToAdd, $pathsExcluded
    }

    # Get final paths to add to the env. variable
    function Join-PathsToAdd {
        [CmdletBinding(PositionalBinding = $false)]
        [OutputType([string])]
        Param(
            [Parameter(Mandatory,
                HelpMessage = 'Specifies a value of env. variable, set to the $null value if ' +
                    "env. variable doesn't exist.")]
            [AllowEmptyString()]
            [string]
            $VariableValue,

            [Parameter(Mandatory,
                HelpMessage = 'Specifies added paths to the env. variable.')]
            [AllowEmptyCollection()]
            [string[]]
            $PathsToAdd
        )

        $pathsJoined = $PathsToAdd -join [IO.Path]::PathSeparator

        # Env. variable value is empty
        if ($VariableValue -eq '') {
            $pathsFinal = $pathsJoined
        }
        # Prepend vs Append
        elseif ($Append) {
            $pathsFinal = $($VariableValue, $pathsJoined)
        }
        else {
            $pathsFinal = $($pathsJoined, $VariableValue)
        }

        return $pathsFinal -join [IO.Path]::PathSeparator
    }

    function Write-Result {
        [CmdletBinding(PositionalBinding=$false)]
        [OutputType([void])]
        Param(
            [Parameter(Mandatory,
                HelpMessage = 'Specifies a value of env. variable, set to the $null value if ' +
                    "env. variable doesn't exist.")]
            [AllowEmptyString()]
            [string]
            $VariableValue,

            [Parameter(Mandatory,
                HelpMessage = "Specifies added paths to the env. variable.")]
            [AllowEmptyCollection()]
            [string[]]
            $PathsToAdd,

            [Parameter(Mandatory,
                HelpMessage = 'Specifies excluded paths from the env. variable (if already ' +
                    'contains).')]
            [AllowEmptyCollection()]
            [string[]]
            $PathsExcluded
        )

        $added = $Append ? 'appended' : 'prepended'

        if ($VariableValue -eq '') {
            Write-Host "All paths were $added." -ForegroundColor DarkGreen
            return
        }

        if ($PathsToAdd.Length -gt 0) {
            $addedCapital = $Append ? 'Appended' : 'Prepended'
            $pathsToAddJoined = $PathsToAdd -join ', '

            Write-Host "$($addedCapital): " -ForegroundColor DarkGreen -NoNewline
            Write-Host $pathsToAddJoined
        }

        if ($PathsExcluded.Length -gt 0) {
            $pathsExcludedJoined = $PathsExcluded -join ', '

            Write-Host "Already contains: $pathsExcludedJoined" -ForegroundColor DarkGray
        }
    }

    function Export-Path {
        [CmdletBinding(PositionalBinding = $false)]
        [OutputType([void])]
        Param(
            [Parameter(Position = 0, Mandatory, ValueFromPipeline, ValueFromPipelinebyPropertyName,
                HelpMessage = "Specifies a path to export.")]
            [string[]]
            $Path
        )

        begin {
            Write-Host
            Write-Host "Updating `$$Script:EnvVariable environment variable..." `
                -ForegroundColor DarkBlue
            Write-Host
        }

        process {
            # Value of env. variable, set to the $null value if env. variable doesn't exist
            $variableValue = Get-Item $Script:EnvVariable -ErrorAction SilentlyContinue
                | Select-Object -ExpandProperty Value

            # Obtain paths to add and excluded paths
            $pathsToAdd, $pathsExcluded = Get-Paths -VariableValue $variableValue

            # Nothing to add
            if ($pathsToAdd.Length -eq 0) {
                return
            }

            # Get final paths to add to the env. variable
            $pathsFinal = Join-PathsToAdd -VariableValue $variableValue -PathsToAdd $pathsToAdd

            Set-Item -Path $Script:EnvVariable -Value $pathsFinal
        }

        end {
            Write-Result -VariableValue $variableValue -PathsToAdd $pathsToAdd `
                -PathsExcluded $pathsExcluded
        }
    }

    if ($RestorePath) {
        Restore-Environment
        exit 0
    }

    $Script:EnvVariable, $Script:EnvVariableRaw = Get-EnvVariable
}

process {
    try {
        Backup-Environment

        Export-Path -Path $Path
    }
    catch {
        # Clean global state when something goes wrong
        if ($Script:EnvVariableRaw) {
            Redo-GlobalState -Variable $($Script:EnvVariableRaw)
        }

        throw
    }
}

<#
 .Synopsis
  Prepends or appends a path to the defined environment variable.

 .Description
  `Add-FolderOnPath` script prepends or appends a path to the environment variable defined by
  the Variable parameter or PATH env. variable by default. A path is prepended by default or
  you can use the Append parameter to append a path. You can also use the LibraryPath parameter
  that adds a path to the `LD_LIBRARY_PATH` environment variable.

  The RestorePath parameter restores a current environment to its original state. It tracks all
  the changed environment variables and after the call, all the changed env. variables will be
  restored at once.

  A path will not be added when an environment variable already contains this path. You can
  add more paths add once and the script also works on Unix systems.

 .Parameter Path
  Specifies the folder to add on the system path, is pwd by default.

 .Parameter Variable
  Specifies the environment variable to which to add a Path, is env:PATH by default.

 .Parameter LibraryPath
  Add a Path to the LD_LIBRARY_PATH environment variable (Variable parameter has higher priority).

 .Parameter RestorePath
  Restore original path values for all changed environment variables.

 .Parameter Append
  Append to the environment variable (default action is prepend).

 .INPUTS
  System.String
    You can pipe a string that contains a path to `Add-FolderOnPath` script.

 .OUTPUTS
  Nothing.

 .Example
   # Prepends /tmp path to the $env:Path environment variable
   Add-FolderOnPath -Path /tmp

 .Example
   # Prepends /tmp path to the $env:LIBRARY_PATH environment variable
   Add-FolderOnPath -Path /tmp -Variable LIBRARY_PATH

 .Example
   # Appends /tmp path to the $env:Path environment variable
   Add-FolderOnPath -Path /tmp -Append

 .Example
   # Appends /tmp and /usr/local/bin paths to the $env:Path environment variable
   Add-FolderOnPath -Path /tmp, /usr/local/bin -Append

   # Or

   '/tmp', '/usr/local/bin' | Add-FolderOnPath -Append

 .Example
   # Restores a current environment to its original state
   Add-FolderOnPath -RestorePath
#>
