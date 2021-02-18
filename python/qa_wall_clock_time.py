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
import pdu_utils
import time
import pmt
try:
    import timing_utils
except ImportError:
    import os
    import sys
    dirname, filename = os.path.split(os.path.abspath(__file__))
    sys.path.append(os.path.join(dirname, "bindings"))
    import timing_utils


class qa_wall_clock_time(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

        self.emitter = pdu_utils.message_emitter()
        self.wall_clock_time = timing_utils.wall_clock_time(pmt.intern('wall_clock_time'))
        self.debug = blocks.message_debug()

        # make connections
        self.tb.msg_connect((self.emitter, 'msg'), (self.wall_clock_time, 'pdu_in'))
        self.tb.msg_connect((self.wall_clock_time, 'pdu_out'), (self.debug, 'store'))

    def tearDown(self):
        self.tb = None

    def test_timing(self):
        self.tb.start()
        self.emitter.emit(pmt.intern("BAD PDU"))
        time.sleep(.01)
        self.emitter.emit(pmt.cons(pmt.make_dict(), pmt.init_u8vector(1, [0])))
        time.sleep(1.0)
        self.emitter.emit(pmt.cons(pmt.make_dict(), pmt.init_u8vector(1, [0])))
        time.sleep(.05)
        self.tb.stop()
        self.tb.wait()

        t0 = pmt.to_double(pmt.dict_ref(pmt.car(self.debug.get_message(0)),
                                        pmt.intern("wall_clock_time"),
                                        pmt.from_double(0.0)))
        t1 = pmt.to_double(pmt.dict_ref(pmt.car(self.debug.get_message(1)),
                                        pmt.intern("wall_clock_time"),
                                        pmt.from_double(0.0)))
        self.assertTrue(((t1 - t0) - 1) < 0.05)

    def test_basic_io(self):
        self.tb.start()
        self.emitter.emit(pmt.intern("BAD PDU"))
        time.sleep(.01)
        self.emitter.emit(pmt.cons(pmt.from_long(4), pmt.PMT_NIL))
        time.sleep(.01)
        self.emitter.emit(pmt.cons(pmt.make_dict(), pmt.init_u8vector(1, [0])))
        time.sleep(.05)
        self.tb.stop()
        self.tb.wait()

        self.assertTrue(True)


if __name__ == '__main__':
    gr_unittest.run(qa_wall_clock_time)
