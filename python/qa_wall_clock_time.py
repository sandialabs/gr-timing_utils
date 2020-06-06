#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2020 gr-timing_utils author.
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import timing_utils_swig as timing_utils
import pdu_utils
import time
import pmt

class qa_wall_clock_time(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

        self.emitter = pdu_utils.message_emitter()
        self.wall_clock_time = timing_utils.wall_clock_time(pmt.intern('wall_clock_time'))
        self.debug = blocks.message_debug()

        # make connections
        self.tb.msg_connect((self.emitter,'msg'),(self.wall_clock_time,'pdu_in'))
        self.tb.msg_connect((self.wall_clock_time,'pdu_out'),(self.debug,'store'))

    def tearDown(self):
        self.tb = None

    def test_timing(self):
        self.tb.start()
        self.emitter.emit(pmt.intern("BAD PDU"))
        time.sleep(.01)
        self.emitter.emit(pmt.cons(pmt.make_dict(), pmt.init_u8vector(1,[0])))
        time.sleep(1.0)
        self.emitter.emit(pmt.cons(pmt.make_dict(), pmt.init_u8vector(1,[0])))
        time.sleep(.05)
        self.tb.stop()
        self.tb.wait()

        t0 = pmt.to_double(pmt.dict_ref(pmt.car(self.debug.get_message(0)),
                           pmt.intern("wall_clock_time"),
                           pmt.from_double(0.0)))
        t1 = pmt.to_double(pmt.dict_ref(pmt.car(self.debug.get_message(1)),
                           pmt.intern("wall_clock_time"),
                           pmt.from_double(0.0)))
        self.assertTrue(((t1-t0)-1) < 0.05)

    def test_basic_io(self):
        self.tb.start()
        self.emitter.emit(pmt.intern("BAD PDU"))
        time.sleep(.01)
        self.emitter.emit(pmt.cons(pmt.from_long(4), pmt.PMT_NIL))
        time.sleep(.01)
        self.emitter.emit(pmt.cons(pmt.make_dict(), pmt.init_u8vector(1,[0])))
        time.sleep(.05)
        self.tb.stop()
        self.tb.wait()

        self.assertTrue(True)



if __name__ == '__main__':
    gr_unittest.run(qa_wall_clock_time)
