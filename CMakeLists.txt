# Copyright © 2018-2019 Pascal JEAN, All rights reserved.
# This file is part of the libpiphons Project.
#
# The libpiphons Project is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# The libpiphons Project is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with the libpiphons Project.  If not, see <http://www.gnu.org/licenses/>.

# piphons top-level CMakeLists.txt
cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

project (PIPHONS)
set (PROJECT_DESCRIPTION "libpiphons C++ library for PiPhons Shield")
set (PROJECT_DESCRIPTION_TEXT "\
  C++ library for Pi Phone Server Shield."
)

#library for the Modbus protocol
# A Modbus library written in C, to send/receive data with a device which
# respects the Modbus protocol. This library can use a serial port or an
# Ethernet connection.

string(TOLOWER ${CMAKE_PROJECT_NAME} PROJECT_NAME)

# Appends the cmake/modules path to MAKE_MODULE_PATH variable.
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dev/cmake ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_MODULE_PATH})
#message(STATUS "CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")

# Set a default build type if none was specified
set(default_build_type "Release")
 
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set (PIPHONS_WITH_TOUERIS 1 CACHE BOOL "Enable Toueris support")

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")
set(INSTALL_CODELITE_DIR /usr/share/codelite CACHE PATH "Installation directory for codelite templates")

set(DEF_INSTALL_CMAKE_DIR lib)
set(DEF_INSTALL_DOC_DIR share/doc)
set(DEF_INSTALL_DATA_DIR share)

set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")
set(INSTALL_DOC_DIR ${DEF_INSTALL_DOC_DIR} CACHE PATH
  "Installation directory for documentation")
set(INSTALL_DATA_DIR ${DEF_INSTALL_DATA_DIR} CACHE PATH
  "Installation directory for data")

set (CMAKE_CXX_STANDARD 14)

find_package(Gzip)
find_package(Pico REQUIRED PATHS ${CMAKE_CURRENT_SOURCE_DIR}/dev/cmake)
find_package(PkgConfig REQUIRED)
pkg_check_modules (PIDUINO REQUIRED piduino)
pkg_check_modules (ALSA REQUIRED alsa)

# message(STATUS "ALSA_LDFLAGS=${ALSA_LDFLAGS}")
# message(STATUS "ALSA_CFLAGS=${ALSA_CFLAGS}")
# message(STATUS "PIDUINO_LDFLAGS=${PIDUINO_LDFLAGS}")
# message(STATUS "PIDUINO_CFLAGS=${PIDUINO_CFLAGS}")
# message(STATUS "PICO_LDFLAGS=${PICO_LDFLAGS}")
# message(STATUS "PICO_CFLAGS=${PICO_CFLAGS}")

set (PIPHONS_CFLAGS_OTHER ${PIDUINO_CFLAGS} ${ALSA_CFLAGS} ${PICO_CFLAGS})
list(REMOVE_DUPLICATES PIPHONS_CFLAGS_OTHER)

set (PIPHONS_LDFLAGS_OTHER  ${PIDUINO_LDFLAGS} ${ALSA_LDFLAGS} ${PICO_LDFLAGS})
list(REMOVE_DUPLICATES PIPHONS_LDFLAGS_OTHER)
# message(STATUS "PIPHONS_CFLAGS_OTHER=${PIPHONS_CFLAGS_OTHER}")
# message(STATUS "PIPHONS_LDFLAGS_OTHER=${PIPHONS_LDFLAGS_OTHER}")

include (GetDate)
GetDate(DATE_)

include (GitVersion)
GetGitVersion(PIPHONS_VERSION)
set(PIPHONS_VERSION
  ${PIPHONS_VERSION_MAJOR}.${PIPHONS_VERSION_MINOR}.${PIPHONS_VERSION_PATCH})

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE DATA DOC CODELITE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

