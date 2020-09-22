/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_THRESH_TRIGGER_F_IMPL_H
#define INCLUDED_TIMING_UTILS_THRESH_TRIGGER_F_IMPL_H

#include <timing_utils/thresh_trigger_f.h>

namespace gr {
namespace timing_utils {

class thresh_trigger_f_impl : public thresh_trigger_f
{
private:
    float d_thresh_hi;
    float d_thresh_lo;
    bool d_blank;
    uint64_t d_length;
    int d_length_thresh;

public:
    /**
     * Constructor
     *
     * @param hi -
     * @param lo -
     * @param length -
     */
    thresh_trigger_f_impl(float hi, float lo, int length);
    ~thresh_trigger_f_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    void disarm(pmt::pmt_t);

    /**
     *
     * @param t -
     */
    void set_hi(float t) { d_thresh_hi = t; };

    /**
     *
     * @param t -
     */
    void set_lo(float t) { d_thresh_lo = t; };

    /**
     *
     * @param len -
     */
    void set_len(int len) { d_length_thresh = len; }
};

} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_THRESH_TRIGGER_F_IMPL_H */
