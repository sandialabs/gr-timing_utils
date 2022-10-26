/* -*- c++ -*- */
/*
 * Copyright 2018-2021 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_THRESH_TRIGGER_F_H
#define INCLUDED_TIMING_UTILS_THRESH_TRIGGER_F_H

#include <gnuradio/sync_block.h>
#include <gnuradio/timing_utils/api.h>

namespace gr {
namespace timing_utils {

/*!
 * \brief <+description of block+>
 * \ingroup timing_utils
 *
 */
class TIMING_UTILS_API thresh_trigger_f : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<thresh_trigger_f> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of timing_utils::thresh_trigger_f.
     *
     * @param hi -
     * @param lo -
     * @param length -
     */
    static sptr make(float hi, float lo, int length);

    /**
     *
     * @param t -
     */
    virtual void set_hi(float t) = 0;

    /**
     *
     * @param t -
     */
    virtual void set_lo(float t) = 0;

    /**
     *
     * @param len -
     */
    virtual void set_len(int len) = 0;
};

} // namespace timing_utils
  // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_THRESH_TRIGGER_F_H */
