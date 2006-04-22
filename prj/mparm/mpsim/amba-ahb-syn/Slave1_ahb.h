#ifndef __SLAVE1_AHB_H
#define __SLAVE1_AHB_H

#include "core_signal.h"
#include "Slave_ahb.h"

class Slave1_ahb// A Simple Slave1 Module
        : public Slave_ahb
{
private:
uint32_t size;
int intno;
 
inline void Write(uint32_t addr, uint32_t data, uint8_t bw)
 {
  // Calculate the interrupt number
  intno = (addr / 4) - 1;
  if (intno < 0 || intno >= N_CORES)
  {
    printf("%s: Fatal Error: cannot send interrupt on wire %d!\n", name(), intno);
    exit(1);
  }
  // Let's raise the interrupt signal during the data phase (to be compliant with the OCCN model)
  extint[intno].write (true); //AST-OCCN
 }

inline uint32_t Read(uint32_t addr)
 {
  printf("Fatal error: Interrupt slave is a write only slave, received read request at time %10.1f\n", sc_simulation_time());
  exit(0);
 }

public:
sc_inout<bool> *extint;

// constructor
Slave1_ahb(sc_module_name name_, // name of the instance
       ahb_namespace::ahb_addr_t  base_address, // base address of the slave1
       int my
       )
     : Slave_ahb(name_,base_address,my,INT_WS,0)  
    {
type = "Interrupt Slave";
extint = new sc_inout<bool> [N_CORES];


size = addresser->ReturnInterruptSize();
printf("%s %hu  - Size: 0x%08x, Base Address: 0x%08x\n\n",type, my, size, _base_address); 

ahb_address_region tmp(_base_address, _base_address+0x00014fff);
_address_map.push_back(tmp);
}  

}; // end module Slave1

#endif
