/* -*- c++ -*- */

#define TIMING_UTILS_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "timing_utils_swig_doc.i"

%{
#include "timing_utils/constants.h"
#include "timing_utils/timed_freq_xlating_fir_ccc.h"
#include "timing_utils/timed_freq_xlating_fir_ccf.h"
#include "timing_utils/timed_freq_xlating_fir_fcc.h"
#include "timing_utils/timed_freq_xlating_fir_fcf.h"
#include "timing_utils/timed_freq_xlating_fir_scf.h"
#include "timing_utils/timed_freq_xlating_fir_scc.h"
#include "timing_utils/clock_mm_tags_ff.h"
#include "timing_utils/add_usrp_tags_c.h"
#include "timing_utils/tag_uhd_offset_c.h"
#include "timing_utils/tag_uhd_offset_f.h"
#include "timing_utils/tag_uhd_offset_s.h"
#include "timing_utils/tag_uhd_offset_b.h"
#include "timing_utils/uhd_timed_pdu_emitter.h"
#include "timing_utils/thresh_trigger_f.h"
%}

%include "timing_utils/constants.h"

%include "timing_utils/timed_freq_xlating_fir_ccc.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, timed_freq_xlating_fir_ccc);
%include "timing_utils/timed_freq_xlating_fir_ccf.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, timed_freq_xlating_fir_ccf);
%include "timing_utils/timed_freq_xlating_fir_fcc.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, timed_freq_xlating_fir_fcc);
%include "timing_utils/timed_freq_xlating_fir_fcf.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, timed_freq_xlating_fir_fcf);
%include "timing_utils/timed_freq_xlating_fir_scf.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, timed_freq_xlating_fir_scf);
%include "timing_utils/timed_freq_xlating_fir_scc.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, timed_freq_xlating_fir_scc);
%include "timing_utils/clock_mm_tags_ff.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, clock_mm_tags_ff);
%include "timing_utils/add_usrp_tags_c.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, add_usrp_tags_c);
%include "timing_utils/tag_uhd_offset_c.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, tag_uhd_offset_c);
%include "timing_utils/tag_uhd_offset_f.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, tag_uhd_offset_f);
%include "timing_utils/tag_uhd_offset_s.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, tag_uhd_offset_s);
%include "timing_utils/tag_uhd_offset_b.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, tag_uhd_offset_b);
%include "timing_utils/uhd_timed_pdu_emitter.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, uhd_timed_pdu_emitter);
%include "timing_utils/thresh_trigger_f.h"
GR_SWIG_BLOCK_MAGIC2(timing_utils, thresh_trigger_f);
