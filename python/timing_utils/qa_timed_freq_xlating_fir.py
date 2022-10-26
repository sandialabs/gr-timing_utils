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
    from gnuradio import timing_utils
except ImportError:
    import os
    import sys
    dirname, filename = os.path.split(os.path.abspath(__file__))
    sys.path.append(os.path.join(dirname, "bindings"))
    from gnuradio import timing_utils

import pmt
import cmath
from gnuradio import sandia_utils


class qa_timed_freq_xlating_fir (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_instantiate(self):
        self.src = blocks.vector_source_c([0] * 1000, False, 1, [])
        self.dut = timing_utils.timed_freq_xlating_fir_ccc(1, [1], 915e6, 30.72e6)
        self.tag_dbg = blocks.tag_debug(gr.sizeof_gr_complex * 1, '', "")

        self.tb.connect((self.src, 0), (self.dut, 0))
        self.tb.connect((self.dut, 0), (self.tag_dbg, 0))

        # set up fg
        self.tb.run()
        time.sleep(0.05)

        # check data

    def test_002_tag_propagation_no_dec(self):
        ''' Validate basic tag propagation without decimation'''
        rate = 250000
        tag_loc = 0
        nsamps = 1000
        freq = 100
        decimation = 1
        taps = [0, 0, 1, 0, 0] # symmetric FIR filter (linear phase)
        group_delay = round((len(taps) - 1) / 2)

        # add tag
        tag = gr.tag_t()
        tag.offset = tag_loc
        tag.key = pmt.intern('freq')
        tag.value = pmt.from_double(freq)

        # dummy data
        data = [1 + 0j] * nsamps
        expected = [data[i] * cmath.exp(1j * 2 * cmath.pi * -1 * freq * i / rate) for i in range(nsamps)]

        self.src = blocks.vector_source_c(data, False, 1, [tag])
        self.dut = timing_utils.timed_freq_xlating_fir_ccc(decimation, taps, 0, rate, "freq")
        self.vector_sink = blocks.vector_sink_c(1, 1024)
        self.tag_dbg = blocks.tag_debug(gr.sizeof_gr_complex * 1, '', "")
        self.tag_dbg.set_display(True)

        self.tb.connect((self.src, 0), (self.dut, 0))
        self.tb.connect((self.dut, 0), (self.tag_dbg, 0))
        self.tb.connect((self.dut, 0), (self.vector_sink, 0))

        # set up fg
        self.tb.run()
        time.sleep(0.5)
        self.tb.stop()
        self.tb.wait()

        # collect tags
        self.assertEqual(self.tag_dbg.num_tags(), 1)
        tags = self.tag_dbg.current_tags()
        self.assertEqual(tags[0].offset, group_delay)

    def test_003_tag_propagation_dec(self):
        ''' Validate basic tag propagation while decimating'''
        # TODO: Validate this output.  Still unclear when the decimation is
        # larger than the number of taps what the expected output should be
        rate = 250000
        tag_loc = 20
        nsamps = 1000
        freq = 100
        decimation = 10
        taps = [0, 0, 1, 0, 0] # symmetric FIR filter (linear phase)
        group_delay = round((len(taps) - 1) / 2)

        # add tag
        tag = gr.tag_t()
        tag.offset = tag_loc
        tag.key = pmt.intern('freq')
        tag.value = pmt.from_double(freq)

        # dummy data
        data = [i + 0j for i in range(nsamps)]
        expected = [data[i] * cmath.exp(1j * 2 * cmath.pi * -1 * freq * i / rate) for i in range(nsamps)]

        self.src = blocks.vector_source_c(data, False, 1, [tag])
        self.dut = timing_utils.timed_freq_xlating_fir_ccc(decimation, taps, 0, rate, "freq")
        self.vector_sink = blocks.vector_sink_c(1, 1024)
        self.tag_dbg = blocks.tag_debug(gr.sizeof_gr_complex * 1, '', "")

        self.tb.connect((self.src, 0), (self.dut, 0))
        self.tb.connect((self.dut, 0), (self.tag_dbg, 0))
        self.tb.connect((self.dut, 0), (self.vector_sink, 0))

        # set up fg
        self.tb.run()
        time.sleep(0.5)
        self.tb.stop()
        self.tb.wait()

        # collect tags
        self.assertEqual(self.tag_dbg.num_tags(), 1)
        tags = self.tag_dbg.current_tags()
        #self.assertEqual(tags[0].offset,round(group_delay / decimation))

        # received data
        #received = self.vector_sink.data()
        #print(f"received = {received}")

    def test_004_frequency_tag_no_offset(self):
        ''' Tag the output data stream and ensure that it is rotated properly '''
        rate = 250000
        tag_loc = 0
        nsamps = 1000
        freq = 100
        decimation = 1
        taps = [1]

        # add tag
        tag = gr.tag_t()
        tag.offset = tag_loc
        tag.key = pmt.intern('freq')
        tag.value = pmt.from_double(freq)

        # data and expected data after translation
        data = [1 + 0j] * nsamps
        expected = [data[i] * cmath.exp(1j * 2 * cmath.pi * -1 * freq * i / rate) for i in range(nsamps)]

        self.src = blocks.vector_source_c(data, False, 1, [tag])
        self.dut = timing_utils.timed_freq_xlating_fir_ccc(decimation, taps, 0, rate, "freq")
        self.vector_sink = blocks.vector_sink_c(1, 1024)
        self.tag_dbg = blocks.tag_debug(gr.sizeof_gr_complex * 1, '', "")

        self.tb.connect((self.src, 0), (self.dut, 0))
        self.tb.connect((self.dut, 0), (self.tag_dbg, 0))
        self.tb.connect((self.dut, 0), (self.vector_sink, 0))

        # set up fg
        self.tb.run()
        time.sleep(0.5)

        # check tag
        self.assertEqual(self.tag_dbg.num_tags(), 1)
        tags = self.tag_dbg.current_tags()
        self.assertTrue(tags[0].offset == 0)
        self.assertTrue(pmt.equal(tags[0].key, tag.key))
        self.assertTrue(pmt.equal(tags[0].value, pmt.from_double(freq)))

        # collect data
        received = list(self.vector_sink.data())
        for i in range(min(len(received), len(expected))):
            self.assertAlmostEqual(received[i], expected[i], places=5)

    def test_005_frequency_tag_with_phase(self):
        ''' Tag the output data stream and ensure that it is rotated properly '''
        rate = 250000
        tag_loc = 0
        nsamps = 1000
        freq = 100
        phase = cmath.pi / 2.
        decimation = 1
        taps = [1]

        # add tag
        tag = gr.tag_t()
        tag.offset = tag_loc
        tag.key = pmt.intern('freq')
        tag.value = pmt.cons(pmt.from_double(freq), pmt.from_double(phase))

        # data and expected data after translation
        data = [1 + 0j] * nsamps
        expected = [data[i] * cmath.exp(1j * (2 * cmath.pi * -1 * freq * i / rate - phase)) for i in range(nsamps)]

        self.src = blocks.vector_source_c(data, False, 1, [tag])
        self.dut = timing_utils.timed_freq_xlating_fir_ccc(decimation, taps, 0, rate, "freq")
        self.vector_sink = blocks.vector_sink_c(1, 1024)
        self.tag_dbg = blocks.tag_debug(gr.sizeof_gr_complex * 1, '', "")

        self.tb.connect((self.src, 0), (self.dut, 0))
        self.tb.connect((self.dut, 0), (self.tag_dbg, 0))
        self.tb.connect((self.dut, 0), (self.vector_sink, 0))

        # set up fg
        self.tb.run()
        time.sleep(0.1)

        # check tag
        self.assertEqual(self.tag_dbg.num_tags(), 1)
        tags = self.tag_dbg.current_tags()
        self.assertTrue(tags[0].offset == 0)
        self.assertTrue(pmt.equal(tags[0].key, tag.key))
        self.assertTrue(pmt.equal(tags[0].value, tag.value))

        # collect data
        received = list(self.vector_sink.data())
        for i in range(min(len(received), len(expected))):
            # expected differences due to double vs. single precision
            # floating point computation so number of places can't be
            # too high
            self.assertAlmostEqual(received[i], expected[i], places=5)

    def test_006_multiple_freqs_cont_phase(self):
        ''' Tag the output data stream with multiple frequencies and ensure the phase is continous '''
        rate = 250000
        tag_loc = 0
        nsamps = 1000
        freqs = [100, -100]
        tag_locs = [0, 100]
        decimation = 1
        taps = [1]

        # add tag
        tags = []
        for i in range(len(freqs)):
            tag = gr.tag_t()
            tag.offset = tag_locs[i]
            tag.key = pmt.intern('freq')
            tag.value = pmt.from_double(freqs[i])
            tags.append(tag)

        # data and expected data after translation
        data = [1 + 0j] * nsamps
        expected = [0] * len(data)
        phase_incr = 2 * cmath.pi * -1 * freqs[0] / rate
        phase = -1 * phase_incr
        for ind in range(len(data)):
            if ind - 1 in tag_locs:
                # increment applied AFTER sample with tag
                phase_incr = 2 * cmath.pi * -1 * freqs[tag_locs.index(ind - 1)] / rate
            phase += phase_incr
            expected[ind] = data[ind] * cmath.exp(1j * phase)

        self.src = blocks.vector_source_c(data, False, 1, tags)
        self.dut = timing_utils.timed_freq_xlating_fir_ccc(decimation, taps, 0, rate, "freq")
        self.vector_sink = blocks.vector_sink_c(1, 1024)
        self.tag_dbg = sandia_utils.sandia_tag_debug(gr.sizeof_gr_complex * 1, '', "", True)

        self.tb.connect((self.src, 0), (self.dut, 0))
        self.tb.connect((self.dut, 0), (self.tag_dbg, 0))
        self.tb.connect((self.dut, 0), (self.vector_sink, 0))

        # set up fg
        self.tb.run()
        time.sleep(0.5)

        # check tag
        self.assertEqual(self.tag_dbg.num_tags(), len(tags))
        received_tags = self.tag_dbg.current_tags()
        for i in range(len(received_tags)):
            self.assertTrue(received_tags[i].offset == tags[i].offset)
            self.assertTrue(pmt.equal(received_tags[i].key, tags[i].key))
            self.assertTrue(pmt.equal(received_tags[i].value, tags[i].value))

        # collect data
        received = list(self.vector_sink.data())
        for i in range(min(len(received), len(expected))):
            # expected differences due to double vs. single precision
            # floating point computation so number of places can't be
            # too high
            self.assertAlmostEqual(received[i], expected[i], places=5)

    def test_007_multiple_freqs_updated_phase(self):
        ''' Tag the output data stream with multiple frequencies and ensure the phase is continous '''
        rate = 250000
        tag_loc = 0
        nsamps = 1000
        freqs = [100, -100]
        tag_locs = [0, 100]
        phases = [0, cmath.pi / 8.]
        decimation = 1
        taps = [1]

        # add tag
        tags = []
        for i in range(len(freqs)):
            tag = gr.tag_t()
            tag.offset = tag_locs[i]
            tag.key = pmt.intern('freq')
            tag.value = pmt.cons(pmt.from_double(freqs[i]), pmt.from_double(phases[i]))
            tags.append(tag)

        # data and expected data after translation
        data = [1 + 0j] * nsamps
        expected = [0] * len(data)
        phase_incr = 2 * cmath.pi * -1 * freqs[0] / rate
        phase = 0
        for ind in range(len(data)):
            if ind in tag_locs:
                # initial phase offset is set explicitly with tag so do on the
                # exact sample offset
                phase = -1 * phases[tag_locs.index(ind)]
                phase_incr = 2 * cmath.pi * -1 * freqs[tag_locs.index(ind)] / rate
            else:
                phase += phase_incr
            expected[ind] = data[ind] * cmath.exp(1j * phase)

        self.src = blocks.vector_source_c(data, False, 1, tags)
        self.dut = timing_utils.timed_freq_xlating_fir_ccc(decimation, taps, 0, rate, "freq")
        self.vector_sink = blocks.vector_sink_c(1, 1024)
        self.tag_dbg = sandia_utils.sandia_tag_debug(gr.sizeof_gr_complex * 1, '', "", True)

        self.tb.connect((self.src, 0), (self.dut, 0))
        self.tb.connect((self.dut, 0), (self.tag_dbg, 0))
        self.tb.connect((self.dut, 0), (self.vector_sink, 0))

        # set up fg
        self.tb.run()
        time.sleep(0.5)

        # check tag
        self.assertEqual(self.tag_dbg.num_tags(), len(tags))
        received_tags = self.tag_dbg.current_tags()
        for i in range(len(received_tags)):
            self.assertTrue(received_tags[i].offset == tags[i].offset)
            self.assertTrue(pmt.equal(received_tags[i].key, tags[i].key))
            self.assertTrue(pmt.equal(received_tags[i].value, tags[i].value))

        # collect data
        received = list(self.vector_sink.data())
        for i in range(min(len(received), len(expected))):
            # expected differences due to double vs. single precision
            # floating point computation so number of places can't be
            # too high
            self.assertAlmostEqual(received[i], expected[i], places=5)

    def test_009_duplicates_freqs_updated_phase(self):
        ''' Tag the output data stream with a duplicate frequency but different phase  and ensure the phase is continous '''
        rate = 250000
        tag_loc = 0
        nsamps = 1000
        freqs = [100, 100]
        tag_locs = [0, 100]
        phases = [cmath.pi / 16., cmath.pi / 8.]
        decimation = 1
        taps = [1]

        # add tag
        tags = []
        for i in range(len(freqs)):
            tag = gr.tag_t()
            tag.offset = tag_locs[i]
            tag.key = pmt.intern('freq')
            tag.value = pmt.cons(pmt.from_double(freqs[i]), pmt.from_double(phases[i]))
            tags.append(tag)

        # data and expected data after translation
        data = [1 + 0j] * nsamps
        expected = [0] * len(data)
        phase_incr = 2 * cmath.pi * -1 * freqs[0] / rate
        phase = 0
        for ind in range(len(data)):
            if ind in tag_locs:
                # initial phase offset is set explicitly with tag so do on the
                # exact sample offset
                phase = -1 * phases[tag_locs.index(ind)]
                phase_incr = 2 * cmath.pi * -1 * freqs[tag_locs.index(ind)] / rate
            else:
                phase += phase_incr
            expected[ind] = data[ind] * cmath.exp(1j * phase)

        self.src = blocks.vector_source_c(data, False, 1, tags)
        self.dut = timing_utils.timed_freq_xlating_fir_ccc(decimation, taps, 0, rate, "freq")
        self.vector_sink = blocks.vector_sink_c(1, 1024)
        self.tag_dbg = sandia_utils.sandia_tag_debug(gr.sizeof_gr_complex * 1, '', "", True)

        self.tb.connect((self.src, 0), (self.dut, 0))
        self.tb.connect((self.dut, 0), (self.tag_dbg, 0))
        self.tb.connect((self.dut, 0), (self.vector_sink, 0))

        # set up fg
        self.tb.run()
        time.sleep(0.5)

        # check tag
        self.assertEqual(self.tag_dbg.num_tags(), len(tags))
        received_tags = self.tag_dbg.current_tags()
        for i in range(len(received_tags)):
            self.assertTrue(received_tags[i].offset == tags[i].offset)
            self.assertTrue(pmt.equal(received_tags[i].key, tags[i].key))
            self.assertTrue(pmt.equal(received_tags[i].value, tags[i].value))

        # collect data
        received = list(self.vector_sink.data())
        for i in range(min(len(received), len(expected))):
            # expected differences due to double vs. single precision
            # floating point computation so number of places can't be
            # too high
            self.assertAlmostEqual(received[i], expected[i], places=5)


if __name__ == '__main__':
    gr_unittest.run(qa_timed_freq_xlating_fir)
