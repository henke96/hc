@echo off
setlocal disabledelayedexpansion
set "script_dir=%~dp0"
set "script_dir=%script_dir:~0,-1%"

git -C "%script_dir%" clean -fdqX --exclude="!*.keystore"
