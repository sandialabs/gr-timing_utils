/* -*- c++ -*- */
/*
 * Copyright 2018-2021 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_TAG_UHD_OFFSET_H
#define INCLUDED_TIMING_UTILS_TAG_UHD_OFFSET_H

#include <gnuradio/sync_block.h>
#include <gnuradio/timing_utils/api.h>

namespace gr {
namespace timing_utils {

/*!
 * \brief <+description of block+>
 * \ingroup timing_utils
 *
 */
template <class T>
class TIMING_UTILS_API tag_uhd_offset : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<tag_uhd_offset<T>> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of timing_utils::tag_uhd_offset.
     *
     * @param rate
     * @param tag_interval
     */
    static sptr make(float rate, uint32_t tag_interval);

    /**
     *
     * @param rate -
     */
    virtual void set_rate(float rate) = 0;

    /**
     *
     * @param tag_interval
     */
    virtual void set_interval(uint32_t tag_interval) = 0;

    /**
     *
     * @param tag_key -
     */
    virtual void set_key(pmt::pmt_t tag_key) = 0;
};

typedef tag_uhd_offset<unsigned char> tag_uhd_offset_b;
typedef tag_uhd_offset<short> tag_uhd_offset_s;
typedef tag_uhd_offset<float> tag_uhd_offset_f;
typedef tag_uhd_offset<gr_complex> tag_uhd_offset_c;
} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_TAG_UHD_OFFSET_S_H */
