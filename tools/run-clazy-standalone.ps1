#!/usr/bin/env pwsh

Set-StrictMode -Version 3.0

. $PSScriptRoot\private\Common-Host.ps1
. $PSScriptRoot\private\Common-Linting.ps1

if (-not (Test-Path -LiteralPath "$(Get-CompileCommandsPath @args)\compile_commands.json")) {
    Write-ExitError 'Compile database compile_commands.json doesn''t exist.' -NoNewlineBefore
}

# run-clazy-standalone.py is our own script

$platform = $PSVersionTable.Platform

switch ($platform) {
    'Win32NT' {
        python.exe '<your_path>\run-clazy-standalone.py' @args
    }
    'Unix' {
        run-clazy-standalone.py @args
    }
    Default {
        throw "$platform platform is not supported."
    }
}
