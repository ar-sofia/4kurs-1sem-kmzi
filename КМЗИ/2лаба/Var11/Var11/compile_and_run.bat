@echo off
echo Trying to compile full_cript.cpp...

REM Try different compilers
where g++ >nul 2>&1
if %errorlevel% == 0 (
    echo Found g++, compiling...
    g++ -o full_cript.exe full_cript.cpp
    if %errorlevel% == 0 (
        echo Compilation successful! Running...
        full_cript.exe
    ) else (
        echo Compilation failed with g++
    )
    goto :end
)

where cl >nul 2>&1
if %errorlevel% == 0 (
    echo Found cl, compiling...
    cl /EHsc full_cript.cpp /Fe:full_cript.exe
    if %errorlevel% == 0 (
        echo Compilation successful! Running...
        full_cript.exe
    ) else (
        echo Compilation failed with cl
    )
    goto :end
)

where clang++ >nul 2>&1
if %errorlevel% == 0 (
    echo Found clang++, compiling...
    clang++ -o full_cript.exe full_cript.cpp
    if %errorlevel% == 0 (
        echo Compilation successful! Running...
        full_cript.exe
    ) else (
        echo Compilation failed with clang++
    )
    goto :end
)

echo No C++ compiler found. Please install one of the following:
echo - MinGW-w64
echo - Visual Studio Build Tools
echo - Clang
echo - Or use an online compiler like https://onlinegdb.com/

:end
pause
