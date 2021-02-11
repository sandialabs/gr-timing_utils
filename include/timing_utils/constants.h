/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_CONSTANTS_H
#define INCLUDED_TIMING_UTILS_CONSTANTS_H

#include <pmt/pmt.h>
#include <timing_utils/api.h>

namespace gr {
namespace timing_utils {


TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__time();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__trig();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__set();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__disarm();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__rx_time();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__rx_rate();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__rx_freq();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__rx_sample();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__freq();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__wall_clock_time();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__pdu_out();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__pdu_in();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__trigger_time();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__trigger_sample();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__trigger_now();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__late_delta();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__command();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__set_freq();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__in();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__dsp_freq();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__START();
TIMING_UTILS_API const pmt::pmt_t PMTCONSTSTR__END();

} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_CONSTANTS_H */
