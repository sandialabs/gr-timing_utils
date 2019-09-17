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
#include <stdio.h>
#include "add_usrp_tags_c_impl.h"

namespace gr {
  namespace timing_utils {

    add_usrp_tags_c::sptr
    add_usrp_tags_c::make(double freq, double rate, uint64_t epoch_int, double epoch_frac)
    {
      return gnuradio::get_initial_sptr
        (new add_usrp_tags_c_impl(freq, rate, epoch_int, epoch_frac));
    }

    /*
     * The private constructor
     */
    add_usrp_tags_c_impl::add_usrp_tags_c_impl(double freq, double rate, uint64_t epoch_int, double epoch_frac)
      : gr::sync_block("add_usrp_tags_c",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)))
    {
      d_rate_pmt = pmt::from_double(rate);
      d_freq_pmt = pmt::from_double(freq);
      d_time_pmt = pmt::make_tuple(pmt::from_uint64(epoch_int),
                                  pmt::from_double(epoch_frac));

      tag_now(0);
      d_tagged_sample = 0;
      d_pmt_dict = pmt::make_dict();
      d_pmt_dict = pmt::dict_add(d_pmt_dict, pmt::mp("rx_rate"), d_rate_pmt);
      d_pmt_dict = pmt::dict_add(d_pmt_dict, pmt::mp("rx_freq"), d_freq_pmt);
      d_pmt_dict = pmt::dict_add(d_pmt_dict, pmt::mp("rx_time"), d_time_pmt);
    }

    /*
     * Our virtual destructor.
     */
    add_usrp_tags_c_impl::~add_usrp_tags_c_impl()
    {
    }

    void add_usrp_tags_c_impl::update_tags(pmt::pmt_t update) {
        if (!pmt::is_dict(update)) {
            GR_LOG_NOTICE(d_logger, "received unexpected PMT (non-dict)");
            return;
        }

        pmt::pmt_t value = pmt::dict_ref(update, pmt::mp("rx_freq"), pmt::PMT_NIL);
        if (value != pmt::PMT_NIL) d_freq_pmt = value;
        value = pmt::dict_ref(update, pmt::mp("rx_rate"), pmt::PMT_NIL);
        if (value != pmt::PMT_NIL) d_rate_pmt = value;
        value = pmt::dict_ref(update, pmt::mp("rx_time"), pmt::PMT_NIL);
        if (value != pmt::PMT_NIL) d_time_pmt = value;
        d_tag_now = true;
        d_pmt_dict = pmt::dict_add(d_pmt_dict, pmt::mp("rx_rate"), d_rate_pmt);
        d_pmt_dict = pmt::dict_add(d_pmt_dict, pmt::mp("rx_freq"), d_freq_pmt);
        d_pmt_dict = pmt::dict_add(d_pmt_dict, pmt::mp("rx_time"), d_time_pmt);
        printf("Updating tags\n");
    }

    pmt::pmt_t add_usrp_tags_c_impl::last_tag() {
        pmt::pmt_t retDict = pmt::dict_add(d_pmt_dict, pmt::mp("rx_sample"), pmt::from_uint64(d_tagged_sample));
        return retDict;
    }


    int
    add_usrp_tags_c_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      // pass through
      memcpy(out, in, noutput_items * sizeof(gr_complex));

      // add tags on the first sample in the output buffer if indicated
      if (d_tag_now) {
        add_item_tag(0, nitems_read(0), pmt::mp("rx_freq"), d_freq_pmt);
        add_item_tag(0, nitems_read(0), pmt::mp("rx_rate"), d_rate_pmt);
        add_item_tag(0, nitems_read(0), pmt::mp("rx_time"), d_time_pmt);
        d_tagged_sample = nitems_read(0);
        d_tag_now = false;
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace timing_utils */
} /* namespace gr */
