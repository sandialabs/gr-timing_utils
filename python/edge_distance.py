#!/usr/bin/env python3.6
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

RISING_EDGE = 0
FALLING_EDGE = 1


class edge_distance(gr.sync_block):
    """
    docstring for block edge_distance
    """
    def __init__(self, samp_rate):
        gr.sync_block.__init__(self,
            name="edge_distance",
            in_sig=[numpy.float32],
            out_sig=None)

        self.st = pmt.intern("START")
        self.et = pmt.intern("END")
        self.rate = samp_rate
        self.start_idx = 0

    def work(self, input_items, output_items):
        in0 = input_items[0]
        start = self.nitems_read(0)
        end = start + len(in0)

        tags = self.get_tags_in_range(0, start, end)
        for tag in tags:
            if pmt.eqv(tag.key, self.st):
                self.start_idx = tag.offset
            elif pmt.eqv(tag.key, self.et):
                if self.start_idx:
                    print(repr(round((tag.offset - self.start_idx) * 1000.0 / self.rate, 3)) + ", ")
                self.start_idx = 0

        return len(in0)
