@echo off
rem Builds both programs with MinGW g++ (no CMake needed):
rem   screen_addict_freezer.exe      windowed version
rem   screen_addict_freezer_cli.exe  console version
set CORE=src\core.cpp src\i18n.cpp src\password_generator.cpp src\phrase.cpp src\vault.cpp
set FLAGS=-std=c++17 -O2 -Wall -Wextra -static

windres res\app.rc -O coff -o app_res.o
if %errorlevel% neq 0 goto fail

g++ %FLAGS% src\gui\gui_main.cpp src\gui\gui_app.cpp %CORE% app_res.o -mwindows -o screen_addict_freezer.exe
if %errorlevel% neq 0 goto fail

g++ %FLAGS% src\main.cpp src\app.cpp src\console.cpp %CORE% -o screen_addict_freezer_cli.exe
if %errorlevel% neq 0 goto fail

del app_res.o
echo Built screen_addict_freezer.exe and screen_addict_freezer_cli.exe
exit /b 0

:fail
echo Build failed.
exit /b 1
