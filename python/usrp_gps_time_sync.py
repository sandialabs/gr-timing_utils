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
from gnuradio import uhd
import time
import pmt
import timing_utils

class usrp_gps_time_sync(gr.sync_block):
    """
    Synchronize a USRP to GPS time

    To provide synchronziation across USRP devices, the radio time within the
    hardware will be set to the correct GPS epoch on a received PPS interval.

    To synchronize, a PPS is associated with a GPS epoch by monitoring the
    received GPS time and the last observed PPS time.  Once a known boundary
    is observed, on the next PPS, the radio time is set to the correct GPS
    epoch.

    If the GPSDO is not available or is unlocked, the time will be set to the
    current system time.   The time can be resynchronized by publishing a
    message into the block at  any time.  This DOES cause the USRP object to be
    stopped during the time synchronization.

    It is recommended to use a USRP source object for synchronization.
    """
    def __init__(self, parent, usrp_source):
      gr.sync_block.__init__(self,
          name="usrp_gps_time_sync",
          in_sig=None,
          out_sig=None)

      # setup logger
      logger_name = 'gr_log.' + self.to_basic_block().alias()
      if logger_name in gr.logger_get_names():
        self.log = gr.logger(logger_name)
      else:
        self.log = gr.logger('log')

      self.parent = parent
      self.usrp_source = usrp_source
      self.usrp_source_object = None

      self.message_port_register_in( timing_utils.PMTCONSTSTR__in() )
      self.set_msg_handler( timing_utils.PMTCONSTSTR__in() ,self.msg_handler)

      # current gps status
      self.has_gpsdo = False
      self.gps_locked = False

    def start(self):
      self.log.debug("Starting GPS Time Sync")

      '''
      Overload start function
      '''
      try:
        # Check to ensure
        self.usrp_source_object = eval("self.parent.%s"%self.usrp_source)

        # Check if we have a GPSDO
        names = self.usrp_source_object.get_mboard_sensor_names()
        if 'gps_time' in names:
          self.has_gpsdo = True

      except AttributeError:
        self.log.error("Unable to acquire usrp object for synchronization")
        return True

      # synchronize
      self.synchronize()

      # if not locked, default time to current system time
      if not self.gps_locked:
        try:
          self.log.debug("USRP GPS Time Sync: Defaulting to Current System Time")
          self.usrp_source_object.set_time_now(uhd.time_spec_t(time.time()))
        except Exception as e:
          self.log.error("Set Time Next PPS Error: " + repr(e))

      return True

    def synchronize(self):
      if (self.usrp_source_object == None) or (self.has_gpsdo == False):
        return False

      # check if gps is locked
      locked = self.usrp_source_object.get_mboard_sensor("gps_locked").to_bool()

      # only set time if changing from unlocked to locked
      if (self.gps_locked) and (locked):
        # no change in lock status
        self.gps_locked = locked
        return True
      elif (self.gps_locked) and (not locked):
        # system became unlocked - do nothing
        self.gps_locked = locked
        return True
      elif (not self.gps_locked) and (not locked):
        # system remains unlocked
        self.gps_locked = locked
        return True
      else:
        # system has gone from unlocked to locked
        self.gps_locked = locked
        pass

      # stop first
      self.usrp_source_object.stop()
      self.log.debug("USRP Object Stopped for Time Synchronization")

      # TODO: Find way to ensure we poll as closely to the second boundary as
      # possible to ensure we get as accurate a reading as possible
        #gps_time = self.usrp_source_object.get_mboard_sensor("gps_time")

      ## check PPS and compare UHD device time to GPS time
      ## we only care about the full seconds for now
      #gps_time = self.usrp_source_object.get_mboard_sensor("gps_time")
      #print "gps_time = " + repr(gps_time)
      #last_pps_time = self.usrp_source_object.get_time_last_pps()
      #print "last_pps_time = " + repr(last_pps_time)

      # we only care about the full seconds
      gps_seconds = self.usrp_source_object.get_mboard_sensor("gps_time").to_int()
      self.log.debug("gps_seconds = " + repr(gps_seconds))
      pps_seconds = self.usrp_source_object.get_time_last_pps().to_ticks(1.0)
      self.log.debug("pps_seconds = " + repr(pps_seconds))

      if (pps_seconds != gps_seconds):
        self.log.debug("Trying to align the device time to GPS time...")
        try:
          self.usrp_source_object.set_time_next_pps(uhd.time_spec_t(gps_seconds + 1))
        except Exception as e:
          self.log.error("Set Time Next PPS Error: " + repr(e))

        # allow some time to make sure the PPS has come
        time.sleep(1.1)

        # check again
        gps_seconds = self.usrp_source_object.get_mboard_sensor("gps_time").to_int()
        self.log.debug("updated gps_time = " + repr(gps_seconds))
        pps_seconds = self.usrp_source_object.get_time_last_pps().to_ticks(1.0)
        self.log.debug("updated last_pps_time = " + repr(pps_seconds))

      if (pps_seconds == gps_seconds):
        self.log.debug("Successful USRP GPS Time Synchronization")
      else:
        self.log.debug("Unable to synchronize GPS time")

      # set start time in future
      self.usrp_source_object.set_start_time(uhd.time_spec_t(gps_seconds+2.0))

      # restart
      self.usrp_source_object.start()
      self.log.debug("USRP Objected Restarted After Time Synchronization")

      return True

    def msg_handler(self,p):
      self.synchronize()

    def work(self, input_items, output_items):
      pass
