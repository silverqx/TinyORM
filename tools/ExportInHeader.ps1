Set-StrictMode -Version 3.0

$hppFiles = Get-ChildItem -Path ..\include\*.hpp -Recurse
$cppFiles = Get-ChildItem -Path ..\src\*.cpp -Recurse

foreach ($hppFile in $hppFiles)
{
    $cppFile = $hppFile -replace '\\include\\', '\src\'
    $cppFile = $cppFile -replace '\.hpp', '.cpp'

    $hasCpp = @($cppFiles | Where-Object FullName -CEQ $cppFile)
    if ($hasCpp.Count -gt 1)
    {
        throw 'More results for ' + $cppFile
    }
    if ($hasCpp.Count -eq 1)
    {
        continue
    }

    $containsExport = @(Select-String -Path $hppFile -Pattern 'SHAREDLIB_EXPORT')
    if ($containsExport.Count -eq 0)
    {
        continue
    }

    Write-Output $hppFile.FullName
}
