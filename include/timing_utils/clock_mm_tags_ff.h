/* -*- c++ -*- */
/*
 * <COPYRIGHT PLACEHOLDER>
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


#ifndef INCLUDED_TIMING_UTILS_CLOCK_MM_TAGS_FF_H
#define INCLUDED_TIMING_UTILS_CLOCK_MM_TAGS_FF_H

#include <timing_utils/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace timing_utils {

    /*!
     * \brief <+description of block+>
     * \ingroup timing_utils
     *
     */
    class TIMING_UTILS_API clock_mm_tags_ff : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<clock_mm_tags_ff> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of timing_utils::clock_mm_tags_ff.
       *
       * To avoid accidental use of raw pointers, timing_utils::clock_mm_tags_ff's
       * constructor is in a private implementation
       * class. timing_utils::clock_mm_tags_ff::make is the public interface for
       * creating new instances.
       */
      static sptr make(float omega, float gain_omega, float mu, float gain_mu, float omega_relative_limit, uint32_t tag_interval);


      virtual float mu() const = 0;
      virtual float omega() const = 0;
      virtual float gain_mu() const = 0;
      virtual float gain_omega() const = 0;

      virtual void set_verbose(bool verbose) = 0;
      virtual void set_gain_mu (float gain_mu) = 0;
      virtual void set_gain_omega (float gain_omega) = 0;
      virtual void set_mu (float mu) = 0;
      virtual void set_omega (float omega) = 0;

    };

  } // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_CLOCK_MM_TAGS_FF_H */
