/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_SYSTEM_TIME_TAGGER_H
#define INCLUDED_TIMING_UTILS_SYSTEM_TIME_TAGGER_H

#include <gnuradio/sync_block.h>
#include <timing_utils/api.h>

namespace gr {
namespace timing_utils {

/*!
 * \brief System Time Tagger
 * \ingroup timing_utils
 *
 * Inserts system time tag every N samples
 */
template <class T>
class TIMING_UTILS_API system_time_tagger : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<system_time_tagger<T>> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of timing_utils::system_time_tagger.
     *
     * @param tag_interval - number of samples between tags
     */
    static sptr make(uint32_t tag_interval);

    /**
     * Sets interval between tags
     *
     * @param tag_interval
     */
    virtual void set_interval(uint32_t tag_interval) = 0;
};

typedef system_time_tagger<unsigned char> system_time_tagger_b;
typedef system_time_tagger<short> system_time_tagger_s;
typedef system_time_tagger<float> system_time_tagger_f;
typedef system_time_tagger<gr_complex> system_time_tagger_c;
} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_SYSTEM_TIME_TAGGER_H */
