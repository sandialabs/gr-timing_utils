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

#include "system_time_diff_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace timing_utils {

template <class T>
typename system_time_diff<T>::sptr system_time_diff<T>::make(bool update_time,
                                                             bool output_diff)
{
    return gnuradio::make_block_sptr<system_time_diff_impl<T>>(update_time, output_diff);
}

/*
 * The private constructor
 */
template <class T>
system_time_diff_impl<T>::system_time_diff_impl(bool update_time, bool output_diff)
    : gr::block("system_time_diff",
                gr::io_signature::make(1, 1, sizeof(T)),
                gr::io_signature::makev(1, 2, std::vector<int>(4, sizeof(T)))),
      d_update_time(update_time),
      d_output_diff(output_diff)

{
    // Brute force fix of the output io_signature, because I can't get
    // an anonymous std::vector<int>() rvalue, with a const expression
    // initializing the vector, to work.  Lvalues seem to make everything
    // better.
    int output_io_sizes[2] = { sizeof(T), sizeof(float) };
    std::vector<int> output_io_sizes_vector(&output_io_sizes[0], &output_io_sizes[2]);
    this->set_output_signature(io_signature::makev(1, 2, output_io_sizes_vector));

    // get posix time
    boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
    d_epoch = epoch;

    // don't propagate tags - all tags will be propagated
    // directly in work function since we can not propagate
    // tags on only a single stream
    this->set_tag_propagation_policy(gr::block::TPP_DONT);
}

/*
 * Our virtual destructor.
 */
template <class T>
system_time_diff_impl<T>::~system_time_diff_impl()
{
}

template <class T>
int system_time_diff_impl<T>::general_work(int noutput_items,
                                           gr_vector_int& ninput_items,
                                           gr_vector_const_void_star& input_items,
                                           gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock l(this->d_setlock);

    // total number of input items
    int nitems = std::min(ninput_items[0], noutput_items);

    // get input dat
    const T* in = (const T*)input_items[0];

    // get output data only if necessary
    T* out = (T*)output_items[0];
    float* diff_out = (float*)output_items[1];

    // check to see if there are any tags
    std::vector<tag_t> tags;
    this->get_tags_in_range(
        tags, 0, this->nitems_read(0), (this->nitems_read(0) + nitems));
    if (tags.size()) {
        // get current system time
        double t_now((boost::get_system_time() - d_epoch).total_microseconds() /
                     1000000.0);

        // process all tags
        int ntime_tags = 0;
        for (int ii = 0; ii < tags.size(); ii++) {
            if (pmt::eq(PMTCONSTSTR__wall_clock_time(), tags[ii].key)) {
                double diff = t_now - pmt::to_double(tags[ii].value);
                if (d_output_diff) {
                    std::cout << boost::format("diff = %0.9f s") % diff << std::endl;
                }

                // update tag if requested
                if (d_update_time) {
                    this->add_item_tag(0,
                                       tags[ii].offset,
                                       PMTCONSTSTR__wall_clock_time(),
                                       pmt::from_double(t_now));
                } else {
                    this->add_item_tag(0, tags[ii].offset, tags[ii].key, tags[ii].value);
                }


                // output time
                diff_out[ntime_tags++] = (float)diff;
            } else {
                // add tag back
                this->add_item_tag(0, tags[ii].offset, tags[ii].key, tags[ii].value);
            }
        }

        // update number of items produced
        this->produce(1, ntime_tags);
    } /* end if tags.size() */

    // copy all of the items to the output
    memcpy((void*)out, (void*)in, sizeof(T) * nitems);
    this->produce(0, nitems);

    // tell the scheduler how many items were consumed
    this->consume(0, nitems);

    // Tell runtime system how many output items we produced.
    return gr::block::WORK_CALLED_PRODUCE;
}

template class system_time_diff<unsigned char>;
template class system_time_diff<short>;
template class system_time_diff<float>;
template class system_time_diff<gr_complex>;
} /* namespace timing_utils */
} /* namespace gr */
