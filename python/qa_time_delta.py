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
import timing_utils
import pdu_utils
import pmt
import numpy
import time


class qa_time_delta(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

        self.emitter = pdu_utils.message_emitter(pmt.PMT_NIL)
        self.dut = timing_utils.time_delta(pmt.intern("time_delta_ms"), pmt.intern("wall_clock_time"))
        self.debug = blocks.message_debug()

        self.tb.msg_connect((self.emitter, 'msg'), (self.dut, 'pdu_in'))
        self.tb.msg_connect((self.dut, 'pdu_out'), (self.debug, 'store'))

    def tearDown(self):
        self.tb = None

    def test_001_invalid_a(self):
        in_data = [0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1]

        # set up fg
        self.tb.start()
        time.sleep(.001)
        self.emitter.emit(pmt.intern("MALFORMED PDU"))
        time.sleep(.01)
        self.tb.stop()
        self.tb.wait()

        self.assertEqual(0, self.debug.num_messages())

    def test_001_invalid_b(self):
        in_data = [0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1]
        meta = pmt.dict_add(pmt.make_dict(), pmt.intern('sam'), pmt.from_double(25.1))
        in_pdu = pmt.cons(meta, pmt.init_c32vector(len(in_data), in_data))

        # set up fg
        self.tb.start()
        time.sleep(.001)
        self.emitter.emit(in_pdu)
        time.sleep(.01)
        self.tb.stop()
        self.tb.wait()

        self.assertEqual(0, self.debug.num_messages())

    def test_002_normal(self):

        tnow = time.time()

        in_data = [0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1]
        meta = pmt.dict_add(pmt.make_dict(), pmt.intern('wall_clock_time'), pmt.from_double(tnow - 10))
        in_pdu = pmt.cons(meta, pmt.init_c32vector(len(in_data), in_data))

        e_data = [0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1]
        e_meta = pmt.dict_add(pmt.make_dict(), pmt.intern('wall_clock_time'), pmt.from_double(tnow))
        e_pdu = pmt.cons(e_meta, pmt.init_c32vector(len(e_data), e_data))

        # set up fg
        self.tb.start()
        time.sleep(.001)
        self.emitter.emit(in_pdu)
        time.sleep(.01)
        self.tb.stop()
        self.tb.wait()
        # check data

        self.assertEqual(1, self.debug.num_messages())

        #print("test_002_normal2:")
        #print("pdu expected: " + repr(pmt.car(e_pdu)))
        #print("pdu got:      " + repr(pmt.car(self.debug.get_message(0))))
        #print("data expected: " + repr(pmt.to_python(pmt.cdr(e_pdu))))
        #print("data got:      " + repr(pmt.to_python(pmt.cdr(self.debug.get_message(0)))))
        #print

        a_meta = pmt.car(self.debug.get_message(0))

        time_tag = pmt.dict_ref(a_meta, pmt.intern("wall_clock_time"), pmt.PMT_NIL)
        if not pmt.eqv(time_tag, pmt.PMT_NIL):
            self.assertAlmostEqual(tnow, pmt.to_double(time_tag), delta=60)
        else:
            self.assertTrue(False)

        delta_tag = pmt.dict_ref(a_meta, pmt.intern("time_delta_ms"), pmt.PMT_NIL)
        if not pmt.eqv(delta_tag, pmt.PMT_NIL):
            self.assertAlmostEqual(10000, pmt.to_double(delta_tag), delta=10)
        else:
            self.assertTrue(False)


if __name__ == '__main__':
    gr_unittest.run(qa_time_delta)
