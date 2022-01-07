#!/usr/bin/env pwsh

Param(
    [Parameter(Position = 0, Mandatory = $true,
        HelpMessage = 'Specifies todo tasks to find on each line. The pattern value is treated as a regular expression.')]
    [ValidateNotNullOrEmpty()]
    [string[]] $Pattern,

    [Parameter(Position = 1, Mandatory = $false,
        HelpMessage = 'Specifies an array of one or more string patterns to be matched as the cmdlet gets child items.')]
    [ValidateNotNullOrEmpty()]
    [string[]] $Include = @('*.cpp', '*.hpp'),

    [Parameter(Position = 2, Mandatory = $false,
        HelpMessage = 'Specifies the path to the project folder, is pwd by default.')]
    [ValidateNotNullOrEmpty()]
    [string] $Path = $($(Get-Location).Path) + '\*'
)

Set-StrictMode -Version 3.0

$Script:TodoKeywordsPattern = ' (TODO|NOTE|FIXME|BUG|WARNING|CUR|FEATURE|TEST|FUTURE|CUR1|TMP) '

Get-ChildItem -Path $Path -Include $Include -Recurse | `
    Select-String -Pattern $Script:TodoKeywordsPattern -CaseSensitive | `
    Select-Object -Property `
        @{
            Name       = 'Line'
            Expression = { $_.Line.Trim().TrimStart('// ') }
        },
        LineNumber, Path | `
    Where-Object Line -Match $Pattern | `
    Sort-Object Line, Path, LineNumber
