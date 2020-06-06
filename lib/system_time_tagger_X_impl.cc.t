/* -*- c++ -*- */
/* 
 * Copyright 2018 <+YOU OR YOUR COMPANY+>.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "@IMPL_NAME@.h"

namespace gr {
  namespace timing_utils {

    @BASE_NAME@::sptr
    @BASE_NAME@::make(uint32_t tag_interval)
    {
      return gnuradio::get_initial_sptr
        (new @IMPL_NAME@(tag_interval));
    }

    /*
     * The private constructor
     */
    @IMPL_NAME@::@IMPL_NAME@(uint32_t tag_interval)
      : gr::sync_block("@NAME@",
          gr::io_signature::make(1, 1, sizeof(@I_TYPE@)),
          gr::io_signature::make(1, 1, sizeof(@I_TYPE@))),
        d_total_nitems_read(0)
    {
      // set tag generating tag_interval
      set_interval(tag_interval);
      
      // get posix time
      boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));
      d_epoch = epoch;
    }

    /*
     * Our virtual destructor.
     */
    @IMPL_NAME@::~@IMPL_NAME@()
    {
    }

    int
    @IMPL_NAME@::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock l(d_setlock);
      
      const @I_TYPE@ *in = (const @I_TYPE@*) input_items[0];
      @I_TYPE@ *out = (@I_TYPE@ *) output_items[0];
      
      // total number of items read
      d_total_nitems_read = nitems_read(0) + noutput_items;
      
      // see if we need to add any more time tags
      if (d_tagging_enabled) {
        while (d_next_tag_offset < d_total_nitems_read) {
          if (d_next_tag_offset >= nitems_read(0)) {
            // add tag
            double t_now((boost::get_system_time() - d_epoch).total_microseconds()/1000000.0);
            add_item_tag(0, d_next_tag_offset, PMTCONSTSTR__WALL_CLOCK_TIME, pmt::from_double(t_now));

            // set next offset
            d_next_tag_offset += d_interval;
          } else {
            // we should not have gotten into this state...
            GR_LOG_WARN(d_logger, "unexpected state: attempted to tag item in previous work call!");
            // reset the next tag offset to the first item in the next input buffer;
            d_next_tag_offset = d_total_nitems_read;
          }
        }
      }
      
      // then copy all of the items to the output
      memcpy((void *) out, (void *) in, sizeof(@I_TYPE@) * noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }
    
    void
    @IMPL_NAME@::set_interval(uint32_t interval)
    {
      gr::thread::scoped_lock l(d_setlock);
      
      d_interval = interval;
      if (d_interval != 0) {
        d_tagging_enabled = true;
        d_next_tag_offset = d_total_nitems_read + interval;
      } else {
        d_tagging_enabled = false;
      }
    }
    
  } /* namespace timing_utils */
} /* namespace gr */

