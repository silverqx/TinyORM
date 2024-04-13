@echo off
setlocal

rem This is a helper script for GitHub self-hosted runner to avoid using msys2_shell.cmd,
rem msys2_shell.cmd is more complex and supports more features, but is also slower,
rem this script is the exact opposite, it only contains what we need.

IF NOT DEFINED MSYSTEM set MSYSTEM=UCRT64
IF NOT DEFINED MSYS2_PATH_TYPE set MSYS2_PATH_TYPE=minimal
set CHERE_INVOKING=1

"%MSYS2_ROOT%\usr\bin\bash.exe" -leo pipefail %*
