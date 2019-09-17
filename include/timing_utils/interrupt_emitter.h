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


#ifndef INCLUDED_TIMING_UTILS_INTERRUPT_EMITTER_H
#define INCLUDED_TIMING_UTILS_INTERRUPT_EMITTER_H

#include <timing_utils/api.h>
#include <gnuradio/sync_block.h>
//#include <boost/chrono.hpp>

namespace gr {
  namespace timing_utils {

    /*!
     * \brief <+description of block+>
     * \ingroup timing_utils
     *
     */
    class TIMING_UTILS_API interrupt_emitter : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<interrupt_emitter> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of timing_utils::uhd_timed_pdu_emitter.
       *
       * To avoid accidental use of raw pointers, timing_utils::uhd_timed_pdu_emitter's
       * constructor is in a private implementation
       * class. timing_utils::uhd_timed_pdu_emitter::make is the public interface for
       * creating new instances.
       */
      virtual void set_rate(double rate) = 0;
      virtual void set_debug(bool value) = 0;
      virtual bool start() = 0;

      static sptr make(double rate, bool drop_late);
    };

  } // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_INTERRUPT_EMITTER_H */
