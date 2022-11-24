@echo off
setlocal
set "root_dir=%~dp0..\.."

call "%root_dir%\tools\genLib\gen_lib.bat" "%root_dir%/src/hc/windows/dll/kernel32.def" "%~dp0kernel32.lib"
call "%root_dir%\tools\genLib\gen_lib.bat" "%root_dir%/src/hc/windows/dll/user32.def" "%~dp0user32.lib"

set "common_flags=-L%~dp0 -Wl,-subsystem,console -O2 -s %FLAGS%"
call "%root_dir%\cc_pe.bat" %common_flags% -S -o "%~dp0release.exe.s" "%~dp0main.c"
call "%root_dir%\cc_pe.bat" %common_flags% -o "%~dp0release.exe" "%~dp0main.c" -l:kernel32.lib -l:user32.lib
endlocal
