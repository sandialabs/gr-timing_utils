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
import timing_utils_swig as timing_utils
import sandia_utils
import pdu_utils
import pmt
import time


class qa_timed_tag_retuner(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_instantiate(self):
        # data
        src_data = (1 + 1j, 2 + 2j, 3 + 3j)
        expected_result = (1 + 1j, 2 + 2j, 3 + 3j)

        # blocks
        src = blocks.vector_source_c(src_data)
        cts = timing_utils.timed_tag_retuner(1e6, pmt.intern("haha"), 1, 0.1)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, cts)
        self.tb.connect(cts, dst)

        self.tb.run()
        result_data = dst.data()

        # assert
        print("got {}, expected {}".format(result_data, expected_result))
        self.assertEqual(expected_result, result_data)

    def test_002_tag_immediately(self):
        # tune message
        tune = pmt.dict_add(pmt.make_dict(), pmt.intern('freq'), pmt.from_double(100))

        # blocks
        src = blocks.null_source(gr.sizeof_gr_complex * 1)
        throttle = blocks.throttle(gr.sizeof_gr_complex * 1, 32000, True)
        retuner = timing_utils.timed_tag_retuner(1e6, pmt.intern("freq"), 1, 0.1)
        debug = sandia_utils.sandia_tag_debug(gr.sizeof_gr_complex * 1, '', "", True)
        emitter = pdu_utils.message_emitter()
        debug.set_display(True)
        self.tb.connect(src, throttle)
        self.tb.connect(throttle, retuner)
        self.tb.connect(retuner, debug)
        self.tb.msg_connect((emitter, 'msg'), (retuner, 'command'))

        self.tb.start()
        time.sleep(.1)
        emitter.emit(tune)
        time.sleep(.1)
        self.tb.stop()

        # assert
        self.assertEqual(debug.num_tags(), 1)
        tag = debug.get_tag(0)
        self.assertTrue(pmt.equal(tag.key, pmt.intern('set_freq')))
        freq = pmt.to_double(tag.value)
        self.assertAlmostEqual(-100, freq)

    def test_003_time_tag(self):
        fs = 32000

        # tune message at exactly 1.0 second
        tune = pmt.dict_add(pmt.make_dict(), pmt.intern('freq'), pmt.from_double(100))
        tune = pmt.dict_add(tune, pmt.intern('time'), pmt.cons(pmt.from_uint64(1), pmt.from_double(0)))

        # blocks
        src = blocks.null_source(gr.sizeof_gr_complex * 1)
        throttle = blocks.throttle(gr.sizeof_gr_complex * 1, fs, True)
        retuner = timing_utils.timed_tag_retuner(fs, pmt.intern("freq"), 0, 0.0)
        debug = sandia_utils.sandia_tag_debug(gr.sizeof_gr_complex * 1, '', "", True)
        emitter = pdu_utils.message_emitter()
        debug.set_display(True)
        self.tb.connect(src, throttle)
        self.tb.connect(throttle, retuner)
        self.tb.connect(retuner, debug)
        self.tb.msg_connect((emitter, 'msg'), (retuner, 'command'))

        self.tb.start()
        time.sleep(.1)
        emitter.emit(tune)
        time.sleep(1.5)
        self.tb.stop()

        # assert
        self.assertEqual(debug.num_tags(), 1)
        tag = debug.get_tag(0)
        self.assertTrue(pmt.equal(tag.key, pmt.intern('set_freq')))
        freq = pmt.to_double(tag.value)
        self.assertAlmostEqual(-100, freq)
        self.assertEqual(tag.offset, 32000)


if __name__ == '__main__':
    gr_unittest.run(qa_timed_tag_retuner)
