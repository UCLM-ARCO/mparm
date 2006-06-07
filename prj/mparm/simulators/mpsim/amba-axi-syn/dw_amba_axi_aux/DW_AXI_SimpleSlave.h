// ============================================================================
// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI TL Layer-1 Simple Slave Example
//    - Example for usage of Slave IF method calls (PutChannel()) and
//      Master IF implementation class methods (GetChannel()) 
//    - Pure sequential slave
//    - Can accept 1 address
//    - Delay between receiving and accepting requests is > 0
// ============================================================================

#ifndef _DW_AXI_SIMPLESLAVE_H
#define _DW_AXI_SIMPLESLAVE_H

#ifdef __EDG__
#define axi_namespace std
#endif

#include "axi_slave_port.h"
#include "axi_master_if_impl.h"
#include "axi_memory_impl.h"

// This module is a slave example that shows the usage of Slave IF method calls
// (PutChannel()) and Master IF implementation class methods (GetChannel()).
// It is a pure sequential slave in the sense that the data busses are only
// served after the address has been received. This slave can only accept one
// address at a time and has therefore a write interleaving depth of one.
template <bool NegEdge, bool Verbose>

class DW_AXI_SimpleSlave // Slave Example for the Usage of Slave IF Methods
  : public axi_namespace::AXI_MasterIFimpl<NegEdge>
#ifdef __EDG__
, public sc_interface
#endif
{
public:
  
axi_namespace::AXI_SlavePort SlaveP;

SC_HAS_PROCESS(DW_AXI_SimpleSlave);

DW_AXI_SimpleSlave(sc_module_name Name
      , int ID // Unique number among all masters attached to the same IC
               // Must satisfy: 0 <= ID <= 31
      , axi_namespace::axi_addr_t StartAddress // Start address of the slaves memory.
      , axi_namespace::axi_addr_t EndAddress // End address of the slaves memory.
               // The addresses must be 4K aligned.
      , const sc_string& FileName // File name for the slave memory
               // initialization file, if no name is specified the memory
               // is initialized with zeros.
      , bool BigEndian // Flag specifying whether the data in the slave
               // memory initialization file are interpreted as big or
               // little endian data. Note that the slave memory itself is
               // always little endian.
      , bool ExclusiveAccess // Flag specifying if the slave supports
               // exclusive access (Alock = 1).
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
      );
~DW_AXI_SimpleSlave();

void SeqSlave();
void Areset_SimpleSlave();
void Reset();

// Direct access methods Master to Slave direction.
virtual bool PutDirect(int ID, bool Write,
                       axi_namespace::axi_addr_t Addr,
                       axi_namespace::axi_data_t Data, int NumBytes);
virtual bool PutDirect(int ID, int BeatNumber,
                       axi_namespace::Achannel &A,
                       const axi_namespace::Wchannel& W,
                       axi_namespace::axi_resp_t& Status);
virtual bool PutDirect(int ID, int BeatNumber,
                       axi_namespace::Achannel &A,
                       axi_namespace::Rchannel &R,
                       axi_namespace::axi_resp_t& Status);
virtual bool GetSlaveParam(int Idx, int& ItrlDepth,
                           axi_namespace::AXI_AddressMap& M);

axi_namespace::AXI_MemoryImpl *m_MemoryImpl;

private:

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

// states
bool m_Reset;
};

#endif //_DW_AXI_SIMPLESLAVE_H
