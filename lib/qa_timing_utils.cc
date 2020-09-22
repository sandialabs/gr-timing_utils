/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/*
 * This class gathers together all the test cases for the gr-filter
 * directory into a single test suite.  As you create new test cases,
 * add them here.
 */

#include "qa_timing_utils.h"

CppUnit::TestSuite* qa_timing_utils::suite()
{
    CppUnit::TestSuite* s = new CppUnit::TestSuite("timing_utils");

    return s;
}
