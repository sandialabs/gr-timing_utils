/* -*- c++ -*- */
/*
 * Copyright 2018 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
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

#ifndef INCLUDED_TIMING_UTILS_ADD_USRP_TAGS_H
#define INCLUDED_TIMING_UTILS_ADD_USRP_TAGS_H

#include <gnuradio/sync_block.h>
#include <timing_utils/api.h>

namespace gr {
namespace timing_utils {

/*!
 * \brief Add USRP tags to data stream
 *
 * \ingroup timing_utils
 *
 * Add the standard USRP tags to a data stream.  These are the tags that are
 * emitted from a USRP source block to indicate the main operating parameters
 * of the radio.
 *
 * The tags added will be:
 *  - rx_rate (double)
 *  - rx_freq (double)
 *  - rx_time (pmt time tuple (uint64_t whole seconds, double frac seconds))
 *
 * Subsequent tags will have an updated rx_time based on the current rx_rate.
 *
 * Note: The block has been templatized to maintain backward compatability,
 * rather than using a generic itemsize argument to determine the size of
 * each sample.
 */
template <class T>
class TIMING_UTILS_API add_usrp_tags : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<add_usrp_tags<T>> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of timing_utils::add_usrp_tags.
     *
     *
     * \param freq Center frequency (Hz)
     * \param rate Sampling rate (Hz)
     * \param epoch_int Whole seconds of first sample
     * \param epoch_frac Fractional seconds of first sample
     *
     * \return Pointer to new instance
     */
    static sptr make(double freq, double rate, uint64_t epoch_int, double epoch_frac);

    /*! \brief Tag data stream now
     *
     * Tag the next sample out of the block
     */
    virtual void tag_now(uint32_t dummy) = 0;

    /*! \brief Update the current tags
     *
     * Update the current tags being added
     */
    virtual void update_tags(pmt::pmt_t update) = 0;

    /*! \brief Last tag emitted
     *
     * Get the last set of tags emitted.  Additionally, the absolute index
     * of the last sample tagged will be added to the dictionary with the
     * key `rx_sample`
     *
     * \return pmt dictionary of last tags emitted
     */
    virtual pmt::pmt_t last_tag() = 0;
};

typedef add_usrp_tags<unsigned char> add_usrp_tags_b;
typedef add_usrp_tags<short> add_usrp_tags_s;
typedef add_usrp_tags<int32_t> add_usrp_tags_i;
typedef add_usrp_tags<float> add_usrp_tags_f;
typedef add_usrp_tags<gr_complex> add_usrp_tags_c;

} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_ADD_USRP_TAGS_C_H */
