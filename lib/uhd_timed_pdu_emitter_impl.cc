/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "uhd_timed_pdu_emitter_impl.h"
#include <timing_utils/constants.h>
#include <gnuradio/io_signature.h>

namespace gr {
namespace timing_utils {

uhd_timed_pdu_emitter::sptr uhd_timed_pdu_emitter::make(float rate, bool drop_late)
{
    return gnuradio::get_initial_sptr(new uhd_timed_pdu_emitter_impl(rate, drop_late));
}

/*
 * The private constructor
 */
uhd_timed_pdu_emitter_impl::uhd_timed_pdu_emitter_impl(float rate, bool drop_late)
    : gr::sync_block("uhd_timed_pdu_emitter",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(0, 0, 0)),
      d_rate(rate),
      d_drop_late(drop_late),
      d_armed(false)
{

    // start time is zero
    d_start_time = to_tpmt(0, 0.0);

    message_port_register_out(PMTCONSTSTR__time());
    message_port_register_in(PMTCONSTSTR__set());

    set_msg_handler(PMTCONSTSTR__set(),
                    boost::bind(&uhd_timed_pdu_emitter_impl::handle_set_time, this, _1));
}


/*
 * Our virtual destructor.
 */
uhd_timed_pdu_emitter_impl::~uhd_timed_pdu_emitter_impl() {}


/*
 * returns a time pmt of integer seconds and fractional seconds
 */
pmt::pmt_t uhd_timed_pdu_emitter_impl::to_tpmt(uint64_t sec, double frac)
{
    return pmt::cons(pmt::from_uint64(sec), pmt::from_double(frac));
}


/*
 * takes a sample offset and time and returns the start time pmt, if the
 * epoch is before zero, the value will be represented in the doubles place
 */
pmt::pmt_t uhd_timed_pdu_emitter_impl::calc_start_time(uint64_t samps, pmt::pmt_t tpmt)
{
    // global vars
    double rate = d_rate;

    int64_t secs = uint64_t(samps / rate);
    double frac = tpmt_to_f(tpmt) - (samps - (secs * rate)) / rate;
    secs = tpmt_to_s(tpmt) - secs;

    // ensure fractional value is between zero and 1
    if (frac < 0.0) {
        frac += 1.0;
        secs--;
    }
    // if time in the past, issue error
    if (frac < 0.0) {
        std::cout << "ERROR: USRP epoch is negative! (" << secs << " " << frac << ")"
                  << std::endl;
    }

    // ensure seconds fits into unsigned...
    while (secs < 0) {
        frac -= 1.0;
        secs++;
    }
    return pmt::cons(pmt::from_uint64(secs), pmt::from_double(frac));
}


/*
 * converts samples to time pmt pair, rate and start time are defaulted properly
 */
pmt::pmt_t uhd_timed_pdu_emitter_impl::samples_to_tpmt(uint64_t samps)
{
    // global vars
    double rate = d_rate;
    pmt::pmt_t start = d_start_time;

    int64_t secs = uint64_t(samps / rate);
    double frac = (samps - (secs * rate)) / rate;
    secs += tpmt_to_s(start);
    frac += tpmt_to_f(start);
    // ensure fractional value is less than 1
    if (frac >= 1.0) {
        frac -= 1.0;
        secs++;
    }
    return pmt::cons(pmt::from_uint64(secs), pmt::from_double(frac));
}


/*
 * converts time pmt pair to samples, rate and start time are defaulted properly
 * negative sample offsets will be coerced to zero...
 */
uint64_t uhd_timed_pdu_emitter_impl::tpmt_to_samples(pmt::pmt_t tpmt)
{
    // global vars
    double rate = d_rate;
    pmt::pmt_t start = d_start_time;

    // use signed value in case we end up past zero
    int64_t secs = tpmt_to_s(tpmt) - tpmt_to_s(start);
    double frac = tpmt_to_f(tpmt) - tpmt_to_f(start);
    // ensure fractional value is not less than zero
    if (frac < 0.0) {
        frac += 1.0;
        secs--;
    }
    if (secs < 0) {
        return 0;
    } else {
        return uint64_t((secs + frac) * rate);
    }
}


/*
 * Arms the block. Input is a PMT with either a single uint64_t PMT containting
 * the trigger sample, or a uint64_t/double pair containing the trigger time
 *
 * THIS CODE USES TIME PAIRS, NOT TUPLES! The rx_time tag is a PMT tuple
 * however timed commands are issued as pairs.
 */
void uhd_timed_pdu_emitter_impl::handle_set_time(pmt::pmt_t time_pmt)
{
    pmt::pmt_t trigger_time = pmt::PMT_NIL;
    if (pmt::is_uint64(time_pmt)) {
        // if the tuple is a single uint64_t it is the sample to trigger
        d_trigger_samp = pmt::to_uint64(time_pmt);
        trigger_time = samples_to_tpmt(d_trigger_samp);

    } else if (pmt::is_pair(time_pmt)) {
        // if it is a pair, check the car and cdr for correct types
        if (pmt::is_uint64(pmt::car(time_pmt)) & pmt::is_real(pmt::cdr(time_pmt))) {
            trigger_time = time_pmt;
            d_trigger_samp = tpmt_to_samples(trigger_time);

            // for anything else, ignore
        } else {
            return;
        }
    } else {
        return;
    }

    // build the output PMT dictionary
    d_pmt_out = pmt::make_dict();
    d_pmt_out = pmt::dict_add(d_pmt_out, PMTCONSTSTR__trigger_time(), trigger_time);
    d_pmt_out = pmt::dict_add(d_pmt_out, PMTCONSTSTR__trigger_sample(), pmt::from_double(d_trigger_samp));
    d_pmt_out = pmt::dict_add(d_pmt_out, PMTCONSTSTR__late_delta(), pmt::from_double(0));
    d_armed = true;
    // std::cout << "ARMED! FOR TIME " << d_pmt_out << std::endl;
}


int uhd_timed_pdu_emitter_impl::work(int noutput_items,
                                     gr_vector_const_void_star& input_items,
                                     gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];

    std::vector<tag_t> tags;

    if (d_armed) {
        // if we are armed, check to see if trigger occurs in this buffer
        if (d_trigger_samp <= (nitems_read(0) + noutput_items)) {
            // check if we are late to trigger
            if (d_trigger_samp < nitems_read(0)) {
                if (!d_drop_late) {
                    message_port_pub(PMTCONSTSTR__trig(), d_pmt_out);
                }
            } else {
                message_port_pub(PMTCONSTSTR__trig(), d_pmt_out);
            }
            d_armed = false;
        }
    }

    // check for uhd rx_time tags to set baseline time and correct for overflows
    get_tags_in_range(
        tags, 0, nitems_read(0), (nitems_read(0) + noutput_items), PMTCONSTSTR__rx_time());
    if (tags.size()) {
        for (int ii = 0; ii < tags.size(); ii++) {
            pmt::pmt_t tpmt = pmt::cons(pmt::tuple_ref(tags[ii].value, 0),
                                        pmt::tuple_ref(tags[ii].value, 1));
            uint64_t t_int = tpmt_to_s(tpmt);
            double t_frac = tpmt_to_f(tpmt);
            uint64_t samp = tags[ii].offset;

            if (samp == 0) {
                d_start_time = to_tpmt(t_int, t_frac);
                // std::cout << "SET INITIAL START TIME TO " << d_start_time << "!" <<
                // std::endl;

            } else {
                d_start_time = calc_start_time(samp, tpmt);
            }
        }
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}


} /* namespace timing_utils */
} /* namespace gr */
