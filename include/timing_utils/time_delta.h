/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_TIME_DELTA_H
#define INCLUDED_TIMING_UTILS_TIME_DELTA_H

#include <gnuradio/block.h>
#include <timing_utils/api.h>

namespace gr {
namespace timing_utils {

/*!
 * \brief <+description of block+>
 * \ingroup timing_utils
 *
 */
class TIMING_UTILS_API time_delta : virtual public gr::block
{
public:
    typedef boost::shared_ptr<time_delta> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of timing_utils::time_delta.
     *
     * @param delta_key - key to use for time delta metadata field
     * @param time_key - key to use as reference time for delta
     */
    static sptr make(const pmt::pmt_t delta_key, const pmt::pmt_t time_key);
};

} // namespace timing_utils
  // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_TIME_DELTA_H */
