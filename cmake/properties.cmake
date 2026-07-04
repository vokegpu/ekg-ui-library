set(EKG_VERSION 2.6.0)
set(EKG_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include")

if(CMAKE_TOOLCHAIN_FILE)
  string(
    REGEX MATCH
    "/Emscripten"
    compiler_filename
    ${CMAKE_TOOLCHAIN_FILE}
  )

  if(
    EKG_EMSCRIPTEN_BUILD_TYPE
    OR
    ${compiler_filename} STREQUAL "/Emscripten"
  )
    message(STATUS "Toolchain ID: ${compiler_filename}, WASM build-type")
    set(EKG_EMSCRIPTEN_BUILD_TYPE 1)
  elseif(EKG_EMSCRIPTEN_BUILD_TYPE)
    message(FATAL_ERROR "No specialized toolchain ID  was found with '/Emscripten' for WASM build-type")
  endif()
elseif(EKG_EMSCRIPTEN_BUILD_TYPE)
  message(FATAL_ERROR "No specialized toolchain ID  was found with '/Emscripten' for WASM build-type")
endif()

if(
  CMAKE_BUILD_TYPE STREQUAL "Release"
  AND
  EKG_EMSCRIPTEN_BUILD_TYPE
  OR
  CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
  OR
  CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
)
  set(COMPILE_OPTIMIZATION_NUMBER -O3)
endif()
  
if(WIN32 OR EKG_FORCE_WINDOWS)
  set(LIBRARY_OUTPUT_PATH "../lib/windows/")
  set(PLATFORM "windows")
  set(PLATFORM_EOF_SYSTEM "\\r\\n")
elseif(ANDROID OR EKG_FORCE_ANDROID)
  set(LIBRARY_OUTPUT_PATH "${ANDROID_ABI}/")
  set(PLATFORM "${ANDROID_ABI}")
  set(PLATFORM_EOF_SYSTEM "\\n")
elseif(EKG_EMSCRIPTEN_BUILD_TYPE)
  set(LIBRARY_OUTPUT_PATH "../lib/linux-wasm/")
  set(PLATFORM "linux-wasm")
  set(PLATFORM_EOF_SYSTEM "\\n")
elseif(LINUX OR EKG_FORCE_LINUX)
  # WSL is not detected as Linux-kernel based OS; force is necessary ><
  set(LIBRARY_OUTPUT_PATH "../lib/linux/")
  set(PLATFORM "linux")
  set(PLATFORM_EOF_SYSTEM "\\n")
endif()
