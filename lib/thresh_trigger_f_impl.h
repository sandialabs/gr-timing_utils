/* -*- c++ -*- */
/*
 * Copyright 2018-2021 National Technology & Engineering Solutions of
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

    void disarm(pmt::pmt_t);

public:
    thresh_trigger_f_impl(float hi, float lo, int length);
    ~thresh_trigger_f_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    void set_hi(float t) override { d_thresh_hi = t; };
    void set_lo(float t) override { d_thresh_lo = t; };
    void set_len(int len) override { d_length_thresh = len; }
};

} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_THRESH_TRIGGER_F_IMPL_H */
