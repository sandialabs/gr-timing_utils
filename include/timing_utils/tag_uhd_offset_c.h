/* -*- c++ -*- */
/*
 * Copyright 2018 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
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

 /* WARNING: this file is machine generated. Edits will be overwritten */

 #ifndef INCLUDED_TIMING_UTILS_TAG_UHD_OFFSET_C_H
 #define INCLUDED_TIMING_UTILS_TAG_UHD_OFFSET_C_H

#include <timing_utils/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace timing_utils {

    /*!
     * \brief <+description of block+>
     * \ingroup timing_utils
     *
     */
    class TIMING_UTILS_API tag_uhd_offset_c : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<tag_uhd_offset_c> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of timing_utils::tag_uhd_offset_c.
       *
       * To avoid accidental use of raw pointers, timing_utils::tag_uhd_offset_c's
       * constructor is in a private implementation
       * class. timing_utils::tag_uhd_offset_c::make is the public interface for
       * creating new instances.
       */
      static sptr make(float rate, uint32_t tag_interval);

      virtual void set_rate(float rate) = 0;
      virtual void set_interval(uint32_t tag_interval) = 0;
      virtual void set_key(pmt::pmt_t tag_key) = 0;
    };

  } // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_TAG_UHD_OFFSET_C_H */
