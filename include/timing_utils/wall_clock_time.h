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
