# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later

########################################################################
# Include python install macros
########################################################################
include(GrPython)
if(NOT PYTHONINTERP_FOUND)
    return()
endif()

########################################################################
# Install python sources
########################################################################
set(pyfiles
    __init__.py
    retune_uhd_to_timed_tag.py
    edge_distance.py
    edge_detector_bb.py
    usrp_gps_time_sync.py 
    uhd_timed_cordic_emulator.py
)

# copy all python files to local package for testing
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/python/timing_utils)
foreach(file ${pyfiles})
   configure_file(${file} ${CMAKE_BINARY_DIR}/python/timing_utils COPYONLY)
endforeach()

GR_PYTHON_INSTALL(
    FILES ${pyfiles}
    DESTINATION ${GR_PYTHON_DIR}/timing_utils
)

########################################################################
# Handle the unit tests
########################################################################
include(GrTest)

set(GR_TEST_TARGET_DEPS gnuradio-timing_utils)
set(GR_TEST_PYTHON_DIRS ${CMAKE_BINARY_DIR}/python)
GR_ADD_TEST(qa_timed_freq_xlating_fir ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_timed_freq_xlating_fir.py)
GR_ADD_TEST(qa_retune_uhd_to_timed_tag ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_retune_uhd_to_timed_tag.py)
GR_ADD_TEST(qa_add_usrp_tags ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_add_usrp_tags.py)
GR_ADD_TEST(qa_tag_uhd_offset ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_tag_uhd_offset.py)
GR_ADD_TEST(qa_system_time_diff ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_system_time_diff.py)
GR_ADD_TEST(qa_interrupt_emitter ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_interrupt_emitter.py)
GR_ADD_TEST(qa_wall_clock_time ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_wall_clock_time.py)
GR_ADD_TEST(qa_time_delta ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_time_delta.py)
GR_ADD_TEST(qa_system_time_tagger ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_system_time_tagger.py)
GR_ADD_TEST(qa_timed_tag_retuner ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_timed_tag_retuner.py)
GR_ADD_TEST(qa_constants ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_constants.py)
