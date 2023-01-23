@echo off
for /R %%f in (*build.bat) do (
    echo "%%f"
    call "%%f"
)