#!/usr/bin/env pwsh

Param(
    [Parameter(Position = 0,
        HelpMessage = 'Specifies how many times all AutoTests should be invoked.')]
    [ValidateNotNullOrEmpty()]
    [int] $Count = 1,

    [Parameter(Position = 1,
        HelpMessage = 'Specifies the path to the qmake build folder, is pwd by default.')]
    [ValidateNotNullOrEmpty()]
    [string] $BuildPath = $($(Get-Location).Path)
)

Set-StrictMode -Version 3.0

. $PSScriptRoot\private\Common.ps1

$Script:TotalElapsed = 0
$Script:AverageElapsed = 0
$Script:EnvPathBackup = $env:Path
$Script:Slashes = $null

function Initialize-Environment {
    [OutputType([void])]
    Param()

    Write-Host
    Write-Host 'Initializing environment...' -ForegroundColor DarkBlue
    Write-Host

    Initialize-DBEnvironment
    Initialize-RuntimeEnvironment

    Write-Host
}

# Source database env. variables if dotenv.ps1 exists
function Initialize-DBEnvironment {
    [OutputType([void])]
    Param()

    if ((Test-Path env:DB_MYSQL_USERNAME) -or (Test-Path env:DB_PGSQL_USERNAME) -or `
        (Test-Path env:DB_SQLITE_DATABASE)
    ) {
        return
    }

    $dotenv = Get-ChildItem -Path $PSScriptRoot/.. -Include 'dotenv.ps1' -Recurse -File | `
        Select-Object -First 1 -ExpandProperty FullName

    if ($null -eq $dotenv) {
        Write-Verbose "dotenv.ps1 file not found, look at tests/testdata/dotenv.example.ps1."
        return
    }

    . $dotenv

    Write-Host "Sourced env. file: " -ForegroundColor DarkGreen -NoNewline
    Write-Host $dotenv
}

function Export-SharedLibraries {
    [CmdletBinding(PositionalBinding = $false)]
    [OutputType([string[]])]
    Param(
        [Parameter(Position = 0, Mandatory, ValueFromPipeline, ValueFromPipelinebyPropertyName,
            HelpMessage = "Specifies path to export.")]
        [string[]]
        $Path
    )

    process {
        # Check if they are already on the $env:Path
        $dllPathsToAdd = @()
        if (-not ($env:Path -eq '')) {
            foreach ($dllPath in $Path) {
                $dllPathToMatch = Get-PathToMatch -Path $dllPath

                if ($env:Path -notmatch $dllPathToMatch) {
                    $dllPathsToAdd += Get-FullPath -Path $dllPath
                }
            }
        }
        # When the Path env. variable value is empty then add all Paths
        else {
            $dllPathsToAdd = Get-FullPath -Path $Path
        }

        # Nothing to pipe
        if ($dllPathsToAdd.Length -eq 0) {
            return
        }

        # Add to the $env:Path
        $dllPathsJoined = $dllPathsToAdd -join [IO.Path]::PathSeparator
        $env:Path = $dllPathsJoined, $env:Path -join [IO.Path]::PathSeparator

        # Pipe exported path/s
        Write-Output $dllPathsToAdd
    }
}

# Export TinyOrm and TinyUtils dll folders
function Initialize-RuntimeEnvironment {
    [OutputType([void])]
    Param()

    $platform = $PSVersionTable.Platform

    switch ($platform) {
        'Win32NT' {
            # Find dll-s parent folders
            $exportedPaths = Get-ChildItem -Path $BuildPath -Include 'Tiny*.dll' -Recurse -File
                | Select-Object -ExpandProperty DirectoryName
                | Export-SharedLibraries
        }
        'Unix' {
            # Find dll-s parent folders (pwsh does not work with print0 :/)
            $exportedPaths = find $BuildPath -type f -and -executable -and -name 'lib*.so*' -print
                | xargs dirname | xargs realpath
                | Export-SharedLibraries
        }
        Default {
            throw "$platform platform is not supported."
        }
    }

    Write-Host "Exported paths: " -ForegroundColor DarkGreen
    $exportedPaths
}

function Get-Tests {
    if ($PSVersionTable.Platform -ceq 'Unix') {
        return find $BuildPath -type f -and -executable -and `
            -not -name '*.sh' -and -not -name 'lib*.so*'
    }
    else {
        return Get-ChildItem -Path $BuildPath -Include '*.exe' -Recurse
    }
}

function Get-Elapsed {
    [CmdletBinding(PositionalBinding = $false)]
    [OutputType([int])]
    Param(
        [Parameter(Position = 0, Mandatory,
            HelpMessage = 'Specifies the RegEx pattern to look up the elapsed value.')]
        [ValidateNotNullOrEmpty()]
        [string]
        $Pattern,

        [Parameter(Position = 1, Mandatory,
            HelpMessage = 'Specifies the string in which to find the elapsed value.')]
        [ValidateNotNullOrEmpty()]
        [string]
        $String
    )

    $matchInfo = Select-String -Pattern $Pattern -InputObject $String

    if ($null -eq $matchInfo) {
        Write-Host
        throw "Pattern 'elapsed' not found, any RegEx matches."
    }

    $elapsed = $matchInfo.Matches.Groups
        | Where-Object Name -eq elapsed
        | Select-Object -ExpandProperty Value -Last 1

    return $elapsed
}

function Write-HighlightedOutput {
    [CmdletBinding(PositionalBinding = $false)]
    [OutputType([void])]
    Param(
        [Parameter(Position = 0, Mandatory,
            HelpMessage = 'Specifies the Test Output in which to find and highlight the elapsed ' +
                'value.')]
        [ValidateNotNullOrEmpty()]
        [string[]]
        $Value
    )

    $pattern = ', (?<elapsed>\d+)ms'
    $matchInfo = Select-String -Pattern $pattern -InputObject $Value[1]

    if ($null -eq $matchInfo) {
        Write-Host
        throw "Pattern ', (?<elapsed>\d+)ms' not found, any RegEx matches."
    }

    $elapsed = $matchInfo.Matches.Groups
        | Where-Object Name -eq elapsed
        | Select-Object -ExpandProperty Value -Last 1

    Write-Host $Value[0] -ForegroundColor DarkCyan
    Write-Host "$($Value[1] -replace $pattern, ''), " -NoNewline
    Write-Host "$($elapsed)ms" -ForegroundColor DarkGreen
}

function Invoke-Tests {
    [OutputType([void])]
    Param()

    for ($i = 1; $i -le $Count; $i++) {
        if ($i -gt 1) {
            Write-Host
        }
        Write-Host "$i. AutoTests Run" -ForegroundColor DarkBlue
        Write-Host

        $testsElapsed = 0

        foreach ($test in Get-Tests) {
            $testOutput = & $test -silent

            if ($LASTEXITCODE -ne 0) {
                Write-Host
                throw "Last Exit code was not 0, it was: $LASTEXITCODE"
            }

            # Execution time of the current test
            $testsElapsed += Get-Elapsed -Pattern ', (?<elapsed>\d+)ms' `
                -String $($testOutput -join "`n")

            Write-HighlightedOutput -Value $testOutput
        }

        Write-Host
        Write-Host "Tests Execution time : " -NoNewline
        Write-Host "$($testsElapsed)ms" -ForegroundColor Green

        $Script:TotalElapsed += $testsElapsed
    }

    Write-Host
    Write-Host "All AutoTests Execution time : " -NoNewline
    Write-Host "$($Script:TotalElapsed)ms" -ForegroundColor Green

    [int] $averageElapsed = $Script:TotalElapsed / $Count
    Write-Host "All AutoTests Average Execution time : " -NoNewline
    Write-Host "$($averageElapsed)ms" -ForegroundColor Green
}

Initialize-Environment

try {
    Invoke-Tests
}
finally {
    $env:Path = $Script:EnvPathBackup
}
