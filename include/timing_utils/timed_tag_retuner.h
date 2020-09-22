/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_TIMED_TAG_RETUNER_H
#define INCLUDED_TIMING_UTILS_TIMED_TAG_RETUNER_H

#include <gnuradio/sync_block.h>
#include <pmt/pmt.h>
#include <timing_utils/api.h>

namespace gr {
namespace timing_utils {

/*!
 * \brief Timed tag retuning
 *
 * Tag a data stream for frequency translation at a specified timed
 *
 * To provide the timed CORDIC tuning capability of many SDRs, this block,
 * when used in conjunction with the timed frequency translating FIR filter,
 * provides a software solution for this capability.  The timed tag
 * retuner accepts a dictionary indicating the desired CORDIC frequency and
 * time of the desired frequency translation and places a tag on the output
 * data stream to be consumed and processed by the translating FIR filter.
 *
 * The tuning message must be a dictionary with the following elements:
 *   - lo_offset (double) : Required parameter indicating the desired center
 *                          frequency with respect to the signal center
 *                          frequency
 *   - rx_time (pair) : Optional parameter with the first element (car) being
 *                      the integer number of seconds and the second element
 *                      (cdr) being the fractional number of seconds to apply
 *                      the frequency translation.
 *
 * The outgoing data stream is tagged using the specified `dict_key` tag at
 * the sample to begin applying the translation.  The `dict_key` tag should be
 * the same used for the timed_freq_xlating_fir `tag_key`
 *
 * \ingroup timing_utils
 *
 */
class TIMING_UTILS_API timed_tag_retuner : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<timed_tag_retuner> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of timing_utils::timed_tag_retuner.
     *
     * To avoid accidental use of raw pointers, timing_utils::timed_tag_retuner's
     * constructor is in a private implementation
     * class. timing_utils::timed_tag_retuner::make is the public interface for
     * creating new instances.
     *
     * @param sample_rate Sample rate (Hz)
     * @param dict_key Dictionary key to add to outgoing time tag for retune
     * @param origin_t_secs Start time (integer seconds)
     * @param origin_t_frac Start time (fractional seconds)
     */
    static sptr make(double sample_rate,
                     pmt::pmt_t dict_key,
                     uint64_t origin_t_secs,
                     double origin_t_frac);
};

} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_TIMED_TAG_RETUNER_H */
