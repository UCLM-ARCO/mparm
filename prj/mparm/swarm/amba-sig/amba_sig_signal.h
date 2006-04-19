///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_sig_signal.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Global signals of AMBA (signal) bus
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __AMBA_SIG_SIGNAL_H__
#define __AMBA_SIG_SIGNAL_H__

#include <systemc.h>
#include "config.h"

// AMBA control bus signals
struct request
{
  sc_uint<3>  hburst;
  sc_uint<2>  htrans;
  sc_uint<3>  hsize;
  bool hwrite;

  inline bool operator == (const request& rhs) const
  {
    return (rhs.hburst == hburst && rhs.htrans == htrans && rhs.hsize == hsize && rhs.hwrite == hwrite);
  }
};

inline void sc_trace(sc_trace_file *tf, const request& v, const sc_string& NAME)
{
  sc_trace(tf,v.hwrite, NAME + ".hwrite");
  sc_trace(tf,v.hburst, NAME + ".hburst");
  sc_trace(tf,v.htrans, NAME + ".htrans");
  sc_trace(tf,v.hsize, NAME + ".hsize");
};


//AMBA HTRANS modes
#define AMBA_SIG_HTRANS_IDLE    0
#define AMBA_SIG_HTRANS_BUSY    1
#define AMBA_SIG_HTRANS_NONSEQ  2
#define AMBA_SIG_HTRANS_SEQ     3

//AMBA HBURST modes
#define AMBA_SIG_HBURST_SINGLE  0
#define AMBA_SIG_HBURST_INCR    1
#define AMBA_SIG_HBURST_WRAP4   2
#define AMBA_SIG_HBURST_INCR4   3
#define AMBA_SIG_HBURST_WRAP8   4
#define AMBA_SIG_HBURST_INCR8   5
#define AMBA_SIG_HBURST_WRAP16  6
#define AMBA_SIG_HBURST_INCR16  7

//AMBA HSIZE values
#define AMBA_SIG_HSIZE_BYTE     0
#define AMBA_SIG_HSIZE_HALFWORD 1
#define AMBA_SIG_HSIZE_WORD     2
#define AMBA_SIG_HSIZE_SQ       3  // This one and below not actually used
#define AMBA_SIG_HSIZE_WORD4    4
#define AMBA_SIG_HSIZE_WORD8    5
#define AMBA_SIG_HSIZE_WORD16   6
#define AMBA_SIG_HSIZE_WORD32   7

//master's out_signal
extern sc_signal<request> *mast;
extern sc_signal<sc_uint<32> > *address;
extern sc_signal<sc_uint<32> > *hwdata;
extern sc_signal<bool> *hreq;

//multi1's out_signal
extern sc_signal<request> ctrl_signals;
extern sc_signal<sc_uint<32> > haddr;
//multi2's out_signal
extern sc_signal<sc_uint<32> > hwdataout;
//delayer_hwdata's out_signal
extern sc_signal<sc_uint<32> > delayed_hwdataout;
//multi3's out_signal
extern sc_signal<sc_uint<32> > hrdata;
//slaving's out_signal
extern sc_signal<bool> hready;
//decoder's out_signal
extern sc_signal<bool> *hsel;
//delayer_hsel's out_signal
extern sc_signal<bool> *delayed_hsel;
//arbiter's out_signal
extern sc_signal<sc_uint<5> > hmaster;
extern sc_signal<bool> *hgrant;

// Clock signals and dividers
extern sc_signal< bool >                       *init_clock_amba;
extern sc_signal< bool >                       *interconnect_clock_amba;
extern sc_signal< sc_uint<FREQSCALING_BITS> >  *init_div_amba;
extern sc_signal< sc_uint<FREQSCALING_BITS> >  *interconnect_div_amba;

//slave's out_signals
extern sc_signal<bool> *ready;
extern sc_signal<sc_uint<32> > *readdata;

#endif // __AMBA_SIG_SIGNAL_H__
