##
# EKG-LICENSE - this software/library LICENSE can not be modified in any instance.
#
# --
# ANY NON-OFFICIAL MODIFICATION IS A CRIME.
# DO NOT SELL THIS CODE SOFTWARE, FOR USE EKG IN A COMMERCIAL PRODUCT ADD EKG-LICENSE TO PROJECT,
# RESPECT THE COPYRIGHT TERMS OF EKG, NO SELL WITHOUT EKG-LICENSE (IT IS A CRIME).
# DO NOT FORK THE PROJECT SOURCE WITHOUT EKG-LICENSE.
#
# END OF EKG-LICENSE.
##
cmake_minimum_required(VERSION 3.9)
project(ekg VERSION 1.0.0 DESCRIPTION "ui library")
message("-- Preparing EKG setup")
add_compile_options(-O3)

if (LINUX)
    set(LIBRARY_OUTPUT_PATH "../lib/linux/")
    message("-- LINUX platform detected!")
else()
    set(LIBRARY_OUTPUT_PATH "../lib/win32/")
    message("-- WIN32 platform detected!")
endif()

set(PROJECT_VERSION 1.0)
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "include/ekg/*.hpp")

include(GNUInstallDirs)

# Generate shared lib (DLL).
message("-- Creating static lib")
add_library(ekg STATIC ${SOURCE_FILES})

message("-- Configuring static lib")
set_target_properties(ekg PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1)
set_target_properties(ekg PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")
target_include_directories(ekg PUBLIC include/)

message("-- Setting configure file")
configure_file(ekg.pc.in ekg.pc @ONLY)

message("-- Ending EKG CMake")
install(TARGETS ekg LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/ekg.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

message("-- EKG lib successfully done")