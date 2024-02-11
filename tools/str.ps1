#!/usr/bin/env pwsh

Param(
    [Parameter(Position = 0, ValueFromPipeline, ValueFromPipelinebyPropertyName,
        HelpMessage = 'Specifies the TinyORM release build folder to add.')]
    [ValidateNotNullOrEmpty()]
    [string[]] $BuildPath = $($(Get-Location).Path)
)

Set-StrictMode -Version 3.0

Add-FolderOnPath.ps1 -Path $BuildPath\drivers\common\release,
                           $BuildPath\drivers\mysql\release,
                           $BuildPath\src\release,
                           $BuildPath\examples\tom\release,
                           $BuildPath\tests\testdata_tom\release
