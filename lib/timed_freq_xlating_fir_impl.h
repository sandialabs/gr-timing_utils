/* -*- c++ -*- */
/*
 * Copyright 2018-2021 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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

    // overloaded scaling methods for all input types
    void scale(std::vector<gr_complex>& output, gr_complex* input);
    void scale(std::vector<gr_complex>& output, float* input);
    void scale(std::vector<gr_complex>& output, short* input);


public:
    timed_freq_xlating_fir_impl(int decimation,
                                const std::vector<T>& taps,
                                double center_freq,
                                double sampling_freq,
                                std::string tag_key);
    ~timed_freq_xlating_fir_impl() override;

    void set_decim(int decimation) override;
    int decim() const override;

    void set_rate(double rate) override;
    double rate() const override;

    void set_center_freq(double center_freq) override;
    double center_freq() const override;

    void set_taps(const std::vector<T>& taps) override;
    std::vector<T> taps() const override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_TIMED_FREQ_XLATING_FIR_IMPL_H */
