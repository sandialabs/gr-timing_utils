# Copyright 2011 Free Software Foundation, Inc.
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
install(FILES
    timing_utils_timed_freq_xlating_fir_XXX.xml
    timing_utils_clock_mm_tags_ff.xml
    timing_utils_retune_uhd_to_timed_tag.xml
    timing_utils_add_usrp_tags_c.xml
    timing_utils_tag_uhd_offset_X.xml
    timing_utils_uhd_timed_pdu_emitter.xml
    timing_utils_thresh_trigger_f.xml
    timing_utils_edge_distance.xml
    timing_utils_edge_detector_bb.xml
    timing_utils_usrp_gps_time_sync.xml
    timing_utils_interrupt_emitter.xml
    timing_utils_system_time_diff_X.xml 
    timing_utils_system_time_tagger_X.xml DESTINATION share/gnuradio/grc/blocks
)
