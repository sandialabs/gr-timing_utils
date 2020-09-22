/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cppunit/TextTestRunner.h>
#include <cppunit/XmlOutputter.h>

#include "qa_timing_utils.h"
#include <gnuradio/unittests.h>
#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{
    CppUnit::TextTestRunner runner;
    std::ofstream xmlfile(get_unittest_path("timing_utils.xml").c_str());
    CppUnit::XmlOutputter* xmlout = new CppUnit::XmlOutputter(&runner.result(), xmlfile);

    runner.addTest(qa_timing_utils::suite());
    runner.setOutputter(xmlout);

    bool was_successful = runner.run("", false);

    return was_successful ? 0 : 1;
}
