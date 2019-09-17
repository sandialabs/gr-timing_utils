#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Validate Tt
# GNU Radio version: 3.7.13.5
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.qtgui import Range, RangeWidget
from optparse import OptionParser
import pdu_utils
import pmt
import sys
import timing_utils
from gnuradio import qtgui


class validate_tt(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Validate Tt")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Validate Tt")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "validate_tt")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())


        ##################################################
        # Variables
        ##################################################
        self.taps = taps = firdes.low_pass_2(1, 1, 1/10.0, 1/40.0, 20)
        self.tti = tti = 10000
        self.tl = tl = len(taps)
        self.t = t = 1
        self.sym_rate = sym_rate = 19200
        self.sps = sps = 4
        self.samp_rate = samp_rate = 19200*8
        self.rf_rate = rf_rate = 153600
        self.l = l = 5000
        self.ati = ati = 10000

        ##################################################
        # Blocks
        ##################################################
        self._t_range = Range(0, 2, 0.001, 1, 200)
        self._t_win = RangeWidget(self._t_range, self.set_t, 'Threshold', "counter_slider", float)
        self.top_grid_layout.addWidget(self._t_win)
        self._l_range = Range(-1, 10000, 1, 5000, 200)
        self._l_win = RangeWidget(self._l_range, self.set_l, 'Length Threshold', "counter_slider", int)
        self.top_grid_layout.addWidget(self._l_win)
        self._tti_range = Range(100, 100000, 100, 10000, 200)
        self._tti_win = RangeWidget(self._tti_range, self.set_tti, 'Trigger tag interval', "counter_slider", int)
        self.top_grid_layout.addWidget(self._tti_win)
        self.timing_utils_thresh_trigger_f_0 = timing_utils.thresh_trigger_f(t, t/10.0, l)
        self.pdu_utils_tag_message_trigger_asdf_0 = pdu_utils.tag_message_trigger_c(pmt.intern("TRIGGER"), pmt.PMT_NIL, pmt.cons(pmt.intern("KABOOOOOOM"), pmt.PMT_NIL), 0, 1e6, 0.0, 0.0, False)
        self.fir_filter_xxx_0_0 = filter.fir_filter_ccc(int(1.0* rf_rate / (sps * sym_rate)), (firdes.low_pass_2(20, rf_rate, 10000, 10000, 60)))
        self.fir_filter_xxx_0_0.declare_sample_delay(0)
        self.fir_filter_xxx_0 = filter.fir_filter_fff(1, (taps))
        self.fir_filter_xxx_0.declare_sample_delay(0)
        self.blocks_throttle_0_0 = blocks.throttle(gr.sizeof_gr_complex*1, samp_rate,True)
        self.blocks_message_debug_1 = blocks.message_debug()
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*1, '/home/jacob/data/bd2016_data/bd_8xosr.dat', True)
        self.blocks_file_source_0.set_begin_tag(pmt.PMT_NIL)
        self.blocks_complex_to_mag_0 = blocks.complex_to_mag(1)
        self._ati_range = Range(100, 100000, 100, 10000, 200)
        self._ati_win = RangeWidget(self._ati_range, self.set_ati, 'Arm tag interval', "counter_slider", int)
        self.top_grid_layout.addWidget(self._ati_win)



        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.pdu_utils_tag_message_trigger_asdf_0, 'msg'), (self.blocks_message_debug_1, 'print'))
        self.connect((self.blocks_complex_to_mag_0, 0), (self.fir_filter_xxx_0, 0))
        self.connect((self.blocks_file_source_0, 0), (self.blocks_throttle_0_0, 0))
        self.connect((self.blocks_throttle_0_0, 0), (self.fir_filter_xxx_0_0, 0))
        self.connect((self.fir_filter_xxx_0, 0), (self.timing_utils_thresh_trigger_f_0, 0))
        self.connect((self.fir_filter_xxx_0_0, 0), (self.blocks_complex_to_mag_0, 0))
        self.connect((self.fir_filter_xxx_0_0, 0), (self.pdu_utils_tag_message_trigger_asdf_0, 0))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "validate_tt")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_taps(self):
        return self.taps

    def set_taps(self, taps):
        self.taps = taps
        self.set_tl(len(self.taps))
        self.fir_filter_xxx_0.set_taps((self.taps))

    def get_tti(self):
        return self.tti

    def set_tti(self, tti):
        self.tti = tti

    def get_tl(self):
        return self.tl

    def set_tl(self, tl):
        self.tl = tl

    def get_t(self):
        return self.t

    def set_t(self, t):
        self.t = t
        self.timing_utils_thresh_trigger_f_0.set_hi(self.t)
        self.timing_utils_thresh_trigger_f_0.set_lo(self.t/10.0)

    def get_sym_rate(self):
        return self.sym_rate

    def set_sym_rate(self, sym_rate):
        self.sym_rate = sym_rate

    def get_sps(self):
        return self.sps

    def set_sps(self, sps):
        self.sps = sps

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle_0_0.set_sample_rate(self.samp_rate)

    def get_rf_rate(self):
        return self.rf_rate

    def set_rf_rate(self, rf_rate):
        self.rf_rate = rf_rate
        self.fir_filter_xxx_0_0.set_taps((firdes.low_pass_2(20, self.rf_rate, 10000, 10000, 60)))

    def get_l(self):
        return self.l

    def set_l(self, l):
        self.l = l
        self.timing_utils_thresh_trigger_f_0.set_len(self.l)

    def get_ati(self):
        return self.ati

    def set_ati(self, ati):
        self.ati = ati


def main(top_block_cls=validate_tt, options=None):

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
