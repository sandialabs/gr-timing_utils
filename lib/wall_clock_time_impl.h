/* -*- c++ -*- */
/*
 * Copyright 2018-2021 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_WALL_CLOCK_TIME_IMPL_H
#define INCLUDED_TIMING_UTILS_WALL_CLOCK_TIME_IMPL_H

#include <gnuradio/timing_utils/constants.h>
#include <gnuradio/timing_utils/wall_clock_time.h>

namespace gr {
namespace timing_utils {

class wall_clock_time_impl : public wall_clock_time
{
private:
    boost::posix_time::ptime d_epoch;
    std::string d_name;
    pmt::pmt_t d_key;

public:
    wall_clock_time_impl(const pmt::pmt_t key);
    ~wall_clock_time_impl();

    void handle_pdu(pmt::pmt_t pdu);
};

} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_WALL_CLOCK_TIME_IMPL_H */
