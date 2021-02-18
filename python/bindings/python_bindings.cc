/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/pybind11.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

namespace py = pybind11;

// Headers for binding functions
/**************************************/
// The following comment block is used for
// gr_modtool to insert function prototypes
// Please do not delete
/**************************************/
// BINDING_FUNCTION_PROTOTYPES(
void bind_add_usrp_tags(py::module& m);
void bind_constants(py::module& m);
void bind_interrupt_emitter(py::module& m);
void bind_system_time_diff(py::module& m);
void bind_system_time_tagger(py::module& m);
void bind_tag_uhd_offset(py::module& m);
void bind_thresh_trigger_f(py::module& m);
void bind_time_delta(py::module& m);
void bind_timed_freq_xlating_fir(py::module& m);
void bind_timed_tag_retuner(py::module& m);
void bind_uhd_timed_pdu_emitter(py::module& m);
void bind_wall_clock_time(py::module& m);
// ) END BINDING_FUNCTION_PROTOTYPES


// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(timing_utils_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    /**************************************/
    // The following comment block is used for
    // gr_modtool to insert binding function calls
    // Please do not delete
    /**************************************/
    // BINDING_FUNCTION_CALLS(
    bind_add_usrp_tags(m);
    bind_constants(m);
    bind_interrupt_emitter(m);
    bind_system_time_diff(m);
    bind_system_time_tagger(m);
    bind_tag_uhd_offset(m);
    bind_thresh_trigger_f(m);
    bind_time_delta(m);
    bind_timed_freq_xlating_fir(m);
    bind_timed_tag_retuner(m);
    bind_uhd_timed_pdu_emitter(m);
    bind_wall_clock_time(m);

    // ) END BINDING_FUNCTION_CALLS
}
