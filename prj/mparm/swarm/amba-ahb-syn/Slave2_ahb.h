#ifndef __SLAVE2_AHB_H
#define __SLAVE2_AHB_H

#include "scratch_mem.h"
#include "Slave_ahb.h"

class Slave2_ahb// A Simple Slave2 Module
: public Slave_ahb
{
private:
  
  Mem_class* scra;
  Scratch_queue_mem* queue;
  uint8_t   dummy;
  uint16_t my1;

  inline void Write(uint32_t addr, uint32_t data, uint8_t bw)
  {
   addr = addr + _base_address;
   if(addresser->PhysicalInQueueSpace(my1, addresser->Logical2Physical(addr, my1)))
     {
      queue->Write(addr, data, MEM_WORD);
     }
   else
    if(addresser->PhysicalInScratchSpace(my1, addresser->Logical2Physical(addr, my1), &dummy))
    scra->Write(addr, data, bw);
   else
    {
     printf("%s %d,%d wrong write: I cannot map it in scratch or queue:0x%08x\n", 
     type, my, my1, addr);
     exit(0);
    }
  };

  inline uint32_t Read(uint32_t addr)
  {
   addr = addr + _base_address;
   if(addresser->PhysicalInQueueSpace(my1, addresser->Logical2Physical(addr, my1)))
     {
      return queue->Read(addr);
     }
   else
    if(addresser->PhysicalInScratchSpace(my1, addresser->Logical2Physical(addr, my1), &dummy))
     return scra->Read(addr);
   else
    {
     printf("%s %d,%d wrong read: I cannot map it in scratch or queue:0x%08x\n", 
     type, my, my1, addr);
     exit(0);
    } 
  };  
  
public:    

// constructor
Slave2_ahb(sc_module_name name_, // name of the instance
       ahb_namespace::ahb_addr_t base_address,// base address of the Slave2
       int my,
       Mem_class *builderscra, Scratch_queue_mem *builderqueue
       )
       :Slave_ahb(name_,base_address,my,INT_WS,0)
{
 
type = "CoreSlave";
my1= my - addresser->CoreSlaveStartID();
printf("%s %d,%d address:0x%x\n", type, my, my1, addresser->ReturnSlavePhysicalAddress(my));
//They are initialized in the builder
scra=builderscra;
queue=builderqueue;
   
ahb_address_region tmp(_base_address, _base_address+0x00014fff);
_address_map.push_back(tmp);
}
}; // end module Slave2
#endif
