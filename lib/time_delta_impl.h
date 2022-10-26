/* -*- c++ -*- */
/*
 * Copyright 2018-2021 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_TIME_DELTA_IMPL_H
#define INCLUDED_TIMING_UTILS_TIME_DELTA_IMPL_H

#include <gnuradio/timing_utils/constants.h>
#include <gnuradio/timing_utils/time_delta.h>

namespace gr {
namespace timing_utils {

class time_delta_impl : public time_delta
{
private:
    boost::posix_time::ptime d_epoch;
    std::string d_name;
    pmt::pmt_t d_delta_key;
    pmt::pmt_t d_time_key;

    // statistic tracking
    double d_sum_x, d_sum_x2;
    int d_n;

public:
    /**
     * Constructor
     *
     */
    time_delta_impl(const pmt::pmt_t delta_key, const pmt::pmt_t time_key);
    ~time_delta_impl();

    // overloaded functions
    bool stop();

    void handle_pdu(pmt::pmt_t pdu);
};

} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_TIME_DELTA_IMPL_H */
