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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clock_mm_tags_ff_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <stdexcept>

namespace gr {
  namespace timing_utils {

    clock_mm_tags_ff::sptr
    clock_mm_tags_ff::make(float omega, float gain_omega, float mu, float gain_mu, float omega_relative_limit, uint32_t tag_interval)
    {
      return gnuradio::get_initial_sptr
        (new clock_mm_tags_ff_impl(omega, gain_omega, mu, gain_mu, omega_relative_limit, tag_interval));
    }

    /*
     * The private constructor
     */
    clock_mm_tags_ff_impl::clock_mm_tags_ff_impl(float omega, float gain_omega, float mu, float gain_mu, float omega_relative_limit, uint32_t tag_interval)
      : gr::block("clock_mm_tags_ff",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(1, 1, sizeof(float))),
            	d_mu(mu), d_gain_mu(gain_mu), d_gain_omega(gain_omega),
            	d_omega_relative_limit(omega_relative_limit),
            	d_last_sample(0), d_interp(new filter::mmse_fir_interpolator_ff())
    {
      d_tag_output = true;
      d_tag_interval = tag_interval;
      d_tag_counter = -1;
      d_nitems_read_prev = 0;
      d_start_omega = omega;
      if(omega <  1)
	      throw std::out_of_range("clock rate must be > 0");
      if(gain_mu <  0  || gain_omega < 0)
	      throw std::out_of_range("Gains must be non-negative");

      set_omega(omega);			// also sets min and max omega
      set_relative_rate (1.0 / omega);
    }

    /*
     * Our virtual destructor.
     */
    clock_mm_tags_ff_impl::~clock_mm_tags_ff_impl()
    {
    }

    void
    clock_mm_tags_ff_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size();
      for(unsigned i=0; i < ninputs; i++)
	      ninput_items_required[i] =
	        (int) ceil((noutput_items * d_omega) + d_interp->ntaps());
    }

    static inline float
    slice(float x)
    {
      return x < 0 ? -1.0F : 1.0F;
    }

    void
    clock_mm_tags_ff_impl::set_omega (float omega)
    {
      d_omega = omega;
      d_omega_mid = omega;
      d_omega_lim = d_omega_mid * d_omega_relative_limit;
    }

    int
    clock_mm_tags_ff_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const float *in = (const float *)input_items[0];
      float *out = (float *)output_items[0];

      int ii = 0; // input index
      int oo = 0; // output index
      int ni = ninput_items[0] - d_interp->ntaps(); // don't use more input than this
      float mm_val;

      while(oo < noutput_items && ii < ni ) {
        // produce output sample
        out[oo] = d_interp->interpolate(&in[ii], d_mu);
        mm_val = slice(d_last_sample) * out[oo] - slice(out[oo]) * d_last_sample;
        d_last_sample = out[oo];

        d_omega = d_omega + d_gain_omega * mm_val;
        d_omega = d_omega_mid + gr::branchless_clip(d_omega-d_omega_mid, d_omega_lim);
        d_mu = d_mu + d_omega + d_gain_mu * mm_val;

        ii += (int)floor(d_mu);
        d_mu = d_mu - floor(d_mu);
        oo++;
      }

      d_tag_counter -= ii;
      if (d_tag_output && d_tag_counter < 0) {
        d_tag_counter += d_tag_interval;
        add_item_tag(0, nitems_written(0), pmt::intern("mm_n_read"), pmt::from_uint64(d_nitems_read_prev));
        //add_item_tag(0, nitems_written(0), pmt::intern("mm_n_written"), pmt::from_uint64(nitems_written(0)));
        add_item_tag(0, nitems_written(0), pmt::intern("mm_omega"), pmt::from_double(d_omega));
        add_item_tag(0, nitems_written(0), pmt::intern("mm_osr"), pmt::from_double(d_start_omega));
        //std::cout << d_nitems_read_prev << "\t" << nitems_written(0) << "\t" << d_omega << std::endl;
      } else {
        //std::cout << d_nitems_read_prev << "\t" << nitems_written(0) << "\t" << d_omega << std::endl;
        ;
      }

      consume_each(ii);
      d_nitems_read_prev = nitems_read(0);
      return oo;
    }

  } /* namespace timing_utils */
} /* namespace gr */
