///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         st_trace.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Selects the STBus signals to be plotted if VCD is enabled
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __ST_TRACE_H__
#define __ST_TRACE_H__

#include <systemc.h>
#include "globals.h"
#include "st_signal.h"
#ifdef WITH_POWER_NODE
  #include "power.h"
  #include <power_stbus_sysc_lib.h>
#endif

void traceSTBusSignals(sc_trace_file *tf);

#endif // __ST_TRACE_H__
