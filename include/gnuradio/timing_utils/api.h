/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_API_H
#define INCLUDED_TIMING_UTILS_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_timing_utils_EXPORTS
#define TIMING_UTILS_API __GR_ATTR_EXPORT
#else
#define TIMING_UTILS_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_TIMING_UTILS_API_H */
