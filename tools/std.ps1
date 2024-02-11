#!/usr/bin/env pwsh

Param(
    [Parameter(Position = 0, ValueFromPipeline, ValueFromPipelinebyPropertyName,
        HelpMessage = 'Specifies the TinyORM debug build folder to add.')]
    [ValidateNotNullOrEmpty()]
    [string[]] $BuildPath = $($(Get-Location).Path)
)

Set-StrictMode -Version 3.0

Add-FolderOnPath.ps1 -Path $BuildPath\drivers\common\debug,
                           $BuildPath\drivers\mysql\debug,
                           $BuildPath\src\debug,
                           $BuildPath\examples\tom\debug,
                           $BuildPath\tests\testdata_tom\debug
