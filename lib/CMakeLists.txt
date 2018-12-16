# This file is part of the libdaadtmf Project.
#
# The libdaadtmf Project is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# The libdaadtmf Project is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with the libdaadtmf Project.  If not, see <http://www.gnu.org/licenses/>.

# libdaadtmf CMakeLists.txt
cmake_minimum_required(VERSION 2.8.11)

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

WriteGitVersionFile(${CMAKE_CURRENT_BINARY_DIR}/version.h)
  
file(GLOB src_daadtmf  ${DAADTMF_SRC_DIR}/*.cpp)

set (hdr_daadtmf 
  ${DAADTMF_INC_DIR}/global.h
  ${DAADTMF_INC_DIR}/daadtmf.h
  ${DAADTMF_INC_DIR}/daatoueris.h
)

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

set(hdr_public ${hdr_daadtmf} ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set (libsrc  ${src_daadtmf})

include_directories(BEFORE
  ${PIDUINO_INCLUDE_DIRS} 
  ${DAADTMF_INC_DIR} 
  ${DAADTMF_SRC_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

add_library(daadtmf-shared SHARED ${libsrc})
target_compile_definitions(daadtmf-shared PUBLIC DAADTMF_WITH_CONFIG_H)
set_target_properties(daadtmf-shared PROPERTIES 
  OUTPUT_NAME daadtmf 
  CLEAN_DIRECT_OUTPUT 1 
  VERSION ${DAADTMF_VERSION} 
  SOVERSION ${DAADTMF_VERSION_SHORT}
  PUBLIC_HEADER "${hdr_public}"
  )

if(DAADTMF_WITH_STATIC)
  add_library(daadtmf-static STATIC ${libsrc})
  target_compile_definitions(daadtmf-static PUBLIC DAADTMF_WITH_CONFIG_H)
  set_target_properties(daadtmf-static PROPERTIES
    OUTPUT_NAME daadtmf 
    CLEAN_DIRECT_OUTPUT 1 
    VERSION ${DAADTMF_VERSION} 
  )
endif(DAADTMF_WITH_STATIC)

# CMake Package ----------------------------------------------------------------
# The interesting stuff goes here
# ===============================

# Add all targets to the build-tree export set
if (DAADTMF_WITH_STATIC)
  set (exported_targets daadtmf-shared daadtmf-static)
else(DAADTMF_WITH_STATIC)
  set (exported_targets daadtmf-shared)
endif(DAADTMF_WITH_STATIC)

export(TARGETS ${exported_targets}
  FILE "${PROJECT_BINARY_DIR}/libdaadtmf.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE libdaadtmf)

# Create the daadtmf-config.cmake and daadtmf-config-version files
file(RELATIVE_PATH REL_LIB_DIR "${DAADTMF_INSTALL_CMAKE_DIR}" 
  "${INSTALL_LIB_DIR}")
file(RELATIVE_PATH REL_INCLUDE_DIR "${DAADTMF_INSTALL_CMAKE_DIR}"
  "${INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${DAADTMF_INC_DIR}" "${DAADTMF_SRC_DIR}" "${PROJECT_BINARY_DIR}")
set(CONF_LIB_DIRS "${PROJECT_BINARY_DIR}/lib/libdaadtmf")
configure_file(libdaadtmf-config.cmake.in
  "${PROJECT_BINARY_DIR}/libdaadtmf-config.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${DAADTMF_CMAKE_DIR}/${REL_INCLUDE_DIR}")
set(CONF_LIB_DIRS "\${DAADTMF_CMAKE_DIR}/${REL_LIB_DIR}")
configure_file(libdaadtmf-config.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/libdaadtmf-config.cmake" @ONLY)

# ... for both
configure_file(libdaadtmf-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/libdaadtmf-config-version.cmake" @ONLY)

# Install the libdaadtmf-config.cmake and libdaadtmf-config-version.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/libdaadtmf-config.cmake"
  "${PROJECT_BINARY_DIR}/libdaadtmf-config-version.cmake"
  DESTINATION "${DAADTMF_INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT daadtmf DESTINATION
  "${DAADTMF_INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the daadtmf licence files
install(FILES
  "${PROJECT_SOURCE_DIR}/COPYING.LESSER"
  DESTINATION "${DAADTMF_INSTALL_DATA_DIR}" COMPONENT lib)

install(FILES
  "${PROJECT_SOURCE_DIR}/README.md"
  DESTINATION "${DAADTMF_INSTALL_DOC_DIR}" COMPONENT lib)

install (TARGETS daadtmf-shared
  # IMPORTANT: Add the daadtmf library to the "export-set"
  EXPORT daadtmf
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}" COMPONENT dev
  )

if(DAADTMF_WITH_STATIC)
  install (TARGETS daadtmf-static 
    EXPORT daadtmf
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  )
endif(DAADTMF_WITH_STATIC)


## pkg-config ------------------------------------------------------------------
foreach(item ${DAADTMF_CFLAGS_OTHER})
  string(APPEND DAADTMF_CFLAGS_PKGCONFIG "${item} ")
endforeach(item DAADTMF_CFLAGS_OTHER)

foreach(item ${DAADTMF_LDFLAGS_OTHER})
  string(APPEND DAADTMF_LDFLAGS_PKGCONFIG "${item} ")
endforeach(item DAADTMF_LDFLAGS_OTHER)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/libdaadtmf.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/libdaadtmf.pc @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/libdaadtmf.pc 
  DESTINATION "${INSTALL_LIB_DIR}/pkgconfig"
  COMPONENT dev)
