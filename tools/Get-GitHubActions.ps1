#!/usr/bin/env pwsh

Set-StrictMode -Version 3.0

Get-Content -Path (Resolve-Path -Path $PSScriptRoot\..\.github\workflows\*.yml)
    | Select-String -Pattern '(?:uses: +)(?<action>.*)$' -NoEmphasis
    | ForEach-Object { $_.Matches[0].Groups['action'].Value }
    | Sort-Object -Unique
