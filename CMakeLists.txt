# MIT License
# 
# Copyright (c) 2022-2024 Rina Wilk / vokegpu@gmail.com
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

cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)
set(EKG_VERSION        1.1.0)

message("-- Building EKG user interface library version ${EKG_VERSION}")
project(ekg VERSION ${EKG_VERSION} DESCRIPTION "graphical user interface")

# Enable if your base OS is Debian-based (e.g Ubuntu).
# It is a common issue related by the Linux community.
if (EKG_LINUX_NOT_FOUND_FREETYPE)
  include_directories(/usr/include/freetype2)
  message("-- Using var EKG_LINUX_NOT_FOUND_FREETYPE to include `/usr/include/freetype2` directly")
endif()

if (WIN32 OR EKG_FORCE_WINDOWS)
  set(LIBRARY_OUTPUT_PATH "../lib/windows/")
  set(PLATFORM            "windows")
elseif(ANDROID OR EKG_FORCE_ANDROID)
  set(LIBRARY_OUTPUT_PATH "${ANDROID_ABI}/")
  set(PLATFORM            "${ANDROID_ABI}")
elseif(LINUX OR EKG_FORCE_LINUX)
  # WSL is not detected as Linux-based OS, same you use a Linux kernel-based distribution.
  set(LIBRARY_OUTPUT_PATH "../lib/linux/")
  set(PLATFORM            "linux")
endif()

message("-- Generating EKG ${PLATFORM} native library")

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

if (EKG_ENABLE_DEBUG)
  message("-- EKG debug mode on")
  add_compile_options(-g)
elseif()
  add_compile_options(-O3)
endif()

include_directories(./include)
add_library(ekg STATIC ${SOURCE_FILES})

message("-- EKG native library building done")

if(LINUX OR EKG_FORCE_LINUX)
  install(
    TARGETS ekg
    LIBRARY DESTINATION /usr/lib
  )

  install(
    DIRECTORY ./include DESTINATION /usr
  )

  message("-- EKG install on Linux ok!")
endif()