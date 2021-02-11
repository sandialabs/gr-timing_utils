#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
# (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
# retains certain rights in this software.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from builtins import range
from gnuradio import gr, gr_unittest
from gnuradio import blocks
import timing_utils
import time
import pmt


class qa_system_time_diff (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_notags(self):
        self.tb = gr.top_block()
        self.rate = 250000
        self.interval = 173
        self.duration = 9000

        self.src = blocks.vector_source_c(list(range(self.duration)), False, 1, [])
        self.time_diff = timing_utils.system_time_diff_c(False, False)
        self.nullc = blocks.null_sink(gr.sizeof_gr_complex * 1)
        self.nullf = blocks.null_sink(gr.sizeof_float * 1)
        self.tb.connect((self.src, 0), (self.time_diff, 0))
        self.tb.connect((self.time_diff, 0), (self.nullc, 0))
        self.tb.connect((self.time_diff, 1), (self.nullf, 0))

        # set up fg
        self.tb.run()
        time.sleep(0.05)

    def test_002_tags(self):
        start_time = 0.1
        self.duration = 125000
        tnow = time.time()

        src_tag = gr.tag_utils.python_to_tag([0, pmt.intern("wall_clock_time"), pmt.from_double(tnow - 10000), pmt.intern("test_002_tags")])
        self.src = blocks.vector_source_c(list(range(self.duration)), False, 1, [src_tag])
        self.throttle = blocks.throttle(gr.sizeof_gr_complex * 1, 250000)
        self.dut = timing_utils.system_time_diff_c(True, False)
        self.tag_dbg = blocks.tag_debug(gr.sizeof_gr_complex * 1, '', "")
        self.nullf = blocks.null_sink(gr.sizeof_float * 1)

        self.tb.connect((self.src, 0), (self.throttle, 0))
        self.tb.connect((self.throttle, 0), (self.dut, 0))
        self.tb.connect((self.dut, 0), (self.tag_dbg, 0))
        self.tb.connect((self.dut, 1), (self.nullf, 0))

        self.tb.start()
        time.sleep(.01)

        tags = self.tag_dbg.current_tags()
        print("Dumping tags")
        for t in tags:
            print('Tag:', t.key, ' ', t.value)
            if pmt.eq(t.key, pmt.intern("wall_clock_time")):
                time_tag = t

        if time_tag:
            self.assertAlmostEqual(tnow, pmt.to_double(time_tag.value), delta=60)
        else:
            self.assertTrue(False)

        time.sleep(.1)
        self.tb.stop()


if __name__ == '__main__':
    gr_unittest.run(qa_system_time_diff)
