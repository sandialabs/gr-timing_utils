# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later

########################################################################
# Install public header files
########################################################################
install(FILES
    api.h
    add_usrp_tags.h
    interrupt_emitter.h
    system_time_diff.h
    system_time_tagger.h
    tag_uhd_offset.h
    uhd_timed_pdu_emitter.h
    timed_freq_xlating_fir.h
    thresh_trigger_f.h
    constants.h
    wall_clock_time.h
    time_delta.h
    timed_tag_retuner.h DESTINATION include/gnuradio/timing_utils
)
