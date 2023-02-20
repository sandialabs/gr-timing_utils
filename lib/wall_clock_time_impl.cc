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

#include "wall_clock_time_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace timing_utils {

wall_clock_time::sptr wall_clock_time::make(const pmt::pmt_t key)
{
    return gnuradio::make_block_sptr<wall_clock_time_impl>(key);
}

/*
 * The private constructor
 */
wall_clock_time_impl::wall_clock_time_impl(const pmt::pmt_t key)
    : gr::block("wall_clock_time",
                gr::io_signature::make(0, 0, 0),
                gr::io_signature::make(0, 0, 0)),
      d_key(key)
{
    boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
    d_epoch = epoch;

    message_port_register_in(PMTCONSTSTR__pdu_in());
    set_msg_handler(PMTCONSTSTR__pdu_in(),
                    [this](pmt::pmt_t msg) { this->handle_pdu(msg); });
    message_port_register_out(PMTCONSTSTR__pdu_out());
}

/*
 * Our virtual destructor.
 */
wall_clock_time_impl::~wall_clock_time_impl() {}

void wall_clock_time_impl::handle_pdu(pmt::pmt_t pdu)
{
    // make sure PDU data is formed properly
    if (!(pmt::is_pair(pdu))) {
        d_logger->warn("received unexpected PMT (non-pair)");
        return;
    }

    pmt::pmt_t meta = pmt::car(pdu);

    // append time and publish
    double t_now((boost::get_system_time() - d_epoch).total_microseconds() / 1000000.0);
    meta = pmt::dict_add(meta, d_key, pmt::from_double(t_now));
    message_port_pub(PMTCONSTSTR__pdu_out(), (pmt::cons(meta, pmt::cdr(pdu))));
}

} /* namespace timing_utils */
} /* namespace gr */
