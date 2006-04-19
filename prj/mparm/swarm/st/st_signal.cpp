///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         st_signal.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Global signals of STBus interconnect
//
///////////////////////////////////////////////////////////////////////////////


#include "st_signal.h"

// Clock signals and dividers
sc_signal< bool >                       *init_clock;
sc_signal< bool >                       *interconnect_clock;
sc_signal< sc_uint<FREQSCALING_BITS> >  *init_div;
sc_signal< sc_uint<FREQSCALING_BITS> >  *interconnect_div;

stbus_T2T3_link< uint, uint, uchar > *node_to_target;
stbus_T2T3_link< uint, uint, uchar > *initiator_to_node;

#ifndef NOSNOOP
stbus_T2T3_link_direct< uint, uint, uchar> SnoopSignal_d[1];
stbus_T2T3_link_response< uint, uint, uchar> SnoopSignal_r[1];
#endif

#ifdef DRAMBUILD
   // Definizione dei segnali
   sc_signal<bool> *S_FPOP_PROCESSOR;
   sc_signal<bool> *S_FPUSH_PROCESSOR;
   sc_signal<uint> *S_FCOUNT;
   sc_signal<uint> *S_DATA_SDRAM;

   sc_signal<uint> *S_DATA_PROCESSOR[BUS_MAX_BURST_LENGHT];
   sc_signal<uint> *S_DATA_DIMENSION; //porta per dare la dimensione del burst

   sc_signal<PINOUT> *S_DMAPIN;
   sc_signal<bool> *S_READYDMA;
   sc_signal<bool> *S_REQUESTDMA;

   // Segnali tra control e transfer
   sc_signal<DMA_CONT_REG1> *S_DATADMA;		
   sc_signal<bool> *S_REQUESTDMATRANSFER;		
   sc_signal<bool> *S_FINISHED;				
   sc_signal<uint> *S_FINISHED_TRANSFER_ID; 
   sc_signal<uint> *S_FINISHED_TRANSFER_PROC;
   sc_signal<bool> *S_TRANSFER_ACK;

   // Segnali tra transfer e LMI
   sc_signal<bool> *S_READY;
   sc_signal<bool> *S_REQUEST;
   sc_signal<uint> *S_ADDRESSBUS;
   sc_signal<uint> *S_SIZEBUS;
   sc_signal<bool> *S_RD;
   sc_signal<bool> *S_DATA_OUT_PROC;

   // Segnali tra LMI e SDRAM
   sc_signal<uint> *S_COMMAND;
   sc_signal<uint> *S_AB;
   sc_signal<uint> *S_BA;
   sc_signal<bool> *S_DQM_SIGNAL;
#endif

