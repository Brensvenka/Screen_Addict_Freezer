@echo off
rem Builds screen_addict_freezer.exe with MinGW g++ (no CMake needed).
g++ -std=c++17 -O2 -Wall -Wextra -static ^
    src\main.cpp src\app.cpp src\console.cpp src\i18n.cpp ^
    src\password_generator.cpp src\phrase.cpp src\vault.cpp ^
    -o screen_addict_freezer.exe
if %errorlevel% neq 0 (
    echo Build failed.
    exit /b 1
)
echo Built screen_addict_freezer.exe
