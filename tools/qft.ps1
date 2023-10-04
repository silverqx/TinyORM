#!/usr/bin/env pwsh

Param(
    [Parameter(Position = 0, Mandatory,
        HelpMessage = 'Specifies todo tasks to find on each line. The pattern value is used ' +
            'in regular expression.')]
    [ValidateNotNullOrEmpty()]
    [string] $Pattern,

    [Parameter(Position = 1, HelpMessage = 'Todo keywords regex pattern.')]
    [ValidateNotNullOrEmpty()]
    [string] $TodoKeywordsPattern = ' (TODO|NOTE|FIXME|BUG|WARNING|CUR|FEATURE|TEST|FUTURE|CUR1|TMP|SEC) '
)

Set-StrictMode -Version 3.0

Find-TinyORMTodos.ps1 @PSBoundParameters
