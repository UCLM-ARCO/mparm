#ifndef _SLAVE_AXI_H
#define _SLAVE_AXI_H

#ifdef __EDG__
#define axi_namespace std
#endif

#include "axi_slave_port.h"
#include "axi_master_if_impl.h"
#include "globals.h"
#include "address.h"
#include "mem_class.h"
#include "DW_AXI_IC.h"
using namespace axi_namespace;
extern DW_AXI_IC<true, false> *AXI_IC;


class Slave_axi // Slave
  : public axi_namespace::AXI_MasterIFimpl<true>
#ifdef __EDG__
, public sc_interface
#endif
{

public:

uint16_t my;
char* type;
  
axi_namespace::AXI_SlavePort SlaveP;

SC_HAS_PROCESS(Slave_axi);

Slave_axi(sc_module_name Name
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
      ,int WaitState
      );

void SeqSlave();
void Areset_SimpleSlave();
void Reset();


virtual bool GetSlaveParam(int Idx, int& ItrlDepth,
                           axi_namespace::AXI_AddressMap& M);
   
// Direct read from the slave
virtual bool direct_read_axi(int,         // caller id
                             int,                 //num of beat
                             unsigned int,  // address
                             unsigned int*,  // data
                             int);        // number of bytes

// Direct write to the slave
virtual bool direct_write_axi(int,        // caller id
                              int,                //num of beat 
                              unsigned int, // address
                              unsigned int*, // data
                              int);       // number of bytes

protected:
virtual void Write(uint32_t, uint32_t, uint8_t){};
virtual uint32_t Read(uint32_t){return 0;};
protected:

// parameter
const char* m_Name;
int m_ID;
axi_namespace::axi_addr_t m_StartAddress;
axi_namespace::axi_addr_t m_EndAddress;
bool m_DefaultSlave;
bool m_MonitorOn;
int m_DataBusBytes;
// constants
int m_DataBusWords; // Width of the data bus in units of word.
                    // Wordsize is AXI_WORDSIZE_BYTES = sizeof(axi_data_bt)
                    // A word is 4 bytes for unsigned int
int m_WstrbWords;
int m_WstrbBytes;
int m_BitMask;
unsigned int TempWSdata;
uint32_t mask;

// states
bool m_Reset;
uint dev_wait_state;

};

#endif //_SLAVE_SYN_H


