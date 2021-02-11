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
import pmt
import threading
from gnuradio import gr
from collections import deque
import timing_utils

NO_TAG_PENDING = -1
TAG_IMMEDIATELY = -2

class retune_uhd_to_timed_tag(gr.sync_block):
  """
  Tag a data stream for frequency translation at a specified timed

  To provide the timed CORDIC tuning capability of many SDRs, this block,
  when used in conjunction with the timed frequency translating FIR filter,
  provides a software solution for this capability.  The timed tag
  retuner accepts a dictionary indicating the desired CORDIC frequency and
  time of the desired frequency translation and places a tag on the output
  data stream to be consumed and processed by the translating FIR filter.

  The tuning message must be a dictionary with the following elements:
    - lo_offset (double) : Required parameter indicating the desired center
                           frequency with respect to the signal center
                           frequency
    - rx_time (pair) : Optional parameter with the first element (car) being
                       the integer number of seconds and the second element
                       (cdr) being the fractional number of seconds to apply
                       the frequency translation.

  The outgoing data stream is tagged using the specified `dict_key` tag at
  the sample to begin applying the translation.  The `dict_key` tag should be
  the same used for the timed_freq_xlating_fir `tag_key`
  """
  def __init__(self, sample_rate, dict_key, origin_t_secs, origin_t_frac):
    gr.sync_block.__init__(self,
        name="retune_uhd_to_timed_tag",
        in_sig=[numpy.complex64],
        out_sig=[numpy.complex64])

    # setup logger
    logger_name = 'gr_log.' + self.to_basic_block().alias()
    if logger_name in gr.logger_get_names():
      self.log = gr.logger(logger_name)
    else:
      self.log = gr.logger('log')

    self.set_sample_rate(sample_rate)
    self.set_ref_time(offset = 0, secs = origin_t_secs, frac = origin_t_frac)

    # this is exposed as a member variable, but really should not be
    # modified as the downstream block respects this key
    self.set_tag_key( timing_utils.PMTCONSTSTR__set_freq() )
    self.set_dict_key(dict_key)

    # time reference key
    self.time_key = timing_utils.PMTCONSTSTR__rx_time()

    self.message_port_register_in( timing_utils.PMTCONSTSTR__command() )
    self.set_msg_handler( timing_utils.PMTCONSTSTR__command(), self.handle_command)
    self.message_port_register_out( timing_utils.PMTCONSTSTR__freq() )

    # queue for tune commands
    self.tune_commands = deque((),maxlen=64)

    self.lock = threading.Lock()

  def set_dict_key(self, key):
    try:
      pmt.symbol_to_string(key)
      self.dict_key = key
    except TypeError:
      self.dict_key = pmt.intern(key)
      self.log.warn("input dictionary key should be provided as a PMT...converted automatically")

  def set_tag_key(self, key):
    try:
      pmt.symbol_to_string(key)
      self.tag_key = key
    except TypeError:
      self.tag_key = pmt.intern(key)
      self.log.warn("input dictionary key should be provided as a PMT...converted automatically")

  def set_sample_rate(self, sample_rate):
    self.sample_rate = sample_rate

  def set_ref_time(self, offset=0, secs= 0, frac=0):
    self.ref_time = {}
    self.ref_time['offset'] = int(offset)
    self.ref_time['secs'] = secs
    self.ref_time['frac'] = frac
    print("Setting reference time for tuner to {}.{} for offset {}".format(secs,frac,offset))

  def handle_command(self, msg):
    with self.lock:
      # incoming message will be a dictionary that should contain the items
      # freq and lo_offset at a minimum - if this is met, issue a command
      # that can be handled by the freq_xlating_fir_filter_ccf block
      try:
        # print "got a message!"
        # we don't care about the frequency since we are CORDIC tuning
        lo_offset = pmt.dict_ref(msg, self.dict_key, pmt.PMT_NIL)
        if not pmt.eqv(lo_offset, pmt.PMT_NIL):
          offset = pmt.to_python(lo_offset)
          # print "lo offset is " + repr(offset*-1.0)
          self.message_port_pub( timing_utils.PMTCONSTSTR__freq(),
              pmt.cons( timing_utils.PMTCONSTSTR__freq(), pmt.from_double(-1.0*offset)))
          # print "published msg, offset = " + repr(-1.0*offset)

          # if the dictionary has a time value, use it
          time_tag = pmt.dict_ref(msg, timing_utils.PMTCONSTSTR__time(), pmt.PMT_NIL)
          if not pmt.eqv(time_tag, pmt.PMT_NIL):
            secs = pmt.to_uint64(pmt.car(time_tag)) - self.ref_time['secs']
            frac = pmt.to_double(pmt.cdr(time_tag)) - self.ref_time['frac']
            tune_sample = int(secs * self.sample_rate) + int(frac * self.sample_rate) + self.ref_time['offset']
          else:
            tune_sample = TAG_IMMEDIATELY

          # we will also set the block to tag the output when it is time
          if len(self.tune_commands) < self.tune_commands.maxlen:
              self.tune_commands.append((tune_sample,pmt.from_double(-1.0*offset)))

      except Exception as e:
          print("exception: " + repr(e))

  def work(self, input_items, output_items):
      with self.lock:
        # print "nitems_read = {}".format(self.nitems_read(0))
        in0 = input_items[0]
        out = output_items[0]
        noutput_items = len(input_items[0])
        nitems_read = self.nitems_read(0)
        out[:] = in0

        # look for time reference in tags
        tags = self.get_tags_in_window(0,0,noutput_items,self.time_key);
        if len(tags):
          # use last tag in window to update reference time
          try:
            offset = tags[-1].offset
            sec = pmt.to_uint64(pmt.tuple_ref(tags[-1].value,0))
            frac = pmt.to_double(pmt.tuple_ref(tags[-1].value,1))
            self.set_ref_time(offset,sec,frac)
          except Exception as e:
            self.log.warn("invalid tag value: ", repr(e))
            pass

        # if there is a tune that needs to be tagged
        while len(self.tune_commands):
          (tag_offset,tag_value) = self.tune_commands[0]
          tag = False

          if tag_offset is TAG_IMMEDIATELY:
            offset = nitems_read
            tag = True
          elif tag_offset < nitems_read:
              # time has already elapsed - tag immediatey
            offset = nitems_read
            tag = True
          elif nitems_read <= tag_offset < (nitems_read + noutput_items):
              # time within current window
            offset = tag_offset
            tag = True

          # tag it
          if tag:
            # print "n = {}, offset = {}, value = {}".format(len(self.tune_commands),offset,tag_value)
            self.add_item_tag(0, offset, self.tag_key, tag_value)
            self.tune_commands.popleft()
            # print "length now = {}".format(len(self.tune_commands))
          else:
            break

        # print "noutput_items = {}".format(noutput_items)
        return noutput_items
