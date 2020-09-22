/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_ADD_USRP_TAGS_IMPL_H
#define INCLUDED_TIMING_UTILS_ADD_USRP_TAGS_IMPL_H

#include <timing_utils/add_usrp_tags.h>

namespace gr {
namespace timing_utils {

template <class T>
class add_usrp_tags_impl : public add_usrp_tags<T>
{
private:
    pmt::pmt_t d_freq_pmt;
    pmt::pmt_t d_rate_pmt;
    pmt::pmt_t d_time_pmt;
    pmt::pmt_t d_pmt_dict;
    uint64_t d_tagged_sample;
    bool d_tag_now;

public:
    /*!
     * Constructor
     *
     *
     * \param freq Center frequency (Hz)
     * \param rate Sampling rate (Hz)
     * \param epoch_int Whole seconds of first sample
     * \param epoch_frac Fractional seconds of first sample
     */
    add_usrp_tags_impl(double freq, double rate, uint64_t epoch_int, double epoch_frac);
    ~add_usrp_tags_impl();

    // the dummy argument allows the GRC callback to call this function
    void tag_now(uint32_t dummy) { d_tag_now = true; }
    void update_tags(pmt::pmt_t update);
    pmt::pmt_t last_tag();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_ADD_USRP_TAGS_IMPL_H */
