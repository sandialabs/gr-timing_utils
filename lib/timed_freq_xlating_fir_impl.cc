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

#include "timed_freq_xlating_fir_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include <stdexcept>
#include <boost/format.hpp>

namespace gr {
namespace timing_utils {

template <class I, class O, class T>
typename timed_freq_xlating_fir<I, O, T>::sptr
timed_freq_xlating_fir<I, O, T>::make(int decimation,
                                      const std::vector<T>& taps,
                                      double center_freq,
                                      double sampling_freq,
                                      std::string tag_key)
{
    return gnuradio::make_block_sptr<timed_freq_xlating_fir_impl<I, O, T>>(
        decimation, taps, center_freq, sampling_freq, tag_key);
}

template <class I, class O, class T>
timed_freq_xlating_fir_impl<I, O, T>::timed_freq_xlating_fir_impl(
    int decimation,
    const std::vector<T>& taps,
    double center_freq,
    double sampling_freq,
    std::string tag_key)
    : sync_decimator("timed_freq_xlating_fir",
                     io_signature::make(1, 1, sizeof(I)),
                     io_signature::make(1, 1, sizeof(O)),
                     decimation),
      d_center_freq(center_freq),
      d_sampling_freq(sampling_freq),
      d_updated(false),
      d_decim(decimation),
      d_proto_taps(taps),
      d_tag_key(tag_key),
      d_phase(0.0),
      d_tag_freq_applied(false),
      d_phase_updated(false)
{
    // fir filter - output is always complex so taps for the fir are always complex
    // even though specified taps can be of a different type
    std::vector<gr_complex> dummy_taps;
    d_composite_fir = new filter::kernel::fir_filter<I, O, gr_complex>(dummy_taps);

    // set taps
    set_taps(taps);


    // set history size to be the maximum number of supported taps
    // as the buffer can not be resized during operation
    this->set_history(d_proto_taps.size());
    build_composite_fir();

    // ensure tags are propagated properly
    assert(d_proto_taps.size() != 0);
    this->declare_sample_delay((d_proto_taps.size() - 1) / 2);

    // set tag pmt
    d_tag_pmt = pmt::string_to_symbol(d_tag_key);

    // ensure update not marked till a tag is actually received
    d_updated = false;

    this->message_port_register_in(PMTCONSTSTR__freq());
    this->set_msg_handler(PMTCONSTSTR__freq(),
                          [this](pmt::pmt_t msg) { this->handle_set_center_freq(msg); });

}

template <class I, class O, class T>
timed_freq_xlating_fir_impl<I, O, T>::~timed_freq_xlating_fir_impl()
{
    delete d_composite_fir;
}

template <class I, class O, class T>
void timed_freq_xlating_fir_impl<I, O, T>::build_composite_fir()
{
    // set the decimation for the block scheduler
    this->set_decimation(d_decim);

    // std::cout << "Generating taps for filter using proto taps of length " <<
    // d_proto_taps.size() << std::endl;
    d_ctaps.resize(d_proto_taps.size());
    // std::vector<gr_complex> ctaps(d_proto_taps.size());

    // The basic principle of this block is to perform:
    //    x(t) -> (mult by -fwT0) -> LPF -> decim -> y(t)
    // We switch things up here to:
    //    x(t) -> BPF -> decim -> (mult by fwT0*decim) -> y(t)
    // The BPF is the baseband filter (LPF) moved up to the
    // center frequency fwT0. We then apply a derotator
    // with -fwT0 to downshift the signal to baseband.

    float fwT0 = 2 * M_PI * d_center_freq / d_sampling_freq;
    for (unsigned int i = 0; i < d_proto_taps.size(); i++) {
        d_ctaps[i] = d_proto_taps[i] * exp(gr_complex(0, i * fwT0));
    }

    d_composite_fir->set_taps(d_ctaps);
    if (d_phase_updated) {
        d_r.set_phase(exp(gr_complex(0, -1.0 * d_phase)));
        d_phase_updated = false;
    }
    d_r.set_phase_incr(exp(gr_complex(0, -fwT0 * this->decimation())));
}

template <class I, class O, class T>
void timed_freq_xlating_fir_impl<I, O, T>::set_decim(int decimation)
{
    gr::thread::scoped_lock l(this->d_setlock);
    if (decimation > 0) {
        d_decim = decimation;
        d_updated = true;

        // update offsets
        d_in_tag_offset = this->nitems_read(0);
        d_out_tag_offset = this->nitems_written(0);
    }
}

template <class I, class O, class T>
int timed_freq_xlating_fir_impl<I, O, T>::decim() const
{
    return d_decim;
}

template <class I, class O, class T>
void timed_freq_xlating_fir_impl<I, O, T>::set_rate(double rate)
{
    gr::thread::scoped_lock l(this->d_setlock);
    if (rate > 0) {
        d_sampling_freq = rate;
        d_updated = true;
    }
}

template <class I, class O, class T>
double timed_freq_xlating_fir_impl<I, O, T>::rate() const
{
    return d_sampling_freq;
}

template <class I, class O, class T>
void timed_freq_xlating_fir_impl<I, O, T>::set_center_freq(double center_freq,
                                                           double phase)
{
    gr::thread::scoped_lock l(this->d_setlock);
    set_center_freq_(center_freq, phase);
}

template <class I, class O, class T>
void timed_freq_xlating_fir_impl<I, O, T>::set_center_freq_(double center_freq,
                                                            double phase)
{
    d_center_freq = center_freq;
    d_phase = phase;
    d_updated = true;
}

template <class I, class O, class T>
double timed_freq_xlating_fir_impl<I, O, T>::center_freq() const
{
    return d_center_freq;
}

template <class I, class O, class T>
void timed_freq_xlating_fir_impl<I, O, T>::set_taps(const std::vector<T>& taps)
{
    assert(taps.size() != 0);
    d_proto_taps = taps;
    d_updated = true;
}

template <class I, class O, class T>
std::vector<T> timed_freq_xlating_fir_impl<I, O, T>::taps() const
{
    return d_proto_taps;
}

template <class I, class O, class T>
void timed_freq_xlating_fir_impl<I, O, T>::handle_set_center_freq(pmt::pmt_t msg)
{
    if (pmt::is_dict(msg) && pmt::dict_has_key(msg, PMTCONSTSTR__freq())) {
        pmt::pmt_t x = pmt::dict_ref(msg, PMTCONSTSTR__freq(), pmt::PMT_NIL);
        if (pmt::is_real(x)) {
            double freq = pmt::to_double(x);
            set_center_freq_(freq);
        } else if (pmt::is_pair(x)) {
            // specifying frequency and phase - mark phase as updated so that
            // no discontinuities are introduced if phase is left unspecified
            double freq = pmt::to_double(pmt::car(x));
            double phase = pmt::to_double(pmt::cdr(x));
            d_phase_updated = true;
            set_center_freq_(freq, phase);
        }
    } else if (pmt::is_pair(msg)) {
        pmt::pmt_t x = pmt::cdr(msg);
        if (pmt::is_real(x)) {
            double freq = pmt::to_double(x);
            set_center_freq_(freq);
        }
    }
}

template <class I, class O, class T>
void timed_freq_xlating_fir_impl<I, O, T>::scale(std::vector<gr_complex>& output,
                                                 gr_complex* input)
{
    volk_32fc_s32fc_multiply_32fc(&output[0], &input[0], d_ctaps[0], output.size());
}


template <class I, class O, class T>
void timed_freq_xlating_fir_impl<I, O, T>::scale(std::vector<gr_complex>& output,
                                                 float* input)
{
    std::vector<float> real(output.size()), imag(output.size());
    volk_32f_s32f_multiply_32f(&real[0], &input[0], d_ctaps[0].real(), output.size());
    volk_32f_s32f_multiply_32f(&imag[0], &input[0], d_ctaps[0].imag(), output.size());
    volk_32f_x2_interleave_32fc(&output[0], &real[0], &imag[0], output.size());

    return;
}

template <class I, class O, class T>
void timed_freq_xlating_fir_impl<I, O, T>::scale(std::vector<gr_complex>& output,
                                                 short* input)
{
    // no volk kernel so do by hand
    // TODO: could use volk_16i_s32f_convert_32f on real/imag portions separately
    for (size_t i = 0; i < output.size(); ++i) {
        output[i] = static_cast<gr_complex>(input[i]) * d_ctaps[0];
    }
}

template <class I, class O, class T>
int timed_freq_xlating_fir_impl<I, O, T>::work(int noutput_items,
                                               gr_vector_const_void_star& input_items,
                                               gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock l(this->d_setlock);

    I* in = (I*)input_items[0];
    O* out = (O*)output_items[0];

    std::vector<tag_t> tags;
    std::vector<tag_t> all_tags;
    uint64_t a_start = this->nitems_read(0);
    uint64_t a_end = a_start + noutput_items * d_decim;
    int consumed = noutput_items * d_decim;
    int produced = noutput_items;

    // if we have received a tag to change the frequency, we will consume
    // only up until the tag and change the freq on the next work
    // function call
    this->get_tags_in_range(tags, 0, a_start, a_end, d_tag_pmt);

    if (tags.size()) {
        // only update the frequency if the first sample is tagged
        if (tags[0].offset == a_start) {
            if (not d_tag_freq_applied) {
                if (pmt::is_pair(tags[0].value)) {
                    // inform the block to retune the next time it is run
                    double new_freq = pmt::to_double(pmt::car(tags[0].value));
                    double new_phase = pmt::to_double(pmt::cdr(tags[0].value));
                    handle_set_center_freq(pmt::dict_add(
                        pmt::make_dict(), PMTCONSTSTR__freq(), tags[0].value));
                    GR_LOG_INFO(
                        this->d_logger,
                        str(boost::format("Synchronously setting freq xlator to %f Hz with a "
                                      "phase of %f radians at sample %d") %
                            new_freq % new_phase % tags[0].offset));
                    d_tag_freq_applied = true;
                } else if (pmt::is_real(tags[0].value)) {
                    double new_freq = pmt::to_double(tags[0].value);
                    if (new_freq != d_center_freq) {
                        // inform the block to retune the next time it is run
                        handle_set_center_freq(pmt::dict_add(
                            pmt::make_dict(), PMTCONSTSTR__freq(), tags[0].value));
                        GR_LOG_INFO(
                            this->d_logger,
                            str(boost::format(
                                "Synchronously setting freq xlator to %f at sample %d") %
                                new_freq % tags[0].offset));
                    }
                    d_tag_freq_applied = true;
                } else {
                    GR_LOG_ERROR(this->d_logger, "Invalid frequency tag type");
                }
            } else {
                // tag frequency was applied on previous iteration so mark unapplied
                // for next tag to be processed
                d_tag_freq_applied = false;
            } /* end not d_tag_freq_applied */

            // potentially consume up to the next tagged sample if the
            // frequency was not updated
            if (tags.size() > 1) {
                consumed = tags[1].offset - a_start;
                produced = ceil(1.0 * consumed / d_decim);
            }
        } else {
            // consume up to (but not including) the first tag so that the
            // frequency update will be applied to the tagged sample on the
            // next work function call
            consumed = tags[0].offset - a_start;
            produced = ceil(1.0 * consumed / d_decim);
        } /* end tags[0].offset = a_start */
    }     /* end tags.size() */

    // rebuild composite FIR if the center freq has changed
    if (d_updated) {
        this->set_history(d_proto_taps.size());
        this->declare_sample_delay((d_proto_taps.size() - 1) / 2);
        build_composite_fir();
        d_updated = false;

        // Tell downstream items where the frequency change was applied
        // but only when the change was not applied with an existing tag
        if (not d_tag_freq_applied) {
            this->add_item_tag(0,
                               this->nitems_written(0),
                               PMTCONSTSTR__freq(),
                               pmt::from_double(d_center_freq),
                               this->alias_pmt());
        }
        return 0; // history requirements may have changed.
    }

    // the magic
    std::vector<O> tmp(produced);
    unsigned decimation = this->decimation();
    if (d_ctaps.size() > 1) {
        // any taps at all, we must filter
        unsigned j = 0;
        for (int i = 0; i < produced; i++) {
            // out[i] = d_r.rotate(d_composite_fir->filter(&in[j]));
            tmp[i] = d_composite_fir->filter(&in[j]);
            j += decimation;
        }
    } else {
        if (decimation == 1) {
            // scale only required
            this->scale(tmp, in);
        } else {
            // decimate then scale
            std::vector<O> scaled(produced);
            unsigned j = 0;
            for (int i = 0; i < produced; ++i) {
                tmp[i] = static_cast<O>(in[j]);
                j += decimation;
            }
            this->scale(tmp, &scaled[0]);
        }
    }

    // only supported output type is gr_complex so this is safe
    d_r.rotateN(out, (const gr_complex*)&tmp[0], produced);

    return produced;
}

template class timed_freq_xlating_fir<gr_complex, gr_complex, gr_complex>;
template class timed_freq_xlating_fir<gr_complex, gr_complex, float>;
template class timed_freq_xlating_fir<float, gr_complex, gr_complex>;
template class timed_freq_xlating_fir<float, gr_complex, float>;
template class timed_freq_xlating_fir<short, gr_complex, gr_complex>;
template class timed_freq_xlating_fir<short, gr_complex, float>;
} /* namespace timing_utils */
} /* namespace gr */
