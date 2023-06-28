#!/usr/bin/env pwsh

Param(
    [Parameter(Position = 0, Mandatory,
        HelpMessage = 'Specifies todo tasks to find on each line. The pattern value is treated ' +
            'as a regular expression.')]
    [ValidateNotNullOrEmpty()]
    [string] $Pattern,

    [Parameter(Position = 1,
        HelpMessage = 'Specifies the path to the project folder, is pwd by default.')]
    [ValidateNotNullOrEmpty()]
    [string] $Path = $($(Get-Location).Path) + '\*',

    [Parameter(Position = 2,
        HelpMessage = 'Specifies an array of one or more string patterns to be matched as the ' +
            'cmdlet gets child items.')]
    [ValidateNotNullOrEmpty()]
    [string[]] $Include = @('*.cpp', '*.hpp'),

    [Parameter(Position = 3,
        HelpMessage = 'Specifies subfolders to search. The pattern value is treated ' +
            'as a regular expression, eg. (include|src|tests).')]
    [AllowEmptyString()]
    [string] $InSubFoldersPattern = '(include|src|tests|tom)',

    [Parameter(HelpMessage = 'Todo keywords regex pattern.')]
    [ValidateNotNullOrEmpty()]
    [string] $TodoKeywordsPattern = ' (TODO|NOTE|FIXME|BUG|WARNING|CUR|FEATURE|TEST|FUTURE|' +
        'CUR1|TMP|SEC) '
)

Set-StrictMode -Version 3.0

# If empty search all subfolders
if ($InSubFoldersPattern.Length -eq 0) {
    $Script:InFolders = @($Path.TrimEnd('\*'), '?.*') -join [IO.Path]::DirectorySeparatorChar
}
else {
    $Script:InFolders = `
        (@($Path.TrimEnd('\*'), $InSubFoldersPattern) -join [IO.Path]::DirectorySeparatorChar) + `
        [IO.Path]::DirectorySeparatorChar
}

if ($PSVersionTable.Platform -eq 'Win32NT') {
    $Script:InFolders = $Script:InFolders -replace '\\', '\\'
}

Get-ChildItem -Path $Path -Include $Include -Recurse -File
    | Where-Object DirectoryName -Match "^$Script:InFolders"
    | Select-String -Pattern "(?://)$($TodoKeywordsPattern.toUpper())" -CaseSensitive
    | Select-Object -Property `
        @{
            Name       = 'Line'
            Expression = { $_.Line.Trim().TrimStart('// ') }
        },
        LineNumber, Path
    | Where-Object Line -Match $Pattern
    | Sort-Object Line, Path, LineNumber
