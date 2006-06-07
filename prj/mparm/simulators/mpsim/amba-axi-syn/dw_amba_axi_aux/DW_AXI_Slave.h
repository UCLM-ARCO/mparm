// ============================================================================
//      Project : AXI Platform Modeling
//       Author : Norman Weyrich, Synopsys Inc.
//         Date : 07/15/2003
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
//
//  Description : AXI TL Layer-1 slave example.
//    - Uses external slave state machine
//    - The 4 AXI channels (address, read, write, response)
//      are served in parallel
// ============================================================================

#ifndef _DW_AXI_SLAVE_H
#define _DW_AXI_SLAVE_H

#ifdef __EDG__
#define axi_namespace std
#endif

#include "axi_memory_impl.h"
#include "axi_sl_request.h"

// This module is a slave example that processes the address, write data,
// read data, and write response channel in parallel. Additionally, this slave
// is programmable with respect to the number of interleaved write bursts
// it can accept.
// If the slave receives write data before the corresponding address
// it will accept the first beat and then wait for the address.
template <bool NegEdge, bool Verbose>

class DW_AXI_Slave // AXI Slave Example, Parallel Handling of the 4 AXI Channels
: public axi_namespace::AXI_MasterIFimpl<NegEdge>
#ifdef __EDG__
, public sc_interface
#endif
{
public:
  
axi_namespace::AXI_SlavePort SlaveP;

SC_HAS_PROCESS(DW_AXI_Slave);

DW_AXI_Slave(sc_module_name Name
      , int ID // Unique number among all slaves attached to the same IC.
               // Must satisfy: 0 <= ID <= 31
      , axi_namespace::axi_addr_t StartAddress // Start address of the slaves memory.
      , axi_namespace::axi_addr_t EndAddress // End address of the slaves memory.
               // The addresses must be 4K aligned.
      , int AddrDepth // Number of addresses the slave can accept.
               // Must satisfy: AddrDepth >= 1
      , int IntrlDepth // Number of interleaved write bursts the slave can
               // accept. Must satisfy: AddrDepth >= IntrlDepth >= 1
      , int ReadReorderDepth // ReadReorderDepth > 1 enables re-ordering
               // of data transmission compared to the order of received 
               // read addresses. E.g. The data for the second address may be
               // transmitted before the data for the first address.
               // Must satisfy (Limitation): ReadReorderDepth = 1
      , int TimeOut // Time out value, used for read data reordering
               // if the data for the second address should be
               // transmitted before the data for the first address
               // The slave FSM waits at most Timeout cycles for the
               // second address. Must satisfy: TimeOut >= 0
      , int NumWriteWaits // Number of cycles the slave FSM waits before
               // accepting the next write data.
               // Must satisfy: NumWriteWaits >= 0
      , int NumReadWaits // Number of cycles the slave FSM waits before
               // transmitting the next read data.
               // Must satisfy: NumReadWaits >= 0
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
      , int DataBusBytes // Number of byte lanes of the AXI IC.
               // The same number must be used in the entire AXI system
      , double ClockFrequency // Frequency of the AXI system clock (Aclk)
               // in units of cycles per second. This value is only used
               // for NegEdge = false, i.e., if the negative clock edge
               // toggling scheme is not used.
      );
~DW_AXI_Slave();

// processes
void Slave();
void AresetSl(); // Note: Areset() already used in IF impl

// methods
void Reset();

// Direct access methods Master to Slave direction.
virtual bool GetSlaveParam(int Idx, int& IntrlDepth,
                           axi_namespace::AXI_AddressMap& M);
virtual bool PutDirect(int ID, bool Write, axi_namespace::axi_addr_t Addr,
                       axi_namespace::axi_data_t Data, int NumBytes);
virtual bool PutDirect(int ID, int BeatNumber,
                       axi_namespace::Achannel &A,
                       const axi_namespace::Wchannel& W,
                       axi_namespace::axi_resp_t& Status);
virtual bool PutDirect(int ID, int BeatNumber,
                       axi_namespace::Achannel &A,
                       axi_namespace::Rchannel &R,
                       axi_namespace::axi_resp_t& Status);

private:

// parameter
int m_ID;
axi_namespace::axi_addr_t m_StartAddress;
axi_namespace::axi_addr_t m_EndAddress;
int m_IntrlDepth;
bool m_DefaultSlave;
int m_DataBusBytes;

// constants
int m_DataBusWords; // Width of the data bus in units of word.
                    // Wordsize is AXI_WORDSIZE_BYTES = sizeof(axi_data_bt)
                    // A word is 4 bytes for unsigned int

// states
bool m_Reset;
// data
axi_namespace::AXI_MemoryImpl* m_MemoryImpl;
axi_namespace::AXI_SlRequest<axi_namespace::AXI_MemoryImpl,
                             NegEdge, Verbose>*  m_SlaveFSM;
axi_namespace::Achannel* m_A;
axi_namespace::Wchannel* m_W;
axi_namespace::Rchannel* m_R;
axi_namespace::Bchannel* m_B;

};

#endif //_DW_AXI_SLAVE_H
