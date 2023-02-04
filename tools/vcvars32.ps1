#!/usr/bin/env pwsh

Param(
    [Parameter(Position = 0, HelpMessage = 'Visual Studio version.')]
    [ValidateNotNull()]
    [ValidatePattern('2019|2022')]
    [Int] $Version = 2022
)

Set-StrictMode -Version 3.0

$programFiles = $Version -gt 2019 ? 'Program Files' : 'Program Files (x86)'

cmd.exe /c "call `"C:\$programFiles\Microsoft Visual Studio\$Version\Community\VC\Auxiliary\Build\vcvars32.bat`" && set > %TEMP%\vcvars32_$Version.tmp"

Get-Content "$env:TEMP\vcvars32_$Version.tmp" | Foreach-Object {
    if ($_ -match "^(.*?)=(.*)$") {
        Set-Content "env:\$($matches[1])" $matches[2]
    }
}
