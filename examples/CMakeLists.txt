# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later

########################################################################
# example applications that get installed into the share path
########################################################################
SET(example_sources
    timed_xlating_fir_example.grc
    uhd_timed_pdu_emitter_example.grc
    usrp_tag_test.grc
    validate_tt.grc
    README
)

INSTALL(DIRECTORY test
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${GR_PKG_DATA_DIR}/)

FOREACH(example_source ${example_sources})
    GET_FILENAME_COMPONENT(example_name ${example_source} NAME)
    INSTALL(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/${example_name}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${GR_PKG_DATA_DIR}
        COMPONENT examples
    )
ENDFOREACH(example_source)
