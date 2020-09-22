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
import timing_utils_swig as timing_utils
import pmt
import time
import pdu_utils
from math import ceil

class qa_interrupt_emitter (gr_unittest.TestCase):

    def setUp (self):
        self.rate = 250000

        self.duration = int(self.rate*4)
        self.start_time = .098765
        self.skey = pmt.intern("trigger_sample")
        self.tkey = pmt.intern("trigger_time")

        self.tb = gr.top_block ()

        self.src = blocks.vector_source_c(list(range(self.duration)), False, 1, [])
        self.throttle = blocks.throttle(gr.sizeof_gr_complex*1, self.rate)

        self.emitter = pdu_utils.message_emitter(pmt.PMT_NIL)
        self.start_time = .098765
        self.utag = timing_utils.add_usrp_tags_c(1090e6, self.rate, 0, self.start_time)
        #self.tag_dbg = blocks.tag_debug(gr.sizeof_gr_complex*1, '', "");
        self.msg_dbg = blocks.message_debug()
        self.timer = timing_utils.interrupt_emitter_c(self.rate, True)

        self.tb.connect((self.src, 0), (self.throttle, 0))
        self.tb.connect((self.throttle, 0), (self.utag, 0))
        #self.tb.connect((self.utag, 0), (self.tag_dbg, 0))
        self.tb.connect((self.utag, 0), (self.timer, 0))
        self.tb.msg_connect((self.emitter, "msg"), (self.timer, "set"))
        self.tb.msg_connect((self.timer, "trig"), (self.msg_dbg, "store"))

        #self.timer.set_debug(True)

    def tearDown (self):
        self.tb = None

    def timemsg(self, abstime, fmt):
        if fmt == "sample":
            return pmt.from_uint64(int(abstime*self.rate))
        elif fmt in ["pair", "tuple"]:
            t_int = int(abstime)
            t_frac = abstime - t_int
            if t_frac > 1:
                t_int += 1
                t_frac -= 1.0
            if fmt == "pair": return pmt.cons(pmt.from_uint64(t_int), pmt.from_double(t_frac))
            else: return pmt.make_tuple(pmt.from_uint64(t_int), pmt.from_double(t_frac))

    def checkmsgtime(self, msg, expected_time, expected_sample):
        sample = pmt.to_uint64(pmt.dict_ref(msg, self.skey, pmt.PMT_NIL))
        trig = pmt.dict_ref(msg, self.tkey, pmt.PMT_NIL)
        trig_int, trig_frac = pmt.to_uint64(pmt.car(trig)), pmt.to_double(pmt.cdr(trig))
        trig_time = trig_int + trig_frac
        self.assertAlmostEqual(float(sample)/self.rate, float(expected_sample)/self.rate, 3,
            "Incorrect Sample from message: expected {}, received {}".format(expected_sample, sample))

        self.assertAlmostEqual(trig_time, expected_time, 3,
            "Incorrect interrupt time from message: expected {}, received {}".format(expected_time, trig_time))

    def test_001_basic(self):
        # This test checks to see that we can send and receive messages using all of the accepted input types
        t0, t1, t2 = .15, .25, .28
        self.tb.start ()
        time.sleep(.010)
        self.emitter.emit(self.timemsg(t0, "pair"))
        time.sleep(0.10)
        self.emitter.emit(self.timemsg(t1, "tuple"))
        time.sleep(0.03)
        self.emitter.emit(self.timemsg(t2-self.start_time, "sample"))
        for i in range(15):
            if self.msg_dbg.num_messages() == 3:
                break
            time.sleep(0.02)

        # DO NOT call wait!!!!  It won't return because the emitter block doesn't have any inputs.
        self.tb.stop()
        time.sleep(.01)

        if (self.msg_dbg.num_messages() != 3): raise Exception("Did not send required messages")
        self.checkmsgtime(self.msg_dbg.get_message(0), t0, (t0 - self.start_time)*self.rate)
        self.checkmsgtime(self.msg_dbg.get_message(1), t1, (t1 - self.start_time)*self.rate)
        self.checkmsgtime(self.msg_dbg.get_message(2), t2, (t2 - self.start_time)*self.rate)

    def test_002_update_int(self):
        # This test checks that we can update a time
        self.tb.start ()
        time.sleep(.01)
        t0, t1 = .1, .15
        self.emitter.emit(self.timemsg(t0, "pair"))
        time.sleep(.01)
        self.emitter.emit(self.timemsg(t1, "pair"))
        for i in range(15):
            if self.msg_dbg.num_messages() == 1:
                break
            time.sleep(0.02)

        # DO NOT call wait!!!!  It won't return because the emitter block doesn't have any inputs.
        self.tb.stop()
        time.sleep(.01)

        if (self.msg_dbg.num_messages() != 1): raise Exception("Did not send required messages")
        self.checkmsgtime(self.msg_dbg.get_message(0), t1, (t1 - self.start_time)*self.rate)

    def makeTimeDict(self, timeval):
        pmtDict = pmt.make_dict()
        intt = int(timeval)
        fract = timeval - intt
        pmtDict = pmt.dict_add(pmtDict, pmt.intern("rx_time"),
                pmt.make_tuple(pmt.from_uint64(intt), pmt.from_double(fract)))
        return pmtDict

    def test_003_update_time(self):
        # This test checks that we can update the hardware time
        self.tb.start ()
        time.sleep(.01)
        t0 = .2
        self.emitter.emit(self.timemsg(t0, "pair"))
        time.sleep(.01)
        start_time = .16
        self.utag.update_tags(self.makeTimeDict(start_time))
        for i in range(15):
            if self.msg_dbg.num_messages() == 1:
                break
            time.sleep(0.02)

        # DO NOT call wait!!!!  It won't return because the emitter block doesn't have any inputs.
        self.tb.stop()
        time.sleep(.01)

        tag = self.utag.last_tag()
        sample = pmt.to_uint64(pmt.dict_ref(tag, pmt.intern("rx_sample"), pmt.PMT_NIL))

        if (self.msg_dbg.num_messages() != 1): raise Exception("Did not send required messages")
        self.checkmsgtime(self.msg_dbg.get_message(0), t0, (t0 - start_time)*self.rate + sample)

    #TODO Add test for late request

if __name__ == '__main__':
    gr_unittest.run(qa_interrupt_emitter)
