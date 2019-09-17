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
from gnuradio import gr
from gnuradio import uhd
import time
import pmt

class usrp_gps_time_sync(gr.sync_block):
    """
    docstring for block usrp_gps_time_sync
    """
    def __init__(self, parent, usrp_source):
      gr.sync_block.__init__(self,
          name="usrp_gps_time_sync",
          in_sig=None,
          out_sig=None)
      
      self.parent = parent
      self.usrp_source = usrp_source
      self.usrp_source_object = None
      
      self.message_port_register_in(pmt.intern("in"))
      self.set_msg_handler(pmt.intern("in"),self.msg_handler)
      
      # current gps status
      self.has_gpsdo = False
      self.gps_locked = False

    def start(self):
      print "Starting GPS Time Sync"

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
        print "Unable to acquire usrp object for synchronization"
        return True

      # synchronize
      self.synchronize()
      
      # if not locked, default time to current system time
      if not self.gps_locked:
	try:
          print "USRP GPS Time Sync: Defaulting to Current System Time"
          self.usrp_source_object.set_time_now(uhd.time_spec_t(time.time()))
        except Exception as e:
          print "Set Time Next PPS Error: " + repr(e)
      
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
      print "USRP Object Stopped for Time Synchronization"
      
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
      print "gps_seconds = " + repr(gps_seconds)
      pps_seconds = self.usrp_source_object.get_time_last_pps().to_ticks(1.0)
      print "pps_seconds = " + repr(pps_seconds)
      
      if (pps_seconds != gps_seconds):
        print "\nTrying to align the device time to GPS time..."
        try:
          self.usrp_source_object.set_time_next_pps(uhd.time_spec_t(gps_seconds + 1))
        except Exception as e:
          print "Set Time Next PPS Error: " + repr(e)
        
        # allow some time to make sure the PPS has come
        time.sleep(1.1)
        
        # check again
        gps_seconds = self.usrp_source_object.get_mboard_sensor("gps_time").to_int()
        print "updated gps_time = " + repr(gps_seconds)
        pps_seconds = self.usrp_source_object.get_time_last_pps().to_ticks(1.0)
        print "updated last_pps_time = " + repr(pps_seconds)
        
      if (pps_seconds == gps_seconds):
        print "Successful USRP GPS Time Synchronization"
      else:
        print "Unable to synchronize GPS time"
        
      # set start time in future
      self.usrp_source_object.set_start_time(uhd.time_spec_t(gps_seconds+2.0))
      
      # restart
      self.usrp_source_object.start()
      print "USRP Objected Restarted After Time Synchronization"

      return True
    
    def msg_handler(self,p):
      self.synchronize()

    def work(self, input_items, output_items):
      pass

