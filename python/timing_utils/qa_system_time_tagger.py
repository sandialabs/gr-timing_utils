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
import time
import pmt
try:
    from gnuradio import timing_utils
except ImportError:
    import os
    import sys
    dirname, filename = os.path.split(os.path.abspath(__file__))
    sys.path.append(os.path.join(dirname, "bindings"))
    from gnuradio import timing_utils


class qa_system_time_tagger (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_instantiate(self):
        self.tb = gr.top_block()
        self.rate = 250000
        self.interval = 173
        self.duration = 9000

        self.src = blocks.vector_source_c(list(range(self.duration)), False, 1, [])
        self.dut = timing_utils.system_time_tagger_c(7000)
        self.tag_dbg = blocks.tag_debug(gr.sizeof_gr_complex * 1, '', "")

        self.tb.connect((self.src, 0), (self.dut, 0))
        self.tb.connect((self.dut, 0), (self.tag_dbg, 0))

        # set up fg
        self.tb.run()
        time.sleep(0.05)

        # check data

    def test_002_normal(self):
        start_time = 0.1
        self.duration = 10
        tnow = time.time()

        #src_tag = gr.tag_utils.python_to_tag([0, pmt.intern("wall_clock_time"), pmt.from_double(tnow - 10000), pmt.intern("test_002_tags")])
        self.src = blocks.vector_source_c(list(range(self.duration)), False, 1, [])
        self.throttle = blocks.throttle(gr.sizeof_gr_complex * 1, 250000)
        self.dut = timing_utils.system_time_tagger_c(7)
        self.tag_dbg = blocks.tag_debug(gr.sizeof_gr_complex * 1, '', "")

        self.tb.connect((self.src, 0), (self.throttle, 0))
        self.tb.connect((self.throttle, 0), (self.dut, 0))
        self.tb.connect((self.dut, 0), (self.tag_dbg, 0))

        self.tb.start()
        time.sleep(.01)

        time_tag = None
        tags = self.tag_dbg.current_tags()
        print("Dumping tags")
        for t in tags:
            print('Tag:', t.key, ' ', t.value)
            if pmt.eq(t.key, pmt.intern("wall_clock_time")):
                time_tag = t

        if time_tag is not None:
            self.assertAlmostEqual(tnow, pmt.to_double(time_tag.value), delta=60)
        else:
            self.assertTrue(False)

        time.sleep(.1)
        self.tb.stop()


if __name__ == '__main__':
    gr_unittest.run(qa_system_time_tagger)
