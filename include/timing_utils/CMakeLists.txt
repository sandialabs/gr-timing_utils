# Copyright 2011,2012 Free Software Foundation, Inc.
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
# Invoke macro to generate various headers
#######################################################################
include(GrMiscUtils)
GR_EXPAND_X_H(timing_utils timed_freq_xlating_fir_XXX  ccc ccf fcc fcf scf scc)
GR_EXPAND_X_H(timing_utils tag_uhd_offset_X            c f s b)

add_custom_target(timing_utils_generated_includes DEPENDS
    ${generated_includes}
)

########################################################################
# Install public header files
########################################################################
install(FILES
    ${generated_includes}
    api.h
    constants.h
    clock_mm_tags_ff.h
    add_usrp_tags_c.h
    uhd_timed_pdu_emitter.h
    thresh_trigger_f.h DESTINATION include/timing_utils
)
