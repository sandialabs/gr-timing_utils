#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
# (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
# retains certain rights in this software.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

import numpy
from gnuradio import gr

RISING_EDGE = 0
FALLING_EDGE = 1
ANY_EDGE = 2


class edge_detector_bb(gr.sync_block):
    """
    This block accepts a binary input stream of data and will output zeros except when an specified edge is detected.
    """

    def __init__(self, mode):
        gr.sync_block.__init__(self,
                               name="edge_detector_bb",
                               in_sig=[numpy.uint8],
                               out_sig=[numpy.uint8])
        '''
        Constructor
        
        @param mode - ANY_EDGE, RISING_EDGE, FALLING_EDGE
        '''

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
