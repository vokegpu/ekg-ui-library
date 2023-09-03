#
# MIT License
# 
# Copyright (c) 2022-2023 Rina Wilk / vokegpu@gmail.com
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)
set(EKG_VERSION                   1.0.4)

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
                                    -static-libgcc -static-libstdc++
    )

    message("-- EKG tests building done")
endif()