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


#ifndef INCLUDED_TIMING_UTILS_THRESH_TRIGGER_F_IMPL_H
#define INCLUDED_TIMING_UTILS_THRESH_TRIGGER_F_IMPL_H

#include <timing_utils/thresh_trigger_f.h>

namespace gr {
  namespace timing_utils {

    class thresh_trigger_f_impl : public thresh_trigger_f
    {
     private:
      float d_thresh_hi;
      float d_thresh_lo;
      bool d_blank;
      uint64_t d_length;
      int d_length_thresh;

     public:
      thresh_trigger_f_impl(float hi, float lo, int length);
      ~thresh_trigger_f_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);

      void disarm(pmt::pmt_t);

      void set_hi(float t) { d_thresh_hi = t; };
      void set_lo(float t) { d_thresh_lo = t; };
      void set_len(int len) { d_length_thresh = len; }
    };

  } // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_THRESH_TRIGGER_F_IMPL_H */
