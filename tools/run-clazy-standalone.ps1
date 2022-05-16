Set-StrictMode -Version 3.0

if (-not (Test-Path .\compile_commands.json)) {
    throw 'Compile database compile_commands.json doesn''t exist.'
}

python.exe 'E:\dotfiles\python\run-clazy-standalone.py' @args
