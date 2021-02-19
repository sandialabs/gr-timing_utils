/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_INTERRUPT_EMITTER_IMPL_H
#define INCLUDED_TIMING_UTILS_INTERRUPT_EMITTER_IMPL_H

#include "reference_timer.h"
#include <timing_utils/interrupt_emitter.h>

namespace gr {
namespace timing_utils {

template <class T>
class interrupt_emitter_impl : public interrupt_emitter<T>, public reference_timer
{
private:
    boost::thread* timer_thread;
    double d_rate;
    bool d_drop_late;
    bool d_armed;
    uint64_t d_trigger_samp;
    uint64_t d_start_sample;
    pmt::pmt_t d_pmt_out;
    double d_start_time;
    boost::posix_time::ptime d_epoch;
    double d_time_offset;

    pmt::pmt_t samples_to_tpmt(uint64_t trigger_sample);
    uint64_t time_to_samples(double time);
    void process_interrupt();

    double d_last_tag_time;
    uint64_t d_last_tag_samp;

public:
    /*!
     * Constructor
     *
     *
     * \param rate Sample rate (Hz)
     * \param drop_late If true, do not emit a message for interrupt requests
     *    in the past
     *
     */
    interrupt_emitter_impl(double rate, bool drop_late);
    ~interrupt_emitter_impl();

    void set_rate(double rate) { d_rate = rate; }
    void set_debug(bool value) { debug = value; }
    void handle_set_time(pmt::pmt_t int_time);
    bool isLoaded() { return loaded; }

    // overloaded block functions
    bool start();
    bool stop();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_INTERRUPT_EMITTER_IMPL_H */
