/* -*- c++ -*- */

#define TIMING_UTILS_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "timing_utils_swig_doc.i"

%{
#include "timing_utils/constants.h"
#include "timing_utils/interrupt_emitter.h"
#include "timing_utils/timed_freq_xlating_fir.h"
#include "timing_utils/add_usrp_tags.h"
#include "timing_utils/tag_uhd_offset.h"
#include "timing_utils/uhd_timed_pdu_emitter.h"
#include "timing_utils/thresh_trigger_f.h"
#include "timing_utils/system_time_tagger.h"
#include "timing_utils/system_time_diff.h"
#include "timing_utils/wall_clock_time.h"
#include "timing_utils/time_delta.h"
#include "timing_utils/timed_tag_retuner.h"
%}

%include "timing_utils/constants.h"

%include "timing_utils/interrupt_emitter.h"
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, interrupt_emitter_b, interrupt_emitter<unsigned char>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, interrupt_emitter_s, interrupt_emitter<short>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, interrupt_emitter_i, interrupt_emitter<int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, interrupt_emitter_f, interrupt_emitter<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, interrupt_emitter_c, interrupt_emitter<gr_complex>);

// timed frequency translating fir filter
%include "timing_utils/timed_freq_xlating_fir.h"
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, timed_freq_xlating_fir_ccc, timed_freq_xlating_fir<gr_complex,gr_complex,gr_complex>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, timed_freq_xlating_fir_ccf, timed_freq_xlating_fir<gr_complex,gr_complex,float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, timed_freq_xlating_fir_fcc, timed_freq_xlating_fir<float,gr_complex,gr_complex>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, timed_freq_xlating_fir_fcf, timed_freq_xlating_fir<float,gr_complex,float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, timed_freq_xlating_fir_scc, timed_freq_xlating_fir<short,gr_complex,gr_complex>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, timed_freq_xlating_fir_scf, timed_freq_xlating_fir<short,gr_complex,float>);

// usrp tags
%include "timing_utils/add_usrp_tags.h"
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, add_usrp_tags_b, add_usrp_tags<unsigned char>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, add_usrp_tags_s, add_usrp_tags<short>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, add_usrp_tags_i, add_usrp_tags<int32_t>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, add_usrp_tags_f, add_usrp_tags<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, add_usrp_tags_c, add_usrp_tags<gr_complex>);

// tag uhd offset
%include "timing_utils/tag_uhd_offset.h"
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, tag_uhd_offset_b, tag_uhd_offset<unsigned char>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, tag_uhd_offset_s, tag_uhd_offset<short>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, tag_uhd_offset_f, tag_uhd_offset<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, tag_uhd_offset_c, tag_uhd_offset<gr_complex>);

%include "timing_utils/uhd_timed_pdu_emitter.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, uhd_timed_pdu_emitter);
%include "timing_utils/thresh_trigger_f.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, thresh_trigger_f);

// system time tagger
%include "timing_utils/system_time_tagger.h"
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, system_time_tagger_b, system_time_tagger<unsigned char>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, system_time_tagger_s, system_time_tagger<short>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, system_time_tagger_f, system_time_tagger<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, system_time_tagger_c, system_time_tagger<gr_complex>);

// system time diff
%include "timing_utils/system_time_diff.h"
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, system_time_diff_b, system_time_diff<unsigned char>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, system_time_diff_s, system_time_diff<short>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, system_time_diff_f, system_time_diff<float>);
GR_SWIG_BLOCK_MAGIC2_TMPL(timing_utils, system_time_diff_c, system_time_diff<gr_complex>);
%include "timing_utils/wall_clock_time.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, wall_clock_time);
%include "timing_utils/time_delta.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, time_delta);
%include "timing_utils/timed_tag_retuner.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, timed_tag_retuner);
