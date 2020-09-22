/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_SYSTEM_TIME_DIFF_H
#define INCLUDED_TIMING_UTILS_SYSTEM_TIME_DIFF_H

#include <gnuradio/sync_block.h>
#include <timing_utils/api.h>

namespace gr {
namespace timing_utils {

/*!
 * \brief Time difference between current time and the system time tag
 *
 * \ingroup timing_utils
 *
 * Print to stdout the time difference between the system time a tag is
 * received and the time associated with the tag.  The tag is typically
 * added to a data stream by the system_time_tagger block.
 *
 * The tag 'wall_clock_time' must be a double.
 */
template <class T>
class TIMING_UTILS_API system_time_diff : virtual public gr::block
{
public:
    typedef boost::shared_ptr<system_time_diff<T>> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of timing_utils::system_time_diff.
     *
     * \param output_data Flag to pass data through block
     * \param update_time Flag to update the time tag to the current system time
     * \param output_diff Output the time difference as a float stream (note,
     *        this feature is not currently enabled)
     */
    static sptr make(bool output_data, bool update_time, bool output_diff);
};

typedef system_time_diff<unsigned char> system_time_diff_b;
typedef system_time_diff<short> system_time_diff_s;
typedef system_time_diff<float> system_time_diff_f;
typedef system_time_diff<gr_complex> system_time_diff_c;
} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_SYSTEM_TIME_DIFF_H */
