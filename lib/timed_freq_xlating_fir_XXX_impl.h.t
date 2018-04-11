/* -*- c++ -*- */
/*
 * Copyright 2018 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


/* @WARNING@ */

#ifndef @GUARD_NAME@
#define	@GUARD_NAME@

#include <string>
#include <timing_utils/api.h>
#include <timing_utils/@BASE_NAME@.h>
#include <timing_utils/constants.h>
#include <gnuradio/blocks/rotator.h>
#include <gnuradio/filter/fir_filter.h>

namespace gr {
  namespace timing_utils {

    class TIMING_UTILS_API @IMPL_NAME@ : public @BASE_NAME@
    {
    protected:
      std::vector<@TAP_TYPE@>	d_proto_taps;
      filter::kernel::@CFIR_TYPE@      *d_composite_fir;
      blocks::rotator		d_r;
      double			d_center_freq;
      double			d_sampling_freq;
      bool			d_updated;
      uint32_t  d_decim;
      std::string               d_tag_key;
      pmt::pmt_t                d_tag_pmt;

      virtual void build_composite_fir();
    public:

      @IMPL_NAME@(int decimation,
		  const std::vector<@TAP_TYPE@> &taps,
		  double center_freq,
		  double sampling_freq,
                  std::string tag_key
                 );
      virtual ~@IMPL_NAME@();

      void set_center_freq(double center_freq);
      double center_freq() const;

      void set_taps(const std::vector<@TAP_TYPE@> &taps);
      std::vector<@TAP_TYPE@> taps() const;

      void handle_set_center_freq(pmt::pmt_t msg);

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace timing_utils
} // namespace gr

#endif /* @GUARD_NAME@ */
