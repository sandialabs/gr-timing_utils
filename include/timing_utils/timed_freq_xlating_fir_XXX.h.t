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

 /* @WARNING@ */

#ifndef @GUARD_NAME@
#define	@GUARD_NAME@

#include <timing_utils/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
  namespace timing_utils {


        /*!
         * \brief FIR filter combined with frequency translation with
         * @I_TYPE@ input, @O_TYPE@ output and @TAP_TYPE@ taps
         *
         * \ingroup channelizers_blk
         *
         * This class efficiently combines a frequency translation
         * (typically "down conversion") with a FIR filter (typically
         * low-pass) and decimation.  It is ideally suited for a "channel
         * selection filter" and can be efficiently used to select and
         * decimate a narrow band signal out of wide bandwidth input.
         *
         * Uses a single input array to produce a single output array.
         * Additional inputs and/or outputs are ignored.
         *
         * - freq (input):
         *        Receives a PMT pair: (intern("freq"), double(frequency).
         *        The block then sets its frequency translation value to
         *        the new frequency provided by the message. A tag is then
         *        produced when the new frequency is applied to let
         *        downstream blocks know when this has taken affect.
         *        Use the filter's group delay to determine when the
         *        transients after the change have settled down.
         */
        class TIMING_UTILS_API @BASE_NAME@ : virtual public sync_decimator
        {
        public:
          typedef boost::shared_ptr<@BASE_NAME@> sptr;

          /*!
           * \brief FIR filter with @I_TYPE@ input, @O_TYPE@ output, and
           * @TAP_TYPE@ taps that also frequency translates a signal from
           * \p center_freq.
           *
           * Construct a FIR filter with the given taps and a composite
           * frequency translation that shifts center_freq down to zero
           * Hz. The frequency translation logically comes before the
           * filtering operation.
           *
           * \param decimation set the integer decimation rate
           * \param taps a vector/list of taps of type @TAP_TYPE@
           * \param center_freq Center frequency of signal to down convert from (Hz)
           * \param sampling_freq Sampling rate of signal (in Hz)
           * \param tag_key Frequency tag key
           */
          static sptr make(int decimation,
    				  const std::vector<@TAP_TYPE@> &taps,
    				  double center_freq,
    				  double sampling_freq,
                                  std::string tag_key = "set_freq");

          /*!
           * \brief Set FIR filter decimation
           *
           * \param decimation set the integer decimation rate
           */
          virtual void set_decim(int decimation) = 0;

          /*!
           * \brief Get FIR filter decimation
           *
           * \return integer decimation rate
           */
          virtual int decim() const = 0;

          /*!
           * \brief Set FIR filter sampling rate
           *
           * \param rate  set the sampling rate (Hz)
           */
          virtual void set_rate(double rate) = 0;

          /*!
           * \brief Get FIR filter sampling rate
           *
           * \return sampling rate (Hz)
           */
          virtual double rate() const = 0;

          /*! \brief Set center frequency
           *
           * All tune requests are based on an assumed center frequency
           * for the incoming data.
           *
           * \param center_freq Center frequency (Hz)
           */
          virtual void set_center_freq(double center_freq) = 0;

          /*! \brief Get center frequency
           *
           * \return Center frequency (Hz)
           */
          virtual double center_freq() const = 0;

          /*! \brief Set taps
           *
           * Set the FIR filter taps
           *
           * \param taps FIR filter taps
           */
          virtual void set_taps(const std::vector<@TAP_TYPE@> &taps) = 0;

          /*! \brief Get FIR filter taps
           *
           * \return Filter taps
           */
          virtual std::vector<@TAP_TYPE@> taps() const = 0;
        };

  } // namespace timing_utils
} // namespace gr

#endif /* @GUARD_NAME@ */
