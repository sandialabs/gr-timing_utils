/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(uhd_timed_pdu_emitter.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(6832c33ed8fd897ba75bbd641ca6e7af)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/timing_utils/uhd_timed_pdu_emitter.h>
// pydoc.h is automatically generated in the build directory
#include <uhd_timed_pdu_emitter_pydoc.h>

void bind_uhd_timed_pdu_emitter(py::module& m)
{

    using uhd_timed_pdu_emitter = ::gr::timing_utils::uhd_timed_pdu_emitter;


    py::class_<uhd_timed_pdu_emitter,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<uhd_timed_pdu_emitter>>(
        m, "uhd_timed_pdu_emitter", D(uhd_timed_pdu_emitter))

        .def(py::init(&uhd_timed_pdu_emitter::make),
             py::arg("rate"),
             py::arg("drop_late"),
             D(uhd_timed_pdu_emitter, make))


        .def("set_rate",
             &uhd_timed_pdu_emitter::set_rate,
             py::arg("rate"),
             D(uhd_timed_pdu_emitter, set_rate))

        ;
}
