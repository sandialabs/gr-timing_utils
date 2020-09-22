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

// message port names
static const pmt::pmt_t PMTCONSTSTR__TIME = pmt::mp("time");
static const pmt::pmt_t PMTCONSTSTR__TRIG = pmt::mp("trig");
static const pmt::pmt_t PMTCONSTSTR__SET = pmt::mp("set");
static const pmt::pmt_t PMTCONSTSTR__DISARM = pmt::mp("disarm");

// other constants
static const pmt::pmt_t PMTCONSTSTR__RX_TIME = pmt::mp("rx_time");
static const pmt::pmt_t PMTCONSTSRT__RX_RATE = pmt::mp("rx_rate");
static const pmt::pmt_t PMTCONSTSRT__RX_FREQ = pmt::mp("rx_freq");
static const pmt::pmt_t PMTCONSTSRT__RX_SAMPLE = pmt::mp("rx_sample");
static const pmt::pmt_t PMTCONSTSTR__FREQ = pmt::mp("freq");
static const pmt::pmt_t PMTCONSTSTR__WALL_CLOCK_TIME = pmt::mp("wall_clock_time");
static const pmt::pmt_t PMTCONSTSTR__PDU_OUT = pmt::mp("pdu_out");
static const pmt::pmt_t PMTCONSTSTR__PDU_IN = pmt::mp("pdu_in");
static const pmt::pmt_t PMTCONSTSTR__TRIG_TIME = pmt::mp("trigger_time");
static const pmt::pmt_t PMTCONSTSTR__TRIG_SAMP = pmt::mp("trigger_sample");
static const pmt::pmt_t PMTCONSTSTR__TRIG_NOW = pmt::mp("trigger_now");
static const pmt::pmt_t PMTCONSTSTR__LATE_DELTA = pmt::mp("late_delta");
static const pmt::pmt_t PMTCONSTSTR__COMMAND = pmt::mp("command");
static const pmt::pmt_t PMTCONSTSTR__SET_FREQ = pmt::mp("set_freq");
} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_CONSTANTS_H */
