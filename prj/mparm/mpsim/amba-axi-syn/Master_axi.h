#ifndef _MASTER_AXI_H
#define _MASTER_AXI_H

#ifdef __EDG__
#define axi_namespace std
#endif


#include "config.h"
#include "globals.h"
#include "address.h"
#include "stats.h"
#include "axi_master_port.h"
#include "axi_slave_if_impl.h"



class Master_axi // Master 
 : public axi_namespace::AXI_SlaveIFimpl<true>
#ifdef __EDG__
, public sc_interface
#endif
{
public:

axi_namespace::AXI_MasterPort MasterP;
//TO WRAPPER
sc_out<bool> ready_to_wrapper;
sc_in<bool> request_from_wrapper;
sc_inout<PINOUT> pinout;
uint16_t my;

SC_HAS_PROCESS(Master_axi);

Master_axi(sc_module_name name_
      , int ID // Unique number among all masters attached to the same IC.
               // Must satisfy: 0 <= ID <= 31
      , int Priority// Priority of the master.
               // Must satisfy: Priority >= 0
      , int DataBusBytes // Number of byte lanes of the AXI IC.
               // The same number must be used in the entire AXI system. 
      , double ClockFrequency // Frequency of the AXI system clock (Aclk)
               // in units of cycles per second. This value is only used
               // for NegEdge = false, i.e., if the negative clock edge
               // toggling scheme is not used.
      );

void Send();
void Areset_SimpleMaster();

void Reset();
virtual bool GetMasterParam(int Idx, int& Priority);

private:

// parameter
const char* m_Name;
int m_ID;
int m_Priority;
int m_DataBusBytes;

// constants
int m_DataBusWords; // Width of the data bus in units of word.
                    // Wordsize is AXI_WORDSIZE_BYTES = sizeof(axi_data_bt)
                    // A word is 4 bytes for unsigned int
int m_WstrbWords;
int m_WstrbBytes;
int m_BitMask;

// states
bool m_Reset;
};
#endif // _MASTER_AXI_H
