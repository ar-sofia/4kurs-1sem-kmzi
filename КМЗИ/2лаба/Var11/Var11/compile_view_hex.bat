@echo off
echo Setting up Visual Studio environment...

REM Call vcvarsall.bat to set up the environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

echo Compiling view_hex.cpp...
cl /EHsc view_hex.cpp /Fe:view_hex.exe

if %errorlevel% == 0 (
    echo Compilation successful! Running the program...
    echo.
    view_hex.exe
) else (
    echo Compilation failed!
)

pause
