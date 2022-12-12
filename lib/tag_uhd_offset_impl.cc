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

#include "tag_uhd_offset_impl.h"
#include <gnuradio/io_signature.h>
#include <boost/format.hpp>
namespace gr {
namespace timing_utils {

template <class T>
typename tag_uhd_offset<T>::sptr tag_uhd_offset<T>::make(float rate,
                                                         uint32_t tag_interval)
{
    return gnuradio::make_block_sptr<tag_uhd_offset_impl<T>>(rate, tag_interval);
}

/*
 * The private constructor
 */
template <class T>
tag_uhd_offset_impl<T>::tag_uhd_offset_impl(float rate, uint32_t tag_interval)
    : gr::sync_block("tag_uhd_offset",
                     gr::io_signature::make(1, 1, sizeof(T)),
                     gr::io_signature::make(1, 1, sizeof(T))),
      d_rate(rate),
      d_key(PMTCONSTSTR__rx_time()),
      d_total_nitems_read(0),
      d_time_tag_offset(0),
      d_time_tag_int_sec(0),
      d_time_tag_frac_sec(0.0),
      d_time_tag(pmt::PMT_NIL)
{
    set_interval(tag_interval);
    GR_LOG_INFO(this->d_logger, "setting up time tagger");

    this->message_port_register_out(PMTCONSTSTR__time());
}

/*
 * Our virtual destructor.
 */
template <class T>
tag_uhd_offset_impl<T>::~tag_uhd_offset_impl()
{
}

/*
 * set the time_tag parameters to input UHD tag
 */
template <class T>
void tag_uhd_offset_impl<T>::set_time_tag(tag_t uhd_time_tag)
{
    if (pmt::is_tuple(uhd_time_tag.value)) {
        d_time_tag_offset = uhd_time_tag.offset;
        d_time_tag_int_sec = pmt::to_uint64(pmt::tuple_ref(uhd_time_tag.value, 0));
        d_time_tag_frac_sec = pmt::to_double(pmt::tuple_ref(uhd_time_tag.value, 1));
        d_time_tag = pmt::make_tuple(pmt::tuple_ref(uhd_time_tag.value, 0),
                                     pmt::tuple_ref(uhd_time_tag.value, 1),
                                     pmt::from_uint64(uhd_time_tag.offset),
                                     pmt::from_double(d_rate));
    }
}


/*
 * move the time_tag OFFSET samples into the future
 */
template <class T>
void tag_uhd_offset_impl<T>::update_time_tag(uint64_t offset)
{
    double delta = ((offset - d_time_tag_offset) / d_rate);
    if (delta < 0) {
        GR_LOG_ERROR(this->d_logger,
                     boost::format("can't go back in time...updating time tag failed "
                                   "(requested delta = %fs)") %
                         delta);
        return;
    }
    d_time_tag_frac_sec += delta;
    d_time_tag_offset = offset;
    // fractional seconds rollover
    if (d_time_tag_frac_sec >= 1.0) {
        d_time_tag_frac_sec -= 1.0;
        d_time_tag_int_sec += 1;
    }
    d_time_tag = pmt::make_tuple(pmt::from_uint64(d_time_tag_int_sec),
                                 pmt::from_double(d_time_tag_frac_sec),
                                 pmt::from_uint64(d_time_tag_offset),
                                 pmt::from_double(d_rate));
}


/*
 * Work function - will add the tag offset to the UHD time tuple when it
 * occurrs. Can also emit periodic timing tags based on this information.
 */
template <class T>
int tag_uhd_offset_impl<T>::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock l(this->d_setlock);

    const T* in = (const T*)input_items[0];
    T* out = (T*)output_items[0];

    std::vector<tag_t> tags;
    d_total_nitems_read = this->nitems_read(0) + noutput_items;

    // first check to see if there are any UHD time tags
    this->get_tags_in_range(tags,
                            0,
                            this->nitems_read(0),
                            (this->nitems_read(0) + noutput_items),
                            PMTCONSTSTR__rx_time());
    if (tags.size()) {
        for (size_t ii = 0; ii < tags.size(); ii++) {
            set_time_tag(tags[ii]);
            this->add_item_tag(0, tags[ii].offset, d_key, d_time_tag);
            // GR_LOG_INFO(this->d_logger, boost::format("TAGGG! at sample %d") %
            // d_time_tag_offset);
            this->message_port_pub(PMTCONSTSTR__time(), pmt::cons(d_key, d_time_tag));
            d_next_tag_offset = tags[ii].offset + d_interval;
        }
    }

    // next see if we need to add any more time tags
    if (d_tagging_enabled) {
        while (d_next_tag_offset < d_total_nitems_read) {
            if (d_next_tag_offset >= this->nitems_read(0)) {
                // necessary tag sample is within the window
                update_time_tag(d_next_tag_offset);
                this->add_item_tag(0, d_next_tag_offset, d_key, d_time_tag);
                // GR_LOG_INFO(this->d_logger, boost::format("TAG! at sample %d") %
                // d_time_tag_offset);
                d_next_tag_offset += d_interval;
                // GR_LOG_INFO(this->d_logger, boost::format("next one at %d") %
                // d_next_tag_offset);
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
void tag_uhd_offset_impl<T>::set_rate(float rate)
{
    gr::thread::scoped_lock l(this->d_setlock);

    d_rate = rate;
}

template <class T>
void tag_uhd_offset_impl<T>::set_interval(uint32_t interval)
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

template <class T>
void tag_uhd_offset_impl<T>::set_key(pmt::pmt_t key)
{
    gr::thread::scoped_lock l(this->d_setlock);

    d_key = key;
}

template class tag_uhd_offset<unsigned char>;
template class tag_uhd_offset<short>;
template class tag_uhd_offset<float>;
template class tag_uhd_offset<gr_complex>;
} /* namespace timing_utils */
} /* namespace gr */
