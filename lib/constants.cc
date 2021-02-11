/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <timing_utils/constants.h>

namespace gr {
namespace timing_utils {


const pmt::pmt_t PMTCONSTSTR__time()
{
	static const pmt::pmt_t val = pmt::mp("time");
	return val;
}
const pmt::pmt_t PMTCONSTSTR__trig()
{
	static const pmt::pmt_t val = pmt::mp("trig");
	return val;
}
const pmt::pmt_t PMTCONSTSTR__set()
{
	static const pmt::pmt_t val = pmt::mp("set");
	return val;
}
const pmt::pmt_t PMTCONSTSTR__disarm()
{
	static const pmt::pmt_t val = pmt::mp("disarm");
	return val;
}
const pmt::pmt_t PMTCONSTSTR__rx_time()
{
	static const pmt::pmt_t val = pmt::mp("rx_time");
	return val;
}
const pmt::pmt_t PMTCONSTSTR__rx_rate()
{
	static const pmt::pmt_t val = pmt::mp("rx_rate");
	return val;
}
const pmt::pmt_t PMTCONSTSTR__rx_freq()
{
	static const pmt::pmt_t val = pmt::mp("rx_freq");
	return val;
}
const pmt::pmt_t PMTCONSTSTR__rx_sample()
{
	static const pmt::pmt_t val = pmt::mp("rx_sample");
	return val;
}
const pmt::pmt_t PMTCONSTSTR__freq()
{
	static const pmt::pmt_t val = pmt::mp("freq");
	return val;
}
const pmt::pmt_t PMTCONSTSTR__wall_clock_time()
{
	static const pmt::pmt_t val = pmt::mp("wall_clock_time");
	return val;
}
const pmt::pmt_t PMTCONSTSTR__pdu_out()
{
	static const pmt::pmt_t val = pmt::mp("pdu_out");
	return val;
}
const pmt::pmt_t PMTCONSTSTR__pdu_in()
{
	static const pmt::pmt_t val = pmt::mp("pdu_in");
	return val;
}
const pmt::pmt_t PMTCONSTSTR__trigger_time()
{
	static const pmt::pmt_t val = pmt::mp("trigger_time");
	return val;
}
const pmt::pmt_t PMTCONSTSTR__trigger_sample()
{
	static const pmt::pmt_t val = pmt::mp("trigger_sample");
	return val;
}
const pmt::pmt_t PMTCONSTSTR__trigger_now()
{
	static const pmt::pmt_t val = pmt::mp("trigger_now");
	return val;
}
const pmt::pmt_t PMTCONSTSTR__late_delta()
{
	static const pmt::pmt_t val = pmt::mp("late_delta");
	return val;
}
const pmt::pmt_t PMTCONSTSTR__command()
{
	static const pmt::pmt_t val = pmt::mp("command");
	return val;
}
const pmt::pmt_t PMTCONSTSTR__set_freq()
{
	static const pmt::pmt_t val = pmt::mp("set_freq");
	return val;
}
const pmt::pmt_t PMTCONSTSTR__in()
{
  static const pmt::pmt_t val = pmt::mp("in");
  return val;
}
const pmt::pmt_t PMTCONSTSTR__dsp_freq()
{
  static const pmt::pmt_t val = pmt::mp("dsp_freq");
  return val;
}
const pmt::pmt_t PMTCONSTSTR__START()
{
  static const pmt::pmt_t val = pmt::mp("START");
  return val;
}
const pmt::pmt_t PMTCONSTSTR__END()
{
  static const pmt::pmt_t val = pmt::mp("END");
  return val;
}

}
}

