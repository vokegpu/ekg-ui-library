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

set(CMAKE_CXX_STANDART 17)
set(EKG_VERSION        0.0.3)

message("-- Building EKG user interface library version ${EKG_VERSION}")
project(ekg VERSION ${EKG_VERSION} DESCRIPTION "graphical user interface")

if (WIN32)
    message("-- Building EKG WIN32 native library")
    set(LIBRARY_OUTPUT_PATH "../lib/win32/")
elseif(ANDROID)
    message("-- Building EKG Android Linux native library")
    set(LIBRARY_OUTPUT_PATH "${ANDROID_ABI}/")
elseif(LINUX)
    if (LINUX_NOT_FOUND_FREETYPE)
        include_directories(/usr/include/freetype2)
    endif()

    message("-- Generating EKG Linux native library")
    set(LIBRARY_OUTPUT_PATH "../lib/linux/")
endif()

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
include_directories(include/)

add_compile_options(-O3)
add_library(ekg STATIC ${SOURCE_FILES})

message("-- EKG native library building done")

if (EKG_ENABLE_TEST)
    message("-- EKG tests enabled")
    project(ekg-tests)

    if (WIN32)
        set(EXECUTABLE_OUTPUT_PATH ../test/build/win32/)
    else()
        set(EXECUTABLE_OUTPUT_PATH ../test/build/linux/)
    endif()

    add_compile_options(-O3)
    add_executable(ekg-tests "test/src/main.cpp")

    if (WIN32)
        file(GLOB LIBRARY_PATH "./lib/win32/libekg.a")
        set(THIRD_PARTY_LIBRARIES mingw32 SDL2main SDL2 opengl32 glew32 freetype)
    else()
        file(GLOB LIBRARY_PATH "./lib/linux/libekg.a")
        set(THIRD_PARTY_LIBRARIES SDL2main SDL2 GL GLEW freetype)
    endif()

    target_link_libraries(ekg-tests ${LIBRARY_PATH}
                                    ${THIRD_PARTY_LIBRARIES}
    )

    message("-- EKG tests building done")
endif()