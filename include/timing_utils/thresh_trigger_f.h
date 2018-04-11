/* -*- c++ -*- */
/*
 * Copyright 2018 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef INCLUDED_TIMING_UTILS_THRESH_TRIGGER_F_H
#define INCLUDED_TIMING_UTILS_THRESH_TRIGGER_F_H

#include <timing_utils/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace timing_utils {

    /*!
     * \brief <+description of block+>
     * \ingroup timing_utils
     *
     */
    class TIMING_UTILS_API thresh_trigger_f : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<thresh_trigger_f> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of timing_utils::thresh_trigger_f.
       *
       * To avoid accidental use of raw pointers, timing_utils::thresh_trigger_f's
       * constructor is in a private implementation
       * class. timing_utils::thresh_trigger_f::make is the public interface for
       * creating new instances.
       */
      static sptr make(float hi, float lo, int length);
      virtual void set_hi(float t) = 0;
      virtual void set_lo(float t) = 0;
      virtual void set_len(int len) = 0;
    };

  } // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_THRESH_TRIGGER_F_H */
