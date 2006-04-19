#ifndef _SLAVE1_AXI_H
#define _SLAVE1_AXI_H

#ifdef __EDG__
#define axi_namespace std
#endif

#include "core_signal.h"
#include "Slave_axi.h"

class Slave1_axi // Slave1
  : public Slave_axi
{
public:
sc_inout<bool> *extint;

private:
int intno;
uint32_t size;
 
inline void Write(uint32_t addr, uint32_t data, uint8_t bw)
 {
  intno = (addr / 4) - 1;
  if (intno < 0 || intno >= N_CORES)
  {
    printf("%s: Fatal Error: cannot send interrupt on wire %d!\n", name(), intno);
    exit(1);
  }
  // Let's raise the interrupt signal during the data phase (to be compliant with the OCCN model)
  if (CURRENT_ISS == LX)
  {
    if (data > 0)
      extint[intno].write(true);
    else
      extint[intno].write(false);
  }
  else 
    extint[intno].write(true);
 }

inline uint32_t Read(uint32_t addr)
 {
  printf("Fatal error: Interrupt slave is a write only slave, received read request at time %10.1f\n", sc_simulation_time());
  exit(0);
 }
 
public:  

SC_HAS_PROCESS(Slave1_axi);

Slave1_axi(sc_module_name Name
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
      ,int WaitState   
      )
  : Slave_axi(Name,ID,StartAddress,EndAddress,DefaultSlave,MonitorOn,DataBusBytes,ClockFrequency,WaitState)      
{
extint = new sc_inout<bool> [N_CORES];
type = "Interrupt Slave";
size = addresser->ReturnInterruptSize();
printf("%s %hu  - Size: 0x%08x, Base Address: 0x%08x\n",type, my, size, StartAddress);
}
 
};

#endif //_Slave1_SYN_H
