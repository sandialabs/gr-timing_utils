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

#include "timed_tag_retuner_impl.h"
#include <gnuradio/io_signature.h>

#define MAX_NUM_COMMANDS 64

namespace gr {
namespace timing_utils {

timed_tag_retuner::sptr timed_tag_retuner::make(double sample_rate,
                                                pmt::pmt_t dict_key,
                                                uint64_t origin_t_secs,
                                                double origin_t_frac)
{
    return gnuradio::make_block_sptr<timed_tag_retuner_impl>(
        sample_rate, dict_key, origin_t_secs, origin_t_frac);
}


/*
 * The private constructor
 */
timed_tag_retuner_impl::timed_tag_retuner_impl(double sample_rate,
                                               pmt::pmt_t dict_key,
                                               uint64_t origin_t_secs,
                                               double origin_t_frac)
    : gr::sync_block("timed_tag_retuner",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(1, 1, sizeof(gr_complex))),
      d_sample_rate(sample_rate),
      d_dict_key(dict_key),
      d_ref_time(origin_t_secs, origin_t_frac, 0)
{

    // downstream timed frequency translating fir filter block
    // uses this tag to change frequencies
    d_tag_key = PMTCONSTSTR__set_freq();

    // time reference key
    d_time_key = PMTCONSTSTR__rx_time();

    // register message ports
    this->message_port_register_out(PMTCONSTSTR__freq());
    this->message_port_register_in(PMTCONSTSTR__command());
    set_msg_handler(PMTCONSTSTR__command(),
                    [this](pmt::pmt_t msg) { this->command_handler(msg); });
}

/*
 * Our virtual destructor.
 */
timed_tag_retuner_impl::~timed_tag_retuner_impl() {}

void timed_tag_retuner_impl::command_handler(pmt::pmt_t msg)
{
    gr::thread::scoped_lock l(this->d_setlock);

    if (!pmt::is_dict(msg)) {
        d_logger->error("Retune commands must be dictioanries");
        return;
    }

    pmt::pmt_t lo_offset;
    try {
        lo_offset = pmt::dict_ref(msg, d_dict_key, pmt::PMT_NIL);
    } catch (...) {
        d_logger->error("Unable to read dictionary keys");
        return;
    }
    if (!pmt::equal(lo_offset, pmt::PMT_NIL)) {
        double offset = 0.0;
        try {
            offset = pmt::to_double(lo_offset);
        } catch (...) {
            d_logger->error("Tune offset is wrong type.  Should be double");
            return;
        }

        this->message_port_pub(
            PMTCONSTSTR__freq(),
            pmt::cons(PMTCONSTSTR__freq(), pmt::from_double(-1. * offset)));

        bool tag_now = true;
        uint64_t tag_offset = 0;
        try {
            pmt::pmt_t time_tag = pmt::dict_ref(msg, PMTCONSTSTR__time(), pmt::PMT_NIL);
            if (!pmt::equal(time_tag, pmt::PMT_NIL)) {
                uint64_t secs = pmt::to_uint64(pmt::car(time_tag)) - d_ref_time.secs;
                double frac = pmt::to_double(pmt::cdr(time_tag)) - d_ref_time.frac;
                tag_offset = static_cast<uint64_t>(secs * d_sample_rate) +
                             static_cast<uint64_t>(frac * d_sample_rate) +
                             d_ref_time.offset;
                tag_now = false;
            }
        } catch (...) {
            d_logger->error("Unable to determine retune time.  Tagging now");
        }

        // only push command onto stack if space allows
        if (d_tune_commands.size() < MAX_NUM_COMMANDS) {
            tune_command_t tune_command(
                pmt::from_double(-1 * offset), tag_offset, tag_now);
            d_tune_commands.push(tune_command);
        }
    }
}

int timed_tag_retuner_impl::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock l(this->d_setlock);
    gr_complex* in = (gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    // copy data in to out
    memcpy((void*)out, (void*)in, sizeof(gr_complex) * noutput_items);

    // number of items currently read
    uint64_t nitems_read = this->nitems_read(0);

    // look for time reference tags
    std::vector<tag_t> tags;
    this->get_tags_in_window(tags, 0, 0, noutput_items, d_time_key);
    if (tags.size()) {
        size_t end = tags.size() - 1;
        try {
            uint64_t offset = tags[end].offset;
            uint64_t secs = pmt::to_uint64(pmt::tuple_ref(tags[end].value, 0));
            double frac = pmt::to_double(pmt::tuple_ref(tags[end].value, 1));
            d_ref_time.set(secs, frac, offset);
        } catch (...) {
            d_logger->error("Invalid tag value");
        }
    }

    // check for tunes that needs to be tagged
    while (!d_tune_commands.empty()) {
        tune_command_t tune_command = d_tune_commands.front();
        bool tag = false;
        uint64_t offset;

        if (tune_command.tag_now) {
            offset = nitems_read;
            tag = true;
        } else if (tune_command.offset < nitems_read) {
            offset = nitems_read;
            tag = true;
        } else if ((nitems_read <= tune_command.offset) and
                   (tune_command.offset < nitems_read + noutput_items)) {
            offset = tune_command.offset;
            tag = true;
        }

        if (tag) {
            this->add_item_tag(0, offset, d_tag_key, tune_command.tag);
            d_tune_commands.pop();
        } else {
            break;
        }
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace timing_utils */
} /* namespace gr */
