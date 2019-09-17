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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "@IMPL_NAME@.h"

namespace gr {
  namespace timing_utils {

    @BASE_NAME@::sptr
    @BASE_NAME@::make(bool output_data, bool update_time, bool output_diff)
    {
      return gnuradio::get_initial_sptr
        (new @IMPL_NAME@(output_data,update_time, output_diff));
    }

    /*
     * The private constructor
     */
    @IMPL_NAME@::@IMPL_NAME@(bool output_data, bool update_time, bool output_diff)
      : gr::block("@NAME@",
          gr::io_signature::make(1, 1, sizeof(@I_TYPE@)),
          gr::io_signature::makev(1, 2,  std::vector<int>(4, sizeof(@I_TYPE@)))),
        d_output_data(output_data),
        d_update_time(update_time),
        d_output_diff(output_diff)
        
    {
      // Brute force fix of the output io_signature, because I can't get
      // an anonymous std::vector<int>() rvalue, with a const expression
      // initializing the vector, to work.  Lvalues seem to make everything
      // better.
      if (d_output_data or d_output_diff) {
        int output_io_sizes[2] = {sizeof(@I_TYPE@),sizeof(float)};
        std::vector<int> output_io_sizes_vector(&output_io_sizes[0],
                                              &output_io_sizes[2]);
        set_output_signature(io_signature::makev(1, 2, output_io_sizes_vector));
      }
      else {
        set_output_signature(io_signature::make(0, 0, 0));
      }
      
      // time tag update only supported if data is being output
      d_update_time &= d_output_data;

      // get posix time
      boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));
      d_epoch = epoch;
      
      // set port to output data to
      
      d_diff_port = d_output_data ? 1 : 0;
      
      if (d_update_time) {
        // don't propagate tags - all tags will be propagated 
        // directly in work function if necessary
        set_tag_propagation_policy(gr::block::TPP_DONT);
      }
      else {
        set_tag_propagation_policy(gr::block::TPP_ALL_TO_ALL);
      }
    }

    /*
     * Our virtual destructor.
     */
    @IMPL_NAME@::~@IMPL_NAME@()
    {
    }

    int
    @IMPL_NAME@::general_work(int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock l(d_setlock);
      
      // total number of input items
      int nitems = ninput_items[0];
      
      // get input dat
      const @I_TYPE@ *in = (const @I_TYPE@*) input_items[0];
      
      // get output data only if necessary
      @I_TYPE@ *out = NULL; 
      float *diff_out = NULL;
      if (d_output_data or d_output_diff) {
        out = (@I_TYPE@ *) output_items[0];
        diff_out = (float *)output_items[1];
      }
      
      
      // check to see if there are any tags
      std::vector<tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0), (nitems_read(0) + nitems));
      if (tags.size()) {
        // get current system time
        double t_now((boost::get_system_time() - d_epoch).total_microseconds()/1000000.0);
        
        // process all tags
        int ntime_tags = 0;
        for (int ii = 0; ii < tags.size(); ii++) {
          if (pmt::eq(PMTCONSTSTR__WALL_CLOCK_TIME,tags[ii].key)) {
            double diff = t_now - pmt::to_double(tags[ii].value);
            std::cout << boost::format("diff = %0.9f s") % diff << std::endl;
          
            // update tag if requested
            if (d_update_time) {
              add_item_tag(0,tags[ii].offset,PMTCONSTSTR__WALL_CLOCK_TIME, pmt::from_double(t_now));
            }
            
            // output time
            if (d_output_diff) {
              diff_out[ntime_tags++] = (float)diff;
            }
          }
          else {
            // add tag back if updating time
            if (d_update_time) {
              add_item_tag(0,tags[ii].offset,tags[ii].key, tags[ii].value);
            }
          }
        }
        // update number of items produced
        produce(1,ntime_tags);
      } /* end if tags.size() */
      
      // then copy all of the items to the output
      if (d_output_data) {
        memcpy((void *) out, (void *) in, sizeof(@I_TYPE@) * nitems);
//         std::cout << "calling produce for channel 0: " << noutput_items << std::endl;
        produce(0, nitems);
      }
      
      // tell the scheduler how many items were consumed
      consume(0,nitems);

      // Tell runtime system how many output items we produced.
      return gr::block::WORK_CALLED_PRODUCE;
    }
  } /* namespace timing_utils */
} /* namespace gr */

