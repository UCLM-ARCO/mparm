
#ifndef __AMBA_AXI_DMA_CLASS_H__
#define __AMBA_AXI_DMA_CLASS_H__

#include <systemc.h>
#include "config.h"
#include "globals.h"
#include "dmatransfer.h"

class Amba_axi_Dma_transfer :public dmatransfer
{  
 protected:
 bool Write(uint32_t addr, uint32_t* data, uint32_t nburst);
 bool Read(uint32_t addr, uint32_t* data, uint32_t nburst);
 uint8_t dummy;

 virtual uint32_t read_local(uint32_t addr)=0;
 virtual void write_local(uint32_t addr,uint32_t data)=0;
 
 public:

 //signals for the master
 sc_inout<PINOUT> pinout;
 sc_in<bool> ready_from_master;
 sc_out<bool> request_to_master;
 
 Amba_axi_Dma_transfer(sc_module_name nm, uint16_t id, uint32_t dimburst, uint32_t obj,
                        uint32_t nproc) :
  dmatransfer(nm,id,dimburst,obj,nproc)
 { 
 };
   
};

#endif 
//__AMBA_AXI_DMA_CLASS_H__
