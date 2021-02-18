#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2018-2021 National Technology & Engineering Solutions of Sandia, LLC
# (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
# retains certain rights in this software.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from builtins import range
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


class qa_add_usrp_tags (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def makeDict(self, **kwargs):
        pmtDict = pmt.make_dict()
        if "freq" in kwargs:
            pmtDict = pmt.dict_add(pmtDict, pmt.intern("rx_freq"), pmt.from_double(kwargs["freq"]))
        if "rate" in kwargs:
            pmtDict = pmt.dict_add(pmtDict, pmt.intern("rx_rate"), pmt.from_double(kwargs["rate"]))
        if "epoch_int" in kwargs and "epoch_frac" in kwargs:
            pmtDict = pmt.dict_add(pmtDict, pmt.intern("rx_time"),
                                   pmt.make_tuple(pmt.from_uint64(kwargs["epoch_int"]), pmt.from_double(kwargs["epoch_frac"])))
        return pmtDict

    def test_002_update(self):
        start_time = 0.1
        self.duration = 125000
        self.src = blocks.vector_source_c(list(range(self.duration)), False, 1, [])
        self.throttle = blocks.throttle(gr.sizeof_gr_complex * 1, 250000)
        self.utag = timing_utils.add_usrp_tags_c(1090e6, 250000, 0, start_time)
        self.tag_dbg = blocks.tag_debug(gr.sizeof_gr_complex * 1, '', "")

        self.tb.connect((self.src, 0), (self.throttle, 0))
        self.tb.connect((self.throttle, 0), (self.utag, 0))
        self.tb.connect((self.utag, 0), (self.tag_dbg, 0))

        self.tb.start()
        time.sleep(.01)
        #print("Dumping tags")
        for t in self.tag_dbg.current_tags():
            #print( 'Tag:' , t.key, ' ', t.value )
            if pmt.eq(t.key, pmt.intern("rx_freq")):
                self.assertAlmostEqual(1090e6, pmt.to_double(t.value))
            if pmt.eq(t.key, pmt.intern("rx_rate")):
                self.assertAlmostEqual(250000, pmt.to_double(t.value))

        self.utag.update_tags(self.makeDict(freq=1091e6, rate=260000, epoch_int=0, epoch_frac=start_time + .3))
        time.sleep(.01)
        #print("Dumping tags")
        for t in self.tag_dbg.current_tags():
            #print( 'Tag:' , t.key, ' ', t.value )
            if pmt.eq(t.key, pmt.intern("rx_freq")):
                self.assertAlmostEqual(1091e6, pmt.to_double(t.value))
            if pmt.eq(t.key, pmt.intern("rx_rate")):
                self.assertAlmostEqual(260000, pmt.to_double(t.value))

        time.sleep(.1)
        self.tb.stop()


if __name__ == '__main__':
    gr_unittest.run(qa_add_usrp_tags)
