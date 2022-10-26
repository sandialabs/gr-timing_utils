/* -*- c++ -*- */
/*
 * Copyright 2018-2021 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "thresh_trigger_f_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/timing_utils/constants.h>

namespace gr {
namespace timing_utils {

static const int LENGTH_MSG_DISABLED = -1;

thresh_trigger_f::sptr thresh_trigger_f::make(float hi, float lo, int length)
{
    return gnuradio::make_block_sptr<thresh_trigger_f_impl>(hi, lo, length);
}

/*
 * The private constructor
 */
thresh_trigger_f_impl::thresh_trigger_f_impl(float hi, float lo, int length)
    : gr::sync_block("thresh_trigger_f",
                     gr::io_signature::make(1, 1, sizeof(float)),
                     gr::io_signature::make(0, 0, 0)),
      d_thresh_hi(hi),
      d_thresh_lo(lo),
      d_blank(false),
      d_length_thresh(length),
      d_length(0)
{
    message_port_register_out(PMTCONSTSTR__trig());
    message_port_register_in(PMTCONSTSTR__disarm());
    set_msg_handler(PMTCONSTSTR__disarm(), [this](pmt::pmt_t msg) { this->disarm(msg); });
}


/*
 * Our virtual destructor.
 */
thresh_trigger_f_impl::~thresh_trigger_f_impl() {}


/*
 * control message handler, handles command dictionaries or pairs
 */
void thresh_trigger_f_impl::disarm(pmt::pmt_t msg)
{
    // effectively disables the trigger
    d_length = d_length_thresh + 1;
    GR_LOG_INFO(d_logger, "disarmed!");
}


int thresh_trigger_f_impl::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    const float* mag = (const float*)input_items[0];
    uint64_t nitems = std::min(64, noutput_items);

    // if magnitude is greater than threshold, output zero, otherwise pass input
    for (size_t ii = 0; ii < nitems; ii++) {

        // start blanking when over the high threshold
        if (mag[ii] > d_thresh_hi) {
            d_blank = true;
            d_length++;
            if (d_length == d_length_thresh) {
                message_port_pub(PMTCONSTSTR__trig(),
                                 pmt::cons(PMTCONSTSTR__trigger_now(), pmt::PMT_NIL));
                // std::cout << "TRIGGERED at offset " << (nitems_read(0) + ii) <<
                // std::endl;
            }

            // stop blanking when under the low threshold
        } else {
            if (mag[ii] < d_thresh_lo) {
                d_blank = false;
            }
            d_length = 0;
        }
    }

    // Tell runtime system how many output items we produced.
    return nitems;
}

} /* namespace timing_utils */
} /* namespace gr */
