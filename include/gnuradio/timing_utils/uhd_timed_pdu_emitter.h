/* -*- c++ -*- */
/*
 * Copyright 2018-2021 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_UHD_TIMED_PDU_EMITTER_H
#define INCLUDED_TIMING_UTILS_UHD_TIMED_PDU_EMITTER_H

#include <gnuradio/sync_block.h>
#include <gnuradio/timing_utils/api.h>

namespace gr {
namespace timing_utils {

/*!
 * \brief <+description of block+>
 * \ingroup timing_utils
 *
 */
class TIMING_UTILS_API uhd_timed_pdu_emitter : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<uhd_timed_pdu_emitter> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of
     * timing_utils::uhd_timed_pdu_emitter.
     *
     * @param rate -
     * @param drop_late -
     */
    static sptr make(float rate, bool drop_late);

    /**
     *
     * @param rate -
     */
    virtual void set_rate(float rate) = 0;
};

} // namespace timing_utils
  // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_UHD_TIMED_PDU_EMITTER_H */
