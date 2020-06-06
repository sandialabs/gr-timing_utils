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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system_time_diff_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace timing_utils {

template <class T>
typename system_time_diff<T>::sptr
system_time_diff<T>::make(bool output_data, bool update_time, bool output_diff)
{
    return gnuradio::get_initial_sptr(
        new system_time_diff_impl<T>(output_data, update_time, output_diff));
}

/*
 * The private constructor
 */
template <class T>
system_time_diff_impl<T>::system_time_diff_impl(bool output_data,
                                                bool update_time,
                                                bool output_diff)
    : gr::block("system_time_diff",
                gr::io_signature::make(1, 1, sizeof(T)),
                gr::io_signature::makev(1, 2, std::vector<int>(4, sizeof(T)))),
      d_output_data(output_data),
      d_update_time(update_time),
      d_output_diff(output_diff)

{
    // Brute force fix of the output io_signature, because I can't get
    // an anonymous std::vector<int>() rvalue, with a const expression
    // initializing the vector, to work.  Lvalues seem to make everything
    // better.
    if (d_output_data or d_output_diff) {
        int output_io_sizes[2] = { sizeof(T), sizeof(float) };
        std::vector<int> output_io_sizes_vector(&output_io_sizes[0], &output_io_sizes[2]);
        this->set_output_signature(io_signature::makev(1, 2, output_io_sizes_vector));
    } else {
        this->set_output_signature(io_signature::make(0, 0, 0));
    }

    // time tag update only supported if data is being output
    d_update_time &= d_output_data;

    // get posix time
    boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
    d_epoch = epoch;

    // set port to output data to

    d_diff_port = d_output_data ? 1 : 0;

    if (d_update_time) {
        // don't propagate tags - all tags will be propagated
        // directly in work function if necessary
        this->set_tag_propagation_policy(gr::block::TPP_DONT);
    } else {
        this->set_tag_propagation_policy(gr::block::TPP_ALL_TO_ALL);
    }
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
    T* out = NULL;
    float* diff_out = NULL;
    if (d_output_data or d_output_diff) {
        out = (T*)output_items[0];
        diff_out = (float*)output_items[1];
    }


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
            if (pmt::eq(PMTCONSTSTR__WALL_CLOCK_TIME, tags[ii].key)) {
                double diff = t_now - pmt::to_double(tags[ii].value);
                std::cout << boost::format("diff = %0.9f s") % diff << std::endl;

                // update tag if requested
                if (d_update_time) {
                    this->add_item_tag(0,
                                       tags[ii].offset,
                                       PMTCONSTSTR__WALL_CLOCK_TIME,
                                       pmt::from_double(t_now));
                }

                // output time
                if (d_output_diff) {
                    diff_out[ntime_tags++] = (float)diff;
                }
            } else {
                // add tag back if updating time
                if (d_update_time) {
                    this->add_item_tag(0, tags[ii].offset, tags[ii].key, tags[ii].value);
                }
            }
        }
        // update number of items produced
        this->produce(1, ntime_tags);
    } /* end if tags.size() */

    // then copy all of the items to the output
    if (d_output_data) {
        memcpy((void*)out, (void*)in, sizeof(T) * nitems);
        //         std::cout << "calling produce for channel 0: " << noutput_items <<
        //         std::endl;
        this->produce(0, nitems);
    }

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
