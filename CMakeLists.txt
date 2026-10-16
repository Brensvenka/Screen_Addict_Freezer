cmake_minimum_required(VERSION 3.16)
project(ScreenAddictFreezer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Logic shared by both programs.
set(CORE_SOURCES
    src/core.cpp
    src/i18n.cpp
    src/password_generator.cpp
    src/phrase.cpp
    src/vault.cpp
)

# Console version: every platform.
add_executable(screen_addict_freezer_cli
    src/main.cpp
    src/app.cpp
    src/console.cpp
    ${CORE_SOURCES}
)
set(TARGETS screen_addict_freezer_cli)

# Windowed version: Windows only (Win32 API).
if(WIN32)
    enable_language(RC)
    add_executable(screen_addict_freezer WIN32
        src/gui/gui_main.cpp
        src/gui/gui_app.cpp
        res/app.rc
        ${CORE_SOURCES}
    )
    if(MSVC)
        # The manifest comes from app.rc; don't let the linker add a second one.
        target_link_options(screen_addict_freezer PRIVATE /MANIFEST:NO)
    endif()
    list(APPEND TARGETS screen_addict_freezer)
endif()

foreach(target IN LISTS TARGETS)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /utf-8)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    # MinGW: link the runtime statically so the .exe runs on PCs without MinGW.
    if(MINGW)
        target_link_options(${target} PRIVATE -static)
    endif()
endforeach()
