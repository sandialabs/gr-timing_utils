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

include(GrMiscUtils)

list(APPEND include_sources
    add_usrp_tags_c.h   
    interrupt_emitter.h   
    system_time_diff_s.h    
    system_time_tagger_s.h    
    tag_uhd_offset_s.h            
    timed_freq_xlating_fir_fcc.h    
    uhd_timed_pdu_emitter.h
    api.h               
    ReferenceTimer.h      
    timed_freq_xlating_fir_fcf.h
    clock_mm_tags_ff.h  
    system_time_diff_b.h  
    system_time_tagger_b.h  
    tag_uhd_offset_b.h        
    thresh_trigger_f.h            
    timed_freq_xlating_fir_scc.h
    system_time_diff_c.h  
    system_time_tagger_c.h  
    tag_uhd_offset_c.h        
    timed_freq_xlating_fir_ccc.h  
    timed_freq_xlating_fir_scf.h
    constants.h         
    system_time_diff_f.h  
    system_time_tagger_f.h  
    tag_uhd_offset_f.h        
    timed_freq_xlating_fir_ccf.h      
)

add_custom_target(timing_utils_generated_includes DEPENDS
    ${generated_includes}
)

########################################################################
# Install public header files
########################################################################
install(FILES
    ${include_sources}
    DESTINATION include/timing_utils
)
