#!/usr/bin/env pwsh

Set-StrictMode -Version 3.0

if (-not (Test-Path .\compile_commands.json)) {
    throw 'Compile database compile_commands.json doesn''t exist.'
}

python.exe 'C:\Users\<your_path>\scoop\apps\llvm\current\bin\run-clang-tidy' @args
