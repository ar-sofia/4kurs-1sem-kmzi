@echo off
echo Setting up Visual Studio environment...

REM Call vcvarsall.bat to set up the environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

echo Compiling change_cript.cpp...
cl /EHsc change_cript.cpp /Fe:change_cript.exe

if %errorlevel% == 0 (
    echo Compilation successful! Running the program...
    echo.
    change_cript.exe
) else (
    echo Compilation failed!
)

pause
