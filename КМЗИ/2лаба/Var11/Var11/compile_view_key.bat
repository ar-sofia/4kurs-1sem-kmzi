@echo off
echo Setting up Visual Studio environment...

REM Call vcvarsall.bat to set up the environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

echo Compiling view_key.cpp...
cl /EHsc view_key.cpp /Fe:view_key.exe

if %errorlevel% == 0 (
    echo Compilation successful! Running the program...
    echo.
    view_key.exe
) else (
    echo Compilation failed!
)

pause
