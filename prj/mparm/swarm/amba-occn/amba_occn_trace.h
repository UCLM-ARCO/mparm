///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_occn_trace.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Selects the AMBA AHB (OCCN) signals to be plotted if VCD is enabled
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_OCCN_TRACE_H__
#define __AMBA_OCCN_TRACE_H__

#include <systemc.h>
#include "globals.h"
#include "amba_occn_signal.h"
#include "core_signal.h"

void traceAMBAAHBOCCNSignals(sc_trace_file *tf);

#endif // __AMBA_OCCN_TRACE_H__
