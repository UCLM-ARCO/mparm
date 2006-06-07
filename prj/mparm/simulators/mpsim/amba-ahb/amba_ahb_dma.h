///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_dma.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements a complete DMA (bus side)
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_AHB_DMA_H__
#define __AMBA_AHB_DMA_H__

//#define DMAPRINTDEBUG1
//#define DMAPRINTDEBUG

#include <systemc.h>
#include "config.h"
#include "globals.h"
#include "dmatransfer.h"

class amba_ahb_dma_transfer : public dmatransfer
{  
  protected:
    bool Write(uint32_t addr, uint32_t* data, uint32_t nburst);
    bool Read(uint32_t addr, uint32_t* data, uint32_t nburst);
    
    virtual uint32_t read_local(uint32_t addr) = 0;
    virtual void write_local(uint32_t addr,uint32_t data) = 0;
  
  public:
    //signals for the master
    sc_inout<PINOUT> pinout;
    sc_in<bool> ready_from_master;
    sc_out<bool> request_to_master;
 
    amba_ahb_dma_transfer(sc_module_name nm,
                          uint16_t id,
                          uint32_t dimburst,
                          uint32_t obj, 
                          uint32_t nproc) :
      dmatransfer(nm, id, dimburst, obj, nproc)
    { 
    };
};

#endif // __AMBA_AHB_DMA_H__
