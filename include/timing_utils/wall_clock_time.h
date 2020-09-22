/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_WALL_CLOCK_TIME_H
#define INCLUDED_TIMING_UTILS_WALL_CLOCK_TIME_H

#include <gnuradio/block.h>
#include <timing_utils/api.h>

namespace gr {
namespace timing_utils {

/*!
 * \brief Adds wall clock K-V dict entry
 * \ingroup timing_utils
 *
 * Adds wall_clock_time key to PDU dict containing wall clock time in  unix epoc
 */
class TIMING_UTILS_API wall_clock_time : virtual public gr::block
{
public:
    typedef boost::shared_ptr<wall_clock_time> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of timing_utils::wall_clock_time.
     *
     * @param key - key to use for wall clock time metadata field
     */
    static sptr make(const pmt::pmt_t key);
};

} // namespace timing_utils
  // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_WALL_CLOCK_TIME_H */
