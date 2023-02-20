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

#include "add_usrp_tags_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/timing_utils/constants.h>

namespace gr {
namespace timing_utils {

template <class T>
typename add_usrp_tags<T>::sptr
add_usrp_tags<T>::make(double freq, double rate, uint64_t epoch_int, double epoch_frac)
{
    return gnuradio::make_block_sptr<add_usrp_tags_impl<T>>(
        freq, rate, epoch_int, epoch_frac);
}

/*
 * The private constructor
 */
template <class T>
add_usrp_tags_impl<T>::add_usrp_tags_impl(double freq,
                                          double rate,
                                          uint64_t epoch_int,
                                          double epoch_frac)
    : gr::sync_block("add_usrp_tags",
                     gr::io_signature::make(1, 1, sizeof(T)),
                     gr::io_signature::make(1, 1, sizeof(T)))
{
    d_rate_pmt = pmt::from_double(rate);
    d_freq_pmt = pmt::from_double(freq);
    d_time_pmt =
        pmt::make_tuple(pmt::from_uint64(epoch_int), pmt::from_double(epoch_frac));

    tag_now(0);
    d_tagged_sample = 0;
    d_pmt_dict = pmt::make_dict();
    d_pmt_dict = pmt::dict_add(d_pmt_dict, PMTCONSTSTR__rx_rate(), d_rate_pmt);
    d_pmt_dict = pmt::dict_add(d_pmt_dict, PMTCONSTSTR__rx_freq(), d_freq_pmt);
    d_pmt_dict = pmt::dict_add(d_pmt_dict, PMTCONSTSTR__rx_time(), d_time_pmt);
}

/*
 * Our virtual destructor.
 */
template <class T>
add_usrp_tags_impl<T>::~add_usrp_tags_impl()
{
}

template <class T>
void add_usrp_tags_impl<T>::update_tags(pmt::pmt_t update)
{
    if (!pmt::is_dict(update)) {
        this->d_logger->notice("received unexpected PMT (non-dict)");
        return;
    }

    pmt::pmt_t value = pmt::dict_ref(update, PMTCONSTSTR__rx_freq(), pmt::PMT_NIL);
    if (value != pmt::PMT_NIL)
        d_freq_pmt = value;
    value = pmt::dict_ref(update, PMTCONSTSTR__rx_rate(), pmt::PMT_NIL);
    if (value != pmt::PMT_NIL)
        d_rate_pmt = value;
    value = pmt::dict_ref(update, PMTCONSTSTR__rx_time(), pmt::PMT_NIL);
    if (value != pmt::PMT_NIL)
        d_time_pmt = value;
    d_tag_now = true;
    d_pmt_dict = pmt::dict_add(d_pmt_dict, PMTCONSTSTR__rx_rate(), d_rate_pmt);
    d_pmt_dict = pmt::dict_add(d_pmt_dict, PMTCONSTSTR__rx_freq(), d_freq_pmt);
    d_pmt_dict = pmt::dict_add(d_pmt_dict, PMTCONSTSTR__rx_time(), d_time_pmt);
    this->d_logger->debug("Updating tags");
}

template <class T>
pmt::pmt_t add_usrp_tags_impl<T>::last_tag()
{
    pmt::pmt_t retDict = pmt::dict_add(
        d_pmt_dict, PMTCONSTSTR__rx_sample(), pmt::from_uint64(d_tagged_sample));
    return retDict;
}


template <class T>
int add_usrp_tags_impl<T>::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    const T* in = (const T*)input_items[0];
    T* out = (T*)output_items[0];

    // pass through
    memcpy(out, in, noutput_items * sizeof(T));

    // add tags on the first sample in the output buffer if indicated
    if (d_tag_now) {
        this->add_item_tag(0, this->nitems_read(0), PMTCONSTSTR__rx_freq(), d_freq_pmt);
        this->add_item_tag(0, this->nitems_read(0), PMTCONSTSTR__rx_rate(), d_rate_pmt);
        this->add_item_tag(0, this->nitems_read(0), PMTCONSTSTR__rx_time(), d_time_pmt);
        d_tagged_sample = this->nitems_read(0);
        d_tag_now = false;
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

template class add_usrp_tags<unsigned char>;
template class add_usrp_tags<short>;
template class add_usrp_tags<int32_t>;
template class add_usrp_tags<float>;
template class add_usrp_tags<gr_complex>;
} /* namespace timing_utils */
} /* namespace gr */
