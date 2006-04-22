#ifndef __SLAVE0_AHB_H
#define __SLAVE0_AHB_H

#include "ext_mem.h"
#include "address.h"
#include "Slave_ahb.h"

class Slave0_ahb// A Simple Slave0 Module
        : public Slave_ahb
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

// constructor
Slave0_ahb(sc_module_name name_, // name of the instance
	       ahb_namespace::ahb_addr_t base_address,// base address of the slave0
	       int my
	       )
       : Slave_ahb(name_,base_address,my,MEM_IN_WS,0)

 {
 
type = "Slave0";

        if (addresser->IsShared(my)){          // Shared Memory
	ahb_address_region tmp(_base_address, _base_address+0x000fffff);
	_address_map.push_back(tmp);
	//printf("size = 0x00100000\n");
	myRam = new Shared_mem(my, addresser->ReturnSharedSize(my));
	}
	else{
	if (addresser->IsSemaphore(my))        // Semaphore Memory
          {
         ahb_address_region tmp(_base_address, _base_address+0x00003fff);
	_address_map.push_back(tmp);
	//printf("size = 0x00004000\n");
	myRam = new Semaphore_mem(my, addresser->ReturnSemaphoreSize(my));
	}
	else{                                  // Private Memory
	  ahb_address_region tmp(_base_address, _base_address+0x00bfffff);
	_address_map.push_back(tmp);
	//printf("size = 0x00c00000\n");
	myRam = new Ext_mem(my, addresser->ReturnPrivateSize());
	}}	
}      

}; // end module Slave0

#endif
