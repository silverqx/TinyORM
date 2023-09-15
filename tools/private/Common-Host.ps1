#!/usr/bin/env pwsh

Set-StrictMode -Version 3.0

Set-Alias NewLine Write-Host -Option ReadOnly -Force

# Write a Header message to a host
function Write-Header {
    [OutputType([void])]
    Param(
        [Parameter(Position = 0, Mandatory, HelpMessage = 'Writes a header message to the host.')]
        [ValidateNotNullOrEmpty()]
        [string]
        $Header,

        [Parameter(HelpMessage = 'No newline before the header message.')]
        [switch] $NoNewlineBefore,

        [Parameter(HelpMessage = 'No newline after the header message.')]
        [switch] $NoNewlineAfter
    )

    if (-not $NoNewlineBefore) {
        NewLine
    }

    Write-Host $Header -ForegroundColor DarkBlue

    if (-not $NoNewlineAfter) {
        NewLine
    }
}

# Write an info message to a host
function Write-Info {
    [OutputType([void])]
    Param(
        [Parameter(Position = 0, Mandatory, HelpMessage = 'Writes an info message to the host.')]
        [ValidateNotNullOrEmpty()]
        [string]
        $Message
    )

    Write-Host $Message -ForegroundColor DarkGreen
}

# Write a progress message to a host
function Write-Progress {
    [OutputType([void])]
    Param(
        [Parameter(Position = 0, Mandatory, HelpMessage = 'Writes a progress message to the host.')]
        [ValidateNotNullOrEmpty()]
        [string]
        $Message
    )

    Write-Host $Message -ForegroundColor DarkYellow
}

# Write an error message to a host
function Write-Error {
    [OutputType([void])]
    Param(
        [Parameter(Position = 0, Mandatory, HelpMessage = 'Writes an error message to the host.')]
        [ValidateNotNullOrEmpty()]
        [string]
        $Message
    )

    Write-Host $Message -ForegroundColor Red
}

# Write an error message to a host and exit with 1 error code
function Write-ExitError {
    [OutputType([void])]
    Param(
        [Parameter(Position = 0, Mandatory, HelpMessage = 'Writes an error message to the host.')]
        [ValidateNotNullOrEmpty()]
        [string]
        $Message,

        [Parameter(HelpMessage = 'Specifies the exit code.')]
        [ValidateNotNull()]
        [int]
        $ExitCode = 1
    )

    NewLine
    Write-Error $Message
    exit $ExitCode
}

# Present a dialog allowing the user to choose continue or quit
function Approve-Continue {
    [OutputType([void])]
    Param()

    $confirmChoices = [System.Management.Automation.Host.ChoiceDescription[]](@(
        New-Object System.Management.Automation.Host.ChoiceDescription('&Yes', 'Continue')
        New-Object System.Management.Automation.Host.ChoiceDescription('&No', 'Quit')
    ))

    NewLine
    $answer = $Host.Ui.PromptForChoice('', 'Ok to proceed?', $confirmChoices, 1)

    switch ($answer) {
        0 { break }
        1 { "`nQuit"; exit 1 }
    }
}
