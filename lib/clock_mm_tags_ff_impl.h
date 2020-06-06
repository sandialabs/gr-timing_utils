/* -*- c++ -*- */
/*
 * <COPYRIGHT PLACEHOLDER>
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


#ifndef INCLUDED_TIMING_UTILS_CLOCK_MM_TAGS_FF_IMPL_H
#define INCLUDED_TIMING_UTILS_CLOCK_MM_TAGS_FF_IMPL_H

#include <timing_utils/clock_mm_tags_ff.h>
#include <gnuradio/filter/mmse_fir_interpolator_ff.h>

namespace gr {
  namespace timing_utils {

    class clock_mm_tags_ff_impl : public clock_mm_tags_ff
    {
     private:
       float d_mu;                   // fractional sample position [0.0, 1.0]
       float d_gain_mu;              // gain for adjusting mu
       float d_omega;                // nominal frequency
       float d_start_omega;          // initial nominal frequency
       float d_gain_omega;           // gain for adjusting omega
       float d_omega_relative_limit; // used to compute min and max omega
       float d_omega_mid;            // average omega
       float d_omega_lim;            // actual omega clipping limit

       float d_last_sample;
       filter::mmse_fir_interpolator_ff *d_interp;

       bool d_verbose;
       bool d_tag_output;
       int32_t d_tag_counter;
       uint32_t d_tag_interval;
       uint64_t d_nitems_read_prev;

     public:
      clock_mm_tags_ff_impl(float omega, float gain_omega, float mu, float gain_mu, float omega_relative_limit, uint32_t tag_interval);
      ~clock_mm_tags_ff_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);


           float mu() const { return d_mu;}
           float omega() const { return d_omega;}
           float gain_mu() const { return d_gain_mu;}
           float gain_omega() const { return d_gain_omega;}

           void set_verbose (bool verbose) { d_verbose = verbose; }
           void set_gain_mu (float gain_mu) { d_gain_mu = gain_mu; }
           void set_gain_omega (float gain_omega) { d_gain_omega = gain_omega; }
           void set_mu (float mu) { d_mu = mu; }
           void set_omega (float omega);
           void tag_output (bool tag) { d_tag_output = tag; }
    };

  } // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_CLOCK_MM_TAGS_FF_IMPL_H */
