/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_TIMED_FREQ_XLATING_FIR_H
#define INCLUDED_TIMING_UTILS_TIMED_FREQ_XLATING_FIR_H

#include <gnuradio/sync_decimator.h>
#include <timing_utils/api.h>

namespace gr {
namespace timing_utils {


/*!
 * \brief FIR filter combined with frequency translation with
 * gr_complex input, gr_complex output and gr_complex taps
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
 *
 * @tparam I input type (short, float, complex)
 * @tparam O output type (float, complex)
 * @tparam T tap type (float, complex)
 */
template <class I, class O, class T>
class TIMING_UTILS_API timed_freq_xlating_fir : virtual public sync_decimator
{
public:
    typedef boost::shared_ptr<timed_freq_xlating_fir<I, O, T>> sptr;

    /*!
     * \brief FIR filter with gr_complex input, gr_complex output, and
     * gr_complex taps that also frequency translates a signal from
     * \p center_freq.
     *
     * Construct a FIR filter with the given taps and a composite
     * frequency translation that shifts center_freq down to zero
     * Hz. The frequency translation logically comes before the
     * filtering operation.
     *
     * \param decimation set the integer decimation rate
     * \param taps a vector/list of taps of type @tparam T
     * \param center_freq Center frequency of signal to down convert from (Hz)
     * \param sampling_freq Sampling rate of signal (in Hz)
     * \param tag_key Frequency tag key
     */
    static sptr make(int decimation,
                     const std::vector<T>& taps,
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
    virtual void set_taps(const std::vector<T>& taps) = 0;

    /*! \brief Get FIR filter taps
     *
     * \return Filter taps
     */
    virtual std::vector<T> taps() const = 0;
};

typedef timed_freq_xlating_fir<gr_complex, gr_complex, gr_complex>
    timed_freq_xlating_fir_ccc;
typedef timed_freq_xlating_fir<gr_complex, gr_complex, float> timed_freq_xlating_fir_ccf;
typedef timed_freq_xlating_fir<float, gr_complex, gr_complex> timed_freq_xlating_fir_fcc;
typedef timed_freq_xlating_fir<float, gr_complex, float> timed_freq_xlating_fir_fcf;
typedef timed_freq_xlating_fir<short, gr_complex, gr_complex> timed_freq_xlating_fir_scc;
typedef timed_freq_xlating_fir<short, gr_complex, float> timed_freq_xlating_fir_scf;
} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_TIMED_FREQ_XLATING_FIR_CCC_H */
