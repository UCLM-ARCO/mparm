#ifndef _SLAVE0_AXI_H
#define _SLAVE0_AXI_H

#ifdef __EDG__
#define axi_namespace std
#endif

#include "ext_mem.h"
#include "address.h"
#include "Slave_axi.h"

class Slave0_axi
    :  public Slave_axi
{

private:
Mem_class* myRam;

inline void Write(uint32_t index, uint32_t data, uint8_t bw)
 {
myRam->Write(index,data, bw);
 }

inline uint32_t Read(uint32_t addr)
 {
 return myRam->Read(addr);
 }
 
public:

SC_HAS_PROCESS(Slave0_axi);

Slave0_axi(sc_module_name Name
      , int ID // Unique number among all masters attached to the same IC
               // Must satisfy: 0 <= ID <= 31
      , axi_namespace::axi_addr_t StartAddress // Start address of the slaves memory.
      , axi_namespace::axi_addr_t EndAddress // End address of the slaves memory.
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
      , int WaitState
      ) 
  : Slave_axi(Name,ID,StartAddress,EndAddress,DefaultSlave,MonitorOn,DataBusBytes,ClockFrequency,WaitState)
{
	type = "Slave0";
        printf("%s %hu  - Base Address: 0x%08x, End Address: 0x%08x\n",type, my, StartAddress,EndAddress);
	if (addresser->IsShared(ID))          // Shared Memory
	  myRam = new Shared_mem(ID, EndAddress - StartAddress);
	else if (addresser->IsSemaphore(ID))        // Semaphore Memory
	  myRam = new Semaphore_mem(ID, EndAddress - StartAddress);
	else{                                  // Private Memory
#ifdef LXBUILD
       if(CURRENT_ISS == LX)
       {
          myRam = new Lx_mem(ID, addresser->ReturnPrivateLXSize());
          addresser->pMem_classDebug[ID]=myRam;
       }
#endif
#ifdef SWARMBUILD
       if(CURRENT_ISS == SWARM)
          myRam = new Ext_mem(ID, EndAddress - StartAddress);
#endif
      }
}

};

#endif //_SLAVE0_SYN_H


