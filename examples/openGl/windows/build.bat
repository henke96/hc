@echo off
setlocal
set "root_dir=%~dp0..\..\.."

call "%root_dir%\tools\genLib\gen_lib.bat" "%root_dir%/src/hc/windows/dll/kernel32.def" "%~dp0kernel32.lib"
call "%root_dir%\tools\genLib\gen_lib.bat" "%root_dir%/src/hc/windows/dll/user32.def" "%~dp0user32.lib"
call "%root_dir%\tools\genLib\gen_lib.bat" "%root_dir%/src/hc/windows/dll/gdi32.def" "%~dp0gdi32.lib"

set "common_flags=-L%~dp0 -Wl,-subsystem,console -O2"
set "debug_flags=%common_flags% -fsanitize-undefined-trap-on-error -fsanitize=undefined -g %FLAGS%"
set "release_flags=%common_flags% -Ddebug_NDEBUG -s %FLAGS%"
call "%root_dir%\cc_pe.bat" %debug_flags% -S -o "%~dp0debug.exe.s" "%~dp0main.c"
call "%root_dir%\cc_pe.bat" %debug_flags% -o "%~dp0debug.exe" "%~dp0main.c" -l:kernel32.lib -l:user32.lib -l:gdi32.lib
call "%root_dir%\cc_pe.bat" %release_flags% -S -o "%~dp0release.exe.s" "%~dp0main.c"
call "%root_dir%\cc_pe.bat" %release_flags% -o "%~dp0release.exe" "%~dp0main.c" -l:kernel32.lib -l:user32.lib -l:gdi32.lib
endlocal
