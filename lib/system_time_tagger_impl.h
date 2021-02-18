/* -*- c++ -*- */
/*
 * Copyright 2018-2021 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_SYSTEM_TIME_TAGGER_IMPL_H
#define INCLUDED_TIMING_UTILS_SYSTEM_TIME_TAGGER_IMPL_H

#include <timing_utils/constants.h>
#include <timing_utils/system_time_tagger.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread_time.hpp>

namespace gr {
namespace timing_utils {

template <class T>
class system_time_tagger_impl : public system_time_tagger<T>
{
private:
    boost::posix_time::ptime d_epoch;
    uint32_t d_interval;
    uint64_t d_next_tag_offset;
    bool d_tagging_enabled;
    uint64_t d_total_nitems_read;

public:
    system_time_tagger_impl(uint32_t tag_interval);
    ~system_time_tagger_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    void set_interval(uint32_t tag_interval) override;
};

} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_SYSTEM_TIME_TAGGER_IMPL_H */
