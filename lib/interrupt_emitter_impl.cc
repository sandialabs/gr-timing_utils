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

#include "interrupt_emitter_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/timing_utils/constants.h>
#include <cmath>
#include <boost/format.hpp>

namespace gr {
namespace timing_utils {

template <class T>
typename interrupt_emitter<T>::sptr
interrupt_emitter<T>::make(double rate, bool drop_late, double loop_gain)
{
    return gnuradio::make_block_sptr<interrupt_emitter_impl<T>>(rate, drop_late, loop_gain);
}

/*
 * The private constructor
 */
template <class T>
interrupt_emitter_impl<T>::interrupt_emitter_impl(double rate,
                                                  bool drop_late,
                                                  double loop_gain)
    : gr::sync_block("interrupt_emitter",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(0, 0, 0)),
      reference_timer(),
      d_rate(rate),
      d_drop_late(drop_late),
      d_gain(loop_gain)
{
    this->message_port_register_out(PMTCONSTSTR__trig());
    this->message_port_register_in(PMTCONSTSTR__set());

    this->set_msg_handler(PMTCONSTSTR__set(),
                          [this](pmt::pmt_t msg) { this->handle_set_time(msg); });
    boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
    d_epoch = epoch;
    timer_thread = new boost::thread(boost::bind(&boost::asio::io_service::run, &io));
    d_last_tag_time = 0; // time,sample starts at 0,0 unless we get an rx_time tag
    d_last_tag_samp = 0;
    debug = false;
}

template <class T>
bool interrupt_emitter_impl<T>::start()
{
    d_time_offset = (boost::get_system_time() - epoch).total_microseconds() * 1e-6;
    return true;
}

/*
 * Our virtual destructor.
 */
template <class T>
interrupt_emitter_impl<T>::~interrupt_emitter_impl()
{
    delete timer_thread;
}

template <class T>
pmt::pmt_t interrupt_emitter_impl<T>::samples_to_tpmt(uint64_t trigger_sample)
{
    // reference this sample to the end of the last buffer processed.
    double time =
        (trigger_sample + int64_t((d_start_time * d_rate) - d_start_sample)) / d_rate;
    if (time < 0) {
        // this is an unlikely edge case where sample/WCT has experienced sudden slew,
        // catch it set the time to the minimum representable value, zero
        this->d_logger->debug(
            "Late sampled-based interrupt request received...setting time to minimum representable value");
        time = 0;
    }
    uint64_t t_int = uint64_t(time);
    double t_frac = time - t_int;
    if (t_frac > 1) {
        t_int += 1;
        t_frac -= 1.0;
    }

    return pmt::cons(pmt::from_uint64(t_int), pmt::from_double(t_frac));
}

template <class T>
uint64_t interrupt_emitter_impl<T>::time_to_samples(double time)
{
    uint64_t sample;
    if (time < d_start_time) {
        this->d_logger->debug(
            "Sample time precedes last sample processed...setting sample index to last sample processed");
        sample = d_start_sample;
    } else {
        sample = (time - d_start_time) * d_rate + d_start_sample;
    }

    return sample;
}

template <class T>
void interrupt_emitter_impl<T>::handle_set_time(pmt::pmt_t time_pmt)
{
    if (debug)
        std::cout << "Received msg: " << time_pmt << std::endl;

    uint64_t trigger_sample;
    pmt::pmt_t trigger_time = pmt::PMT_NIL;
    uint64_t t_int;
    double t_frac;
    if (pmt::is_uint64(time_pmt)) {
        // if the tuple is a single uint64_t it is the sample to trigger
        trigger_sample = pmt::to_uint64(time_pmt);
        trigger_time = samples_to_tpmt(trigger_sample);
        t_int = pmt::to_uint64(pmt::car(trigger_time));
        t_frac = pmt::to_double(pmt::cdr(trigger_time));
    } else if (pmt::is_pair(time_pmt)) {
        // if it is a pair, check the car and cdr for correct types
        if (pmt::is_uint64(pmt::car(time_pmt)) & pmt::is_real(pmt::cdr(time_pmt))) {
            trigger_time = time_pmt;
            t_int = pmt::to_uint64(pmt::car(time_pmt));
            t_frac = pmt::to_double(pmt::cdr(time_pmt));
            trigger_sample = time_to_samples(t_int + t_frac);
        } else {
            // for anything else, ignore
            return;
        }
    } else if (pmt::is_tuple(time_pmt)) {
        if (pmt::is_uint64(pmt::tuple_ref(time_pmt, 0)) &
            pmt::is_real(pmt::tuple_ref(time_pmt, 1))) {
            t_int = pmt::to_uint64(pmt::tuple_ref(time_pmt, 0));
            t_frac = pmt::to_double(pmt::tuple_ref(time_pmt, 1));
            trigger_time =
                pmt::cons(pmt::tuple_ref(time_pmt, 0), pmt::tuple_ref(time_pmt, 1));
            trigger_sample = time_to_samples(t_int + t_frac);
        } else {
            return;
        }
    } else
        return;

    pmt::pmt_t pmt_out = pmt::make_dict();
    pmt_out = pmt::dict_add(pmt_out, PMTCONSTSTR__trigger_time(), trigger_time);
    pmt_out = pmt::dict_add(
        pmt_out, PMTCONSTSTR__trigger_sample(), pmt::from_uint64(trigger_sample));
    pmt_out = pmt::dict_add(pmt_out, PMTCONSTSTR__late_delta(), pmt::from_double(0));

    double current_time((boost::get_system_time() - epoch).total_microseconds() /
                            1000000.0 -
                        d_time_offset);
    double wait_time = t_int + t_frac - current_time;

    if (wait_time < 0) {
        if (d_drop_late) {
            this->d_logger->debug("Dropping late interrupt request: {:e}",
                             wait_time);
        } else {
            // Interrupt right now
            if (debug)
                std::cout << "Sending late message\n";
            // Modify the time that goes out to be right now.  Since this is a real time
            // interrupt system bad things can happen if we go off of the sample time
            // rather than clock time.
            pmt_out = pmt::dict_add(
                pmt_out, PMTCONSTSTR__late_delta(), pmt::from_double((wait_time * -1.0)));
            io.dispatch(
                boost::bind(&interrupt_emitter_impl<T>::StartTimer, this, 0, pmt_out));
        }
    } else {
        if (debug)
            std::cout << "arming interrupt\n";
        io.dispatch(boost::bind(
            &interrupt_emitter_impl<T>::StartTimer, this, wait_time, pmt_out));
    }
}

template <class T>
bool interrupt_emitter_impl<T>::stop()
{
    io.dispatch(boost::bind(&interrupt_emitter_impl<T>::StopTimer, this));
    timer_thread->join();

    return true;
}

template <class T>
void interrupt_emitter_impl<T>::process_interrupt()
{
    pmt::pmt_t time_pmt = pmt::dict_ref(d_out_pmt, PMTCONSTSTR__trigger_time(), pmt::PMT_NIL);
    double int_time =
        pmt::to_uint64(pmt::car(time_pmt)) + pmt::to_double(pmt::cdr(time_pmt));
    d_out_pmt = pmt::dict_add(
        d_out_pmt, PMTCONSTSTR__trigger_sample(), pmt::from_uint64(time_to_samples(int_time)));
    this->message_port_pub(PMTCONSTSTR__trig(), d_out_pmt);
}

template <class T>
int interrupt_emitter_impl<T>::work(int noutput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    // Assume that the current time corresponds with 1 sample after the end of the
    // buffer
    boost::posix_time::ptime current_ptime =
        boost::get_system_time() -
        boost::posix_time::microseconds(static_cast<long>(1e6 * d_time_offset));
    double current_time((current_ptime - epoch).total_microseconds() / 1e6);
    d_start_sample = this->nitems_read(0) + noutput_items;
    d_start_time = current_time;

    //const T* in = (const T*)input_items[0];

    std::vector<tag_t> tags;

    // check for uhd rx_time tags to set baseline time and correct for overflows
    this->get_tags_in_range(tags,
                            0,
                            this->nitems_read(0),
                            (this->nitems_read(0) + noutput_items),
                            PMTCONSTSTR__rx_time());
    if (tags.size()) {
        // Only need to look at the last one
        tag_t last_tag = tags[tags.size() - 1];
        uint64_t t_int = pmt::to_uint64(pmt::tuple_ref(last_tag.value, 0));
        double t_frac = pmt::to_double(pmt::tuple_ref(last_tag.value, 1));
        uint64_t samp = last_tag.offset;

        d_last_tag_time = t_int + t_frac;
        d_last_tag_samp = samp;
        // printf("Tag at time: %f, sample: %ld, offset: %f, time:%f\n", t_int+t_frac,
        // samp, d_time_offset, current_time);
        // What time did we expect to see samp at??
        // Note that we expect to receive the whole buffer at the same time, so we
        // need to reref to the last sample
        double expect_samp =
            current_time - (this->nitems_read(0) + noutput_items - samp) / d_rate;
        double error = expect_samp - t_int - t_frac;
        d_time_offset += error;
        d_start_time -= error;
        current_ptime -= boost::posix_time::microseconds(static_cast<long>(1e6 * error));
        UpdateTimer(error);
        if (debug)
            printf("tag_error = %f\n", error);
    } else {
        // estimate time_now using the last rx_time tag and the samples we have processed since that time.
        double radio_time_est = d_last_tag_time + ((this->nitems_read(0) + noutput_items - d_last_tag_samp) / d_rate);
        double error = current_time - radio_time_est;

        if (std::abs(error) > 200e-6) {
            // Allow for some sample error, because the system clock isn't perfect
            d_time_offset += d_gain * error;
            d_start_time -= d_gain * error;
            current_ptime -=
                boost::posix_time::microseconds(static_cast<long>(1e6 * error));
        }
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

template class interrupt_emitter<unsigned char>;
template class interrupt_emitter<short>;
template class interrupt_emitter<int32_t>;
template class interrupt_emitter<float>;
template class interrupt_emitter<gr_complex>;
} /* namespace timing_utils */
} /* namespace gr */
