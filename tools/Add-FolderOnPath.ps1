#!/usr/bin/env pwsh

[CmdletBinding()]
Param(
    [Parameter(Position = 0, ValueFromPipeline, ValueFromPipelinebyPropertyName,
        HelpMessage = 'Specifies the folder to add on the system path, is pwd by default.')]
    [ValidateNotNullOrEmpty()]
    [string[]] $Path = $($(Get-Location).Path),

    [Parameter(Position = 1, ValueFromPipelinebyPropertyName,
        HelpMessage = 'Specifies the environment variable to which to add a Path, is env:PATH by default.')]
    [string] $Variable,

    [Parameter(HelpMessage = 'Add a Path to the LD_LIBRARY_PATH environment variable (Variable parameter has higher priority).')]
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

    function Initialize-EnvVariable {
        [OutputType([string[]])]
        Param()

        # Initialize by passed Variable value
        if ($Variable) {
            if ($Variable.StartsWith('env:')) {
                return $Variable, ($Variable.TrimStart('env:'))
            }

            return "env:$Variable", $Variable
        }

        $platform = $PSVersionTable.Platform

        # Initialize by platform defaults
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
        if ($RestorePath -or $Global:TinyBackedUpVariables.Contains($($Script:envVariableRaw))) {
            return
        }

        # Keep track of backed up env. variable names
        $Global:TinyBackedUpVariables += $Script:envVariableRaw

        $globalName = "Tiny_$Script:envVariableRaw"

        # Backup to global variable
        if (Test-Path $Script:envVariable) {
            Set-Variable -Scope global -Name $globalName -Value (Get-Item $Script:envVariable).Value
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
            Set-Item -Path "env:$variable" -Value (Get-Variable -Scope global "Tiny_$variable").Value

            Redo-GlobalState -Variable $variable
        }

        Remove-Variable -Scope global -Name 'TinyBackedUpVariables' -ErrorAction SilentlyContinue
    }

    # Get slashes by platform
    function Get-Slashes {
        [OutputType([string])]
        Param()

        # Cached value
        if ($Script:Slashes) {
            return $Script:Slashes
        }

        $platform = $PSVersionTable.Platform

        switch ($platform) {
            'Win32NT' {
                return $Script:Slashes = '\\/'
            }
            'Unix' {
                return $Script:Slashes = '/'
            }
            Default {
                throw "$platform platform is not supported."
            }
        }
    }

    function Get-PathToMatch {
        [OutputType([string])]
        Param(
            [Parameter(Position = 0, Mandatory, ValueFromPipeline, ValueFromPipelinebyPropertyName,
                HelpMessage = "Specifies a path to modify to the match pattern.")]
            [ValidateNotNullOrEmpty()]
            [string]
            $Path
        )

        $slashes = Get-Slashes

        $pathToMatch = $Path `
            -replace "[$slashes]+(?![$slashes]*$)", "[$slashes]+" `
            -replace "[$slashes]+$", ''

        $separator = [IO.Path]::PathSeparator

        return "(?:^|$separator)$pathToMatch[$slashes]*(?:$|$separator)"
    }

    function Get-FullPath {
        [CmdletBinding()]
        [OutputType([string[]])]
        Param(
            [Parameter(Position = 0, Mandatory, ValueFromPipeline, ValueFromPipelinebyPropertyName,
                HelpMessage = "Specifies a path to normalize.")]
            [string[]]
            $Path
        )

        process {
            [string[]] $normalized = @()

            foreach ($p in $Path) {
                $normalized += [System.IO.Path]::GetFullPath(($p -replace "[$(Get-Slashes)]+$", ''))
            }

            return $normalized
        }
    }

    function Export-Path {
        [CmdletBinding()]
        [OutputType([void])]
        Param(
            [Parameter(Position = 0, Mandatory, ValueFromPipeline, ValueFromPipelinebyPropertyName,
                HelpMessage = "Specifies a path to export.")]
            [string[]]
            $Path
        )

        process {
            # Value of env. variable
            if (Test-Path $Script:envVariable) {
                $variableValue = Get-Item $Script:envVariable | Select-Object -ExpandProperty Value
            }
            # Set to empty value if env. variable doesn't exist
            else {
                $variableValue = ''
            }

            # Check if paths to add are already on the $Script:envVariable ($env:$Variable)
            $pathsToAdd = @()
            if (-not ($variableValue -eq '')) {
                foreach ($pathToAdd in $Path) {
                    $pathToMatch = Get-PathToMatch -Path $pathToAdd

                    if ($variableValue -notmatch $pathToMatch) {
                        $pathsToAdd += Get-FullPath -Path $pathToAdd
                    }
                }
            }
            # When env. variable value is empty then add all Paths
            else {
                $pathsToAdd = Get-FullPath -Path $Path
            }

            # Nothing to add
            if ($pathsToAdd.Length -eq 0) {
                return
            }

            $pathsJoined = $pathsToAdd -join [IO.Path]::PathSeparator

            # Env. variable value is empty
            if (-not $variableValue) {
                $pathsFinal = $pathsJoined
            }
            # Prepend vs Append
            elseif ($Append) {
                $pathsFinal = $($variableValue, $pathsJoined)
            }
            else {
                $pathsFinal = $($pathsJoined, $variableValue)
            }

            Set-Item -Path $Script:envVariable -Value ($pathsFinal -join [IO.Path]::PathSeparator)
        }
    }

    if ($RestorePath) {
        Restore-Environment
        exit 0
    }

    $Script:envVariable, $Script:envVariableRaw = Initialize-EnvVariable
}

process {
    try {
        Backup-Environment

        Export-Path -Path $Path
    }
    catch {
        # Clean global state when something goes wrong
        if ($Script:envVariableRaw) {
            Redo-GlobalState -Variable $($Script:envVariableRaw)
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

   Or

   '/tmp', '/usr/local/bin' | Add-FolderOnPath -Append

 .Example
   # Restores a current environment to its original state
   Add-FolderOnPath -RestorePath
#>
