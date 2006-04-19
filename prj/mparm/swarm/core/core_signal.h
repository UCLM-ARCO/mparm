///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         core_signal.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Global signals of the platform
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __CORE_SIGNAL_H__
#define __CORE_SIGNAL_H__

#include <systemc.h>
#include "config.h"
#include "globals.h"
#include "ocp_signal.h"

// ARM pinout
typedef struct POTAG
{
  bool nreset;
  uint32_t address;
  uint32_t data;
  bool rw;          // Write = 1
  bool fiq;
  bool irq;
  bool benable;     // Bus Active?
  uint32_t bw;      // (2 bit) 0 = word, 1 = byte, 2 = half word, 3 = UNDEF
  uint32_t burst;   // (5 bit) Burst size

  inline bool operator == (const POTAG& rhs) const
      {
	return (rhs.address == address &&  rhs.nreset == nreset && rhs.data == data &&
	rhs.fiq == fiq && rhs.irq == irq  && rhs.rw == rw && rhs.benable == benable &&
	rhs.bw == bw && rhs.burst == burst);
      }

} PINOUT;

// Needed for dump/print in systemc 2.0.x and up
inline  ostream& operator << (ostream& os, const PINOUT& v)
{
  os << v.nreset << ","
     << v.address << ","
     << v.data << ","
     << v.rw << ","
     << v.fiq << ","
     << v.irq << ","
     << v.benable << ","
     << v.bw << ","
     << v.burst << ","
     << endl;
  return os;
}

inline void sc_trace(sc_trace_file *tf, const POTAG& v, const sc_string& NAME)
{
  sc_trace(tf,v.nreset, NAME + ".nreset");
  sc_trace(tf,v.address, NAME + ".address");
  sc_trace(tf,v.data, NAME + ".data");
  sc_trace(tf,v.rw, NAME + ".rw");
  sc_trace(tf,v.fiq, NAME + ".fiq");
  sc_trace(tf,v.irq, NAME + ".irq");
  sc_trace(tf,v.benable, NAME + ".benable");
  sc_trace(tf,v.bw, NAME + ".bw");
  sc_trace(tf,v.burst, NAME + ".burst");
};

// DMA controller object register structure
typedef struct DMA_CONT
{
  sc_uint<32> size; 	   		//data size
  sc_uint<32> numberofrow; 		//number of row to transfer 
  sc_uint<32> rowlengthl1;		//size of source row 
  sc_uint<32> rowlengthl2;		//size of target row
  sc_uint<32> l1; 			//scratch memory address
  sc_uint<32> l2; 			//l2 memory address
  sc_uint<4>  state;			//0=l1 level, 1=l1->l2 level, 2=l2 level, 3=l2->l1 
  					//4=l1->l2 and send int 5=l2->l1 and send int
					//6=data_transf 0 7=data_transf 0 and send int
  bool  send_int;			//generate an int at the end of this transfer
  sc_uint<4>  data_transf;		//DATA_TRANSFORMATION
  sc_uint<32> nproc; 	   		//proc
  
  inline bool operator == (const DMA_CONT& rhs) const
      {
       return (rhs.l1 == l1 &&
               rhs.l2 == l2 &&
               rhs.size == size &&
               rhs.state == state &&
               rhs.nproc == nproc &&
               rhs.numberofrow == numberofrow &&
               rhs.rowlengthl1 == rowlengthl1 &&
               rhs.rowlengthl2 == rowlengthl2
              );
      }

} DMA_CONT_REG;

typedef struct DMA_CONT1
{
 DMA_CONT_REG work;
 sc_uint<32> num_obj; //indicate the numof obj for the send_int function
   
 inline bool operator == (const DMA_CONT1& rhs) const
      {
       return (rhs.work == work && rhs.num_obj == num_obj );
      }
} DMA_CONT_REG1; 

// Needed for dump/print in systemc 2.0.x and up
inline  ostream& operator << (ostream& os, const DMA_CONT_REG1& v)
{
  os << v.work.size << ","
     << v.work.numberofrow << ","
     << v.work.rowlengthl1 << ","
     << v.work.rowlengthl2 << ","
     << v.work.l1 << ","
     << v.work.l2 << ","
     << v.work.state << ","
     << v.num_obj << ","
     << endl;
  return os;
}

inline void sc_trace(sc_trace_file *tf, const DMA_CONT1& v, const sc_string& NAME)
{
  sc_trace(tf,v.work.size, NAME + ".size");
  sc_trace(tf,v.work.numberofrow, NAME + ".numberofrow");
  sc_trace(tf,v.work.rowlengthl1, NAME + ".rowlengthl1");
  sc_trace(tf,v.work.rowlengthl2, NAME + ".rowlengthl2");
  sc_trace(tf,v.work.l1, NAME + ".l1");
  sc_trace(tf,v.work.l2, NAME + ".l2");
  sc_trace(tf,v.work.state, NAME + ".state");
  sc_trace(tf,v.num_obj, NAME + ".num_obj");
};

// System clock
extern sc_clock ClockGen_1;
// Reset signal - true high
extern sc_signal<bool> ResetGen_1;
// Reset signal - true low
extern sc_signal<bool> ResetGen_low_1;

//FIXME: MAddrSpace, MDataValid, SCmdAccept, SDataAccept, MRespAccept currently not implemented in wrappers

// DMA signals
// Signals from wrapper to dmacontrol
extern sc_signal<PINOUT>       *pinoutwrappertodma;
extern sc_signal<bool>         *readywrappertodma;
extern sc_signal<bool>         *requestwrappertodma;
// Signals from dmacontrol to dmatransfer
extern sc_signal<DMA_CONT_REG1> *datadmacontroltotransfer;
extern sc_signal<bool>         *requestcontroltotransfer;
extern sc_signal<bool>         *finishedtransfer;
// Signals from dmatransfer to scratch
extern sc_signal<PINOUT>       *spinoutscratchdma;
extern sc_signal<bool>         *sreadyscratchdma;
extern sc_signal<bool>         *srequestscratchdma;

//arm_system or dma_transfer inout_signals (core frequency)
extern sc_signal<bool> *readymast;
extern sc_signal<PINOUT> *pinoutmast;
extern sc_signal<bool> *requestmast;
//arm_system or dma_transfer inout_signals (interconnect frequency)
extern sc_signal<bool> *readymast_interconnect;
extern sc_signal<PINOUT> *pinoutmast_interconnect;
extern sc_signal<bool> *requestmast_interconnect;
//arm_system's inout_signals
extern sc_signal<bool> *extint;

// Signals from fft to master
extern sc_signal<PINOUT>       *pinoutmasterfft;
extern sc_signal<bool>         *readymastertofft;
extern sc_signal<bool>         *requestffttomaster;

#endif // __CORE_SIGNAL_H__
