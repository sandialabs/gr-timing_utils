#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2018-2021 National Technology & Engineering Solutions of Sandia, LLC
# (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
# retains certain rights in this software.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#


from gnuradio import gr, gr_unittest
from gnuradio import blocks
import pmt
import time
try:
    import timing_utils
except ImportError:
    import os
    import sys
    dirname, filename = os.path.split(os.path.abspath(__file__))
    sys.path.append(os.path.join(dirname, "bindings"))
    import timing_utils


class qa_constants (gr_unittest.TestCase):

    def setUp(self):
        self.tb = None

    def tearDown(self):
        self.tb = None

    def test_interned_string_constants(self):

        assert(pmt.eq(timing_utils.PMTCONSTSTR__time(), pmt.intern('time')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__trig(), pmt.intern('trig')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__set(), pmt.intern('set')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__disarm(), pmt.intern('disarm')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__rx_time(), pmt.intern('rx_time')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__rx_rate(), pmt.intern('rx_rate')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__rx_freq(), pmt.intern('rx_freq')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__rx_sample(), pmt.intern('rx_sample')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__freq(), pmt.intern('freq')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__wall_clock_time(), pmt.intern('wall_clock_time')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__pdu_out(), pmt.intern('pdu_out')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__pdu_in(), pmt.intern('pdu_in')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__trigger_time(), pmt.intern('trigger_time')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__trigger_sample(), pmt.intern('trigger_sample')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__trigger_now(), pmt.intern('trigger_now')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__late_delta(), pmt.intern('late_delta')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__command(), pmt.intern('command')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__set_freq(), pmt.intern('set_freq')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__in(), pmt.intern('in')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__dsp_freq(), pmt.intern('dsp_freq')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__START(), pmt.intern('START')))
        assert(pmt.eq(timing_utils.PMTCONSTSTR__END(), pmt.intern('END')))


if __name__ == '__main__':
    gr_unittest.run(qa_constants)
