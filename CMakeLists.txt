# MIT License
# 
# Copyright (c) 2022-2025 Rina Wilk / vokegpu@gmail.com
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

cmake_minimum_required(VERSION 3.15...3.31)

include(cmake/io.cmake)
include(cmake/properties.cmake)

project(
  EKG
  VERSION ${EKG_VERSION}
  DESCRIPTION "Modern descriptor-based GPU-accelerated GUI library"
  LANGUAGES CXX
)

message(STATUS "Found compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Building EKG user interface library version ${EKG_VERSION}")
message(STATUS "Generating EKG ${PLATFORM} native library")

find_package(Freetype REQUIRED)

file(GLOB_RECURSE HEADER_FILES "include/*.hpp")
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

add_library(
  ekg STATIC ${SOURCE_FILES}
)

target_include_directories(
  ekg PRIVATE
  ./include
  ${FREETYPE_INCLUDE_DIRS}
)

target_compile_options(
  ekg PRIVATE
  ${COMPILE_OPTIMIZATION_NUMBER}
  #-Wall
  #-Wextra
  #-g
)

target_compile_definitions(
  ekg PRIVATE
  EKG_VERSION="${EKG_VERSION}"
  EKG_EOF_SYSTEM="${PLATFORM_EOF_SYSTEM}"
)

set_target_properties(
  ekg PROPERTIES
  CXX_STANDARD 17
)

message(STATUS "EKG native library building done")

install(
  TARGETS ekg
  EXPORT ekg
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  DIRECTORY ./include/ekg
  DESTINATION include
)

install(
  EXPORT ekg
  FILE EKGConfig.cmake
  NAMESPACE EKG::
  DESTINATION lib/cmake/ekg
)

message(STATUS "EKG installation config done!")
