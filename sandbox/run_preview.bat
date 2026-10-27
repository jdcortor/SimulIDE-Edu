@echo off
set "PATH=C:\msys64\mingw64\bin;C:\msys64\usr\bin;%PATH%"
cd /d "%~dp0"
echo Compilando preview_ceramic.cpp...
g++ -std=c++17 preview_ceramic.cpp -I C:/msys64/mingw64/include -I C:/msys64/mingw64/include/QtCore -I C:/msys64/mingw64/include/QtGui -I C:/msys64/mingw64/include/QtWidgets -L C:/msys64/mingw64/lib -lQt5Widgets -lQt5Gui -lQt5Core -o preview.exe
if %ERRORLEVEL% equ 0 (
    echo Iniciando previsualizador interactivo...
    start "" preview.exe
) else (
    echo [ERROR] Fallo la compilacion.
    pause
)
