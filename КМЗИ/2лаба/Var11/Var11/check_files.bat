@echo off
echo Comparing files...

echo.
echo File sizes:
dir example2.out
dir example22.out

echo.
echo Hex dump of example2.out:
powershell -Command "Get-Content example2.out -Encoding Byte | ForEach-Object { '{0:X2}' -f $_ } | Out-String"

echo.
echo Hex dump of example22.out:
powershell -Command "Get-Content example22.out -Encoding Byte | ForEach-Object { '{0:X2}' -f $_ } | Out-String"

pause
