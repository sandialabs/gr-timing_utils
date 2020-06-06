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

#ifndef INCLUDED_TIMING_UTILS_TIMED_FREQ_XLATING_FIR_IMPL_H
#define INCLUDED_TIMING_UTILS_TIMED_FREQ_XLATING_FIR_IMPL_H

#define MAX_NUM_TAPS 2048

#include <gnuradio/blocks/rotator.h>
#include <gnuradio/filter/fir_filter.h>
#include <timing_utils/api.h>
#include <timing_utils/constants.h>
#include <timing_utils/timed_freq_xlating_fir.h>
#include <string>

namespace gr {
namespace timing_utils {

template <class I, class O, class T>
class timed_freq_xlating_fir_impl : public timed_freq_xlating_fir<I, O, T>
{
protected:
    std::vector<T> d_proto_taps;
    std::vector<gr_complex> d_ctaps;
    filter::kernel::fir_filter<I, O, gr_complex>* d_composite_fir;
    blocks::rotator d_r;
    double d_center_freq;
    double d_sampling_freq;
    bool d_updated;
    uint32_t d_decim;
    std::string d_tag_key;
    pmt::pmt_t d_tag_pmt;

    // tag propagation offsets
    uint64_t d_in_tag_offset;
    uint64_t d_out_tag_offset;

    virtual void build_composite_fir();
    void handle_set_center_freq(pmt::pmt_t msg);

    // internal setters
    void set_taps_(const std::vector<T>& taps);
    void set_center_freq_(double center_freq);

public:
    timed_freq_xlating_fir_impl(int decimation,
                                const std::vector<T>& taps,
                                double center_freq,
                                double sampling_freq,
                                std::string tag_key);
    ~timed_freq_xlating_fir_impl();

    void set_decim(int decimation);
    int decim() const;

    void set_rate(double rate);
    double rate() const;

    void set_center_freq(double center_freq);
    double center_freq() const;

    void set_taps(const std::vector<T>& taps);
    std::vector<T> taps() const;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_TIMED_FREQ_XLATING_FIR_IMPL_H */
