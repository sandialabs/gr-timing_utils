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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "@IMPL_NAME@.h"

namespace gr {
  namespace timing_utils {

    @BASE_NAME@::sptr
    @BASE_NAME@::make(float rate, uint32_t tag_interval)
    {
      return gnuradio::get_initial_sptr
        (new @IMPL_NAME@(rate, tag_interval));
    }

    /*
     * The private constructor
     */
    @IMPL_NAME@::@IMPL_NAME@(float rate, uint32_t tag_interval)
    : gr::sync_block("@NAME@",
              gr::io_signature::make(1, 1, sizeof(@I_TYPE@)),
              gr::io_signature::make(1, 1, sizeof(@I_TYPE@))),
        d_rate(rate),
        d_key(PMTCONSTSTR__RX_TIME),
        d_total_nitems_read(0),
        d_time_tag_offset(0),
        d_time_tag_int_sec(0),
        d_time_tag_frac_sec(0.0),
        d_time_tag(pmt::PMT_NIL)
    {
      set_interval(tag_interval);
      GR_LOG_INFO(d_logger, "setting up time tagger");

      message_port_register_out(PMTCONSTSTR__TIME);
    }

    /*
     * Our virtual destructor.
     */
    @IMPL_NAME@::~@IMPL_NAME@()
    {
    }


    /*
     * set the time_tag parameters to input UHD tag
     */
    void
    @IMPL_NAME@::set_time_tag(tag_t uhd_time_tag)
    {
      if (pmt::is_tuple(uhd_time_tag.value)) {
        d_time_tag_offset = uhd_time_tag.offset;
        d_time_tag_int_sec = pmt::to_uint64(pmt::tuple_ref(uhd_time_tag.value, 0));
        d_time_tag_frac_sec = pmt::to_double(pmt::tuple_ref(uhd_time_tag.value, 1));
        d_time_tag = pmt::make_tuple(pmt::tuple_ref(uhd_time_tag.value, 0),
                                     pmt::tuple_ref(uhd_time_tag.value, 1),
                                     pmt::from_uint64(uhd_time_tag.offset),
                                     pmt::from_double(d_rate));
      }
    }


    /*
     * move the time_tag OFFSET samples into the future
     */
    void
    @IMPL_NAME@::update_time_tag(uint64_t offset)
    {
      double delta = ((offset - d_time_tag_offset) / d_rate);
      if (delta < 0) {
        GR_LOG_ERROR(d_logger, boost::format("can't go back in time...updating time tag failed (requested delta = %fs)") % delta);
        return;
      }
      d_time_tag_frac_sec += delta;
      d_time_tag_offset = offset;
      // fractional seconds rollover
      if (d_time_tag_frac_sec >= 1.0) {
        d_time_tag_frac_sec -= 1.0;
        d_time_tag_int_sec += 1;
      }
      d_time_tag = pmt::make_tuple(pmt::from_uint64(d_time_tag_int_sec),
                                   pmt::from_double(d_time_tag_frac_sec),
                                   pmt::from_uint64(d_time_tag_offset),
                                   pmt::from_double(d_rate));
    }


    /*
     * Work function - will add the tag offset to the UHD time tuple when it
     * occurrs. Can also emit periodic timing tags based on this information.
     */
    int
    @IMPL_NAME@::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock l(d_setlock);

      const @I_TYPE@ *in = (const @I_TYPE@ *) input_items[0];
      @I_TYPE@ *out = (@I_TYPE@ *) output_items[0];

      std::vector<tag_t> tags;
      d_total_nitems_read = nitems_read(0) + noutput_items;

      // first check to see if there are any UHD time tags
      get_tags_in_range(tags, 0, nitems_read(0), (nitems_read(0) + noutput_items), PMTCONSTSTR__RX_TIME);
      if (tags.size()) {
        for (int ii = 0; ii < tags.size(); ii++) {
          set_time_tag(tags[ii]);
          add_item_tag(0, tags[ii].offset, d_key, d_time_tag);
          //GR_LOG_INFO(d_logger, boost::format("TAGGG! at sample %d") % d_time_tag_offset);
          message_port_pub(PMTCONSTSTR__TIME, pmt::cons(d_key, d_time_tag));
          d_next_tag_offset = tags[ii].offset + d_interval;
        }
      }

      // next see if we need to add any more time tags
      if (d_tagging_enabled) {
        while (d_next_tag_offset < d_total_nitems_read) {
          if (d_next_tag_offset >= nitems_read(0)) {
            // necessary tag sample is within the window
            update_time_tag(d_next_tag_offset);
            add_item_tag(0, d_next_tag_offset, d_key, d_time_tag);
            //GR_LOG_INFO(d_logger, boost::format("TAG! at sample %d") % d_time_tag_offset);
            d_next_tag_offset += d_interval;
            //GR_LOG_INFO(d_logger, boost::format("next one at %d") % d_next_tag_offset);
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
    @IMPL_NAME@::set_rate(float rate)
    {
      gr::thread::scoped_lock l(d_setlock);

      d_rate = rate;
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


    void
    @IMPL_NAME@::set_key(pmt::pmt_t key)
    {
      gr::thread::scoped_lock l(d_setlock);

      d_key = key;
    }

  } /* namespace timing_utils */
} /* namespace gr */
