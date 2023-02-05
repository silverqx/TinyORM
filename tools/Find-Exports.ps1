#!/usr/bin/env pwsh

# Detect SHAREDLIB_EXPORT in header files that don't have a cpp file

Set-StrictMode -Version 3.0

$hppFiles = Get-ChildItem -Path ..\tom\include\*.hpp -Recurse
$cppFiles = Get-ChildItem -Path ..\tom\src\*.cpp -Recurse

foreach ($hppFile in $hppFiles)
{
    # Guess the .cpp filepath
    $cppFile = $hppFile -replace '\\include\\', '\src\'
    $cppFile = $cppFile -replace '\.hpp', '.cpp'

    $hasCpp = @($cppFiles | Where-Object FullName -CEQ $cppFile)
    if ($hasCpp.Count -gt 1)
    {
        throw 'More results for ' + $cppFile
    }
    # If the cpp file was found then skip, we need only header files without cpp files
    if ($hasCpp.Count -eq 1)
    {
        continue
    }

    # Find the SHAREDLIB_EXPORT
    $containsExport = @(Select-String -Path $hppFile -Pattern 'SHAREDLIB_EXPORT')
    if ($containsExport.Count -eq 0)
    {
        continue
    }

    Write-Output $hppFile.FullName
}
