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
import time
try:
    import timing_utils
except ImportError:
    import os
    import sys
    dirname, filename = os.path.split(os.path.abspath(__file__))
    sys.path.append(os.path.join(dirname, "bindings"))
    import timing_utils


class qa_timed_freq_xlating_fir (gr_unittest.TestCase):

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
        self.dut = timing_utils.timed_freq_xlating_fir_ccc(1, [1], 915e6, self.rate)
        self.tag_dbg = blocks.tag_debug(gr.sizeof_gr_complex * 1, '', "")

        self.tb.connect((self.src, 0), (self.dut, 0))
        self.tb.connect((self.dut, 0), (self.tag_dbg, 0))

        # set up fg
        self.tb.run()
        time.sleep(0.05)

        # check data


if __name__ == '__main__':
    gr_unittest.run(qa_timed_freq_xlating_fir)
