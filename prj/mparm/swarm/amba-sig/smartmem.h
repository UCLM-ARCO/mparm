#ifndef __SMARTMEM_H
#define __SMARTMEM_H
#include <systemc.h>
#include "config.h"
#include "globals.h"
#include "scratch_mem.h"
#include "master.h"
#include "amba_sig_dma.h"
#include "dmacontrol.h"
#include  "power.h"
#include "smartmem_signal.h"

class Amba_sig_Dma_transfer_smartmem: public Amba_sig_Dma_transfer
{
 public:
  Amba_sig_Dma_transfer_smartmem(sc_module_name nm, uint16_t id, uint32_t dimburst,uint32_t obj, uint32_t nproc, Mem_class* builderscra) :
     Amba_sig_Dma_transfer(nm,id,dimburst,obj,nproc)
  {
   scra=builderscra;
   
   type = "DMA_TRANSFER_SMARTMEM";
   
   extint = new sc_inout<bool> [obj*nproc];
   
   smart_id=addresser->Smartmem_slave_StartID()+ ID - (DMA * N_CORES);

   size_smart=SMART_MEM_SIZE[0];
  
   TRACEX(WHICH_TRACEX, 7,
          "%s %d, Max obj:%d, N_proc:%d, dimburst:%d\n",
           type, ID, maxobj, nproc, dimburst);
  }
 
 sc_inout<bool> *extint; 
 
 protected:
 
  Mem_class* scra;
  uint size_smart;
  uint16_t smart_id;
  
  inline virtual uint32_t read_local(uint32_t addr)
  {
   double pwr;
   if (POWERSTATS)
   {
     pwr = powerRAM(smart_id, size_smart-SMARTMEM_DMA_SIZE, 32, READop);
     statobject->inspectSMARTMEMAccess(addr, true, pwr, smart_id);
   }
   else if (STATS)
          statobject->inspectSMARTMEMAccess(addr, false, 0, smart_id);
   return scra->Read(addr);
  };
 
  inline virtual void write_local(uint32_t addr,uint32_t data)
  {
   double pwr;
   scra->Write(addr,data,0);
   if (POWERSTATS)
   {
     pwr = powerRAM(smart_id, size_smart-SMARTMEM_DMA_SIZE, 32, WRITEop);
     statobject->inspectSMARTMEMAccess(addr, false, pwr, smart_id);
   }
   else if (STATS)
          statobject->inspectSMARTMEMAccess(addr, false, 0, smart_id);
  };
  
  inline bool local(uint32_t addr)
  {
   return addresser->PhysicalInmySmartmem(smart_id,addr);
  }
  
  inline void send_int(DMA_CONT_REG1 work)
  {
  
   TRACEX(WHICH_TRACEX, 8,"%s:%d Send_int:%d\n",type,ID,(uint)work.num_obj);  
  
   extint[work.num_obj].write(true);
  };
};

class Amba_sig_Dma_control_smartmem : public dmacontrol
{ 
 protected:
 uint32_t base_address; 
 
 public:
  Amba_sig_Dma_control_smartmem(sc_module_name nm, uint16_t id, uint32_t obj, uint32_t nproc, uint32_t base) : 
    dmacontrol(nm,id,obj,nproc)
  {
   type = "DMA_CONTROL_SMARTMEM";    
   
   base_address=base;
   
   TRACEX(WHICH_TRACEX, 7,"%s %d - Size: 0x%08x, Base Address: 0x%08x, Max obj:%d, N_proc:%d\n",
           type, ID, (int)size, base_address, maxobj, nproc);   
  }
  
 inline uint32_t addressing(uint32_t addr)
 {
  return addr -=base_address;
 } 
 
 protected: 
 inline bool control(uint32_t addr)
 {
  return
    (!addresser->PhysicalInSmartmem(addr) 
     && !addresser->PhysicalInPrivateSpace(addr)
     && !addresser->PhysicalInSharedSpace(addr)
     && !addresser->PhysicalInCoreSlaveSpace(addr));
 }
};
#endif
