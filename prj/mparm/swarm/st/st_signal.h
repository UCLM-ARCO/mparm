///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         st_signal.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Global signals of STBus interconnect
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __ST_SIGNAL_H__
#define __ST_SIGNAL_H__

#include <systemc.h>
#include "globals.h"
#include "config.h"
#include "stbus_bca_common.h"
#include "stbus_protocol_defs.h"
#include "core_signal.h"
#include "stbus_adds.h"

#ifdef DRAMBUILD
#include "statemachine3.h"
#endif

// Clock signals and dividers
extern sc_signal< bool >                       *init_clock;
extern sc_signal< bool >                       *interconnect_clock;
extern sc_signal< sc_uint<FREQSCALING_BITS> >  *init_div;
extern sc_signal< sc_uint<FREQSCALING_BITS> >  *interconnect_div;

//data link definitions
extern stbus_T2T3_link< uint, uint, uchar> *node_to_target;
extern stbus_T2T3_link< uint, uint, uchar> *initiator_to_node;

#ifndef NOSNOOP
extern stbus_T2T3_link_direct< uint, uint, uchar>   SnoopSignal_d[1];
extern stbus_T2T3_link_response< uint, uint, uchar> SnoopSignal_r[1];
#endif


#ifdef DRAMBUILD
   // Definizione dei segnali
   extern sc_signal<bool> *S_FPOP_PROCESSOR;
   extern sc_signal<bool> *S_FPUSH_PROCESSOR;
   extern sc_signal<uint> *S_FCOUNT;
   extern sc_signal<uint> *S_DATA_SDRAM;

   extern sc_signal<uint> *S_DATA_PROCESSOR[BUS_MAX_BURST_LENGHT];
   extern sc_signal<uint> *S_DATA_DIMENSION; //porta per dare la dimensione del burst

   extern sc_signal<PINOUT> *S_DMAPIN;
   extern sc_signal<bool> *S_READYDMA;
   extern sc_signal<bool> *S_REQUESTDMA;

   // Segnali tra control e transfer
   extern sc_signal<DMA_CONT_REG1> *S_DATADMA;		
   extern sc_signal<bool> *S_REQUESTDMATRANSFER;		
   extern sc_signal<bool> *S_FINISHED;				
   extern sc_signal<uint> *S_FINISHED_TRANSFER_ID; 
   extern sc_signal<uint> *S_FINISHED_TRANSFER_PROC;
   extern sc_signal<bool> *S_TRANSFER_ACK;

   // Segnali tra transfer e LMI
   extern sc_signal<bool> *S_READY;
   extern sc_signal<bool> *S_REQUEST;
   extern sc_signal<uint> *S_ADDRESSBUS;
   extern sc_signal<uint> *S_SIZEBUS;
   extern sc_signal<bool> *S_RD;
   extern sc_signal<bool> *S_DATA_OUT_PROC;

   // Segnali tra LMI e SDRAM
   extern sc_signal<uint> *S_COMMAND;
   extern sc_signal<uint> *S_AB;
   extern sc_signal<uint> *S_BA;
   extern sc_signal<bool> *S_DQM_SIGNAL;
#endif

#endif // __ST_SIGNAL_H__

