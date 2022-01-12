#!/usr/bin/env pwsh

Set-StrictMode -Version 3.0


# Get slashes by platform
function Get-Slashes {
    [OutputType([string])]
    Param()

    # Cached value
    if ($Script:Slashes) {
        return $Script:Slashes
    }

    $platform = $PSVersionTable.Platform

    switch ($platform) {
        'Win32NT' {
            return $Script:Slashes = '\\/'
        }
        'Unix' {
            return $Script:Slashes = '/'
        }
        Default {
            throw "$platform platform is not supported."
        }
    }
}

function Get-PathToMatch {
    [OutputType([string])]
    Param(
        [Parameter(Position = 0, Mandatory, ValueFromPipeline, ValueFromPipelinebyPropertyName,
            HelpMessage = "Specifies a path to modify to the match pattern.")]
        [ValidateNotNullOrEmpty()]
        [string]
        $Path
    )

    $slashes = Get-Slashes

    $pathToMatch = $Path `
        -replace "[$slashes]+(?![$slashes]*$)", "[$slashes]+" `
        -replace "[$slashes]+$", ''

    $separator = [IO.Path]::PathSeparator

    return "(?:^|$separator)$pathToMatch[$slashes]*(?:$|$separator)"
}

function Get-FullPath {
    [CmdletBinding()]
    [OutputType([string[]])]
    Param(
        [Parameter(Position = 0, Mandatory, ValueFromPipeline, ValueFromPipelinebyPropertyName,
            HelpMessage = "Specifies a path to normalize.")]
        [string[]]
        $Path
    )

    process {
        [string[]] $normalized = @()

        foreach ($p in $Path) {
            $normalized += [System.IO.Path]::GetFullPath(($p -replace "[$(Get-Slashes)]+$", ''))
        }

        return $normalized
    }
}
