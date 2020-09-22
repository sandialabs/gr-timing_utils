# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
install(FILES
    timing_utils_timed_freq_xlating_fir.block.yml
    timing_utils_retune_uhd_to_timed_tag.block.yml
    timing_utils_add_usrp_tags.block.yml
    timing_utils_tag_uhd_offset.block.yml
    timing_utils_uhd_timed_pdu_emitter.block.yml
    timing_utils_thresh_trigger_f.block.yml
    timing_utils_edge_distance.block.yml
    timing_utils_edge_detector_bb.block.yml
    timing_utils_usrp_gps_time_sync.block.yml
    timing_utils_interrupt_emitter.block.yml
    timing_utils_system_time_diff.block.yml
    timing_utils_system_time_tagger.block.yml
    timing_utils_wall_clock_time.block.yml
    timing_utils_time_delta.block.yml
    timing_utils_timed_tag_retuner.block.yml 
    timing_utils_timed_cordic_emulator.block.yml DESTINATION share/gnuradio/grc/blocks
)
