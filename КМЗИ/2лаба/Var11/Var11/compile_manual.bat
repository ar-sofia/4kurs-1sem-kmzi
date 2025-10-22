@echo off
echo Setting up Visual Studio environment...

REM Call vcvarsall.bat to set up the environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

echo Compiling manual_calculation.cpp...
cl /EHsc manual_calculation.cpp /Fe:manual_calculation.exe

if %errorlevel% == 0 (
    echo Compilation successful! Running the program...
    echo.
    manual_calculation.exe
) else (
    echo Compilation failed!
)

pause
