#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# <COPYRIGHT PLACEHOLDER>
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
from gnuradio import gr

RISING_EDGE = 0
FALLING_EDGE = 1
ANY_EDGE = 2

class edge_detector_bb(gr.sync_block):
    """
    This block detects rising and/or falling edges on a binary input stream.
    """
    def __init__(self, mode):
        gr.sync_block.__init__(self,
            name="edge_detector_bb",
            in_sig=[numpy.uint8],
            out_sig=[numpy.uint8])

        self.prev_val = 0
        self.mode = mode


    def work(self, input_items, output_items):
        in0 = input_items[0]
        out = output_items[0]

        # preload output to zeros
        out[:] = numpy.zeros(len(out))

        in_early = numpy.append([self.prev_val], in0[:-1])
        edges = numpy.nonzero(numpy.array(in_early - in0))[0]
        #print repr(edges)
        for edge_index in edges:
            #print repr(edge_index)
            if self.mode == ANY_EDGE:
                out[edge_index] = 1
            elif self.mode == RISING_EDGE and in0[edge_index] == 1:
                out[edge_index] = 1
            elif self.mode == FALLING_EDGE and in0[edge_index] == 0:
                out[edge_index] = 1

        self.prev_val = in0[-1]

        return len(output_items[0])
