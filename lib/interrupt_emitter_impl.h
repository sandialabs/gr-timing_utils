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


#ifndef INCLUDED_TIMING_UTILS_INTERRUPT_EMITTER_IMPL_H
#define INCLUDED_TIMING_UTILS_INTERRUPT_EMITTER_IMPL_H

#include <timing_utils/interrupt_emitter.h>
#include <timing_utils/ReferenceTimer.h>

namespace gr {
  namespace timing_utils {

    class interrupt_emitter_impl : public interrupt_emitter, public ReferenceTimer
    {
     private:       
       boost::thread* timer_thread;
       double       d_rate;
       bool         d_drop_late;
       bool         d_armed;
       uint64_t     d_trigger_samp;
       uint64_t     d_start_sample;
       pmt::pmt_t   d_pmt_out;
       double       d_start_time;
       boost::posix_time::ptime d_epoch;
       boost::posix_time::ptime last_time;
       double time_offset;
       double cum_time;
       double last_error;
       
       pmt::pmt_t samples_to_tpmt(uint64_t trigger_sample);
       uint64_t time_to_samples(double time);
       void process_interrupt();

     public:
      interrupt_emitter_impl(double rate, bool drop_late);
      ~interrupt_emitter_impl();
      bool start();

      void set_rate(double rate) { d_rate = rate; }
      void set_debug(bool value) {debug = value; } 
      void handle_set_time(pmt::pmt_t int_time);
      bool isLoaded() { return loaded;}      

      bool stop();
      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_INTERRUPT_EMITTER_IMPL_H */
