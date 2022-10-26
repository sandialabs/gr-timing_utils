/* -*- c++ -*- */
/*
 * Copyright 2018-2021 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_UHD_TIMED_PDU_EMITTER_IMPL_H
#define INCLUDED_TIMING_UTILS_UHD_TIMED_PDU_EMITTER_IMPL_H

#include <gnuradio/timing_utils/uhd_timed_pdu_emitter.h>

namespace gr {
namespace timing_utils {

class uhd_timed_pdu_emitter_impl : public uhd_timed_pdu_emitter
{
private:
    float d_rate;
    bool d_drop_late;
    bool d_armed;
    uint64_t d_trigger_samp;
    pmt::pmt_t d_pmt_out;
    pmt::pmt_t d_start_time;

    pmt::pmt_t to_tpmt(uint64_t, double);

    uint64_t tpmt_to_s(pmt::pmt_t tpmt) { return pmt::to_uint64(pmt::car(tpmt)); }

    double tpmt_to_f(pmt::pmt_t tpmt) { return pmt::to_double(pmt::cdr(tpmt)); }


    pmt::pmt_t calc_start_time(uint64_t, pmt::pmt_t);
    pmt::pmt_t samples_to_tpmt(uint64_t);
    uint64_t tpmt_to_samples(pmt::pmt_t);

public:
    /**
     * Constructor
     *
     * @param rate -
     * @param drop_late -
     */
    uhd_timed_pdu_emitter_impl(float rate, bool drop_late);
    ~uhd_timed_pdu_emitter_impl();

    // input message handler
    void handle_set_time(pmt::pmt_t);

    /**
     *
     * @param rate -
     */
    void set_rate(float rate) { d_rate = rate; }

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_UHD_TIMED_PDU_EMITTER_IMPL_H */
