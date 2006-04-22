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


#ifndef __SMARTMEM_SIGNAL_H__
#define __SMARTMEM_SIGNAL_H__

#include <systemc.h>
#include "config.h"
#include "globals.h"
#include "dmacontrol.h"

// DMA signals
extern sc_signal<PINOUT> *smartmem_pinoutslavetodma;
extern sc_signal<bool> *smartmem_readyslavetodma;
extern sc_signal<bool> *smartmem_requestslavetodma;
extern sc_signal<DMA_CONT_REG1> *smartmem_datadmacontroltotransfer;
extern sc_signal<bool> *smartmem_requestcontroltotransfer;
extern sc_signal<bool> *smartmem_finishedtransfer;
extern sc_signal<PINOUT> *smartmem_spinoutscratchdma;
extern sc_signal<bool> *smartmem_sreadyscratchdma;
extern sc_signal<bool> *smartmem_srequestscratchdma;

extern unsigned short int SMART_MEM_IN_WS[SMARTMEM_MAX_CONF];
extern unsigned short int SMART_MEM_BB_WS[SMARTMEM_MAX_CONF];
extern unsigned long int  SMART_MEM_SIZE[SMARTMEM_MAX_CONF];
#endif
