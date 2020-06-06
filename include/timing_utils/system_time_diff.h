/* -*- c++ -*- */
/*
 * Copyright 2018 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
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
