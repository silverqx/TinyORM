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
        [switch] $NoNewlineAfter,

        [Parameter(HelpMessage = 'No newlines before and after the header message.')]
        [switch] $NoNewlines
    )

    if (-not $NoNewlineBefore -and -not $NoNewlines) {
        NewLine
    }

    Write-Host $Header -ForegroundColor DarkBlue

    if (-not $NoNewlineAfter -and -not $NoNewlines) {
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
        $ExitCode = 1,

        [Parameter(HelpMessage = 'No newline before the header message.')]
        [switch] $NoNewlineBefore
    )

    if (-not $NoNewlineBefore) {
        NewLine
    }

    Write-Error $Message

    exit $ExitCode
}

# Present a dialog allowing the user to choose continue or quit/return
function Approve-Continue {
    [CmdletBinding(DefaultParameterSetName = 'Return')]
    [OutputType([int], ParameterSetName = 'Return')]
    [OutputType([void], ParameterSetName = 'Exit')]
    Param(
        [Parameter(HelpMessage = 'Specifies the caption to precede or title the prompt.')]
        [ValidateNotNull()]
        [string]
        $Caption = '',

        [Parameter(Position = 0,
            HelpMessage = 'Specifies a message that describes what the choice is for.')]
        [ValidateNotNullOrEmpty()]
        [string]
        $Message = 'Ok to proceed?',

        [Parameter(HelpMessage = 'The index of the label in the choices collection element ' +
            'to be presented to the user as the default choice. -1 means "no default". ' +
            'Must be a valid index.')]
        [ValidateNotNull()]
        [int]
        $DefaultChoice = 1,

        [Parameter(ParameterSetName = 'Exit',
            HelpMessage = 'No newline before the header message.')]
        [switch] $Exit,

        [Parameter(ParameterSetName = 'Exit', HelpMessage = 'Specifies the exit code.')]
        [ValidateNotNull()]
        [int]
        $ExitCode = 1
    )

    $isExitSet = $PsCmdlet.ParameterSetName -eq 'Exit'

    $confirmChoices = [System.Management.Automation.Host.ChoiceDescription[]](@(
        New-Object System.Management.Automation.Host.ChoiceDescription('&Yes', 'Continue')
        New-Object System.Management.Automation.Host.ChoiceDescription( `
            '&No', ($isExitSet ? 'Quit' : 'Skip')
        )
    ))

    NewLine
    $answer = $Host.Ui.PromptForChoice($Caption, $Message, $confirmChoices, $DefaultChoice)

    if ($isExitSet) {
        switch ($answer) {
            0 { return }
            1 { Write-ExitError -ExitCode $ExitCode "Quit" }
        }
    }

    return $answer
}
