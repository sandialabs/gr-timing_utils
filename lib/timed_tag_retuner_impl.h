/* -*- c++ -*- */
/*
 * Copyright 2018-2021 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_TIMED_TAG_RETUNER_IMPL_H
#define INCLUDED_TIMING_UTILS_TIMED_TAG_RETUNER_IMPL_H

#include <pmt/pmt.h>
#include <timing_utils/constants.h>
#include <timing_utils/timed_tag_retuner.h>
#include <queue>

namespace gr {
namespace timing_utils {
// reference time structure
struct ref_time_t {
    uint64_t secs;
    double frac;
    uint64_t offset;

    ref_time_t(uint64_t secs_, double frac_, uint64_t offset_)
        : secs(secs_), frac(frac_), offset(offset_)
    {
    }

    void set(uint64_t secs_, double frac_, uint64_t offset_)
    {
        secs = secs_;
        frac = frac_;
        offset = offset_;
    }
};

// tune command structure
struct tune_command_t {
    pmt::pmt_t tag;
    uint64_t offset;
    bool tag_now;

    tune_command_t(pmt::pmt_t tag_, uint64_t offset_, bool tag_now_)
        : tag(tag_), offset(offset_), tag_now(tag_now_)
    {
    }
};

class timed_tag_retuner_impl : public timed_tag_retuner
{
private:
    // reference time
    ref_time_t d_ref_time;

    // command queue
    std::queue<tune_command_t> d_tune_commands;

    // sample rate
    double d_sample_rate;

    // pmt keys
    pmt::pmt_t d_tag_key;
    pmt::pmt_t d_dict_key;
    pmt::pmt_t d_time_key;

    // command handler
    void command_handler(pmt::pmt_t msg);

public:
    timed_tag_retuner_impl(double sample_rate,
                           pmt::pmt_t dict_key,
                           uint64_t origin_t_secs,
                           double origin_t_frac);
    ~timed_tag_retuner_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_TIMED_TAG_RETUNER_IMPL_H */
