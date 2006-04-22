///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         core_signal.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Global signals of the platform
//
///////////////////////////////////////////////////////////////////////////////

#include "smartmem_signal.h"


// Marchal SMARTMEM DMA signals
//DMA signals
//signals from wrapper to dmacontrol
sc_signal<PINOUT> *smartmem_pinoutslavetodma;
sc_signal<bool> *smartmem_readyslavetodma;
sc_signal<bool> *smartmem_requestslavetodma;
//signals from dmacontrol to dmatransfer
sc_signal<DMA_CONT_REG1> *smartmem_datadmacontroltotransfer;
sc_signal<bool> *smartmem_requestcontroltotransfer;
sc_signal<bool> *smartmem_finishedtransfer;
//signals from dmatransfer to scratch
sc_signal<PINOUT> *smartmem_spinoutscratchdma;
sc_signal<bool> *smartmem_sreadyscratchdma;
sc_signal<bool> *smartmem_srequestscratchdma;

/////////////////////////
//SMART_MEM CONFIGURATION
/////////////////////////

//initial wait states of SMART_MEM slaves
unsigned short int SMART_MEM_IN_WS[SMARTMEM_MAX_CONF]={1,
                                                      };
//back-to-back wait states of SMART_MEM slaves ONLY IF USING STBUS
unsigned short int SMART_MEM_BB_WS[SMARTMEM_MAX_CONF]={3,
                                                      };
//size of the different SMART_MEM slaves
//remeber: maximum smart_mem_size= 32MB each size indicate the size of the memory in Byte
unsigned long int  SMART_MEM_SIZE[SMARTMEM_MAX_CONF]={(4*1024*1024),
    		                                     };
