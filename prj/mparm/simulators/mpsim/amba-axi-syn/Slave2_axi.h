#ifndef _SLAVE2_AXI_H
#define _SLAVE2_AXI_H

#ifdef __EDG__
#define axi_namespace std
#endif

#include "scratch_mem.h"
#include "Slave_axi.h"

class Slave2_axi // Slave2
  : public Slave_axi
{
private:

  Mem_class* scra;
  Scratch_queue_mem* queue;
  uint8_t   dummy;
  uint16_t my1;
  
inline void Write(uint32_t addr, uint32_t data, uint8_t bw)
  {
  my=m_ID;
  addr=addr+m_StartAddress;
   
   if(addresser->PhysicalInQueueSpace(my1, addresser->Logical2Physical(addr, my1)))
      queue->Write(addr, data, MEM_WORD);
   else
    if(addresser->PhysicalInScratchSpace(my1, addresser->Logical2Physical(addr, my1), &dummy))
    scra->Write(addr, data, bw);
   else
    {
     printf("%s %d,%d wrong write: I cannot map it in scratch or queue:0x%x\n", 
     type, my, my1, addr);
     exit(0);
    }
  };

inline uint32_t Read(uint32_t addr)
  {
  addr=addr+m_StartAddress;
   if(addresser->PhysicalInQueueSpace(my1, addresser->Logical2Physical(addr, my1)))
      return queue->Read(addr);
   else
    if(addresser->PhysicalInScratchSpace(my1, addresser->Logical2Physical(addr, my1), &dummy))
     return scra->Read(addr);
   else
    {
     printf("%s %d,%d wrong read: I cannot map it in scratch or queue:0x%x\n", 
     type, my, my1, addr);
     exit(0);
    } 
  };   
  
public:

SC_HAS_PROCESS(Slave2_axi);

Slave2_axi(sc_module_name Name
      , int ID // Unique number among all masters attached to the same IC
               // Must satisfy: 0 <= ID <= 31
      , axi_namespace::axi_addr_t StartAddress // Start address of the slaves memory.
      , axi_namespace::axi_addr_t EndAddress // End address of the slaves memory.
               // The addresses must be 4K aligned.
      , bool DefaultSlave // Flag specifying if the slave is the default slave
               // in the AXI system. There should be only one default slave
               // per AXI system. The default slave gets all requests for
               // which the decoder does not find a valid slave.
      , bool MonitorOn // Flag specifying if a monitor, displaying
               // the memory content, should be created.
      , int DataBusBytes // Number of byte lanes of the AXI IC
               // The same number must be used in the entire AXI system. 
      , double ClockFrequency // Frequency of the AXI system clock (Aclk)
               // in units of cycles per second. This value is only used
               // for NegEdge = false, i.e., if the negative clock edge
               // toggling scheme is not used.
      , Mem_class *builderscra
      , Scratch_queue_mem *builderqueue
      ,int WaitState
      )
  : Slave_axi(Name,ID,StartAddress,EndAddress,DefaultSlave,MonitorOn,DataBusBytes,ClockFrequency,WaitState)            
 {
 type = "CoreSlave";

   my1= my - addresser->CoreSlaveStartID();
   printf("%s %d,%d address:0x%x\n", type, my, my1, addresser->ReturnSlavePhysicalAddress(my));
   //They are initialized in the builder
   scra=builderscra;
   queue=builderqueue;
 }     

};

#endif //_Slave2_SYN_H
