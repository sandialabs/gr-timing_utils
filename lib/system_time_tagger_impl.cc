/* -*- c++ -*- */
/*
 * Copyright 2018-2021 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system_time_tagger_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace timing_utils {

template <class T>
typename system_time_tagger<T>::sptr system_time_tagger<T>::make(uint32_t tag_interval)
{
    return gnuradio::make_block_sptr<system_time_tagger_impl<T>>(tag_interval);
}

/*
 * The private constructor
 */
template <class T>
system_time_tagger_impl<T>::system_time_tagger_impl(uint32_t tag_interval)
    : gr::sync_block("system_time_tagger",
                     gr::io_signature::make(1, 1, sizeof(T)),
                     gr::io_signature::make(1, 1, sizeof(T))),
      d_total_nitems_read(0)
{
    // set tag generating tag_interval
    set_interval(tag_interval);

    // get posix time
    boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
    d_epoch = epoch;
}

/*
 * Our virtual destructor.
 */
template <class T>
system_time_tagger_impl<T>::~system_time_tagger_impl()
{
}

template <class T>
int system_time_tagger_impl<T>::work(int noutput_items,
                                     gr_vector_const_void_star& input_items,
                                     gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock l(this->d_setlock);

    const T* in = (const T*)input_items[0];
    T* out = (T*)output_items[0];

    // total number of items read
    d_total_nitems_read = this->nitems_read(0) + noutput_items;

    // see if we need to add any more time tags
    if (d_tagging_enabled) {
        while (d_next_tag_offset < d_total_nitems_read) {
            if (d_next_tag_offset >= this->nitems_read(0)) {
                // add tag
                double t_now((boost::get_system_time() - d_epoch).total_microseconds() /
                             1000000.0);
                this->add_item_tag(0,
                                   d_next_tag_offset,
                                   PMTCONSTSTR__wall_clock_time(),
                                   pmt::from_double(t_now));

                // set next offset
                d_next_tag_offset += d_interval;
            } else {
                // we should not have gotten into this state...
                GR_LOG_WARN(
                    this->d_logger,
                    "unexpected state: attempted to tag item in previous work call!");
                // reset the next tag offset to the first item in the next input buffer;
                d_next_tag_offset = d_total_nitems_read;
            }
        }
    }

    // then copy all of the items to the output
    memcpy((void*)out, (void*)in, sizeof(T) * noutput_items);

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

template <class T>
void system_time_tagger_impl<T>::set_interval(uint32_t interval)
{
    gr::thread::scoped_lock l(this->d_setlock);

    d_interval = interval;
    if (d_interval != 0) {
        d_tagging_enabled = true;
        d_next_tag_offset = d_total_nitems_read + interval;
    } else {
        d_tagging_enabled = false;
    }
}

template class system_time_tagger<unsigned char>;
template class system_time_tagger<short>;
template class system_time_tagger<float>;
template class system_time_tagger<gr_complex>;
} /* namespace timing_utils */
} /* namespace gr */
