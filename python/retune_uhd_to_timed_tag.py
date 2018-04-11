#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2018 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
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


import numpy
import pmt
from gnuradio import gr

NO_TAG_PENDING = -1
TAG_IMMEDIATELY = -2

class retune_uhd_to_timed_tag(gr.sync_block):
    """
    docstring for block retune_uhd_to_timed_tag
    """
    def __init__(self, sample_rate, dict_key, origin_t_secs, origin_t_frac):
        gr.sync_block.__init__(self,
            name="retune_uhd_to_timed_tag",
            in_sig=[numpy.complex64],
            out_sig=[numpy.complex64])

        self.set_sample_rate(sample_rate)
        self.set_origin_time(origin_t_secs, origin_t_frac)

        # this is exposed as a member variable, but really should not be
        # modified as the downstream block respects this key
        self.set_tag_key(pmt.intern("set_freq"))
        self.set_dict_key(dict_key)

        self.message_port_register_in(pmt.intern("command"))
        self.set_msg_handler(pmt.intern("command"), self.handle_command)
        self.message_port_register_out(pmt.intern("freq"))

        self.tag_offset = NO_TAG_PENDING


    def set_dict_key(self, key):
        try:
            pmt.symbol_to_string(key)
            self.dict_key = key
        except TypeError:
            self.dict_key = pmt.intern(key)
            print "WARNING - input dictionary key should be provided as a PMT!"
            print "            but it has been converted automatically"


    def set_tag_key(self, key):
        try:
            pmt.symbol_to_string(key)
            self.tag_key = key
        except TypeError:
            self.tag_key = pmt.intern(key)
            print "WARNING - output tag key should be provided as a PMT!"
            print "            but it has been converted automatically"


    def set_sample_rate(self, sample_rate):
        self.sample_rate = sample_rate


    def set_origin_time(self, secs, frac=0):
        self.origin_time = {}
        self.origin_time['secs'] = secs
        self.origin_time['frac'] = frac


    def handle_command(self, msg):
        # incoming message will be a dictionary that should contain the items
        # freq and lo_offset at a minimum - if this is met, issue a command
        # that can be handled by the freq_xlating_fir_filter_ccf block
        try:
            #print "got a message!"
            # we don't care about the frequency since we are CORDIC tuning
            lo_offset = pmt.dict_ref(msg, self.dict_key, pmt.PMT_NIL)
            if not pmt.eqv(lo_offset, pmt.PMT_NIL):
                offset = pmt.to_python(lo_offset)
                #print "lo offset is " + repr(offset*-1.0)
                self.message_port_pub(pmt.intern("freq"),
                    pmt.cons(pmt.intern("freq"), pmt.from_double(-1.0*offset)))
                #print "published msg, offset = " + repr(-1.0*offset)

                # if the dictionary has a time value, use it
                time_tag = pmt.dict_ref(msg, pmt.intern("time"), pmt.PMT_NIL)
                if not pmt.eqv(time_tag, pmt.PMT_NIL):
                    secs = pmt.to_uint64(pmt.car(time_tag)) - self.origin_time['secs']
                    frac = pmt.to_double(pmt.cdr(time_tag)) - self.origin_time['frac']
                    tune_sample = long(secs * self.sample_rate) + long(frac * self.sample_rate)
                else:
                    tune_sample = TAG_IMMEDIATELY

                # we will also set the block to tag the output when it is time
                self.tag_offset = tune_sample
                self.tag_value = pmt.from_double(-1.0*offset)

        except Exception as e:
            print "exception: " + repr(e)

    def work(self, input_items, output_items):
        in0 = input_items[0]
        out = output_items[0]
        noutput_items = len(input_items[0])
        nitems_read = self.nitems_read(0)
        out[:] = in0

        # if there is a tune that needs to be tagged
        if self.tag_offset is not NO_TAG_PENDING:
            tag = False
            # and that is in this output buffer
            if nitems_read <= self.tag_offset < (nitems_read + noutput_items):
                offset = self.tag_offset
                tag = True

            # or if it needs to be done immediately
            if self.tag_offset is TAG_IMMEDIATELY:
                offset = nitems_read
                tag = True

            # tag it
            if tag:
                self.add_item_tag(0, offset, self.tag_key, self.tag_value)
                #print "tagged a tune at offset " + repr(offset) + " for frequency " + repr(self.tag_value)
                self.tag_offset = NO_TAG_PENDING

        return noutput_items
