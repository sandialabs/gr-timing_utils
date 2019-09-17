/*
 * Copyright 2011 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_TIMING_UTILS_CONSTANTS_H
#define INCLUDED_TIMING_UTILS_CONSTANTS_H

#include <timing_utils/api.h>

namespace gr {
  namespace timing_utils {

    // message port names
    static const pmt::pmt_t PMTCONSTSTR__TIME = pmt::mp("time");

    // other constants
    static const pmt::pmt_t PMTCONSTSTR__RX_TIME = pmt::mp("rx_time");
    static const pmt::pmt_t PMTCONSTSTR__FREQ = pmt::mp("freq");
    static const pmt::pmt_t PMTCONSTSTR__WALL_CLOCK_TIME = pmt::mp("wall_clock_time");

  } // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_CONSTANTS_H */
