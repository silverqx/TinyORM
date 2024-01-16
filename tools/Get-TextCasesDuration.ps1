#!/usr/bin/env pwsh

Set-StrictMode -Version 3.0

$result = 0

Get-Content .\pp.txt
    | Select-String -Pattern '^  <Duration msecs="(.*)"/>'
    | ForEach-Object {
        $_ -cmatch '<Duration msecs="(.*)"/>' | Out-Null
        [float] $Matches[1]
    }
    | ForEach-Object { $result += $_ }

$result / 1000
