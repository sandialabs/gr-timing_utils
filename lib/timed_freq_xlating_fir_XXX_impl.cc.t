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


/* @WARNING@ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "@IMPL_NAME@.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include <stdexcept>

namespace gr {
  namespace timing_utils {

    @BASE_NAME@::sptr
    @BASE_NAME@::make(int decimation,
		      const std::vector<@TAP_TYPE@> &taps,
		      double center_freq,
		      double sampling_freq,
                      std::string tag_key
                     )
    {
      return gnuradio::get_initial_sptr(new @IMPL_NAME@
					(decimation, taps,
					 center_freq,
					 sampling_freq,
           tag_key
           ));
    }

    @IMPL_NAME@::@IMPL_NAME@(int decimation,
			     const std::vector<@TAP_TYPE@> &taps,
			     double center_freq,
			     double sampling_freq,
                             std::string tag_key
                            )
    : sync_decimator("@BASE_NAME@",
			io_signature::make(1, 1, sizeof(@I_TYPE@)),
			io_signature::make(1, 1, sizeof(@O_TYPE@)),
			decimation),
      d_center_freq(center_freq),
      d_sampling_freq(sampling_freq),
      d_updated(false),
      d_decim(decimation),
      d_tag_key(tag_key)
    {
      // fir filter
      std::vector<gr_complex> dummy_taps;
      d_composite_fir = new filter::kernel::@CFIR_TYPE@(d_decim, dummy_taps);

      // set taps
      set_taps_(taps);

      // set history size to be the maximum number of supported taps
      // as the buffer can not be resized during operation
      set_history(MAX_NUM_TAPS);
      build_composite_fir();

      // set tag pmt
      d_tag_pmt = pmt::string_to_symbol(d_tag_key);

      message_port_register_in(PMTCONSTSTR__FREQ);
      set_msg_handler(PMTCONSTSTR__FREQ,
                      boost::bind(&@IMPL_NAME@::handle_set_center_freq,
                                  this, _1));

      // manually propagate tags
      set_tag_propagation_policy(TPP_DONT);
      d_in_tag_offset = 0;
      d_out_tag_offset = 0;

    }

    @IMPL_NAME@::~@IMPL_NAME@()
    {
      delete d_composite_fir;
    }

    void
    @IMPL_NAME@::build_composite_fir()
    {
      // set the decimation for the block scheduler
      this->set_decimation(d_decim);

      // std::cout << "Generating taps for filter using proto taps of length " << d_proto_taps.size() << std::endl;
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
      for(unsigned int i = 0; i < d_proto_taps.size(); i++) {
	       d_ctaps[i] = d_proto_taps[i] * exp(gr_complex(0, i * fwT0));
      }

      d_composite_fir->set_taps(d_ctaps);
      d_r.set_phase_incr(exp(gr_complex(0, -fwT0 * decimation())));
    }

    void
    @IMPL_NAME@::set_decim(int decimation)
    {
      gr::thread::scoped_lock l(d_setlock);
      if (decimation > 0) {
        d_decim = decimation;
        d_updated = true;

        // update offsets
        d_in_tag_offset = nitems_read(0);
        d_out_tag_offset = nitems_written(0);
      }
    }

    int
    @IMPL_NAME@::decim() const
    {
      return d_decim;
    }

    void
    @IMPL_NAME@::set_rate(double rate)
    {
      gr::thread::scoped_lock l(d_setlock);
      if (rate > 0) {
        d_sampling_freq = rate;
        d_updated = true;
      }
    }

    double
    @IMPL_NAME@::rate() const
    {
      return d_sampling_freq;
    }

    void
    @IMPL_NAME@::set_center_freq(double center_freq)
    {
      gr::thread::scoped_lock l(d_setlock);
      set_center_freq_(center_freq);
    }

    void
    @IMPL_NAME@::set_center_freq_(double center_freq)
    {
      d_center_freq = center_freq;
      d_updated = true;
    }

    double
    @IMPL_NAME@::center_freq() const
    {
      return d_center_freq;
    }

    void
    @IMPL_NAME@::set_taps(const std::vector<@TAP_TYPE@> &taps)
    {
      gr::thread::scoped_lock l(d_setlock);
      set_taps_(taps);
    }

    void
    @IMPL_NAME@::set_taps_(const std::vector<@TAP_TYPE@> &taps)
    {
      if (taps.size() <= MAX_NUM_TAPS) {
        d_proto_taps = taps;
        d_updated = true;
      }
      else {
        throw std::out_of_range(str(
          boost::format("@IMPL_NAME@: invalid number of taps.  cannot exceed %d") % MAX_NUM_TAPS));
      }
    }

    std::vector<@TAP_TYPE@>
    @IMPL_NAME@::taps() const
    {
      return d_proto_taps;
    }

    void
    @IMPL_NAME@::handle_set_center_freq(pmt::pmt_t msg)
    {
      if(pmt::is_dict(msg) && pmt::dict_has_key(msg, PMTCONSTSTR__FREQ) ) {
        pmt::pmt_t x = pmt::dict_ref(msg, PMTCONSTSTR__FREQ, pmt::PMT_NIL );
        if(pmt::is_real(x)) {
          double freq = pmt::to_double(x);
          set_center_freq_(freq);
        }
      } else if(pmt::is_pair(msg)) {
        pmt::pmt_t x = pmt::cdr(msg);
        if(pmt::is_real(x)) {
          double freq = pmt::to_double(x);
          set_center_freq_(freq);
        }
      }
    }

    int
    @IMPL_NAME@::work(int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock l(d_setlock);

      @I_TYPE@ *in  = (@I_TYPE@ *)input_items[0];
      @O_TYPE@ *out = (@O_TYPE@ *)output_items[0];

      std::vector<tag_t> tags;
      std::vector<tag_t> all_tags;
      uint64_t a_start = nitems_read(0);
      uint64_t a_end = a_start + noutput_items * d_decim;
      int consumed = noutput_items * d_decim;
      int produced = noutput_items;

      // rebuild composite FIR if the center freq has changed
      if(d_updated) {
        build_composite_fir();
        d_updated = false;

        // Tell downstream items where the frequency change was applied
        add_item_tag(0, nitems_written(0),
                     PMTCONSTSTR__FREQ, pmt::from_double(d_center_freq),
                     alias_pmt());
        return 0;		     // history requirements may have changed.
      }

      // if we have received a tag to change the frequency, we will consume
      // only up until the tag and change the freq
      get_tags_in_range(tags, 0, a_start, a_end, d_tag_pmt);

      if (tags.size()) {
        // after the first tag, we will consume up to (and including) the tag
        consumed = tags[0].offset - a_start + 1;
        produced = ceil(1.0 * consumed / d_decim);
        // std::cout << "consuming " << consumed << " items, producing " << produced << " items" << ", noutput_items = " << noutput_items << std::endl;
        double new_freq = pmt::to_double(tags[0].value);
        if ((pmt::eq(tags[0].key,d_tag_pmt)) and (new_freq != d_center_freq)) {
          // and inform the block to retune the next time it is run
          handle_set_center_freq(pmt::cons(PMTCONSTSTR__FREQ, pmt::from_double(new_freq)));
          GR_LOG_INFO(d_logger, boost::format("Synchronously setting freq xlator to %f at sample %d") % new_freq % tags[0].offset);
        }
      }

      // propagate all tags for samples consumed
      get_tags_in_range(all_tags, 0, a_start, a_end);
      if (all_tags.size()) {
        // propagate all tags up to and including consumed tag
        size_t ind = 0;
        for (size_t ind = 0; ind < all_tags.size(); ++ind) {
          if (all_tags[ind].offset > a_start + (uint64_t)consumed) { break; }

          uint64_t offset = (all_tags[ind].offset - d_in_tag_offset) / (uint64_t) d_decim + d_out_tag_offset;
          add_item_tag(0,offset,all_tags[ind].key,all_tags[ind].value,all_tags[ind].srcid);
        }
      }

      // the magic
      std::vector<@O_TYPE@> tmp(produced);
      unsigned j = 0;
      for (int i = 0; i < produced; i++) {
        //out[i] = d_r.rotate(d_composite_fir->filter(&in[j]));
        tmp[i] = d_composite_fir->filter(&in[j]);
        j += decimation();
      }
      d_r.rotateN(out, (const gr_complex *)&tmp[0], produced);

      return produced;
    }

  } /* namespace timing_utils */
} /* namespace gr */
