///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_sig_signal.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Global signals of AMBA (signal) bus
//
///////////////////////////////////////////////////////////////////////////////

#include "amba_sig_signal.h"

sc_signal<request> *mast;
sc_signal<sc_uint<32> > *address;
sc_signal<sc_uint<32> > *hwdata;
sc_signal<bool> *hreq;
sc_signal<request> ctrl_signals;
sc_signal<sc_uint<32> > haddr;
sc_signal<sc_uint<32> > hwdataout;
sc_signal<sc_uint<32> > delayed_hwdataout;
sc_signal<sc_uint<32> > hrdata;
sc_signal<bool> hready;
sc_signal<bool> *hsel;
sc_signal<bool> *delayed_hsel;
sc_signal<sc_uint<5> > hmaster;
sc_signal<bool> *hgrant;
sc_signal<bool> *ready;
sc_signal<sc_uint<32> > *readdata;

// Clock signals and dividers
sc_signal< bool >                       *init_clock_amba; //FIXME (to avoid name overlapping with global signals in STBus)
sc_signal< bool >                       *interconnect_clock_amba;
sc_signal< sc_uint<FREQSCALING_BITS> >  *init_div_amba;
sc_signal< sc_uint<FREQSCALING_BITS> >  *interconnect_div_amba;
