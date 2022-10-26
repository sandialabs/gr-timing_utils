#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
# (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
# retains certain rights in this software.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
from retune_uhd_to_timed_tag import retune_uhd_to_timed_tag
import pmt


class qa_retune_uhd_to_timed_tag (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_instantiate(self):

        # data
        src_data = [1 + 1j, 2 + 2j, 3 + 3j]
        expected_result = [1 + 1j, 2 + 2j, 3 + 3j]

        # blocks
        src = blocks.vector_source_c(src_data)
        cts = retune_uhd_to_timed_tag(1e6, pmt.intern("haha"), 1, 0.1)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, cts)
        self.tb.connect(cts, dst)

        self.tb.run()
        result_data = dst.data()

        # assert
        print("got {}, expected {}".format(result_data, expected_result))
        self.assertEqual(expected_result, result_data)


if __name__ == '__main__':
    gr_unittest.run(qa_retune_uhd_to_timed_tag)
