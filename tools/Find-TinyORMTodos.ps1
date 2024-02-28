#!/usr/bin/env pwsh

Param(
    [Parameter(Position = 0, Mandatory,
        HelpMessage = 'Specifies todo tasks to find on each line. The pattern value is used '+
            'in regular expression.')]
    [ValidateNotNullOrEmpty()]
    [string] $Pattern,

    [Parameter(Position = 1, HelpMessage = 'Todo keywords regex pattern.')]
    [ValidateNotNullOrEmpty()]
    [string] $TodoKeywordsPattern = ' (TODO|NOTE|FIXME|BUG|WARNING|CUR|FEATURE|TEST|FUTURE|CUR1|TMP|SEC) ',

    [Parameter(HelpMessage = 'Specifies subfolders to search. The pattern value is used ' +
        'in regular expression, eg. (include|src|tests|tom).')]
    [AllowEmptyString()]
    [string] $InSubFoldersPattern = '(include|src|tests|tom)'
)

Set-StrictMode -Version 3.0

Find-Todos.ps1 -Path 'E:\c\qMedia\TinyORM\TinyORM' -Include *.cpp, *.hpp ` @PSBoundParameters
    | Format-List
    # | Format-List -Property @{
    #     Name       = 'Line'
    #     Expression = {
    #         "`e[33m$($_.Line)`e[0m"
    #     }
    # }, LineNumber, Path
