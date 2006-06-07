///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         ST_DMA_SMARTMEM.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         STBus semaphore device
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __ST_DMA_SMARTMEM_included_
#define __ST_DMA_SMARTMEM_included_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "address.h"
#include "config.h"
#include "globals.h"
#include "st_dma.h"
#include "smartmem_signal.h"

//#define DMAPRINTDEBUG
class St_Dma_transfer_smartmem: public St_Dma_transfer
{
 public:
  St_Dma_transfer_smartmem(sc_module_name nm, uint32_t id_, uint32_t id1, uint32_t dimburst, uint32_t obj, 
  uint32_t nproc, uint32_t base,Mem_class * builderscra) :
     St_Dma_transfer(nm,id_,dimburst,obj,nproc)
  {
   type = "DMA_TRANSFER_SMARTMEM";
   
   base_address=base;
   
   smart_id=id1; 
   
   scra=builderscra;
   
   extint = new sc_inout<bool> [obj*nproc];
   
   //orrible fix...
   size_smart=SMART_MEM_SIZE[0];
 
   TRACEX(WHICH_TRACEX, 7,
          "%s %d Base Address: 0x%08x, Max obj:%d, N_proc:%d, dimburst:%d\n",
           type, ID, base_address, maxobj, nproc, dimburst);
  }
 
 sc_inout<bool> *extint; 
 
 protected:
 
  uint size_smart;
  uint32_t base_address;
  uint16_t smart_id;
  
  Mem_class* scra;
 
  inline bool local(uint32_t addr)
  {
   return addresser->PhysicalInmySmartmem(smart_id,addr);
  }
  
  inline virtual uint32_t read_local(uint32_t addr)
  {
   uint32_t data;
   double pwr;
   
   data=scra->Read(addr);
   
   if (POWERSTATS)
   { 
    pwr = powerRAM(smart_id, size_smart-SMARTMEM_DMA_SIZE, 32, READop);
    statobject->inspectSMARTMEMAccess(addr-base_address, true, pwr, smart_id);
   }
   
   TRACEX(WHICH_TRACEX, 12,"%s:%d: local_read addr:%x, data:%x\n", 
     type, ID, data, addr & 0xFFFFFFFC);
   
   return data;
  }
 
  inline virtual void write_local(uint32_t addr,uint32_t data)
  {
   double pwr;

   TRACEX(WHICH_TRACEX, 12,"%s:%d: local_write addr:%x, data:%x\n", 
     type, ID, data, addr & 0xFFFFFFFC);
   
   scra->Write(addr,data,0);
   
   if (POWERSTATS)
   {
    pwr = powerRAM(smart_id, size_smart-SMARTMEM_DMA_SIZE, 32, WRITEop);
    statobject->inspectSMARTMEMAccess(addr-base_address, false, pwr, smart_id);
   }
    
  }
  
  inline void send_int(DMA_CONT_REG1 work)
  {
   
   TRACEX(WHICH_TRACEX, 8,"%s:%d Send_int:%d\n",type,ID,(uint)work.num_obj);   
  
   extint[work.num_obj].write(true);
  };
};

class St_Dma_control_smartmem : public dmacontrol
{
 protected:
 uint32_t base_address; 
 
 inline bool control(uint32_t addr)
 {  
  return
    (!addresser->PhysicalInSmartmem(addr) 
     && !addresser->PhysicalInPrivateSpace(addr)
     && !addresser->PhysicalInSharedSpace(addr)
     && !addresser->PhysicalInCoreSlaveSpace(addr)
    );
 }
 
 inline uint32_t addressing(uint32_t addr)
 {
  return addr -=base_address;
 } 
 
 public:
  St_Dma_control_smartmem(sc_module_name nm, uint16_t id_, uint32_t obj, uint32_t nproc, uint32_t base) : 
    dmacontrol(nm,id_,obj,nproc)
  {
   type = "DMA_CONTROL_SMARTMEM"; 
    
   base_address=base;
   
   TRACEX(WHICH_TRACEX, 7,"%s %d - Size: 0x%08x, Base Address: 0x%08x, Max obj:%d, N_proc:%d\n",
           type, ID, (int)size, base_address, maxobj, nproc); 
  }

};

#endif /*__ST_DMA_SMARTMEM_included_ */
