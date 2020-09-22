/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _QA_TIMING_UTILS_H_
#define _QA_TIMING_UTILS_H_

#include <gnuradio/attributes.h>
#include <cppunit/TestSuite.h>

//! collect all the tests for the gr-filter directory

class __GR_ATTR_EXPORT qa_timing_utils
{
public:
    //! return suite of tests for all of gr-filter directory
    static CppUnit::TestSuite* suite();
};

#endif /* _QA_TIMING_UTILS_H_ */
