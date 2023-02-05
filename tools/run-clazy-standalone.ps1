#!/usr/bin/env pwsh

Set-StrictMode -Version 3.0

if (-not (Test-Path .\compile_commands.json)) {
    throw 'Compile database compile_commands.json doesn''t exist.'
}

# The run-clazy-standalone python script is in the same folder as this script, copy it where ever
# you want and update the path to it below.
python.exe '<your_path_here>\dotfiles\python\run-clazy-standalone.py' @args
