/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <pmt/pmt.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <cmath>

#include <iostream>

class reference_timer
{
public:
    reference_timer() : error(0), run(false), loaded(false), index(0)
    {
        boost::posix_time::ptime epoch2(boost::gregorian::date(1970, 1, 1));
        epoch = epoch2;
        io.reset();
        timer = new boost::asio::deadline_timer(io);
        work = boost::shared_ptr<boost::asio::io_service::work>(
            new boost::asio::io_service::work(io));
        debug = false;
    }

    ~reference_timer()
    {
        StopTimer();
        delete timer;
    }

    void UpdateError(double _error)
    {
        boost::lock_guard<boost::mutex> guard(mtx);
        error = _error;
    }

    boost::posix_time::ptime GetEpoch() const { return epoch; }

    void Timer(uint64_t interrupt_index)
    {
        boost::lock_guard<boost::mutex> guard(mtx);
        // It is possible for this function to be called while we are cancelling an
        // interrupt. Need to ensure that we don't process this.
        if (interrupt_index != index) {
            if (debug)
                printf("Skipping handler for old interrupt\n");
            return;
        }

        if (debug) {
            double int_time = ((timer->expires_at() - epoch).total_microseconds()) / 1e6;
            double diffval =
                (boost::get_system_time() - timer->expires_at()).total_microseconds();

            printf("interrupt_time = %f, clock_time = %f, diff = %f\n",
                   int_time,
                   (boost::get_system_time() - epoch).total_microseconds() / 1e6,
                   diffval);
        }

        process_interrupt();
        loaded = false;
    }

    void StartTimer(double wait_time = 0, pmt::pmt_t pmt_data = pmt::PMT_NIL)
    {
        boost::lock_guard<boost::mutex> guard(mtx);
        // Clear out any previous requests
        if (loaded) {
            if (debug)
                printf("Received interrupt before previous one fired\n");
            timer->cancel();
        }
        loaded = true;
        d_out_pmt = pmt_data;
        index++;
        if (debug)
            printf("wait_time before interrupt = %f\n", wait_time);
        timer->expires_from_now(
            boost::posix_time::microseconds(static_cast<long>(1e6 * wait_time)));
        timer->async_wait(boost::bind(&reference_timer::Timer, this, index));
    }

    void UpdateTimer(double update_time)
    {
        boost::lock_guard<boost::mutex> guard(mtx);
        if (loaded) {
            boost::posix_time::ptime expire_time = timer->expires_at();
            timer->cancel();
            index++;
            timer->expires_at(expire_time + boost::posix_time::microseconds(
                                                static_cast<long>(1e6 * update_time)));
            timer->async_wait(boost::bind(&reference_timer::Timer, this, index));
        }
    }

    void StopTimer()
    {
        boost::lock_guard<boost::mutex> guard(mtx);
        run = false;
        work.reset();
    }

protected:
    pmt::pmt_t d_out_pmt;
    double error;
    uint64_t index;
    boost::mutex mtx;
    boost::posix_time::ptime epoch;
    boost::asio::deadline_timer* timer;

    bool run;
    bool loaded;
    bool debug;

    virtual void process_interrupt() = 0;

    boost::asio::io_service io;
    boost::shared_ptr<boost::asio::io_service::work> work;
};
