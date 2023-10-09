#!/usr/bin/env pwsh

Set-StrictMode -Version 3.0

# This file contains common functions for Lint-TinyORM.ps1 related scripts

# Functions section
# ---

# Test whether the compile commands database exists
function Get-CompileCommandsPath {
    [OutputType([string])]
    Param()

    $regEx = '-p=(?<path>.*)'
    $pArgs = $args -cmatch $regEx
    $containsPArgs = $pArgs.Count -ge 1

    # Get the last -p path
    if ($containsPArgs) {
        $pArgs[$pArgs.Count - 1] -cmatch $regEx | Out-Null
        $compileCommandsPath = Resolve-Path -Path $Matches.path
    }
    # Use the PWD
    else {
        $compileCommandsPath = Resolve-Path -Path .
    }

    return $compileCommandsPath
}
