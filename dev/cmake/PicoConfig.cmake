# Find the TtsPico includes and library
#
# This module defines
#  PICO_INCLUDE_DIRS - include directories for cppdb
#  PICO_LIBRARY_DIRS - where to find libraries
#  PICO_LIBRARIES    - libraries to link against

find_path(PICO_INCLUDE_DIRS picoapi.h)

find_library(PICO_LIBRARIES ttspico)

if(PICO_INCLUDE_DIRS)
    if(PICO_LIBRARIES)
      get_filename_component(PICO_LIBRARY_DIRS ${PICO_LIBRARIES} PATH)
      set(PICO_FOUND TRUE)
      set(PICO_LIBRARIES ttspico)
      set(PICO_LDFLAGS -L${PICO_LIBRARY_DIRS} -lttspico)
      set(PICO_CFLAGS -I${PICO_INCLUDE_DIRS})
    endif(PICO_LIBRARIES)
endif(PICO_INCLUDE_DIRS)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Pico DEFAULT_MSG PICO_LIBRARIES PICO_INCLUDE_DIRS)

#message(STATUS "PICO_INCLUDE_DIRS=${PICO_INCLUDE_DIRS}")
#message(STATUS "PICO_LIBRARIES=${PICO_LIBRARIES}")
#message(STATUS "PICO_LDFLAGS=${PICO_LDFLAGS}")
#message(STATUS "PICO_CFLAGS=${PICO_CFLAGS}")
