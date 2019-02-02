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

# libpiphons CMakeLists.txt
cmake_minimum_required(VERSION 2.8.11)

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

WriteGitVersionFile(${CMAKE_CURRENT_BINARY_DIR}/version.h)
  
file(GLOB src_piphons  ${PIPHONS_SRC_DIR}/*.cpp)

set (hdr_piphons 
  ${PIPHONS_INC_DIR}/piphons/global.h
  ${PIPHONS_INC_DIR}/piphons/daa.h
  ${PIPHONS_INC_DIR}/piphons/dtmf.h
  ${PIPHONS_INC_DIR}/piphons/tts.h
)

set (hdr_toueris 
  ${PIPHONS_INC_DIR}/piphons/toueris/daa.h
)

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

set(hdr_public ${hdr_piphons} ${CMAKE_CURRENT_BINARY_DIR}/config.h)

if (PIPHONS_WITH_TOUERIS)
  file(GLOB src_toueris  ${PIPHONS_SRC_DIR}/toueris/*.cpp)
#  list(APPEND hdr_public ${hdr_toueris})
endif (PIPHONS_WITH_TOUERIS)

set (libsrc ${src_piphons} ${src_toueris})

include_directories(BEFORE
  ${PIPHONS_INC_DIR} 
  ${PIPHONS_SRC_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

add_custom_command(OUTPUT picoapi.h
                   COMMAND "${PROJECT_SOURCE_DIR}/dev/bin/cp-pico-header.sh" 
                   ${PICO_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}
                   COMMENT "Copy libttspico header files")

add_library(piphons-shared SHARED ${libsrc} picoapi.h)
target_compile_definitions(piphons-shared PUBLIC PIPHONS_WITH_CONFIG_H)
set_target_properties(piphons-shared PROPERTIES 
  OUTPUT_NAME piphons 
  CLEAN_DIRECT_OUTPUT 1 
  VERSION ${PIPHONS_VERSION} 
  SOVERSION ${PIPHONS_VERSION_MAJOR}
  PUBLIC_HEADER "${hdr_public}"
  )


if(PIPHONS_WITH_STATIC)
  add_library(piphons-static STATIC ${libsrc})
  target_compile_definitions(piphons-static PUBLIC PIPHONS_WITH_CONFIG_H)
  set_target_properties(piphons-static PROPERTIES
    OUTPUT_NAME piphons 
    CLEAN_DIRECT_OUTPUT 1 
    VERSION ${PIPHONS_VERSION} 
  )
endif(PIPHONS_WITH_STATIC)

# CMake Package ----------------------------------------------------------------
# The interesting stuff goes here
# ===============================

# Add all targets to the build-tree export set
if (PIPHONS_WITH_STATIC)
  set (exported_targets piphons-shared piphons-static)
else(PIPHONS_WITH_STATIC)
  set (exported_targets piphons-shared)
endif(PIPHONS_WITH_STATIC)

export(TARGETS ${exported_targets}
  FILE "${PROJECT_BINARY_DIR}/libpiphons.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE libpiphons)

# Create the piphons-config.cmake and piphons-config-version files
file(RELATIVE_PATH REL_LIB_DIR "${PIPHONS_INSTALL_CMAKE_DIR}" 
  "${INSTALL_LIB_DIR}")
file(RELATIVE_PATH REL_INCLUDE_DIR "${PIPHONS_INSTALL_CMAKE_DIR}"
  "${INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${PIPHONS_INC_DIR}" "${PIPHONS_SRC_DIR}" "${PROJECT_BINARY_DIR}")
set(CONF_LIB_DIRS "${PROJECT_BINARY_DIR}/lib/libpiphons")
configure_file(libpiphons-config.cmake.in
  "${PROJECT_BINARY_DIR}/libpiphons-config.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${PIPHONS_CMAKE_DIR}/${REL_INCLUDE_DIR}")
set(CONF_LIB_DIRS "\${PIPHONS_CMAKE_DIR}/${REL_LIB_DIR}")
configure_file(libpiphons-config.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/libpiphons-config.cmake" @ONLY)

# ... for both
configure_file(libpiphons-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/libpiphons-config-version.cmake" @ONLY)


# Install the libpiphons-config.cmake and libpiphons-config-version.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/libpiphons-config.cmake"
  "${PROJECT_BINARY_DIR}/libpiphons-config-version.cmake"
  DESTINATION "${PIPHONS_INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT piphons DESTINATION
  "${PIPHONS_INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the piphons licence files
install(FILES
  "${PROJECT_SOURCE_DIR}/COPYING.LESSER"
  DESTINATION "${PIPHONS_INSTALL_DATA_DIR}" COMPONENT lib)

install(FILES
  "${PROJECT_SOURCE_DIR}/README.md"
  DESTINATION "${PIPHONS_INSTALL_DOC_DIR}" COMPONENT lib)

install(FILES
  "${PIPHONS_INC_DIR}/piphons.h"
  DESTINATION "${INSTALL_INCLUDE_DIR}" COMPONENT dev)

install (TARGETS piphons-shared
  # IMPORTANT: Add the piphons library to the "export-set"
  EXPORT piphons
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}" COMPONENT dev
  )

if (PIPHONS_WITH_TOUERIS)
  install(DIRECTORY
    "${PIPHONS_INC_DIR}/${PROJECT_NAME}/toueris" DESTINATION "${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}" COMPONENT dev)
endif (PIPHONS_WITH_TOUERIS)

if(PIPHONS_WITH_STATIC)
  install (TARGETS piphons-static 
    EXPORT piphons
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  )
endif(PIPHONS_WITH_STATIC)


## pkg-config ------------------------------------------------------------------
foreach(item ${PIPHONS_CFLAGS_OTHER})
  string(APPEND PIPHONS_CFLAGS_PKGCONFIG "${item} ")
endforeach(item PIPHONS_CFLAGS_OTHER)

foreach(item ${PIPHONS_LDFLAGS_OTHER})
  string(APPEND PIPHONS_LDFLAGS_PKGCONFIG "${item} ")
endforeach(item PIPHONS_LDFLAGS_OTHER)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/libpiphons.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/libpiphons.pc @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/libpiphons.pc 
  DESTINATION "${INSTALL_LIB_DIR}/pkgconfig"
  COMPONENT dev)
