///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_sig_dma_scratch_class.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements a complete DMA (bus side)
//
///////////////////////////////////////////////////////////////////////////////

//This Class implement a DMA wich move data between the scratch and external memory
//It can be connected to the amba-sig bus and the swarm processor

#ifndef __AMBA_SIG_DMA_SCRATCH
#define __AMBA_SIG_DMA_SCRATCH

#include "amba_sig_dma.h"

class Amba_sig_Dma_transfer_scratch: public Amba_sig_Dma_transfer
{
 public:
 
 Amba_sig_Dma_transfer_scratch(sc_module_name nm, uint16_t id, uint32_t dimburst, uint32_t obj,
  			uint32_t nproc, Mem_class* builderscra) :
 Amba_sig_Dma_transfer(nm,id,dimburst,obj,nproc)
 {
  type = "DMA_TRANSFER";   
  
  TRACEX(WHICH_TRACEX, 7,
          "%s %d, Max obj:%d, N_proc:%d, dimburst:%d\n",
           type, ID, maxobj, nproc, DIM_BURST);
	   
  scra = builderscra;
 }
 
 protected:
 
 Mem_class* scra;
 
 inline virtual uint32_t read_local(uint32_t addr)
 {
  if (STATS)
  statobject->inspectextSCRATCHAccess(addr, false, ID);
  return scra->Read(addr);
 };
 
 inline virtual void write_local(uint32_t addr,uint32_t data)
 {    
  if (STATS)
   statobject->inspectextSCRATCHAccess(addr, true, ID);
  scra->Write(addr,data,0);
 };
 
 inline bool local(uint32_t addr)
 {
  uint8_t dummy;
  return addresser->PhysicalInScratchSpace(ID, addresser->Logical2Physical(addr, ID), &dummy);
 }
};

class Amba_sig_Dma_control_scratch : public dmacontrol
{
 protected:
 uint32_t base_address; 
 uint8_t dummy;
 
 inline uint32_t addressing(uint32_t addr)
 {
  return addr -=base_address;
 }

 public:
  
  Amba_sig_Dma_control_scratch(sc_module_name nm, uint16_t id, uint32_t obj, uint32_t nproc, uint32_t base) : 
    dmacontrol(nm,id,obj,nproc)
  {
   type = "DMA_CONTROL";
   
   base_address=base;
      
   TRACEX(WHICH_TRACEX, 7,"%s %d - Size: 0x%08x, Base Address: 0x%08x, Max obj:%d, N_proc:%d\n",
           type, ID, (int)size, base_address, maxobj, nproc);

  }
};

#endif 
//__DMA_SCRATCH_CLASS_H__
