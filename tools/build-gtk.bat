@echo off
setlocal

set MINGW=C:\msys64\mingw64

:: Archivos de entrada y salida
set SRC=%1
set OUT=%~n1.exe

:: Compilar con g++ usando pkg-config
%MINGW%\bin\g++ %SRC% -o %OUT% -mms-bitfields -std=c++17 -Wall -Wextra -pipe ^
`%MINGW%\bin\pkg-config --cflags --libs gtk+-3.0`

echo ----------------------
echo Compilación terminada
echo Ejecutando %OUT%...
echo ----------------------
%OUT%

pause
