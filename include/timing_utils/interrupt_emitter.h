/* -*- c++ -*- */
/*
 * <COPYRIGHT PLACEHOLDER>
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
     * \brief Emit a message at a desired time based on the time of a processed
     * sample
     *
     * \ingroup timing_utils
     *
     * For applications requiring an interrupt like event to wake or trigger
     * functionality, the interrupt_emitter block emits a message based
     * partially on the time of the currently processed sample and the system
     * time.  Using the system time as a reference point, the interrupt
     * emitter can more accurately emit a message without actually processing
     * the sample nearest in time to the requested interrupt.
     *
     * The emitted message is a dictionary with the following elements:
     *    - trigger_time (original request type, either uint64, pair, or tuple)
     *    - trigger_sample (uint64)
     *
     * Additionally, in the event of a late interrupt being issued, the
     * dictionary will have the following additional element:
     *    - late_delta (double)
     *
     * The `rx_time` stream tag is used to adjust the internal times, accounting
     * for things like overflows or discontinous streams of data
     *
     * Note: This block has been templatized to maintain backward compatability
     * (Each block is instantiated based on the input/output data type)
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
       *
       * \param rate Sample rate (Hz)
       * \param drop_late If true, do not emit a message for interrupt requests
       *    in the past
       *
       * \return pointer to new instance
       */
      static sptr make(double rate, bool drop_late);

      /*! \brief Set the stream rate
       *
       * \param rate Sample rate (Hz)
       */
      virtual void set_rate(double rate) = 0;

      /*! \brief Set debug mode
       *
       * \param value Enable additional debug statements that are printed
       *     directly to stdout
       */
      virtual void set_debug(bool value) = 0;
    };

  } // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_INTERRUPT_EMITTER_H */
