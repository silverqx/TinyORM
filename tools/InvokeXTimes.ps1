#!/usr/bin/env pwsh

Param(
    [Parameter(Position = 0, Mandatory = $false,
        HelpMessage = 'Specifies how many times all AutoTests should be invoked.')]
    [ValidateNotNullOrEmpty()]
    [int] $Count = 1,

    [Parameter(Position = 1, Mandatory = $false,
        HelpMessage = 'Specifies the path to the qmake build folder, is pwd by default.')]
    [ValidateNotNullOrEmpty()]
    [string] $BuildPath = $($(Get-Location).Path)
)

Set-StrictMode -Version 3.0

$Script:TotalElapsed = 0
$Script:AverageElapsed = 0

function Get-Elapsed {
    [OutputType([int])]
    Param(
        [Parameter(Position = 0, Mandatory = $true, HelpMessage = "Specifies the RegEx pattern to look up the elapsed value.")]
        [ValidateNotNullOrEmpty()]
        [string]
        $Pattern,

        [Parameter(Position = 1, Mandatory = $true, HelpMessage = "Specifies the string in which to find the elapsed value.")]
        [ValidateNotNullOrEmpty()]
        [string]
        $String
    )

    $matchInfo = Select-String -Pattern $Pattern -InputObject $String

    if ($null -eq $matchInfo) {
        Write-Host
        throw "Pattern 'elapsed' not found, any RegEx matches."
    }

    $elapsed = $matchInfo.Matches.Groups
        | Where-Object Name -eq elapsed
        | Select-Object -ExpandProperty Value -Last 1

    return $elapsed
}

function Write-HighlightedOutput {
    [OutputType([void])]
    Param(
        [Parameter(Position = 0, Mandatory = $true, HelpMessage = "Specifies the Test Output in which to find and highlight the elapsed value.")]
        [ValidateNotNullOrEmpty()]
        [string[]]
        $Value
    )

    $pattern = ', (?<elapsed>\d+)ms'
    $matchInfo = Select-String -Pattern $pattern -InputObject $Value[1]

    if ($null -eq $matchInfo) {
        Write-Host
        throw "Pattern ', (?<elapsed>\d+)ms' not found, any RegEx matches."
    }

    $elapsed = $matchInfo.Matches.Groups
        | Where-Object Name -eq elapsed
        | Select-Object -ExpandProperty Value -Last 1

    Write-Host $Value[0] -ForegroundColor DarkCyan
    Write-Host "$($Value[1] -replace $pattern, ''), " -NoNewline
    Write-Host "$($elapsed)ms" -ForegroundColor DarkGreen
}

if ($PSVersionTable.Platform -ceq 'Unix') {
    $tests = find . -type f -and -executable -and -not -name '*.sh' -and -not -name 'lib*.so*'
}
else {
    $tests = Get-ChildItem -Path *.exe -Recurse
}

for ($i = 1; $i -le $Count; $i++) {
    if ($i -gt 1) {
        Write-Host
    }
    Write-Host "$i. AutoTests Run" -ForegroundColor DarkBlue
    Write-Host

    $testsElapsed = 0

    foreach ($test in $tests) {
        $testOutput = & $test -silent

        if ($LASTEXITCODE -ne 0) {
            Write-Host
            throw "Last Exit code was not 0, it was: $LASTEXITCODE"
        }

        # Execution time of the current test
        $testsElapsed += Get-Elapsed -Pattern ', (?<elapsed>\d+)ms' `
            -String $($testOutput -join "`n")

        Write-HighlightedOutput -Value $testOutput
    }

    Write-Host
    Write-Host "Tests Execution time : " -NoNewline
    Write-Host "$($testsElapsed)ms" -ForegroundColor Green

    $Script:TotalElapsed += $testsElapsed
}

Write-Host
Write-Host "All AutoTests Execution time : " -NoNewline
Write-Host "$($Script:TotalElapsed)ms" -ForegroundColor Green

[int] $averageElapsed = $Script:TotalElapsed / $Count
Write-Host "All AutoTests Average Execution time : " -NoNewline
Write-Host "$($averageElapsed)ms" -ForegroundColor Green
