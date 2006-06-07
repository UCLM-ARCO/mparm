
//This Class implement a DMA wich move data between the scratch and external memory
//It can be connected to the amba-axi bus and the swarm processor

#ifndef __AMBA_AXI_DMA_SCRATCH
#define __AMBA_AXI_DMA_SCRATCH

#include "amba_axi_dma.h"

class Amba_axi_Dma_transfer_scratch: public Amba_axi_Dma_transfer
{
 public:
 
 Amba_axi_Dma_transfer_scratch(sc_module_name nm, uint16_t id, uint32_t dimburst, uint32_t obj, 
                        uint32_t nproc, Mem_class* builderscra) :
 Amba_axi_Dma_transfer(nm,id,dimburst,obj,nproc)
 {
  type = "DMA_TRANSFER";
  printf("%s %d \n",type, ID);
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
  return addresser->PhysicalInScratchSpace(ID, addresser->Logical2Physical(addr, ID), &dummy);
 }
};

class Amba_axi_Dma_control_scratch : public dmacontrol
{
 protected:
 uint32_t base_address; 
 uint8_t dummy;
 
 inline uint32_t addressing(uint32_t addr)
 {
  return addr -=base_address;
 }

 inline bool control(uint32_t addr, uint mem)
 {
  if (mem==0)
  {
   return
   (!addresser->PhysicalInScratchSpace((uint8_t)ID, addresser->Logical2Physical(membus.data,
    ID), &dummy));
  }
  else
  {
   return
    (!addresser->PhysicalInSmartmem(addr) 
     && !addresser->PhysicalInPrivateSpace(addr)
     && !addresser->PhysicalInSharedSpace(addr)
     && !addresser->PhysicalInCoreSlaveSpace(addr));  
  }
 }
 public:
  
  Amba_axi_Dma_control_scratch(sc_module_name nm, uint16_t id, uint32_t obj, uint32_t nproc, uint32_t base) : 
    dmacontrol(nm,id,obj,nproc)
  {
   type = "DMA_CONTROL";
   
   base_address=base;
   
   printf("%s %d - Size: 0x%08x, Base Address: 0x%08x, Max obj:%d, N_proc:%d\n",
           type, ID, (int)size, base_address, maxobj, nproc);
  }
};

#endif 
//__DMA_SCRATCH_CLASS_H__