set (PIPHONS_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set (PIPHONS_INC_DIR ${PROJECT_SOURCE_DIR}/include)
set (PIPHONS_LIB_DIR ${PROJECT_BINARY_DIR}/lib)
set (PIPHONS_INSTALL_CMAKE_DIR "${INSTALL_CMAKE_DIR}/piphons/cmake")
set (PIPHONS_INSTALL_DOC_DIR "${INSTALL_DOC_DIR}/piphons")
set (PIPHONS_INSTALL_EXAMPLES_DIR "${PIPHONS_INSTALL_DOC_DIR}/examples")
set (PIPHONS_INSTALL_DATA_DIR "${INSTALL_DATA_DIR}/piphons")

message (STATUS "Building for ${CMAKE_SYSTEM_PROCESSOR} architecture.")

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

if (${PIPHONS_WITH_TOUERIS})
  add_definitions (-DPIPHONS_WITH_TOUERIS=1)
  message (STATUS "Build with Toueris support...")
endif()

add_definitions ( -DPIPHONS_INSTALL_DATA_DIR="${PIPHONS_INSTALL_DATA_DIR}" 
                  -DPIPHONS_INSTALL_ETC_DIR="${PIPHONS_INSTALL_ETC_DIR}")
                  
# Suppress GCC warnings
# See https://gcc.gnu.org/gcc-7/changes.html
# On ARM targets (arm*-*-*), a bug introduced in GCC 5 that affects conformance 
# to the procedure call standard (AAPCS) has been fixed. The bug affects some 
# C++ code where class objects are passed by value to functions and could result 
# in incorrect or inconsistent code being generated. This is an ABI change. 
# If the option -Wpsabi is enabled (on by default) the compiler will emit a 
# diagnostic note for code that might be affected. 
# <<< TODO >>> Fix this problem before compile with GCC 7 !
add_compile_options(-Wno-psabi)

add_subdirectory (dev)
add_subdirectory (lib)
#add_subdirectory (utils)
#add_subdirectory (doc)

if (PIPHONS_WITH_DOXYGEN_DOC)
  add_dependencies(piphons-shared doc)
endif()

### Packages generation --------------------------------------------------------
set(CPACK_PACKAGE_VERSION "${PIPHONS_VERSION_MAJOR}.${PIPHONS_VERSION_MINOR}.${PIPHONS_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PIPHONS_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PIPHONS_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PIPHONS_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "Pascal JEAN <pascal.jean@piduino.org>")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING.LESSER")

set(CPACK_GENERATOR "DEB")

if (CPACK_GENERATOR STREQUAL "DEB")

  set(CPACK_DEB_PACKAGE_COMPONENT 1)
  set(CPACK_DEB_COMPONENT_INSTALL 1)
  set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/epsilonrt/piphons")

  find_program(dpkg_query NAMES dpkg-query)
  find_program(dpkg_cmd NAMES dpkg)

  if (NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
    if (dpkg_cmd)
      execute_process(COMMAND ${dpkg_cmd} --print-architecture RESULT_VARIABLE ret OUTPUT_VARIABLE arch OUTPUT_STRIP_TRAILING_WHITESPACE)
      if (ret EQUAL 0)
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${arch}")
      else (ret EQUAL 0)
        message(WARNING "Unable to get debian architecture !")
      endif (ret EQUAL 0)
    endif (dpkg_cmd)
  endif (NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)

  
  message(STATUS "Debian architecture ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
  
  set(CPACK_DEBIAN_LIB_PACKAGE_NAME "libpiphons")
  set(CPACK_COMPONENT_LIB_DESCRIPTION "${CPACK_DEBIAN_LIB_PACKAGE_NAME} - ${PROJECT_DESCRIPTION} (library)\n${PROJECT_DESCRIPTION_TEXT}\n  This package provides the shared libriairie itself.")
  set(CPACK_DEBIAN_LIB_FILE_NAME "lib${PROJECT_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_LIB_PACKAGE_DEPENDS "libstdc++6,libpiduino (>= 0.3-45),libttspico0 (>= 1.0),libasound2 (>= 1.1.3-5)")
  set(CPACK_DEBIAN_LIB_PACKAGE_SECTION "libs")

  set(CPACK_DEBIAN_DEV_PACKAGE_NAME "libpiphons-dev")
  set(CPACK_COMPONENT_DEV_DESCRIPTION "${CPACK_DEBIAN_LIB_PACKAGE_NAME} - ${PROJECT_DESCRIPTION} (development files)\n${PROJECT_DESCRIPTION_TEXT}\n  This package provides the development files.")
  set(CPACK_DEBIAN_DEV_FILE_NAME "lib${PROJECT_NAME}-dev_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS "${CPACK_DEBIAN_LIB_PACKAGE_NAME} (= ${CPACK_PACKAGE_VERSION}),pkg-config,git-core,libpiduino-dev (>= 0.3-45),libttspico-dev (>= 1.0),libasound2-dev (>= 1.1.3-5)")
  set(CPACK_DEBIAN_DEV_PACKAGE_SECTION "libdevel")
  #set(CPACK_PACKAGE_DESCRIPTION_FILE "${PIPHONS_SRC_DIR}/doc/README-deb.md")

  set(CPACK_DEBIAN_DOC_PACKAGE_NAME "libpiphons-doc")
  set(CPACK_COMPONENT_DOC_DESCRIPTION "${CPACK_DEBIAN_LIB_PACKAGE_NAME} - ${PROJECT_DESCRIPTION} (documentation)\n${PROJECT_DESCRIPTION_TEXT}\n  This package provides the API documentation.")
  set(CPACK_DEBIAN_DOC_FILE_NAME "lib${PROJECT_NAME}-doc_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_DOC_PACKAGE_DEPENDS "${CPACK_DEBIAN_DEV_PACKAGE_NAME} (= ${CPACK_PACKAGE_VERSION})")
  set(CPACK_DEBIAN_DOC_PACKAGE_SECTION "libdevel")
    
endif (CPACK_GENERATOR STREQUAL "DEB")

include(CPack)
