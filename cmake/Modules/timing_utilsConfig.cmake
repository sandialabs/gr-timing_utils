INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_TIMING_UTILS timing_utils)

FIND_PATH(
    TIMING_UTILS_INCLUDE_DIRS
    NAMES timing_utils/api.h
    HINTS $ENV{TIMING_UTILS_DIR}/include
        ${PC_TIMING_UTILS_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    TIMING_UTILS_LIBRARIES
    NAMES gnuradio-timing_utils
    HINTS $ENV{TIMING_UTILS_DIR}/lib
        ${PC_TIMING_UTILS_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/timing_utilsTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TIMING_UTILS DEFAULT_MSG TIMING_UTILS_LIBRARIES TIMING_UTILS_INCLUDE_DIRS)
MARK_AS_ADVANCED(TIMING_UTILS_LIBRARIES TIMING_UTILS_INCLUDE_DIRS)
