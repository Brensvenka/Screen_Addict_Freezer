cmake_minimum_required(VERSION 3.16)
project(ScreenAddictFreezer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(screen_addict_freezer
    src/main.cpp
    src/app.cpp
    src/console.cpp
    src/i18n.cpp
    src/password_generator.cpp
    src/phrase.cpp
    src/vault.cpp
)

if(MSVC)
    target_compile_options(screen_addict_freezer PRIVATE /W4 /utf-8)
else()
    target_compile_options(screen_addict_freezer PRIVATE -Wall -Wextra -Wpedantic)
endif()

# MinGW: link the runtime statically so the .exe runs on PCs without MinGW.
if(MINGW)
    target_link_options(screen_addict_freezer PRIVATE -static)
endif()
