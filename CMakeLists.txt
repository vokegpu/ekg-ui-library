#
# VOKEGPU EKG LICENSE
#
# Respect ekg license policy terms, please take a time and read it.
# 1- Any "skidd" or "stole" is not allowed.
# 2- Forks and pull requests should follow the license policy terms.
# 3- For commercial use, do not sell without give credit to vokegpu ekg.
# 4- For ekg users and users-programmer, we do not care, free to use in anything (utility, hacking, cheat, game, software).
# 5- Malware, rat and others virus. We do not care.
# 6- Do not modify this license under any instance.
#
# @VokeGpu 2022 all rights reserved.
#

cmake_minimum_required(VERSION 3.9)
project(ekg VERSION 0.1.0 DESCRIPTION "ui library")
message("-- Preparing EKG setup")

# Maximum optimization
add_compile_options(-O3)

if (WIN32)
    message("-- WIN32 platform detected!")
    set(LIBRARY_OUTPUT_PATH "../lib/win32/")
elseif(ANDROID)
    set(LIBRARY_OUTPUT_PATH "${ANDROID_ABI}/")
    message("-- ANDROID platform detected!")
elseif(LINUX)
    include_directories(/usr/include/freetype2)
    message("-- LINUX platform detected!")
    set(LIBRARY_OUTPUT_PATH "../lib/linux/")
endif()

set(PROJECT_VERSION 1.0)
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "include/ekg/*.hpp")

include(GNUInstallDirs)

# Generate static lib (non - DLL).
message("-- Creating static lib")
add_library(ekg STATIC ${SOURCE_FILES})

message("-- Configuring static lib")
set_target_properties(ekg PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1)
set_target_properties(ekg PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")
target_include_directories(ekg PUBLIC include/)
 
message("-- Setting configure file")
configure_file(ekg.pc.in "ekg.pc" @ONLY)

message("-- Ending EKG CMake")
install(TARGETS ekg LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/ekg.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
message("-- Lib successfully created")

if (EKG_ENABLE_TEST)
    project(ekg_ui_library)
    message("-- Building EKG testing software...")

    if (WIN32)
        set(EXECUTABLE_OUTPUT_PATH ../test/build/win32/)
    else()
        set(EXECUTABLE_OUTPUT_PATH ../test/build/linux/)
    endif()

    add_compile_options(-O3)
    set(CMAKE_CXX_STANDART 20)

    include_directories(./include/)
    add_executable(ekg_ui_library "test/src/main.cpp")

    if (WIN32)
        message("-- WIN32 platform detected!")
        file(GLOB LIBRARY_PATH "./lib/win32/libekg.a")
        target_link_libraries(ekg_ui_library ${LIBRARY_PATH} mingw32 SDL2main SDL2 opengl32 glew32 freetype)
    else()
        message("-- LINUX platform detected!")
        file(GLOB LIBRARY_PATH "./lib/linux/libekg.a")
        target_link_libraries(ekg_ui_library ${LIBRARY_PATH} SDL2main SDL2 GL GLEW freetype)
    endif()
endif()