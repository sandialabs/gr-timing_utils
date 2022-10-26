find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_TIMING_UTILS gnuradio-timing_utils)

FIND_PATH(
    GR_TIMING_UTILS_INCLUDE_DIRS
    NAMES gnuradio/timing_utils/api.h
    HINTS $ENV{TIMING_UTILS_DIR}/include
        ${PC_TIMING_UTILS_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_TIMING_UTILS_LIBRARIES
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

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-timing_utilsTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_TIMING_UTILS DEFAULT_MSG GR_TIMING_UTILS_LIBRARIES GR_TIMING_UTILS_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_TIMING_UTILS_LIBRARIES GR_TIMING_UTILS_INCLUDE_DIRS)
