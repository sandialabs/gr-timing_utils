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

 /* WARNING: this file is machine generated. Edits will be overwritten */

#ifndef INCLUDED_TIMING_UTILS_TIMED_FREQ_XLATING_FIR_FCC_H
#define	INCLUDED_TIMING_UTILS_TIMED_FREQ_XLATING_FIR_FCC_H

#include <timing_utils/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
  namespace timing_utils {


        /*!
         * \brief FIR filter combined with frequency translation with
         * float input, gr_complex output and gr_complex taps
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
        class TIMING_UTILS_API timed_freq_xlating_fir_fcc : virtual public sync_decimator
        {
        public:
          typedef boost::shared_ptr<timed_freq_xlating_fir_fcc> sptr;

          /*!
           * \brief FIR filter with float input, gr_complex output, and
           * gr_complex taps that also frequency translates a signal from
           * \p center_freq.
           *
           * Construct a FIR filter with the given taps and a composite
           * frequency translation that shifts center_freq down to zero
           * Hz. The frequency translation logically comes before the
           * filtering operation.
           *
           * \param decimation set the integer decimation rate
           * \param taps a vector/list of taps of type gr_complex
           * \param center_freq Center frequency of signal to down convert from (Hz)
           * \param sampling_freq Sampling rate of signal (in Hz)
           * \param tag_key Frequency tag key
           */
          static sptr make(int decimation,
    				  const std::vector<gr_complex> &taps,
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

          virtual void set_center_freq(double center_freq) = 0;
          virtual double center_freq() const = 0;

          virtual void set_taps(const std::vector<gr_complex> &taps) = 0;
          virtual std::vector<gr_complex> taps() const = 0;
        };

  } // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_TIMED_FREQ_XLATING_FIR_FCC_H */
