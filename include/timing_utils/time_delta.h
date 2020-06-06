/* -*- c++ -*- */
/*
 * Copyright 2020 gr-timing_utils author.
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
