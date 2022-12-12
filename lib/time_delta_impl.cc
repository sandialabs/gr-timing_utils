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

#include "time_delta_impl.h"
#include <gnuradio/io_signature.h>
#include <boost/format.hpp>
namespace gr {
namespace timing_utils {

time_delta::sptr time_delta::make(const pmt::pmt_t delta_key, const pmt::pmt_t time_key)
{
    return gnuradio::make_block_sptr<time_delta_impl>(delta_key, time_key);
}

/*
 * The private constructor
 */
time_delta_impl::time_delta_impl(const pmt::pmt_t delta_key, const pmt::pmt_t time_key)
    : gr::block("time_delta", io_signature::make(0, 0, 0), io_signature::make(0, 0, 0)),
      d_name(pmt::symbol_to_string(delta_key)),
      d_delta_key(delta_key),
      d_time_key(time_key),
      d_sum_x(0.0),
      d_sum_x2(0.0),
      d_n(0)
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
time_delta_impl::~time_delta_impl() {}

bool time_delta_impl::stop()
{
    double mean = d_sum_x / (double)d_n;
    double var = (d_sum_x2 - (d_sum_x * d_sum_x) / (double)d_n) / (double)d_n;

    GR_LOG_INFO(
        d_logger,
        boost::format("WALL_CLOCK_TIME_DEBUG (%s): Mean = %0.6f ms, Var = %0.6f ms") %
            d_name % mean % var);
    return true;
}

void time_delta_impl::handle_pdu(pmt::pmt_t pdu)
{

    // make sure PDU data is formed properly
    if (!(pmt::is_pair(pdu))) {
        GR_LOG_DEBUG(d_logger, "received unexpected PMT (non-pair)");
        message_port_pub(PMTCONSTSTR__pdu_out(), pdu);
        return;
    }

    double t_now((boost::get_system_time() - d_epoch).total_microseconds() / 1000000.0);

    pmt::pmt_t meta = pmt::car(pdu);

    if (!pmt::is_dict(meta)) {
        GR_LOG_DEBUG(d_logger, "received malformed PDU");
        message_port_pub(PMTCONSTSTR__pdu_out(), pdu);
        return;
    }

    pmt::pmt_t wct_pmt = pmt::dict_ref(meta, d_time_key, pmt::PMT_NIL);
    if (!pmt::is_real(wct_pmt)) {
        GR_LOG_DEBUG(d_logger,
                     boost::format("PDU received with no wall clock time at %f") % t_now);
    } else {
        double pdu_time = pmt::to_double(wct_pmt);
        double time_delta = (t_now - pdu_time) * 1000.0;
        GR_LOG_DEBUG(
            d_logger,
            boost::format("%s PDU received at %f with time delta %f milliseconds") %
                d_name % t_now % time_delta);

        // add to metadata
        meta = pmt::dict_add(meta, d_delta_key, pmt::from_double(time_delta));

        // update estimates
        d_sum_x += time_delta;
        d_sum_x2 += (time_delta * time_delta);
        d_n++;
    }

    // publish
    message_port_pub(PMTCONSTSTR__pdu_out(), (pmt::cons(meta, pmt::cdr(pdu))));
}

} /* namespace timing_utils */
} /* namespace gr */
