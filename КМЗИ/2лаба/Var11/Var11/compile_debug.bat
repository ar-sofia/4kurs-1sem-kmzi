@echo off
echo Setting up Visual Studio environment...

REM Call vcvarsall.bat to set up the environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

echo Compiling debug_calculation.cpp...
cl /EHsc debug_calculation.cpp /Fe:debug_calculation.exe

if %errorlevel% == 0 (
    echo Compilation successful! Running the program...
    echo.
    debug_calculation.exe
) else (
    echo Compilation failed!
)

pause
