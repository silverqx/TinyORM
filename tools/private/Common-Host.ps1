#!/usr/bin/env pwsh

Set-StrictMode -Version 3.0

Set-Alias NewLine Write-Host -Option ReadOnly -Force

# Write a Header message to a host
function Write-Header {
    [OutputType([void])]
    Param(
        [Parameter(Position = 0, Mandatory, HelpMessage = 'Writes a Header message to a host.')]
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
