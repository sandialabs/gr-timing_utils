# Copyright 2011,2012,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Invoke macro to generate various sources and headers
########################################################################
include(GrMiscUtils)
GR_EXPAND_X_CC_H(timing_utils timed_freq_xlating_fir_XXX_impl   ccc ccf fcc fcf scf scc)
GR_EXPAND_X_CC_H(timing_utils tag_uhd_offset_X_impl             c f s b)

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR}, ${VOLK_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND timing_utils_sources
    ${generated_sources}
    clock_mm_tags_ff_impl.cc
    add_usrp_tags_c_impl.cc
    uhd_timed_pdu_emitter_impl.cc
    thresh_trigger_f_impl.cc
)

set(timing_utils_sources "${timing_utils_sources}" PARENT_SCOPE)
if(NOT timing_utils_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT timing_utils_sources)

add_library(gnuradio-timing_utils SHARED ${timing_utils_sources})
target_link_libraries(gnuradio-timing_utils ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES} ${VOLK_LIBRARIES})
set_target_properties(gnuradio-timing_utils PROPERTIES DEFINE_SYMBOL "gnuradio_timing_utils_EXPORTS")
add_dependencies(gnuradio-timing_utils gnuradio-filter
  timing_utils_generated_includes timing_utils_generated_swigs
  gnuradio-runtime)

if(APPLE)
    set_target_properties(gnuradio-timing_utils PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
include(GrMiscUtils)
GR_LIBRARY_FOO(gnuradio-timing_utils RUNTIME_COMPONENT "timing_utils_runtime" DEVEL_COMPONENT "timing_utils_devel")

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_timing_utils_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_timing_utils.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_timing_utils.cc
)

add_executable(test-timing_utils ${test_timing_utils_sources})

target_link_libraries(
  test-timing_utils
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${GNURADIO_BLOCKS_LIBRARIES}
  ${GNURADIO_FILTER_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  ${VOLK_LIBRARIES}
  gnuradio-timing_utils
)

GR_ADD_TEST(test_timing_utils test-timing_utils)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")
