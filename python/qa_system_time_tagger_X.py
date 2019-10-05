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

from builtins import range
from gnuradio import gr, gr_unittest
from gnuradio import blocks
import timing_utils_swig as timing_utils
import time

class qa_system_time_diff_X (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_notags(self):
        self.tb = gr.top_block ()
        self.rate = 250000
        self.interval = 173
        self.duration = 9000
        
        self.src = blocks.vector_source_c(list(range(self.duration)), False, 1, [])
        self.time_diff = timing_utils.system_time_diff_c()
        self.tb.connect((self.src,0),(self.time_diff,0))
        
        # set up fg
        self.tb.run ()
        time.sleep(0.05)
        
        # check data
        
        # break down
        self.tb = None

if __name__ == '__main__':
    gr_unittest.run(qa_system_time_diff_X, "qa_system_time_diff_X.xml")
