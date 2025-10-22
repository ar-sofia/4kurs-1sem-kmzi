@echo off
echo Setting up Visual Studio environment...

REM Call vcvarsall.bat to set up the environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

echo Compiling verify_result.cpp...
cl /EHsc verify_result.cpp /Fe:verify_result.exe

if %errorlevel% == 0 (
    echo Compilation successful! Running the program...
    echo.
    verify_result.exe
) else (
    echo Compilation failed!
)

pause
