/* -*- c++ -*- */
/*
 * Copyright 2018-2021 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_SYSTEM_TIME_DIFF_IMPL_H
#define INCLUDED_TIMING_UTILS_SYSTEM_TIME_DIFF_IMPL_H

#include <timing_utils/constants.h>
#include <timing_utils/system_time_diff.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread_time.hpp>

namespace gr {
namespace timing_utils {

template <class T>
class system_time_diff_impl : public system_time_diff<T>
{
private:
    boost::posix_time::ptime d_epoch;
    bool d_update_time;
    bool d_output_diff;

public:
    system_time_diff_impl(bool update_time, bool output_diff);
    ~system_time_diff_impl() override;

    // Where all the action really happens
    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_SYSTEM_TIME_DIFF_IMPL_H */
