# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT timing_utils_sources)
    MESSAGE(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT timing_utils_sources)

########################################################################
# Check for pygccxml
########################################################################
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################
list(APPEND timing_utils_python_files
    add_usrp_tags_python.cc
    constants_python.cc
    interrupt_emitter_python.cc
    system_time_diff_python.cc
    system_time_tagger_python.cc
    tag_uhd_offset_python.cc
    thresh_trigger_f_python.cc
    time_delta_python.cc
    timed_freq_xlating_fir_python.cc
    timed_tag_retuner_python.cc
    uhd_timed_pdu_emitter_python.cc
    wall_clock_time_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE_OOT(timing_utils
   ../../..
   gr::timing_utils
   "${timing_utils_python_files}")

# copy bindings extension for use in QA test module
add_custom_command(TARGET timing_utils_python POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:timing_utils_python>
    ${PROJECT_BINARY_DIR}/test_modules/gnuradio/timing_utils/
)

install(TARGETS timing_utils_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/timing_utils COMPONENT pythonapi)
