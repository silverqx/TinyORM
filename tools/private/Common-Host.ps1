#!/usr/bin/env pwsh

Set-StrictMode -Version 3.0

# Write a Header message to a host
function Write-Header {
    [OutputType([void])]
    Param(
        [Parameter(Position = 0, Mandatory, HelpMessage = 'Writes a Header message to a host.')]
        [ValidateNotNullOrEmpty()]
        [string]
        $Header
    )

    NewLine
    Write-Host $Header -ForegroundColor DarkBlue
    NewLine
}

# Write a progress message to a host
function Write-Progress {
    [OutputType([void])]
    Param(
        [Parameter(Position = 0, Mandatory, HelpMessage = 'Writes a Progress message to a host.')]
        [ValidateNotNullOrEmpty()]
        [string]
        $Message
    )

    Write-Host $Message -ForegroundColor DarkYellow
}
